-- function that uses a higher order function
largest [] = error "Empty list handed to largest"
largest [hd] = hd
largest (hd : xs) = 
	foldl (\x y -> if x > y then x else y) hd xs