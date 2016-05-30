#!/usr/bin/python2.7


from random import random as rand
from math import log
import numpy as np
import matplotlib.pyplot as plt
# the general technique here (and this is super cool too by the way), is too take the inverse of the pmf
# then for n random numbers generated between 0 and 1, plug that into the inverse, and the resulting vector will
# be of that distrobution. So X ~ exp(lamb)

def Y(x, l):
	# inverse = (1/l) * log(1 - x)	
	return abs((1./l) * log(1-x))


n=int(raw_input("Please enter how much to generate: "))
l=float(raw_input("Please enter lambda: "))
distro = [Y(rand(), l) for i in range(0, n)]
actual = np.random.exponential(l, n)
bins=int(raw_input("please enter number of bins: "))

print "header\t\tMean\t|\tvariance"
print "gen\t\t"+str.format('{0:.4f}',np.mean(distro))+"\t|\t"+str(np.var(distro))
print "act\t\t"+str.format('{0:.2f}',1./l)+"\t|\t"+str(1./pow(l,2))
_,ax = plt.subplots(1,2)
ax[0].hist(distro, bins)
ax[1].hist(actual, bins, normed=True)
plt.show()
