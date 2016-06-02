#!/usr/bin/python2.7
from random import random
import numpy as np
import matplotlib.pyplot as plt

l = [ random() for _ in range(50001) ]
plt.hist(l, bins=np.linspace(0,1,1000))
# plt.show()

# this is not approximately normal. This is
# because the docs actually state that it uses
# a completely deterministic method called
# Mersenne Twister, which is not approximately
# normal. 

# Randrange is implemented very similarly. The only
# way to get a uniformly randomly distrbution in 
# python is too use the guass method, and that's
# not really exciting.

# begin sums part a
n=int(raw_input("Input n: "))
K=int(raw_input("Input k: "))
matr=np.empty([n, K])
for i in range(0, n):
	for j in range(0, K):
		matr[i,j] = random()

matr=np.sum(matr, axis=0)
print "mean,var=",(np.mean(matr), np.var(matr))
plt.hist(matr)
plt.show()
