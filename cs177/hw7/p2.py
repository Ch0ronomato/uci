#!/usr/lib/python2.7
from math import floor
import numpy as np
import matplotlib.pyplot as plt

data = np.genfromtxt("timestamps.txt")
diffs=np.zeros(len(data)-1)
bins=np.zeros(floor(max(data)) + 1)
bins[floor(data[len(data) - 1] / 4)] += 1
for i in range (0, len(data) - 1):
	diffs[i] = data[i+1] - data[i]
	bins[floor(data[i])] += 1

print "Average mean is " + str(np.mean(diffs)) + " minutes"
print "Length is " + str(len(diffs))


_,ax = plt.subplots(1,2)
ax[0].hist(diffs, np.linspace(0, 3, 50))
ax[1].hist(np.random.exponential(np.mean(diffs), len(diffs)), normed=True)
# the two plots are very very close.
plt.show()

print "mean and var: " + str((np.mean(bins),np.var(bins)))
plt.hist(bins);
plt.show()
