P(Y) = 4/10
P(!Y) = 6/10
-------------------------
P(X=know author|Y) = 3/4
P(X=!know author|Y) = 1/4
P(X=know author|!Y) = 3/6
P(X=!know author|!Y) = 3/6
-------------------------
P(X=is long|Y) = 0/4
P(X=!is long|Y) = 4/4
P(X=is long|!Y) = 5/6
P(X=!is long|!Y) = 1/6
-------------------------
P(X=has research|Y) = 3/4
P(X=!has research|Y) = 1/4
P(X=has research|!Y) = 4/6
P(X=!has research|!Y) = 2/6
-------------------------
P(X=has grade|Y) = 2/4
P(X=!has grade|Y) = 2/4
P(X=has grade|!Y) = 5/6
P(X=!has grade|!Y) = 1/6
-------------------------
P(X=has lotter|Y) = 1/4
P(X=!has lotter|Y) = 3/4
P(X=has lotter|!Y) = 2/6
P(X=!has lotter|!Y) = 4/6

b) max { .4 * .25 * .25 * .5 * .75,
         .6 * .5 * .167 * .33 * .167 * .667
        }
    (0 0 0 0 0) = +1
    (1 1 0 1 0) = -1

c) P(+1|(1 1 0 1 0)) = P((1 1 0 1 0)|+1) * P(+1) / (P(author|+1) + P(long|+1) + P(!research|+1) + P(grade|+1) + P(!lottery|+1) + P(!author|+1) + P(!long|+1) + P(research|+1) + P(!grade|+1) + P(lottery|+1))
	= .01125


((.75 + .5 + .5 + .75) + (.25 + 1 + .5 + .5 + .25)) = 5
