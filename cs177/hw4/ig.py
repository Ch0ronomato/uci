from temp import p
import numpy as np
from math import log
	a
	b
def tt(x,c):
	pxc = np.array([p(x,y,c) for y in [0,1]])
	px = np.array([p(x,y,a) for y in [0,1] for a in [0,1] ])
	pc = np.array([p(a,y,c) for y in [0, 1] for a in [0, 1] ])
	return np.sum(pxc) * log( np.sum(pxc) / (np.sum(px * pc)) , 2)

def t(y, c):
	pyc = np.array([p(x,y,c) for x in [0,1]])
	py = np.array([p(x,y,a) for x in [0,1] for a in [0,1] ])
	pc = np.array([p(x,a,c) for x in [0, 1] for a in [0, 1] ])
	return np.sum(pyc) * log( np.sum(pyc) / (np.sum(py * pc)) , 2)

print np.sum([tt(x,c) for x in [0, 1] for c in [0 ,1]])
print np.sum([t(y,c) for y in [0, 1] for c in [0,1]])
