import matplotlib.pyplot as plt
import mltools as ml
import numpy as np
import mltools.linear

iris = np.genfromtxt("data/iris.txt",delimiter=None)
X, Y = iris[:,0:2], iris[:,-1] # get first two features & target
X,Y = ml.shuffleData(X,Y) # reorder randomly (important later)
X,_ = ml.transforms.rescale(X) # works much better on rescaled data
XA, YA = X[Y<2,:], Y[Y<2] # get class 0 vs 1
XB, YB = X[Y>0,:], Y[Y>0] # get class 1 vs 2

# ensure seperability
fig, ax = plt.subplots(1,2)
ml.plotClassify2D(None, XA[:,[0,1]],YA,axis=ax[0])
ml.plotClassify2D(None, XB[:,[0,1]],YB,axis=ax[1])


plt.show()