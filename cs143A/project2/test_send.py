#!/usr/bin/python

import subprocess
import os
import signal
from time import sleep

def sighandler(signum, frame):
    print "Received SIGUSR2!"

signal.signal(signal.SIGUSR2, sighandler)

subprocess.call("gcc send_signals.c", shell=True)

with open("output", "w") as outfile:
    proc = subprocess.Popen("./a.out", stdout=outfile)

    pid = proc.pid

    sleep(1)
    proc.send_signal(signal.SIGUSR1)
    sleep(1)
    proc.send_signal(signal.SIGUSR1)
    sleep(1)
    proc.send_signal(signal.SIGUSR1)
    sleep(1)
    proc.send_signal(signal.SIGINT)
    sleep(1)
    proc.wait()

with open("output") as f:
    answer = f.readlines()[0]
    answer = int(answer.replace("Signals: ", ""))
    if answer == 3:
        print "Your output should parse okay!"
    else:
        print "There was a problem parsing your output. We expected 3 and got", answer
