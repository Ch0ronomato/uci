#!/usr/bin/python

import sys, os

problemName = 'myls'

if len(sys.argv) > 1:
    inputArgs = sys.argv[1:]
else:
    inputArgs = ['/bin', '/var/www']

os.system("make -f Makefile_%s" % problemName)
outputFilename = ".temp_output"
expectedOutputFilename = outputFilename + "_expected"

for inputArg in inputArgs:
    # NOTE: we bit shift the return by one byte as the return code is actually
    # in the second byte
    ret = os.system("./%s %s > %s" % (problemName, inputArg, outputFilename)) >> 8
    if ret != 0:
        print "%s returned error code %d during execution with %s: -5pts!; " % (problemName, ret, inputScript)

    # this command removes the '.' that comes after the file mode bits
    # referring to some SELinux context
    expectedCommand = "/bin/ls -lLR %s | sed 's/^\(.[-rwx]\{9\}\)[ \.]/\\1 /' > %s" % (inputArg, expectedOutputFilename)
    ret = os.system(expectedCommand) >> 8

    # If their program passes the first diff, they get extra credit.
    # Otherwise, we'll run diff and ignore whitespace and newlines,
    # manually determining how many points they lost.
    ret = os.system("diff %s %s > /dev/null" % (outputFilename, expectedOutputFilename)) >> 8
    if ret == 0:
        print "%s output matches perfectly!" % problemName
    else:
        print "%s output didn't match perfectly for these lines:" % problemName
        ret = os.system("diff -wB %s %s" % (outputFilename, expectedOutputFilename)) >> 8
        print "For full points, make sure everything matches up just right!  You will of course receive partial credit if you cannot get it 100%% correct"
