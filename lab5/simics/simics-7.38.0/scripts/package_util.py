# © 2020 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

import unittest
import json
import tarfile
import time
import os.path
import sys
import io
import os
from unittest.mock import patch

__all__ = ['create_package', 'sign_package']

METADATA_VERSION = 2


def _get_pkg_dir(package_type, dist_name, version):
    '''Return the top directory name for the package, or None if no
    top directory.'''
    if version:
        if package_type == 'addon':
            return f'simics-{dist_name.lower()}-{version}'
        elif dist_name in ["Simics-Express", "Simics-Core"]:
            return f'{dist_name.lower()}-{version}'
        elif dist_name == "Simics-Base":
            return f'simics-{version}'
        else:
            raise Exception(f"Dist is not addon but unknown {package_type}")
    else:
        return None

def prefix_dir(metadata):
    return _get_pkg_dir(metadata['type'],
                        metadata['package-name'],
                        metadata['version'])


def prefix_all_files_with_package_dir(metadata, filemap):
    result = []
    for (package_location, src_location) in filemap:
        prefix = prefix_dir(metadata)
        if not package_location.startswith(prefix):
            package_location = os.path.join(prefix, package_location)
        result.append((package_location, src_location))
    return result


def package_name(metadata):
    return "simics-pkg-%s-%s" % (
        metadata['package-number'], metadata['version'])


def package_name_with_host(metadata):
    return "%s-%s" % (package_name(metadata), metadata['host'])


def package_filename(output_dir, metadata):
    return os.path.join(output_dir, f'{package_name_with_host(metadata)}.ispm')


def metadata_filename():
    return 'ispm-metadata'


def inner_package_filename():
    return 'package.tar.gz'


def is_metadata(filename):
    return filename == metadata_filename()


def is_inner_package(filename):
    return filename == inner_package_filename()

class MissingFiles(Exception):
    def __init__(self, missing_files, pkg):
        self.missing_files = missing_files
        self.pkg = pkg
    def __str__(self):
        return f"Missing files in {self.pkg}: {' '.join(self.missing_files)}"

def add_files_to_package(add_function, filemap, ignore_missing, metadata):
    missing = [f for (_, f) in filemap if not os.path.isfile(f)]
    if missing:
        exc = MissingFiles(sorted(missing), metadata['package-name'])
        if ignore_missing:
            print(f"*** Warning: {exc}", file=sys.stderr)
            filemap = [(d, s) for (d, s) in filemap
                       if s not in missing]
        else:
            raise exc
    for (pkg_location, src_location) in sorted(filemap):
        add_function(src_location, pkg_location)
        metadata['uncompressedSize'] += os.stat(src_location).st_size

def add_string_to_tar_archive(archive, data, location, metadata,
                              package_content):
    file_object = io.BytesIO(data.encode('utf8'))
    add_memory_object_to_tar_archive(archive, file_object, location, metadata,
                                     package_content)


def add_memory_object_to_tar_archive(archive, file_object, location, metadata,
                                     package_content):
    info = tarfile.TarInfo(location)
    file_object.seek(0, io.SEEK_END)
    info.size = file_object.tell()
    if (package_content):
        metadata['uncompressedSize'] += info.size
    if (metadata["host"] == "linux64"):
        import pwd
        import grp
        info.uid = os.getuid()
        info.gid = os.getgid()
        info.uname = pwd.getpwuid(os.getuid()).pw_name
        info.gname = grp.getgrgid(os.getgid()).gr_name
    file_object.seek(0, io.SEEK_SET)
    archive.addfile(tarinfo=info, fileobj=file_object)

def metadata_to_ispm(metadata):
    keymaps = {'package-number': 'packageNumber',
               'package-name': 'packageName',
               'build-id': 'buildId',
               'build-id-namespace': 'buildIdNamespace',
               'type': 'kind'}
    return {keymaps.get(k, k): str(v) if k == 'build-id' else v
            for (k, v) in metadata.items()}

def create_tar_archive(output_archive, filemap, ignore_missing, metadata):
    with tarfile.open(output_archive, 'w', dereference=True) as archive:
        inner_tar = create_inner_tar_archive(filemap, ignore_missing, metadata)
        add_string_to_tar_archive(archive,
                                  json.dumps(metadata_to_ispm(metadata)),
                                  metadata_filename(), metadata, False)
        add_memory_object_to_tar_archive(archive, inner_tar, inner_package_filename(),
                                         metadata, False)


def add_signed_entry(archive, tarinfo, buffer):
    info = create_updated_tarinfo(tarinfo)
    buffer.seek(io.SEEK_SET, 0)
    archive.addfile(tarinfo=info,
                    fileobj=buffer)


def sign_file(attach_signature_cb, tmp_file):
    attach_signature_cb(tmp_file)


