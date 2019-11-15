module Operations where

mdiv :: Integer -> Integer -> (Integer, Integer)
mdiv x y = divAccum x y 0

divAccum :: Integer -> Integer -> Integer -> (Integer, Integer)
divAccum 0 _ a = (a, 0)
divAccum x y a = if x >= y then divAccum (x-y) y (a+1) else (a, x)

pow :: Integer -> Integer -> Integer
pow _ 0 = 1
pow 0 _ = 0
pow 1 _ = 1
pow a 1 = a
pow a b = pow (a*a) div * (pow a rem) where
    div = fst wholeDiv
    rem = snd wholeDiv
    wholeDiv = mdiv b 2

mgcd :: Integer -> Integer -> Integer
mgcd a 0 = a
mgcd 0 b = b
mgcd _ 1 = 1
mgcd 1 _ = 1
mgcd a b 
    | a==b = a
    | otherwise = if a > b then gcd (a-b) b else gcd a (b-a)

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