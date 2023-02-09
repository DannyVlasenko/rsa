module Operations where

data BezoutCoefficients = BezoutXY Integer Integer
    deriving(Show)

bezoutX :: BezoutCoefficients -> Integer
bezoutX (BezoutXY x _) = x

bezoutY :: BezoutCoefficients -> Integer
bezoutY (BezoutXY _ y) = y

extendedGcd :: Integer -> Integer -> (Integer, BezoutCoefficients)
extendedGcd a 0 = (a, (BezoutXY 1 0))
extendedGcd a b = (d, (BezoutXY y (x - y * (div a b)))) where
    egcd = extendedGcd b (mod a b)
    d = fst egcd
    x = (bezoutX . snd) egcd
    y = (bezoutY . snd) egcd

inverseModulo :: Integer -> Integer -> Integer
inverseModulo a m = (positiveInverse . bezoutX . snd . extendedGcd a) m where
    positiveInverse i =
        if (i < 0) then
            m + i
        else
            i

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