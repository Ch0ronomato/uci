#!/usr/bin/python2.7
# implementation of the power method.
import numpy as np

# get input for an MxM matrix
input=raw_input
M=int(input("Please enter M: "))
p0 = [float(x) for x in input("Please enter p0, initial probability distro:").split()]
state_transition = [[float(x) for x in input("Please enter line " + str(i) + ": ").split()] for i in range(0,M)]
epi=float(input("Please enter pi, the convergence ratio: "))

# convert to numpy arrays
p0 = np.array(p0)
state_transition = np.array(state_transition)
max_iters=10000
done=False
i=0
lastp = p0
while (not done and i < max_iters):
	x = np.dot(lastp,state_transition)
	diff=0.0
	for j in range(0, M):
		diff = diff + abs(x[j] - lastp[j])
	done = diff <= epi
	i = i + 1	
	lastp = x

print("The final po after " + str(i) + " iters is " + str(lastp))
