#!/usr/bin/python2.7
# implementation of the power method.
import numpy as np
import matplotlib.pyplot as plt

# get input for an MxM matrix
input=raw_input
M=int(input("Please enter M: "))
p0 = [float(x) for x in input("Please enter p0, initial probability distro:").split()]
state_transition = [[float(x) for x in input("Please enter line " + str(i) + ": ").split()] for i in range(0,M)]
epi=float(input("Please enter pi, the convergence ratio: "))

# convert to numpy arrays
p0 = np.array(p0)
state_transition = np.array(state_transition)
max_iters=50
done=False
i=1
lastp = p0
state=[p0[1]]
while (i < max_iters):
	x = np.dot(lastp,state_transition)
	diff=0.0
	for j in range(0, M):
		diff = diff + abs(x[j] - lastp[j])
	done = diff <= epi
	i = i + 1	
	lastp = x
	state.append(x[1])

print(len(state),len(range(1,51)))
p=plt.plot(range(1,51), state)
name=input("please enter the file name")
plt.title(str(p0))
plt.show()
