module RSA where

import Prime (generatePrimeDigits)
import Operations (digitCount, inverseModulo, powerModulo)
import Random (randomIntegerModulo)

newtype RSAModulus = Modulus Integer
    deriving (Show)

newtype PublicExponent = EncExponent Integer
    deriving (Show)

publicExpValue :: PublicExponent -> Integer
publicExpValue (EncExponent a) = a

newtype PrivateExponent = DecExponent Integer
    deriving (Show)

data RSAPublicKey = PublicKey RSAModulus PublicExponent
    deriving (Show)

data RSAPrivateKey = PrivateKey RSAModulus PrivateExponent
    deriving (Show)

data RSAKey = KeyPair RSAPublicKey RSAPrivateKey
    deriving (Show)

newtype PlainTextBlock = PlainBlock Integer
    deriving (Show)

type PlainText = [PlainTextBlock]

newtype CipherTextBlock = CipherBlock Integer
    deriving (Show)

type CipherText = [CipherTextBlock]

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
    let digits = digitCount (2 ^ (div sizeBits 2))
    keySizeDelta <- randomIntegerModulo 5
    return (digits - keySizeDelta, digits + keySizeDelta)

getRandomExp :: Integer -> IO PublicExponent
getRandomExp carmichael = do
    randomExp <- randomIntegerModulo (min carmichael 65537)
    let egcd = gcd randomExp carmichael
    if (egcd == 1) then do
        return (EncExponent randomExp)
    else do
        newRandomExp <- getRandomExp carmichael
        return newRandomExp

keyFromPrimes :: Integer -> Integer -> IO RSAKey
keyFromPrimes p q = do
    let modulusN = Modulus (p*q)
    let carmichael = lcm (p-1) (q-1)
    encriptionExp <- getRandomExp carmichael
    let decriptionExp = inverseModulo (publicExpValue encriptionExp) carmichael
    return (KeyPair (PublicKey modulusN encriptionExp) (PrivateKey modulusN (DecExponent decriptionExp)))

encrypt :: PlainText -> RSAPublicKey -> CipherText
encrypt plainText (PublicKey (Modulus modulus) (EncExponent exponent)) = 
    map (\(PlainBlock block) -> CipherBlock (powerModulo block exponent modulus)) plainText

decrypt :: CipherText -> RSAPrivateKey -> PlainText
decrypt cipherText (PrivateKey (Modulus modulus) (DecExponent exponent)) = 
    map (\(CipherBlock block) -> PlainBlock (powerModulo block exponent modulus)) cipherText