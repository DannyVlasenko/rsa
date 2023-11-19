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
	const auto lhsSize = digits_.size();
	digits_.resize(lhsSize + rhs.digits_.size());
	unsigned long long hi = 0;
	unsigned char carry = 0;
	size_t power{0};
	for (size_t i = 0; i < lhsSize; ++i)
	{
		BigUInt intermediate{0u};
		intermediate.digits_.resize(power);
		for (size_t k = 0; k < rhs.digits_.size(); ++k){
			carry = _addcarry_u64(carry, digits_[i], hi, &digits_[i]);
			digits_[i] = _mulx_u64(digits_[i], rhs.digits_[i], &hi);
		}

	}
	assert(digits_.empty() || digits_.back() != 0);
	return *this;
}

BigUInt& BigUInt::operator<<=(int shift)
{
	const auto eachDigitShift = shift % std::numeric_limits<unsigned long long>::digits;
	const auto digitsToShift = shift / std::numeric_limits<unsigned long long>::digits;
	if(eachDigitShift != 0)
	{
		auto carry = 0ull;
		const auto carryMask = 1ull << (eachDigitShift - 1);
		for(auto& digit : digits_)
		{
			digit = _rotl64(digit, eachDigitShift);
			const auto newCarry = digit & carryMask;
			digit = carry | (digit & ~carryMask);
			carry = newCarry;
		}
		if (carry != 0){
			digits_.push_back(carry);
		}
	}
	digits_.insert(digits_.begin(), digitsToShift, 0ull);
	assert(digits_.empty() || digits_.back() != 0);
	return *this;
}

BigUInt& BigUInt::operator>>=(int shift)
{
	const auto eachDigitShift = shift % std::numeric_limits<unsigned long long>::digits;
	const auto digitsToShift = shift / std::numeric_limits<unsigned long long>::digits;
	digits_.erase(digits_.begin(), digits_.begin() + digitsToShift);
	if(eachDigitShift != 0)
	{
		auto carry = 0ull;
		const auto carryMaskRight = 1ull << (eachDigitShift - 1);
		for(auto it = digits_.rbegin(); it != digits_.rend(); ++it)
		{
			auto &digit = *it;
			const auto newCarry = _rotr64(carryMaskRight & digit, shift);
			digit >>= eachDigitShift;
			digit |= carry;
			carry = newCarry;
		}
		while (!digits_.empty() && digits_.back() == 0)
		{
			digits_.resize(digits_.size() - 1);
		}
	}
	assert(digits_.empty() || digits_.back() != 0);
	return *this;
}

BigUInt operator*(const BigUInt& lhs, const BigUInt& rhs)
{
	BigUInt result{0u};
	unsigned long long hi = 0;
	unsigned char carry = 0;
	size_t power{0};
	for (size_t i = 0; i < rhs.digits_.size(); ++i)
	{
		BigUInt intermediate{0u};
		intermediate.digits_.resize(i + lhs.digits_.size());
		for (size_t k = 0; k < lhs.digits_.size(); ++k){
			carry = _addcarry_u64(carry, intermediate.digits_[i], hi, &intermediate.digits_[i]);
			intermediate.digits_[i+k] = _mulx_u64(lhs.digits_[k], rhs.digits_[i], &hi);
		}
		result += intermediate;
	}
	return result;
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
	return 0_bui;
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
