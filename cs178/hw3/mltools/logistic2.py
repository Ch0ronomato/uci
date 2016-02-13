import numpy as np

from .base import classifier
from .base import regressor
from .utils import toIndex, fromIndex, to1ofK, from1ofK
from numpy import asarray as arr
from numpy import atleast_2d as twod
from numpy import asmatrix as mat
import matplotlib.pyplot as plt


################################################################################
## LOGISTIC REGRESSION CLASSIFIER ##############################################
################################################################################


class logisticClassify2(classifier):
    """A binary (2-class) logistic regression classifier

    Attributes:
        classes : a list of the possible class labels
        theta   : linear parameters of the classifier 
                  (1xN numpy array, where N=# features)
    """

    def __init__(self, *args, **kwargs):
        """
        Constructor for logisticClassify2 object.  

        Parameters: Same as "train" function; calls "train" if available

        Properties:
           classes : list of identifiers for each class
           theta   : linear coefficients of the classifier; numpy array 
                      shape (1,N) for binary classification or (C,N) for C classes
        """
        self.classes = []
        self.theta = np.array([])

        if len(args) or len(kwargs):      # if we were given optional arguments,
            self.train(*args,**kwargs)    #  just pass them through to "train"


    def __repr__(self):
        str_rep = 'logisticClassify2 model, {} features\n{}'.format(
                   len(self.theta), self.theta)
        return str_rep


    def __str__(self):
        str_rep = 'logisticClassify2 model, {} features\n{}'.format(
                   len(self.theta), self.theta)
        return str_rep


## CORE METHODS ################################################################

    def plotBoundary(self,X,Y,axis=None):
        """ Plot the (linear) decision boundary of the classifier, along with data """
        if axis==None: axis=plt
        colors = ['b', 'g', 'r']
        for i in range(1, X.shape[1]):
            for c in np.unique(Y):
                axis.plot( X[Y==c, 0], X[Y==c, i], 'o', color=colors[int(c)])
        xs = np.linspace(min(X[:,0]), max(X[:,0]), 200)
        ys = ((self.theta[0][1] * -1) * xs) - self.theta[0][0]
        ys = ys / self.theta[0][2]
        axis.plot(xs, ys, 'r-')

    def predictSoft(self, X):
        """ Return the probability of each class under logistic regression """
        raise NotImplementedError
        ## You do not need to implement this function.
        ## If you *want* to, it should return an Mx2 numpy array "P", with 
        ## P[:,1] = probability of class 1 = sigma( theta*X )
        ## P[:,0] = 1 - P[:,1] = probability of class 0 
        return P

    def predict(self, X):
        """ Return the predictied class of each data point in X"""
        z=np.zeros(X.shape[0])
        Yhat = np.zeros(X.shape[0])
        X1  = np.hstack((np.ones((X.shape[0],1)),X))
        for i in range(X.shape[0]):
            Yhat[i] = (self.classes[1] if np.dot(self.theta[0],X1[i]) > 0 else self.classes[0])

        return Yhat


    def train(self, X, Y, initStep=1.0, stopTol=1e-4, stopIter=5000, plot=None, alpha=0.0):
        """ Train the logistic regression using stochastic gradient descent """
        ## First do some bookkeeping and setup:
        initStep = float(initStep)
        self.theta,X,Y = twod(self.theta), arr(X), arr(Y)   # convert to numpy arrays
        M,N = X.shape
        if Y.shape[0] != M:
            raise ValueError("Y must have the same number of data (rows) as X")
        self.classes = np.unique(Y)
        if len(self.classes) != 2:
            raise ValueError("Y should have exactly two classes (binary problem expected)")
        if self.theta.shape[1] != N+1:         # if self.theta is empty, initialize it!
            self.theta = np.random.randn(1,N+1)
        # Some useful modifications of the data matrices:
        X1  = np.hstack((np.ones((M,1)),X))    # make data array with constant feature
        Y01 = toIndex(Y, self.classes)         # convert Y to canonical "0 vs 1" classes
        SIs = np.zeros(M)
        it   = 0
        done = False
        Jsur = []
        J01  = []
        while not done:
            step = (2.0 * initStep) / (2.0 + it)   # common 1/iter step size change

            for i in range(M):  # for each data point i:
                zi = np.dot(self.theta[0], X1[i])
                yi = self.classes[0] if zi > 0 else self.classes[1]
                Y01[i] = yi
                si = 1 / (1 + np.exp(-zi))
                SIs[i] = si
                gradi = -(Y01[i] * (1 - si) * X1[i]) + ((1 - Y01[i]) * si * X1[i]) + (alpha * np.sum(self.theta))
                self.theta = self.theta - step * gradi

            # each pass, compute surrogate loss & error rates:
            J01.append( self.err(X,Y) )
            accum=[]
            for i in range(M):
                si = (1 + np.exp(-np.dot(self.theta[0], X1[i]))) ** -1
                accum.append((-np.log(si)) if Y01[i] == self.classes[1] else (-np.log(1 - si)))
            Jsur.append( np.mean(accum) + (alpha * (np.sum(self.theta)**2))) 
           
            ## For debugging: print current parameters & losses
            # raw_input()   # pause for keystroke

            # check stopping criteria:
            it += 1
            done = (it > stopIter) or ( (it>1) and (abs(Jsur[-1]-Jsur[-2])<stopTol) )

        # plot jsur vs j01
        if (plot != None):
            plot.semilogx(Jsur, 'r-', J01, 'g-')

################################################################################
################################################################################
################################################################################

