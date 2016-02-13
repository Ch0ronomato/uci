import matplotlib.pyplot as plt
import numpy as np

thetas=np.linspace(2, 36)
l=[]
q=[]
for _,a in enumerate(thetas):
	for _,b in enumerate(thetas):
		for _,c in enumerate(thetas):
			l.append(a + 3**2)
plt.plot(l,'r-')
plt.show()