def size_from_file_object(file_object):
    file_object.seek(0, io.SEEK_END)
    size = file_object.tell()
    #  the file_object read buffer back to the beginning for addfile
    file_object.seek(0, io.SEEK_SET)
    return size


def sign_tar_entry_and_add_to_archive(file_to_sign, new_archive, info, tmp_dir, attach_signature_cb, metadata):
    tmp_filename = os.path.basename(info.name)
    full_tmp_file_path = os.path.join(tmp_dir, tmp_filename)

    with open(full_tmp_file_path, 'wb') as o:
        o.write(file_to_sign.read())
    sign_file(attach_signature_cb, full_tmp_file_path)

    new_info = tarfile.TarInfo(info.name)
    with open(full_tmp_file_path, 'rb') as o:
        new_info.name = info.name
        new_info.size = size_from_file_object(o)
        metadata['uncompressedSize'] += new_info.size
        add_signed_entry(new_archive, new_info, o)


def create_updated_tarinfo(tarinfo):
    info = tarfile.TarInfo(tarinfo.name)
    info.size = tarinfo.size
    info.uid = tarinfo.uid
    info.gid = tarinfo.gid
    info.uname = tarinfo.uname
    info.gname = tarinfo.gname
    info.mtime = int(time.time())
    return info


def generate_detached_signature(memory_object, options):
    memory_object.seek(0, io.SEEK_SET)
    unsigned_file = os.path.join(
        options.tmp_dir, f"{os.path.basename(options.package)}.unsigned")
    signature_file = os.path.join(
        options.tmp_dir, f"{os.path.basename(options.package)}.signed")

    with open(unsigned_file, 'wb') as f:
        f.write(json.dumps(options.metadata).encode('utf-8'))
        f.write(memory_object.read())
        memory_object.seek(0, io.SEEK_SET)
    options.generate_detached_signature_cb(unsigned_file, signature_file)
    os.remove(unsigned_file)
    return signature_file


def sign_package(generate_detached_signature_cb, add_attach_signature_cb, package,
                 output_dir, tmp_dir, file_list, progress):
    metadata = None
    new_package = os.path.join(output_dir, os.path.basename(package))
    with open(file_list, 'r') as f:
        files_to_sign = json.loads(f.read())

    with tarfile.open(new_package, 'w') as new_archive:
        with tarfile.open(package, 'r') as archive:
            for tarinfo in archive:
                if (is_metadata(tarinfo.name)):
                    metadata = json.loads(archive.extractfile(
                        tarinfo).read().decode('utf8'))
                    metadata['uncompressedSize'] = 0
                elif is_inner_package(tarinfo.name):
                    if metadata is None:
                        raise Exception(
                            f'Invalid package content, expected "{metadata_filename()}" first')
                    options = SignOptions(metadata, generate_detached_signature_cb,
                                          add_attach_signature_cb, package, output_dir, tmp_dir,
                                          files_to_sign, progress)
                    with tarfile.open(fileobj=archive.extractfile(tarinfo)) as inner_package:
                        inner_tar = create_inner_signed_tar_archive(
                            options, inner_package)
        add_string_to_tar_archive(new_archive, json.dumps(options.metadata),
                                  metadata_filename(), options.metadata, False)
        add_memory_object_to_tar_archive(new_archive, inner_tar, inner_package_filename(),
                                         options.metadata, False)
        signature = generate_detached_signature(inner_tar, options)
        new_archive.add(signature, 'signature')
        os.remove(signature)


def should_sign(options, filename):
    filename_without_pkg_prefix = filename.partition('/')[2]
    return filename_without_pkg_prefix in options.files_to_sign


class NullReporter:
    def add_signed_entry(self):
        pass

    def add_unsigned_entry(self):
        pass


class ProgressReporter:
    def __init__(self, total_files, files_to_sign, interval):
        self.total_files = total_files
        self.files_to_sign = files_to_sign
        self.interval = interval
        self.signed_files = 0
        self.added_files = 0

    def add_signed_entry(self):
        self.signed_files += 1
        self.added_files += 1
        self.report()

    def add_unsigned_entry(self):
        self.added_files += 1
        self.report()

    def report(self):
        if self.added_files % self.interval:
            return
        message = f'Added {self.added_files}/{self.total_files} files'
        message += f' signed {self.signed_files}/{self.files_to_sign}'
        print(message)


class SignOptions:
    def __init__(self, metadata, generate_detached_signature_cb,
                 generate_attached_signature_cb, package, output_dir, tmp_dir,
                 files_to_sign, progress):
        self.metadata = metadata
        self.generate_detached_signature_cb = generate_detached_signature_cb
        self.generate_attached_signature_cb = generate_attached_signature_cb
        self.package = package
        self.output_dir = output_dir
        self.tmp_dir = tmp_dir
        self.files_to_sign = files_to_sign
        self.progress = progress


