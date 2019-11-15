module Prime where

import Random (randomIntegerModulo)
import Operations (powerModulo)

checkIsPrimeWitness :: Integer -> Integer -> Bool
checkIsPrimeWitness candidate witness = np1modp == 1 where
    np1modp = powerModulo witness (candidate-1) candidate

checkIsPrime :: Integer -> Integer -> IO Bool
checkIsPrime candidate 0 = do
    witness <- randomIntegerModulo (candidate-1)
    return (checkIsPrimeWitness candidate (witness+1))
checkIsPrime candidate attempts = do
    if (even candidate) then
        return False
    else do
        witness <- randomIntegerModulo (candidate-1)
        if (checkIsPrimeWitness candidate (witness+1)) then do
            isPrime <- checkIsPrime candidate (attempts - 1)
            return isPrime
        else
            return False


generatePrimeDigits :: Integer -> IO Integer
generatePrimeDigits digits = do
    let firstPossible = 10 ^ (digits-1)
    let lastPossible = (10 ^ digits) - 1
    random <- randomIntegerModulo (lastPossible - firstPossible)
    let candidate = random + firstPossible
    isPrime <- checkIsPrime candidate 100
    if (isPrime) then
        return candidate
    else
        generatePrimeDigits digits

