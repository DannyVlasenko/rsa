module Random where

import Data.Time.Clock.POSIX (getPOSIXTime)    

data Seed = ABXSeed Integer Integer Integer 
    deriving (Show)

propagateSeed :: RealFrac real => real -> Seed
propagateSeed value = ABXSeed a b x 
    where
        fracValue = realToFrac value
        a = (ceiling . (\a->a * fracValue + fracValue) . cos) fracValue
        b = (ceiling . (\a->a * fracValue + fracValue) . sin) fracValue
        x = ceiling (fracValue ^ 100)

randomIntegerSeedModulo :: Seed -> Integer -> Integer
randomIntegerSeedModulo (ABXSeed a b x) modulo = mod (a * x + b) modulo

randomIntegerModulo :: Integer -> IO Integer
randomIntegerModulo modulo = do 
    time <- getPOSIXTime
    let seed = propagateSeed time
    return (randomIntegerSeedModulo seed modulo)