#!/usr/bin/python2.7
# implementation of the power method.
import numpy as np

# get input for an MxM matrix
input=raw_input
M=int(input("Please enter M: "))
p0 = [float(x) for x in input("Please enter p0, initial probability distro:").split()]
state_transition = [[float(x) for x in input("Please enter line " + str(i) + ": ").split()] for i in range(0,M)]
i=int(input("Enter current amount of money: "))
n=int(input("Enter stopping point: "))
print(i,n)
# convert to numpy arrays
p0 = np.array(p0)
state_transition = np.array(state_transition)
lastp = p0
freqs=np.zeros(M)
while (i < n and i > 0):
	x = state_transition[lastp.argmax()]
	lastp = state_transition[x.argmax()] 
	if (x.argmax()):
		i = i + 1
		# print("Earned one dollar",i)
	else:
		i = i - 1	
		# print("Lost one dollar",i)
	freqs[x.argmax()] = freqs[x.argmax()] + 1

freqs = [ x / np.sum(freqs) for x in freqs ]
print(freqs)
