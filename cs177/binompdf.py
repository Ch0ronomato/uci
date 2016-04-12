import numpy as np
import matplotlib.pyplot as plt
class Memory:
	def __init__(self):
		self.data = {}

	def inmemory(self, k):
		return self.data.has_key(k)

	def remember(self, k, x):
		self.data[k] = x
		return x

	def recall(self, k):
		return self.data[k]

mem = Memory()

def factorial(k):
	if (k == 0 or k == 1):
		return 1;
	if (mem.inmemory(k)):
		return mem.recall(k)
	else:
		return mem.remember(k, k * factorial(k - 1))

def nchoosek(n, k):
	# N choose K = 
	#		n!
	# --------------
	#  k! * (n - k)!
	nfac = factorial(n)
	kfac = factorial(k)
	nminkfac = factorial(n - k) # will certainly be cached.
	return nfac / (kfac * nminkfac)

def binomialapprox(n, i, p):
	nbyp = n * p
	denom = 2. * np.pi * nbyp * (1. - p)

	# get 'e's power
	x = pow((i - nbyp), 2) / (2.*nbyp*(1. - p))
	return (1. / denom) * pow(np.e, -x)

def binomialexact(n, i, p):
	return nchoosek(n, i) * pow(p, i) * pow((1 - p), n - i)

def binomialdist(n, p):
	n = n * 1.
	p = p * 1.
	if (n > 100):
		return np.array( [ binomialapprox(n, i, p) for i in range(0, int(n + 1)) ] )
	return np.array([ binomialexact(n, i, p) for i in range(0, int(n+1))])

def binomialcdf(n, p):
	toplot = binomialdist(n, p)
	sums=[]
	currentsum=0.0
	for i,x in enumerate(toplot):
		currentsum = currentsum + x
		sums.append(currentsum)

	return sums

fig,ax = plt.subplots(2, 2)
ax[0][0].bar(range(0, 26), binomialdist(25, .1))
ax[0][0].set_title("n = 25, p = .1")
ax[0][1].bar(range(0, 26), binomialdist(25, .75))
ax[0][1].set_title("n = 25, p = .75")
ax[1][0].bar(range(0, 1001), binomialdist(1000, .5))
ax[1][0].set_title("n = 1000, p = .5")
ax[1][1].bar(range(0, 1001), binomialdist(1000, .9))
ax[1][1].set_title("n = 1000, p = .9")
plt.gcf().canvas.set_window_title('Binomial pmf')
plt.show()

fig,ax = plt.subplots(2, 2)
ax[0][0].bar(range(0, 26), binomialcdf(25, .1))
ax[0][0].set_title("n = 25, p = .1")
ax[0][1].bar(range(0, 26), binomialcdf(25, .75))
ax[0][1].set_title("n = 25, p = .75")
ax[1][0].bar(range(0, 1001), binomialcdf(1000, .5))
ax[1][0].set_title("n = 1000, p = .5")
ax[1][1].bar(range(0, 1001), binomialcdf(1000, .9))
ax[1][1].set_title("n = 1000, p = .9")
plt.gcf().canvas.set_window_title('Binomial cdf')
plt.show()


print "Entering problem 9 answers"
print "X ~ binom(10000, .1). P[X = 1000] = ",binomialapprox(10000, 1000, .1) 
print "X ~ binom(10000, .1). P[X >= 1000] = 1 - P[X < 1000] = ",(1. - binomialcdf(10000, .1)[1000])
print "X ~ binom(10000, .1). P[X >= 1100] = 1 - P[X < 1100] = ",(1. - binomialcdf(10000, .1)[1000]) 
print "X ~ binom(10000, .1). P[X <= 900] = ",binomialapprox(10000, 901, .1)

# Find the largest possible value such that P[X >= 500] < .0000002
# for loop over all possible P's from 0 - 1
# print first one that is less than < .0000002
delta = 2 * pow(10, -7)
for i in range(1, 100):
	p = (i*1.)/100.0
	if (binomialcdf(10000, p)[500] <= delta):
		print "The minimum p = ", i
		break