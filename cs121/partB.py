from tokenizer import Tokenizer
import os.path
import timeit
def intersect(fname1, fname2):
    tok = Tokenizer()
    tok2 = Tokenizer()
    tok.tokenize(fname1)
    tok2.tokenize(fname2)
    largest = tok.computeFrequencies()
    smallest = tok.computeFrequencies()

    if (len(largest) < len(smallest)):
        t = largest
        largest = smallest
        smallest = t
    intersect=[]
    count = 0
    for key,value in smallest.iteritems():
        if (largest.has_key(key)):
            intersect.append(key)
        count = count + 1
    print intersect
    print len(intersect)


fname1=raw_input("Path to file1: ")
fname2=raw_input("Path to file2: ")
if (not os.path.isfile(fname1) or not os.path.isfile(fname2)):
    print "File does not exist, exiting"
else:
    intersect(fname1, fname2)
