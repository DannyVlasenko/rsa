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
    fun toInt() = bits
}
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

fun primeLengthFromKeySize(keySize: KeySize): Int {
    val digits = countDigits(2.0.pow(keySize.toInt() / 2))
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
    PublicKey(Modulus(1), PublicExp(2))
    // Try adding program arguments via Run/Debug configuration.
    // Learn more about running applications: https://www.jetbrains.com/help/idea/running-applications.html.
    println("Program arguments: ${args.joinToString()}")
}