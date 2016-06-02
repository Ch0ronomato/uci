#!/usr/bin/python2.7
import random as rand
from math import log
import numpy as np
import matplotlib.pyplot as plt

def Y(x, l):
	# inverse = (1/l) * log(1 - x)	
	return abs((1./l) * log(1-x))

graph=[]
def sim(l, mu, sim, distro):
	distro.sort()
	in_progress=distro.pop(0)+Y(rand.random(), mu)
	queued=[in_progress]
	qc = 1
	for i in range(0, len(distro)):
		item=distro.pop(0)
		if (item > in_progress):
			qc -= 1
			graph.append(in_progress)
			in_progress = item+Y(rand.random(), mu)
		else:
			qc += 1
			queued.append(item)

		# see if we need to move out any item in the queue
		for i,x in enumerate(queued):
			if (x > in_progress):
				qc -= 1
				graph.append(in_progress)
				in_progress = x + Y(rand.random(), mu)
				queued.pop(i)
				break
		print "queue count is " + str(qc) + " current " + str(in_progress)
	
	while (qc > 0):
		for i,x in enumerate(queued):
			if (x > in_progress):
				qc -= 1
				graph.append(in_progress)
				in_progress = x + Y(rand.random(), mu)
				queued.pop(i)
			print "queue count is " + str(qc) + " current " + str(in_progress)

		print "Forcing " + str(in_progress) + " to finish"
		in_progress = 0
	
l=float(raw_input("Please enter lambda: "))
mu=float(raw_input("Enter mu: "))
K=int(raw_input("Input the number of users to simulate: "))
distro = [Y(rand.random(), l) for i in range(0, K)]
actual = np.random.exponential(l, K)
sim(l,mu,K,distro) 
plt.hist(graph)
plt.show()
