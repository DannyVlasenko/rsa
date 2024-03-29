#include <bit>

#include "big_int.hpp"
#include <sstream>
#include <iomanip>

extern "C"
{
	__declspec(dllexport)
	BigUInt* BigInt_CreateFromBytes(unsigned char data[], int size)
	{
		return new BigUInt(std::span(data, size));
	}

	__declspec(dllexport)
	void BigInt_Release(const BigUInt* handle)
	{
		delete handle;
	}

	__declspec(dllexport)
	int BigInt_ToString(const BigUInt* handle, char* str, int length)
	{
		std::stringstream stream;
		stream << /*std::setfill('0') << std::hex << */ *handle;
		const auto bigIntStr = stream.str();
		if (static_cast<int>(bigIntStr.length()) >= length)
		{
			return -1;
		}
		strcpy_s(str, length, bigIntStr.data());
		return static_cast<int>(bigIntStr.length());
	}

	__declspec(dllexport)
	int BigInt_Hash(const BigUInt* handle)
	{
		return static_cast<int>(hash_value(*handle) % std::numeric_limits<int>::max());
	}

	__declspec(dllexport)
	bool BigInt_Equals(const BigUInt* lhs, const BigUInt* rhs)
	{
		return *lhs == *rhs;
	}

	__declspec(dllexport)
	int BigInt_Compare(const BigUInt* lhs, const BigUInt* rhs)
	{
		return std::bit_cast<signed char>(*lhs <=> *rhs);
	}
}