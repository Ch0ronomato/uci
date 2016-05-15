
def p(x,y,c):
	if not x and not y and not c:
		return .1
	elif not x and y and not c:
		return .2
	elif x and not y and not c:
		return .2
	elif x and y and not c:
		return .1
	elif not x and not y and c:
		return .25
	elif not x and y and c:
		return .1
	elif x and not y and c:
		return .05
	elif x and y and c:
		return .0	
