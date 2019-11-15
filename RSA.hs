module RSA where

import Prime (generatePrimeDigits)
import Operations (digitCount, mlcm, mgcd)
import Random (randomIntegerModulo)

data Modulus = ModulusN Integer
    deriving (Show)

data EncryptionExponent = PublicExp Integer
    deriving (Show)

data DecriptionExponent = PrivateExp Integer
    deriving (Show)

data RSAPublicKey = PublicKey Modulus EncryptionExponent
    deriving (Show)

data RSAPrivateKey = PrivateKey DecriptionExponent
    deriving (Show)

data RSAKey = KeyPair RSAPublicKey RSAPrivateKey
    deriving (Show)

public :: RSAKey -> RSAPublicKey
public (KeyPair pub _) = pub

private :: RSAKey -> RSAPrivateKey
private (KeyPair _ priv) = priv

generateKeySizeBits :: Integer -> IO RSAKey
generateKeySizeBits keySize = do
    primeSize <- primeLengthFromKeySize keySize
    p <- (generatePrimeDigits . fst) primeSize
    q <- (generatePrimeDigits . snd) primeSize
    if (p == q) then do
        key <- generateKeySizeBits keySize
        return key
    else do
        key <- keyFromPrimes p q
        return key

primeLengthFromKeySize :: Integer -> IO (Integer, Integer)
primeLengthFromKeySize sizeBits = do
    let digits = digitCount (2 ^ (sizeBits - 1))
    keySizeDelta <- randomIntegerModulo 5
    return (digits - keySizeDelta, digits + keySizeDelta)

getRandomExp :: Integer -> IO EncryptionExponent
getRandomExp carmichael = do
    randomExp <- randomIntegerModulo carmichael
    let egcd = mgcd randomExp carmichael
    if (egcd == 1) then do
        return (PublicExp randomExp)
    else do
        newRandomExp <- getRandomExp carmichael
        return newRandomExp

keyFromPrimes :: Integer -> Integer -> IO RSAKey
keyFromPrimes p q = do
    let modulusN = ModulusN (p*q)
    let carmichael = mlcm (p-1) (q-1)
    encriptionExp <- getRandomExp carmichael
    return (KeyPair (PublicKey modulusN encriptionExp) (PrivateKey (PrivateExp 100)))



