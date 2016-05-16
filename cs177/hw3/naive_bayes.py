import numpy as np
import matplotlib.pyplot as plt

class params:
	_targets = []
	_data = []
	_classes = []
	_classProbs = []
	def __init__(self, Data, Classes, Targets):
		self._targets = np.array(Targets)
		self._data = np.array(Data)
		self._classes = np.array(Classes)
		self._classProbs = np.array([self.lenfrac(Data[Targets==Classes[0]]) + 1, self.lenfrac(Data[Targets==Classes[1]]) + 1])

	# get frame 
	def lenfrac(self, var):
		return 1. * len(var)
	def params(self, j, i):
		if (j < 0 or j >= self._data.shape[1]):
			raise Exception("j out of bounds",j)
		return self._data[self._targets == i, j]

	def mprobs(self, j, k, i):
		var = self.params(j, i);
		return var[var == k]

	def getclass(self, c):
		return self._classProbs[self._classes==c][0]

	def classprobs(self, c):
		return self.getclass(c) / len(self._data)

	def pofx(self, x):
		# given a vector of x values
		# compute the probability of the value
		# then multiply them all together.
		px = 1.0
		for i, xj in enumerate(x):
			X = self._data[:,i]
			counter = 0
			for y in X:
				counter = counter + (y == xj)
			px = (counter / self.lenfrac(X))
		return px

	def cprobs(self, j, k, i):
		denom = self.getclass(i)
		head = self.lenfrac(self.mprobs(j,k,i))
		# naive bayes assumes conditional independence. So P[X|Y] = P(X)P(Y)
		return (head + .5) / self.getclass(i)

X = np.genfromtxt('data.txt')
Y = np.genfromtxt('labels.txt')

classes=[1,2]
p = params(X, classes, Y)
print "p(C = 1) =",p.getclass(classes[0]) / len(X)
print "p(C = 2) =",p.getclass(classes[1]) / len(X)

for c in classes:
	for i in range(0,X.shape[1]): 
		ps = [p.cprobs(i, 1, c), p.cprobs(i, 2, c)]
		print "P(X =",i,"| C =",c,") [1, 2] = ", ps

# train using only a subset of data.
# Xte,Yte = X[1500:], Y[1500:]
# predictions = np.array([])
# for i in [1500, 50, 10]:
# 	Xtr,Ytr = (X[0:i], Y[0:i])
# 	p = params(Xtr, classes, Ytr)
# 	for d in range(Xte.shape[0]):
# 		pxji = np.empty(2) # P(Xj|C=i)
# 		pxji.fill(1.0)
# 		for ci,c in enumerate(classes):
# 			temp = 1.0
# 			for ji,j in enumerate(Xte[d]):
# 				temp = temp * p.cprobs(ji, j, c)
# 			pxji[ci] = temp * p.classprobs(c)
# 		predictions=np.append(predictions, classes[pxji.argmax()])
# 	print np.mean(predictions.reshape(Yte.shape) != Yte)
# 	predictions = np.array([])		

Xte,Yte = X[20:], Y[20:]
predictions = np.array([])
for i in [20]:
	Xtr,Ytr = (X[0:i], Y[0:i])
	p = params(Xtr, classes, Ytr)
	for d in range(Xtr.shape[0]):
		pxji = np.empty(2) # P(Xj|C=i)
		pxji.fill(1.0)
		for ci,c in enumerate(classes):
			temp = 1.0
			for ji,j in enumerate(Xtr[d]):
				temp = temp * p.cprobs(ji, j, c)
			pxji[ci] = (temp * p.classprobs(c)) / p.pofx(Xtr[d])
		print pxji
