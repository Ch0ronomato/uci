insert :: Ord a => [a] -> a -> [a]
insert [] x = [x]
insert (l : ls) x 
    | x <= l = (x:l:ls)
    | otherwise = l:(insert ls x)

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