def create_inner_signed_tar_archive(options, old_archive):
    if options.progress:
        reporter = ProgressReporter(
            len(old_archive.getmembers()), len(options.files_to_sign), options.progress)
    else:
        reporter = NullReporter()

    in_memory_archive = io.BytesIO()
    with tarfile.open(fileobj=in_memory_archive, mode='w:gz',
                      dereference=True, compresslevel=6) as archive:
        for info in old_archive:
            if should_sign(options, info.name):
                sign_tar_entry_and_add_to_archive(old_archive.extractfile(info), archive, info, options.tmp_dir,
                                                  options.generate_attached_signature_cb, options.metadata)
                reporter.add_signed_entry()
            else:
                options.metadata['uncompressedSize'] += info.size
                archive.addfile(
                    tarinfo=info, fileobj=old_archive.extractfile(info))
                reporter.add_unsigned_entry()
    return in_memory_archive


def create_inner_tar_archive(filemap, ignore_missing, metadata):
    in_memory_archive = io.BytesIO()
    with tarfile.open(fileobj=in_memory_archive, mode='w:gz',
                      dereference=True, compresslevel=6) as archive:
        add_string_to_tar_archive(archive,
                                  generate_package_info(metadata, filemap),
                                  '/'.join([prefix_dir(metadata),
                                            'packageinfo',
                                            full_package_name(metadata)]),
                                  metadata, True)
        add_files_to_package(archive.add, filemap, ignore_missing, metadata)
    return in_memory_archive


def full_package_name(metadata):
    return f'{metadata.get("package-name", "")}-{metadata["host"]}'


def generate_package_info(metadata, filemap):
    info = []
    for entry in ['name', 'description', 'version', 'host',
                  'package-name', 'package-number',
                  'build-id', 'build-id-namespace', 'type']:
        info.append(f'{entry}: {metadata[entry]}')
    for entry in ['extra-version', 'confidentiality']:
        info.append(f'{entry}: {metadata.get(entry, "")}')
    info.append('files:')
    for dest in [dest for (dest, f) in filemap
                 if os.path.isfile(f)]:
        info.append(f'\t{dest}')
    return '\n'.join(info)


def validate_metadata(metadata):
    if not isinstance(metadata['build-id'], int):
        raise Exception("build-id must be a number;"
                        f" got {repr(metadata['build-id'])}")
    if not isinstance(metadata['package-number'], int):
        raise Exception("package-number must be a number;"
                        f" got {repr(metadata['package-number'])}")
    if ('metadata-version' in metadata
        and metadata['metadata-version'] != METADATA_VERSION):
        raise Exception(f"Unknown metadata-version: need {METADATA_VERSION},"
                        f" got {metadata['metadata-version']}")


def create_package(metadata, filemap, ignore_missing, output_dir):
    required_fields = ['name', 'package-number', 'version', 'package-name',
                       'type', 'host', 'build-id', 'build-id-namespace',
                       'confidentiality', 'description']
    for field in required_fields:
        if field not in metadata:
            raise Exception(
                f"Invalid metadata: '{field}' is required package metadata")
    metadata = dict(metadata)
    validate_metadata(metadata)
    metadata['uncompressedSize'] = 0
    filemap = prefix_all_files_with_package_dir(metadata, filemap)

    archive = package_filename(output_dir, metadata)

    create_tar_archive(archive, filemap, ignore_missing, metadata)
    return archive


class TestPackageUtil(unittest.TestCase):
    def package_filename(self):
        number = self.metadata['package-number']
        version = self.metadata['version']
        return f'simics-pkg-{number}-{version}'

    def setUp(self):
        self.metadata = {'package-number': 1000, 'version': '7.0.0',
                         'host': 'linux64', 'uncompressedSize': 0}

    def testArchiveName(self):
        self.assertEqual(package_filename('out', self.metadata),
                         os.path.join('out',
                                      'simics-pkg-1000-7.0.0-linux64.ispm'))
        self.metadata['host'] = 'win64'
        self.assertEqual(
            package_filename('out', self.metadata),
            os.path.join('out', 'simics-pkg-1000-7.0.0-win64.ispm'))

    def testMetadataName(self):
        self.assertEqual(metadata_filename(), 'ispm-metadata')

    @patch('os.stat')
    @patch('os.path.isfile')
    def testAddFilesToPackage(self, isfile_mock, stat_mock):
        stat_mock.st_size = 1234
        isfile_mock.return_value = True
        filemap = [('a1', 'a2'), ('b1', 'b2')]
        result = []

        def add_function(pkg_location, src_location):
            result.append((pkg_location, src_location))
        add_files_to_package(
            add_function, filemap, True, self.metadata)
        self.assertEqual(result, [(y, x) for (x, y) in filemap])

    def testCreatePackage(self):
        self.assertRaises(Exception, create_package, {}, [], False, 'out')
