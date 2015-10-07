#!/usr/bin/python

from subprocess import call

call("make")
call("./average numbers.dat > output", shell=True)

with open("output") as f:
    answer = float(f.readlines()[1])
    if answer == 5.5:
        print "Your output should parse okay!"
    else:
        print "There was a problem parsing your output. We expected 5.5 and got", answer
