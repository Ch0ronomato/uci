#!/usr/bin/python

import subprocess
import os
import signal
from time import sleep

subprocess.call("gcc handle_signals.c", shell=True)

with open("output", "w") as outfile:
    proc = subprocess.Popen("./a.out", stdout=outfile)

    pid = proc.pid

    sleep(1)
    proc.send_signal(signal.SIGINT)
    sleep(1)
    proc.send_signal(signal.SIGQUIT)
    sleep(1)
    proc.send_signal(signal.SIGINT)
    sleep(1)
    proc.send_signal(signal.SIGTSTP)
    sleep(1)
    proc.send_signal(signal.SIGQUIT)
    sleep(1)
    proc.send_signal(signal.SIGTSTP)
    sleep(1)
    proc.send_signal(signal.SIGQUIT)
    sleep(1)
    proc.send_signal(signal.SIGINT)
    sleep(1)
    proc.send_signal(signal.SIGTSTP)
    sleep(1)
    proc.wait()

with open("output") as f:
    lines = f.readlines()
    nints = int(lines[1][len("Interrupt: "):])
    ntstps = int(lines[2][len("Stop: "):])
    nquits = int(lines[3][len("Quit: "):])
    if nints == 3 and ntstps == 3 and nquits == 3:
        print "Your output should parse okay!"
    else:
        print "There was a problem parsing your output. We expected 3,3,3 and got %d,%d,%d" % (nints, ntstps, nquits)
