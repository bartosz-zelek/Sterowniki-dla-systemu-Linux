from __future__ import print_function
import argparse
import re
import sys

def stencil(source, output, tag):
    opening = r'<%s>$\n' % tag  # we don't care for the end of the opening tag
    closing = r'^.*</%s>' % tag  # or the start of the closing tag
    match = re.search(r'.*%s(.*)%s.*' % (opening, closing),
                      source.read(), re.MULTILINE | re.S)
    if match:
        output.write(match.group(1))
        return 0
    print("No match for tag %s in %s" % (tag, source.name))
    return 1

if __name__ == '__main__':
    parser = argparse.ArgumentParser(__file__)
    parser.add_argument('--in', type = argparse.FileType('r'),
                        help = ("Input file; if no filename is provided the"
                                " input is expected on the standard input"
                                " stream"))
    parser.add_argument('--out', type = argparse.FileType('w'),
                        help = ("Output file; if no filename is given the"
                                " output is provided on the standard output"
                                " stream"))
    parser.add_argument('--tag', required = True,
                        help = "The tag between which the stencil is applied")

    args = vars(parser.parse_args())
    source = args['in'] if args['in'] else sys.stdin
    output = args['out'] if args['out'] else sys.stdout

    ok = stencil(source, output, args['tag'])
    if source != sys.stdin:
        source.close()
    if output != sys.stdout:
        output.close()
    sys.exit(ok)
