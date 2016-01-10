import numpy as np
import matplotlib.pyplot as plt

iris = np.genfromtxt("data/iris.txt", delimiter=None)
Y = iris[:, -1]
X = iris[:,0:-1]
feats = X.shape[1]
dps = X.shape[0]
means=[]
stds=[]
var=[]
zscore=[]
print feats 
print dps

for i in range(0, feats):
	feature=X[:,i]
	plt.hist(X[:,i])
	plt.show()
	means.append(np.mean(feature))
	var.append(np.var(feature))
	stds.append(np.std(feature))
	zscore.append([((x - means[i]) / stds[i]) for x in feature])

print means
print var
print stds
for i in range(0, feats - 1):
	for j in range(i+1, feats):
		plt.scatter(zscore[i], zscore[j], c=zscore[0])
		plt.show()