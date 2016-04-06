import mltools as ml
import numpy as np
import matplotlib.pyplot as plt
from mltools import dtree

# load and split the data
X = np.genfromtxt("kaggle.X1.train.txt", delimiter=",")
Y = np.genfromtxt("kaggle.Y.train.txt", delimiter=",")
X,Y = ml.shuffleData(X,Y)
Xtr,Xte,Ytr,Yte = ml.splitData(X, Y, train_fraction=0.75)

# get a decision tree
dt = dtree.treeRegress(Xtr, Ytr, maxDepth=20)
print "done"