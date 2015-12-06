center :: [a] -> Int -> a -> [a]
center xs width fill
    | width == (length xs) = xs
    | width == ((length xs) + 1) = [fill] ++ xs
    | otherwise = center ((fill:xs) ++ [fill]) width fill