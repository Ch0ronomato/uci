import math
i = 2
# when is n^2 > 10nlogn
while i > 1:
	if (i**2) > (10 * i * (math.log(i, 2))):
		print i;
		break;
	i += 1

#when is nlogn = 10^6
while i > 1:
	if i * math.log(i, 2) > 10**6:
		print str(i - 1) + " < x < " + str(i)
		break
	elif i * math.log(i, 2) == 10**6:
		print "x == " + str(i)
		break
	else:
		i += 1
