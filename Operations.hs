module Operations where

mgcd :: Integer -> Integer -> Integer
mgcd a 0 = a
mgcd 0 b = b
mgcd _ 1 = 1
mgcd 1 _ = 1
mgcd a b 
    | a==b = a
    | otherwise = if a > b then gcd (a-b) b else gcd a (b-a)

mlcm :: Integer -> Integer -> Integer
mlcm 0 _ = 0
mlcm _ 0 = 0
mlcm a b = div (a * b) (gcd a b)

powerModulo :: Integer -> Integer -> Integer -> Integer
powerModulo _ _ 1 = 0
powerModulo _ 0 _ = 1
powerModulo base 1 modulo = mod base modulo
powerModulo base exp modulo = 
    if (odd exp) then
        mod (base * (powerModulo base (exp-1) modulo)) modulo
    else        
        mod (d*d) modulo where
            d = powerModulo base (div exp 2) modulo

digitCount :: Integer -> Integer
digitCount = count 1 . abs where
    count ds n =
        if (n >= 10) then 
            count (ds + 1) (div n 10)
        else
            ds