-- Ian Schweer
-- 22514022
{-Write a Haskell function called insert that takes 
two parameters (a list and a number). You may assume 
that the list consists of 0 or more numbers that are 
in order. The function should return a list that would 
result from inserting the number into the list into 
the appropriate place to keep the list in order.-}
-- ==============================================
insert :: Ord a => [a] -> a -> [a]
insert [] x = [x]
insert (l : ls) x 
    | x <= l = (x:l:ls)
    | otherwise = l:(insert ls x)

{-Write a polymorphic Haskell function called 
insertSort that takes one parameter 
(a list of elements). The function should 
return the list that would result from sorting 
the list using insertion sort.-}
-- ==============================================
-- insertion sort using the insert method
-- hidden method insertion sort.
insertionSort :: Ord a => [a] -> [a] -> [a]
insertionSort (x:xs) [] = insertionSort xs [x]
insertionSort [] ys = ys
insertionSort (x:xs) ys = insertionSort xs (insert ys x)

-- callable api for insert sort
insertSort :: Ord a => [a] -> [a]
insertSort [] = []
insertSort xs = insertionSort xs []


{-Write a function merge :: (Ord a) => [[a]] -> [a] 
that takes a finite list of sorted finite lists and 
merges them into a single sorted list. A “sorted list” 
means a list sorted in increasing order (using <); you 
may assume that the sorted lists are finite.-}
-- ===================================================
-- merge utility
merge_ :: Ord a => [a] -> [a] -> [a]
merge_ [] [] = []
merge_ [] ys = ys
merge_ xs [] = xs 
merge_ (x:xs) (y:ys) 
   | x <= y = [x] ++ merge_ xs (y:ys)
   | otherwise = [y] ++ merge_ (x:xs) ys

-- merge
merge :: Ord a => [[a]] -> [a]
merge [[]] = []
merge [xs] = xs
merge (x:xs) = merge_ x (merge xs)

{-Write and test the definition of a (polymorphic) 
Haskell function 'center' that takes three arguments, 
a list arg1 of type [a], a width arg2 of type Int, and 
a fill item arg3 of type a, and returns a list of 
length arg2 of type [a] containing list arg1 centered
within fill items (i.e., the difference between the 
number of items preceding arg1 and those following 
arg1 is at most 1).-}
center :: [a] -> Int -> a -> [a]
center _ 0 _ = error("Arg 1 must be nonzero")
center xs width fill
    | width < length xs = error("Arg smaller then string")
    | width == (length xs) = xs
    | width == ((length xs) + 1) = [fill] ++ xs
    | otherwise = center ((fill:xs) ++ [fill]) width fill

{-Write and test the definition of a Haskell function 
'largest', which finds the largest element of a list, 
but is implemented using higher-order functions 
and/or operator sections as appropriate.-}
-- function that uses a higher order function
largest [] = error "Empty list handed to largest"
largest [hd] = hd
largest (hd : xs) = 
  foldl (\x y -> if x > y then x else y) hd xs