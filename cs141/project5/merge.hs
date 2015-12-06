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