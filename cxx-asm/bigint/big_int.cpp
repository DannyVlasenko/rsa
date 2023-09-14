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
	while (!digits_.empty() && digits_.back() == 0)
	{
		digits_.resize(digits_.size() - 1);
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

DivisionResult div(const BigUInt &lhs, const BigUInt& rhs)
{
	DivisionResult res { 0_bui, lhs };
	while (res.remainder >= rhs)
	{
		res.quotient += 1_bui;
		res.remainder -= rhs;
	}
	return res;
}

BigUInt pow(BigUInt base, BigUInt exp)
{
	BigUInt result = 1_bui;
	for (;;)
	{
		if (exp.digits_[0] & 1)
			result *= base;
		exp >>= 1;
		if (exp == 0_bui)
			break;
		base *= base;
	}

	return result;
}

BigUInt operator/(const BigUInt& lhs, const BigUInt& rhs)
{
	return div(lhs, rhs).quotient;
}

BigUInt operator%(const BigUInt& lhs, const BigUInt& rhs)
{
	return div(lhs, rhs).remainder;
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
		const auto dec_digits_number = std::floor(std::log(std::pow(std::numeric_limits<unsigned long long>::max(), value.digits_.size()))/std::log(10)) + 1;
		BigUInt denominator(BigUInt())
		while(dec_digits_number > 0 and )
		auto leftover {value};
		std::vector<unsigned> digits;
		while (leftover > 0_bui)
		{
			auto res = div(leftover, 10_bui);
			digits.push_back(res.remainder.digits_.empty() ? 0 : res.remainder.digits_[0]);
			swap(leftover, res.quotient);
		}
		std::copy(digits.rbegin(), digits.rend(), std::ostream_iterator<unsigned>(sout));
	}
	return sout;
}

std::string to_string(const BigUInt& value)
{
	std::stringstream stream;
	stream << value;
	return stream.str();
}
