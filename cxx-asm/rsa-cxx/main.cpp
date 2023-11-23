#include <array>
#include <iostream>
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
	throw;
}

CipherText encrypt(const PlainText &plain_text) { return {}; }

PlainText decrypt(const CipherText &cipher_text){throw;}

int main(int argc, char *argv[])
{
	std::array<unsigned char, 5> bytes {
		141, 246, 78, 2, 90
	};
	std::array<unsigned char, 1> bytes2 {
		209
	};
	const BigUInt a(bytes2);
	std::cout << a;
}

