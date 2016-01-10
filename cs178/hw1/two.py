import numpy as np
import matplotlib.pyplot as plt
import mltools as ml

iris = np.genfromtxt("data/iris.txt", delimiter=None)
Y = iris[:,-1]
X = iris[:,0:2]
X,Y = ml.shuffleData(X,Y)
Xtr,Xte,Ytr,Yte = ml.splitData(X, Y, 0.75)

def partA(Xtr, Xte, Ytr, Yte):
	knn = ml.knn.knnClassify()
	# varying values of K
	for k in [1, 5, 10, 50]:
		knn.train(Xtr, Ytr, K=k)
		ml.plotClassify2D(knn, Xtr, Ytr, axis=plt)
		print "#A: Plot of K=" + str(k) 
		plt.show()

# computing error values of predictions
def partB(Xtr, Xte, Ytr, Yte):
	ks = [1,2,5,10,50,100,200]
	errTrainTr = []
	errTrainTe = []
	for i,k in enumerate(ks):
		learner = ml.knn.knnClassify(Xtr, Ytr, K=k)
		Yhattr = learner.predict(Xtr)
		errTrainTr.append(np.mean(np.transpose(Yhattr) - Ytr))
		Yhatte = learner.predict(Xte)
		errTrainTe.append(np.mean(np.transpose(Yhatte) - Yte))
	plt.semilogx(errTrainTr, color='r')
	plt.semilogx(errTrainTe, color='g')
	print "#B: Semilog plot of error"
	plt.show()
	print "What we see is that the kNN learner really needs a small amount of K to avoid fitness problems."
	print "What you can see from this graph is that the 2nd and 3rd values are in the optimal area of fit."
	print "This would mean training our data with K=2, K=5 would be optimal."
partA(Xtr, Xte, Ytr, Yte)
partB(Xtr, Xte, Ytr, Yte)
