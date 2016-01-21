import numpy as np
import matplotlib as plt
import mltools as ml
import mltools.linear

X1 = np.genfromtxt("data/kaggle.X1.train.txt", delimiter=",")
Y = np.genfromtxt("data/kaggle.Y.train.txt", delimiter=",")
print X1.shape, Y.shape

Xe1 = np.genfromtxt("data/kaggle.X1.test.txt", delimiter=",")

lr = ml.linear.linearRegress(X1, Y)
print lr.mse(X1, Y)
Y2 = lr.predict(Xe1)
np.savetxt("output.csv", Y2, delimiter=',', header="Predictions")
print Y2
