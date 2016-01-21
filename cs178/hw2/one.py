import mltools as ml
import mltools.linear
import matplotlib.pyplot as plt
import numpy as np

# Load the data set and split it
data = np.genfromtxt("data/curve80.txt", delimiter=None)
X = data[:,0]
X = X[:,np.newaxis]
Y = data[:,1]
Xtr, Xte, Ytr, Yte = ml.splitData(X, Y, 0.75)

# part two
lr = ml.linear.linearRegress(Xtr, Ytr)
xs = np.linspace(0, 10, 200)
xs = xs[:,np.newaxis]
ys = lr.predict(xs)

plt.scatter(xs, ys)
plt.scatter(Xtr, Ytr)
plt.show()
print lr

print "MSE on train: " + str(lr.mse(Xtr, Ytr))
print "MSE on test: " + str(lr.mse(Xte, Yte))

# part 3
ks = [1,3,5,7,10,18]
errTrainTr = []
errTrainTe = []
lrData = []
for i,k in enumerate(ks):
	XtrP = ml.transforms.fpoly(Xtr, k, bias=False)
	XtrP,params = ml.transforms.rescale(XtrP)
	lr = ml.linear.linearRegress(XtrP, Ytr)
	lrData.append(lr.theta)
	Phi = lambda X : ml.transforms.rescale(ml.transforms.fpoly(X, k, False), params)[0]

	YhatTrain = lr.predict(Phi(xs))
	errTrainTr.append(np.mean(YhatTrain - Ytr) ** 2)
	YhatTest = lr.predict(Phi(Xte))
	errTrainTe.append(np.mean(YhatTest - Yte) ** 2)


	plt.scatter(Xtr, Ytr)
	plt.plot(xs, YhatTrain)
	plt.show()

print errTrainTr
print errTrainTe
plt.semilogy(errTrainTr, color='r')
plt.semilogy(errTrainTe, color='g')
plt.show()
