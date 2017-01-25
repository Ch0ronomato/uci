from tokenizer import Tokenizer 
import os.path 
import operator # used to reverse dict

def tokenize(fname):
    tok = Tokenizer()
    tok.tokenize(fname)
    frequencies = sorted(tok.computeFrequencies().items(), key=operator.itemgetter(1), reverse=True)
    printTokens(frequencies)

def printTokens(tokens):
    for token in tokens:
        print token[0],"=",token[1]

fname=raw_input("Path to file")
if (not os.path.isfile(fname)):
    print "File does not exist, exiting"
else:
    tokenize(fname)
