import mltools as ml
import mltools.linear
import matplotlib.pyplot as plt
import numpy as np

ks = [1,3,5,7,10,18]
err = []
lrData = []
data = np.genfromtxt("data/curve80.txt", delimiter=None)
X = data[:,0]
X = X[:,np.newaxis]
Y = data[:,1]
Xtr, Xte, Ytr, Yte = ml.splitData(X, Y, 0.75)
for i,k in enumerate(ks):
	nFolds = 5

	XtrP = ml.transforms.fpoly(Xtr, k, bias=False)
	XtrP,params = ml.transforms.rescale(XtrP)
	Phi = lambda X : ml.transforms.rescale(ml.transforms.fpoly(X, k, False), params)[0]
	for iFold in range(nFolds):
		Xti,Xvi,Yti,Yvi = ml.crossValidate(Xtr, Ytr, nFolds, iFold)
		learner = ml.linear.linearRegress(Phi(Xti), Yti)
		err.append(learner.mse(Phi(Xvi), Yvi))

	plt.semilogy(err)
	plt.show()
