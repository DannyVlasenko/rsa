#include "big_int.hpp"
#include <ostream>
#include <sstream>
#include <ranges>
#include <cassert>
#include <immintrin.h>

BigUInt::BigUInt(std::span<unsigned char> data)
{
	while (!data.empty() && data.back() == 0)
	{
		data = data.subspan(0, data.size() - 1);
	}
	digits_.resize((data.size_bytes() + sizeof(unsigned long long) - 1) / sizeof(unsigned long long));
	memcpy(digits_.data(), data.data(), data.size_bytes());
	assert(digits_.empty() || digits_.back() != 0);
}

BigUInt& BigUInt::operator+=(const BigUInt& rhs)
{
	auto operations_count = digits_.size();
	if (digits_.size() < rhs.digits_.size())
	{
		digits_.reserve(rhs.digits_.size() + 1);
		digits_.resize(rhs.digits_.size());
		operations_count = digits_.size();
	}
	unsigned char carry = 0;
	for (size_t i = 0; i < operations_count; ++i)
	{
		carry = _addcarryx_u64(carry, digits_[i], rhs.digits_[i], &digits_[i]);
	}
	if (carry != 0)
	{
		digits_.push_back(carry);
	}
	assert(digits_.empty() || digits_.back() != 0);
	return *this;
}

BigUInt& BigUInt::operator-=(const BigUInt& rhs)
{
	if (digits_.size() < rhs.digits_.size())
	{
		digits_.resize(0);
		return *this;
	}
	const auto operations_count = rhs.digits_.size();
	unsigned char carry = 0;
	for (size_t i = 0; i < operations_count; ++i)
	{
		carry = _subborrow_u64(carry, digits_[i], rhs.digits_[i], &digits_[i]);
	}
	if (carry != 0)
	{
		digits_.resize(0);
	}
	assert(digits_.empty() || digits_.back() != 0);
	return *this;
}

BigUInt& BigUInt::operator*=(const BigUInt& rhs)
{
	digits_.resize(digits_.size() + rhs.digits_.size());
	const auto operations_count = rhs.digits_.size();
	unsigned long long hi = 0;
	unsigned char carry = 0;
	for (size_t i = 0; i < operations_count; ++i)
	{
		carry = _subborrow_u64(carry, digits_[i], hi, &digits_[i]);
		digits_[i] = _mulx_u64(digits_[i], rhs.digits_[i], &hi);
	}
	digits_[operations_count] = hi + carry;
	assert(digits_.empty() || digits_.back() != 0);
	return *this;
}

BigUInt& BigUInt::operator/=(const BigUInt& rhs)
{
	assert(digits_.empty() || digits_.back() != 0);
	return *this;
}

std::ostream& operator<<(std::ostream& sout, const BigUInt& value)
{
	assert(value.digits_.empty() || value.digits_.back() != 0);
	if (value.digits_.empty())
	{
		sout << 0;
		return sout;
	}
	auto flags = sout.flags();
	if (flags & std::ios_base::hex)
	{
		for (auto d : value.digits_)
		{
			sout << d;
		}
	}
	if (flags & std::ios_base::dec)
	{
		for (size_t i = 0; i < value.digits_.size(); ++i)
		{
			const auto long_digit = value.digits_[i];
			unsigned char carry = 0;
			unsigned char dec_digit = 0;
			for (size_t byte = 0; byte < sizeof(unsigned long long) * 2; ++byte)
			{
				const auto bits = byte * 4;
				const auto bt = static_cast<unsigned char>((long_digit & (0xFull << bits)) >> bits);
				dec_digit = (bt % 16 + carry) % 10;
				carry = (bt % 16 + carry) / 10;
				sout << dec_digit;
			}
		}
	}
	return sout;
}

std::string to_string(const BigUInt& value)
{
	std::stringstream stream;
	stream << value;
	return stream.str();
}
