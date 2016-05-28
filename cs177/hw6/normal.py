#!/usr/bin/python2.7
from random import random
import numpy as np
import matplotlib.pyplot as plt

l = [ random() for _ in range(50001) ]
plt.hist(l, bins=np.linspace(0,1,1000))
plt.show()

# this is not approximately normal. This is
# because the docs actually state that it uses
# a completely deterministic method called
# Mersenne Twister, which is not approximately
# normal. 

# Randrange is implemented very similarly. The only
# way to get a uniformly randomly distrbution in 
# python is too use the guass method, and that's
# not really exciting.
