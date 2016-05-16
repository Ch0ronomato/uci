#!/usr/bin/python2.7
# implementation of the power method.
import numpy as np

# get input for an MxM matrix
input=raw_input
M=int(input("Please enter M: "))
p0 = [float(x) for x in input("Please enter p0, initial probability distro:").split()]
state_transition = [[float(x) for x in input("Please enter line " + str(i) + ": ").split()] for i in range(0,M)]
T=int(input("Please enter the time step: "))

# convert to numpy arrays
p0 = np.array(p0)
state_transition = np.array(state_transition)
i=0
lastp = p0
freqs=np.zeros(M)
print("[")
while (i < T):
	x = np.dot(lastp,state_transition)
	diff=0.0
	for j in range(0, M):
		diff = diff + abs(x[j] - lastp[j])
	i = i + 1	
	lastp = x
	print(x.argmax())
	freqs[x.argmax()] = freqs[x.argmax()] + 1

freqs = [ x / np.sum(freqs) for x in freqs ]
print(freqs)
