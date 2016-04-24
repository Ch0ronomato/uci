import numpy as np
import matplotlib.pyplot as plt

# data: numpy.array
def analyze_session_data(data):
	longest = data.shape[0]
	shortest = data.argmax() + 1
	print "1.) Longest =",longest,"Shortest =",shortest

	# Most common session length
	print "2.) Most common session was a session of length",data.argmax() + 1,"with",data[data.argmax()]

	# total number of sessions
	totalsessions = np.sum(data)
	print "3.) Total number of sessions",totalsessions

	# total number of pages requested.
	# each value multiplied by it's position.
	totalrequests = np.sum([data[i] * (i + 1) for i in range(data.shape[0])])
	print "4.) Total number of requests", totalrequests

	# probabilities of session lengths of 1-6
	print "5.) probabilities of session lengths 1,2,3,4,5,6"
	empericalmodel=[]
	for i in range(1, 7):
		empericalmodel.append(data[i]/totalsessions)
		print "    p("+str(i)+")="+str(np.around(data[i]/totalsessions, decimals=3))

	# Emperical mean
	print "6.) E[X] =",np.mean(data)
	return empericalmodel

def plot_session_model(data, empericalmodel):
	# guess p based our mean
	p = 1. / np.mean(data)
	keys = np.arange(1, 50, .5)
	data=[ pow(p*(1-p), i - 1) for i in keys]
	plt.semilogy(data, color='r')

	# power law distribution
	Z = 6/pow(np.pi,2)
	plt.loglog([Z * pow(1./i, 2) for i in keys], color='g')
	plt.plot(empericalmodel, "b-")
	plt.show()

if __name__ == '__main__':
	data = np.genfromtxt("sessionlengths.txt")
	print "========Problem 7========"
	emperical=analyze_session_data(data)
	print "========Problem 8========"
	plot_session_model(data, emperical)