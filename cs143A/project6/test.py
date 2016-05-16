#!/usr/bin/python

from subprocess import call
import sys

problemName = 'banker'

if len(sys.argv) > 1:
    inputScripts = sys.argv[1:]
else:
    inputScripts = ['test_safe.txt', 'test_unsafe.txt']

call("make", shell=True)
for script in inputScripts:
    call("./%s < %s > output" % (problemName, script), shell=True)

    with open("output") as f:
        answers = f.readlines()
        answer = answers[-1].strip()
        if "unsafe state" in answer:
            print "Your program outputs that script %s is an unsafe state!" % script
        elif "safe state" in answer:
            print "Your program outputs that script %s is a safe state!" % script
        else:
            print "There was a problem parsing your output. Couldn't find either 'unsafe state' or 'safe state' in the last line of your output..."
