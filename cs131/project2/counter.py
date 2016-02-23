f = open("timing_4.txt")
i = 0
for line in f:
	if (line.strip() == ""):
		i+=1

print i
