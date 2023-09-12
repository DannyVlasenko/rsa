#include <vector>
#include "big_int.hpp"

struct PlainText
{
	std::vector<std::byte> data;
};

struct CipherText
{
	std::vector<std::byte> data;
};

struct PublicKey {};

struct PrivateKey {};

struct RSAKey
{
	PublicKey public_key;
	PrivateKey private_key;
};

RSAKey generate(size_t key_length_bytes)
{
	
}

CipherText encrypt(const PlainText &plain_text) { return {}; }

PlainText decrypt(const CipherText &cipher_text){}

int main(int argc, char *argv[])
{

}

