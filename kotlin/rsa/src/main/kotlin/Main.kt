import java.util.*
import kotlin.math.abs
import kotlin.math.log10
import kotlin.math.pow

fun ULong.powerModulo(exp: ULong, modulo: ULong): ULong {
    return when{
        modulo == 0uL -> 0uL
        exp == 0uL -> 1uL
        exp == 1uL -> this % modulo
        exp % 2uL != 0uL -> (this * (this.powerModulo(exp-1uL, modulo))) % modulo
        else -> {
            val d = this.powerModulo(exp / 2uL, modulo)
            return d*d
        }
    }
}
data class KeySize(val bits: Int){
    fun bitDigitsCount() = bits
    fun decimalDigitsCount() = when(bits){
        0 -> 1
        else -> log10(abs(2.0.pow(bits / 2).toDouble())).toInt() + 1
    }
}
data class PrimeSize(val p: Int, val q: Int){}
data class Modulus(val modulus: ULong){
    fun toULong() = modulus
}
data class PrivateExp(val exp: ULong){
    fun toULong() = exp
}
data class PublicExp(val exp: ULong){
    fun toULong() = exp
}
data class PrivateKey(val modulus: Modulus, val exponent: PrivateExp)
data class PublicKey(val modulus: Modulus, val exponent: PublicExp)
data class KeyPair(val publicKey: PublicKey, val privateKey: PrivateKey)
data class PlainBlock(val plainBlock: ULong){
    fun toULong() = plainBlock
    fun powerModulo(exp: PublicExp, modulo: Modulus) = plainBlock.powerModulo(exp.toULong(), modulo.toULong())
}
data class PlainText(val blocks: List<PlainBlock>):Iterable<PlainBlock>{
    override fun iterator(): Iterator<PlainBlock> {
        return blocks.iterator();
    }
}
data class CipherBlock(val cipherBlock: ULong){
    fun toULong() = cipherBlock
    fun powerModulo(exp: PrivateExp, modulo: Modulus) = cipherBlock.powerModulo(exp.toULong(), modulo.toULong())
}
data class CipherText(val blocks: List<CipherBlock>):Iterable<CipherBlock> {
    override fun iterator(): Iterator<CipherBlock> {
        return blocks.iterator();
    }
}

fun primeSizeFromKeySize(keySize: KeySize): PrimeSize {
    val digits = keySize.decimalDigitsCount()
    val keySizeDelta = randonIntegerModulo(5)
    return PrimeSize(digits - keySizeDelta, digits + keySizeDelta)
}

fun generate(keySize: KeySize = KeySize(1024)): KeyPair{

}

fun encrypt(plainText: PlainText, publicKey: PublicKey): CipherText {
    return CipherText(plainText.map { CipherBlock(it.powerModulo(publicKey.exponent, publicKey.modulus)) })
}

fun decrypt(cipherText: CipherText, privateKey: PrivateKey): PlainText {
    return PlainText(cipherText.map { PlainBlock(it.powerModulo(privateKey.exponent, privateKey.modulus)) })
}

fun main(args: Array<String>) {
    println("Hello World!")
    PublicKey(Modulus(1u), PublicExp(2u))

    val reader = Scanner(System.`in`)
    print("Enter a number: ")
    var integer:Int = reader.nextInt()
    
    println("Program arguments: ${args.joinToString()}")
}