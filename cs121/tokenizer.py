import operator # used to reverse dict
import threading
from collections import defaultdict as ddict
from itertools import islice
class Tokenizer:
    def __init__(self):
        self.file=False
        self.flock=threading.Lock()
        self.chunksize=500
        self.threads=[]
        self.lock = threading.Lock()
        self.res=[ddict(int), ddict(int), ddict(int), ddict(int)]
        self.flushamount = 5*100*100
        self.freqs={}
    
    def _getlines(self):
        while True:
            with self.flock:
                items=list(islice(self.file, self.chunksize))
                if not items:
                    break
                yield items

    def _stripWord(self, word):
        '''
	Splits one word into a set of alphanumeric groups. O(n), where n=size(word), assuming append is O(1) time
        '''
        currentword=""
        me=int(threading.current_thread().name)
        for ch in word:
            if (not ch.isalnum()):
                if (not currentword == ""):
                    self.res[me][currentword.lower()] += 1
                    currentword = ""
            else:
                currentword = currentword + ch

        if (not currentword == ""):
            self.res[me][currentword.lower()] += 1
        len(self.res[me])

    def tokenizeThread(self):
        me=int(threading.current_thread().name)
        for l in self._getlines():
            for x in l: 
                self._stripWord(x)

    def tokenize(self, fname):
	'''
	Tokenizes file, and counts the frequency of each token. O(n*m) time, where
		n=average( length( word in words ) )
		m=length( words )
	assuming has_key is O(1)
	'''
        tokenizing=[]
	self.file = open(fname)
        for i in range(4):
            self.threads.append(threading.Thread(target=self.tokenizeThread, name=str(i)))
            self.threads[i].start()
        for i in range(4):
            self.threads[i].join()

        return self.res

    def computeFrequencies(self):
        for i in range(1, 4):
            for k,v in self.res[i].iteritems():
                self.res[0][k] += v
        return self.res[0]
