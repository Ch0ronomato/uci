class Tokenizer:
    def __init__(self):
        self.files=[]

    def addFile(self, fname):
        self.files.append(fname)

    def _stripWord(self, word):
        '''
        Gets the first set of alphanumeric characters. Returns false if none found
        '''
        words=[]
        currentword=""
        for ch in word:
            if (not ch.isalnum()):
                if (not currentword == ""):
                    words.append(currentword)
                    currentword = ""
            else:
                currentword = currentword + ch
        if (not currentword == ""):
            words.append(currentword)

        return words
    def _tokenizeFile(self, f):
        tokenizing={}
        for line in f:
            for word in line.split(" "):
                tokens = self._stripWord(word)
                for token in tokens:
                    if (tokenizing.has_key(token) == False):
                        tokenizing[token] = 1
                    else:
                        tokenizing[token] = tokenizing[token] + 1
        return tokenizing

    def tokenize(self):
        for i in range(len(self.files)):
            print "Tokenizing file",self.files[i]
            print self._tokenizeFile(open(self.files[i]))

tok = Tokenizer()
tok.addFile("test.txt")
tok.addFile("bigtest.txt")
tok.tokenize()
