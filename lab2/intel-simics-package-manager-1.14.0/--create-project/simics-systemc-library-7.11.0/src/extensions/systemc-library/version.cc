// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/module_loaded.h>
#include <simics/systemc/version.h>
#include <systemc>

#if EXTERNAL_BUILD
#define LIBRARY_KERNEL_VERSION unknown_version
#else
#include <systemc-kernel-version.h>
#endif

#define _VER1(v) _VER2(v)
#define _VER2(v) #v

#include <map>
#include <string>

namespace {
static const char *systemc_library_version = _VER1(SCL_VERSION);
static const char *systemc_library_kernel_version =
    _VER1(LIBRARY_KERNEL_VERSION);
}  // namespace

namespace simics {
namespace systemc {

SCL_VERSION_STRING::SCL_VERSION_STRING() {
}

Version::Version() {
    versions_["kernel"] = sc_core::sc_release();
    versions_["library"] = systemc_library_version;
    versions_["library_kernel"] = systemc_library_kernel_version;
    if (ModuleLoaded::module_name())
        versions_["module"] = ModuleLoaded::module_name();
}

const char *Version::kernel_version() const {
    return sc_core::sc_release();
}

const char *Version::library_version() const {
    return systemc_library_version;
}

const char *Version::library_kernel_version() const {
    return systemc_library_kernel_version;
}

const std::map<std::string, std::string> *Version::versions() const {
    return &versions_;
}

bool Version::operator==(const iface::ScVersionInterface &rhs) const {
    std::map<std::string, std::string>::const_iterator i;
    const std::map<std::string, std::string> *rhs_versions =
        rhs.versions();
    for (i = versions_.begin(); i != versions_.end(); ++i) {
        std::map<std::string, std::string>::const_iterator j =
            rhs_versions->find(i->first);
        if (j != rhs_versions->end() && i->second != j->second)
            return false;
    }
    return true;
}

bool Version::operator!=(const iface::ScVersionInterface &rhs) const {
    return !(*this == rhs);
}

void Version::setVersion(const std::string &key, const std::string &value) {
    versions_[key] = value;
}

}  // namespace systemc
}  // namespace simics
