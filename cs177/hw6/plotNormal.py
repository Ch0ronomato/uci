#!/usr/bin/python2.7
import numpy as np
import matplotlib.pyplot as plt
from math import pow
from math import exp
from math import pi
from math import sqrt

# plots the normal distribution
# mu is the mean
# var is the variance
# x are the locations to plot
# f(x) = (1 / var*sqrt(2pi))e(-(x-mu)2)/2var2
input=raw_input
def plotNormal(mu, stddev, X):
	e=lambda x,mu,stddev: exp(-(pow(x-mu,2)/2*pow(stddev,2)))
	Y=[(1/(stddev * sqrt(2*pi))) * e(x, mu ,stddev) for x in X]
	plt.plot(X, Y)
	plt.show()

# 5.5, 8.25
mu=float(input("Please enter mean: "))
var=float(input("Please enter variance: "))
N=int(input("Please enter size of data: "))
X=np.zeros(N)
for i in range(N):
	X[i]=float(input("\t"+str(i)+"="))	

plotNormal(mu, var, X)
