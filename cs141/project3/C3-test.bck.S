set 2, read
set 1, 3
set 1, D[1] + 1
set 0, D[1]
set D[1], 23
set D[1] + 1, 0
set 0, D[1]
set 1, (D[1] + 1) + 2
set D[1]-1, D[2]
set 2, D[2] - 1
jumpt 15, D[D[1] - 1] != 1
set D[0] - 1, 1 
set 1, D[0]
set 0, D[D[0] + 1]
jump D[D[1]]
set 1, D[1] + 1
set D[1], 21
set D[1] + 1, D[0]
set 0, D[1]
set 1, (D[1] + 1) + 2
jump 8
set D[1] - 1, D[D[1] - 1] + D[D[1] - 1]
set D[1] - 1, D[D[1] - 1] + 1 
halt
