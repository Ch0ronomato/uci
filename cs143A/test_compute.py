#!/usr/bin/python

from subprocess import call

call("make")
call("./compute < numbers.dat > output", shell=True)

with open("output") as f:
    answers = f.readlines()
    theMax = float(answers[0].split(":")[1])
    theMin = float(answers[1].split(":")[1])
    theAvg = float(answers[2].split(":")[1])
    if theMax == 10 and theMin == 1 and theAvg == 5.5:
        print "Your output should parse okay!"
    else:
        print "There was a problem parsing your output. We expected 10.0, 1.0, and 5.5 but got %f, %f, and %f" % (theMax, theMin, theAvg)

