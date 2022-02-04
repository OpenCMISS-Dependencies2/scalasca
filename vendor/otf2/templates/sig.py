from __future__ import print_function

import sys
import os

try:
    from hashlib import md5
except:
    from md5 import new as md5

if len(sys.argv) == 1:
    print('Invalid number of arguments', file=sys.stderr)
    sys.exit(3)

m = md5()
hexdigest_length = m.digest_size * 2

verbose = False
if sys.argv[1] in ('--verbose'):
    verbose = True
    sys.argv = sys.argv[:1] + sys.argv[2:]

if sys.argv[1] in ('--check', '--check-output', '--check-input'):
    if len(sys.argv) != 3:
        print('Invalid number of arguments', file=sys.stderr)
        sys.exit(3)

    # next arg is sig file
    signame = sys.argv[2]

    # read the sigfile and the two signatures for the output and input
    fp = open(signame)
    sig = fp.read()
    fp.close()
    outsig = sig[0:hexdigest_length]
    insig  = sig[hexdigest_length+1:2 * hexdigest_length+1]
    filenames = [x for x in sig[2 * hexdigest_length + 2:].split('\n') if x]

    sigdir = os.path.dirname(signame)
    # than the output file
    outname = filenames[0]
    outfilepath = os.path.normpath(os.path.join(sigdir, outname))
    # remaings are the input files
    infiles = filenames[1:]

    if sys.argv[1] in ('--check', '--check-output'):
        fp = open(outfilepath)
        m.update(fp.read().encode())
        fp.close()
        if m.hexdigest() != outsig:
            if verbose:
                print('%s: template result does not match signature, please regenerate' % (outfilepath), file=sys.stderr)
            sys.exit(1)
        m = md5()

    if sys.argv[1] in ('--check', '--check-input'):
        for name in infiles:
            filepath = os.path.normpath(os.path.join(sigdir, name))
            fp = open(filepath)
            m.update(fp.read().encode())
            fp.close()
        if m.hexdigest() != insig:
            if verbose:
                print('%s: template inputs do not match with signature, please regenerate' % (outfilepath,), file=sys.stderr)
            sys.exit(2)
        m = md5()

    sys.exit(0)

# generate mode

if len(sys.argv) < 2:
    print('Invalid number of arguments', file=sys.stderr)
    sys.exit(3)

# second arg is output name
#print('out: %s' % (sys.argv[2],), file=sys.stderr)
fp = open(sys.argv[1])
m.update(fp.read().encode())
fp.close()
outsig = m.hexdigest()
m = md5()

# remaining files are input
for name in sys.argv[2:]:
    #print('in:  %s' % (name,), file=sys.stderr)
    fp = open(name)
    m.update(fp.read().encode())
    fp.close()
insig = m.hexdigest()

# write sig file
sys.stdout.write('%s\n%s\n' % (outsig, insig))
for name in sys.argv[1:]:
    sys.stdout.write('%s\n' % (name,))

sys.exit(0)
