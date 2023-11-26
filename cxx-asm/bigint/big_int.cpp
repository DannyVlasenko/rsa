#include "big_int.hpp"

#include <algorithm>
#include <ostream>
#include <ranges>
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
	auto operations_count = rhs.digits_.size();
	if (digits_.size() < rhs.digits_.size())
	{
		digits_.reserve(rhs.digits_.size() + 1);
		digits_.resize(rhs.digits_.size());
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
		if (digits_.size() == rhs.digits_.size())
		{
			digits_.resize(0);
		}
		else
		{
			--digits_[operations_count];
		}
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
	auto temp = *this * rhs;
	swap(*this, temp);
	return *this;
}

BigUInt& BigUInt::operator++()
{
	if (digits_.empty())
	{
		digits_.push_back(1);
		return *this;
	}
	for(auto& digit : digits_)
	{
		++digit;
		if (digit != 0)
		{
			break;
		}
	}
	return *this;
}

BigUInt BigUInt::operator++(int)
{
	BigUInt old = *this;
	operator++();
	return old;
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
		for (auto& digit : std::ranges::reverse_view(digits_))
		{
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

BigUInt& BigUInt::operator&=(const BigUInt& rhs)
{
	if (digits_.size() > rhs.digits_.size())
	{
		digits_.resize(rhs.digits_.size());
	}
	for (size_t i = 0; i < digits_.size(); ++i)
	{
		digits_[i] &= rhs.digits_[i];
	}
	while (!digits_.empty() && digits_.back() == 0)
	{
		digits_.resize(digits_.size() - 1);
	}
	assert(digits_.empty() || digits_.back() != 0);
	return *this;
}

BigUInt& BigUInt::operator|=(const BigUInt& rhs)
{
	if (rhs.digits_.size() > digits_.size())
	{
		digits_.resize(rhs.digits_.size());
	}
	for (size_t i = 0; i < rhs.digits_.size(); ++i)
	{
		digits_[i] |= rhs.digits_[i];
	}
	while (!digits_.empty() && digits_.back() == 0)
	{
		digits_.resize(digits_.size() - 1);
	}
	assert(digits_.empty() || digits_.back() != 0);
	return *this;
}

BigUInt operator*(BigUInt lhs, BigUInt rhs)
{
	if (lhs == 0_bui || rhs == 0_bui)
	{
		return 0_bui;
	}
	BigUInt result{(lhs.digits_[0] & 1ull) != 0 ? rhs : 0_bui};
	while(lhs > 1_bui)
	{
		lhs >>= 1;
		rhs <<= 1;
		if (!lhs.digits_.empty() && (lhs.digits_[0] & 1ull) != 0)
		{
			result += rhs;
		}
	}
	return result;
}

void div(BigUInt lhs, BigUInt rhs, BigUInt& out_quotient, BigUInt& out_remainder)
{
	/**
	 *	Implementation of Algorithm 1.6 from https://arxiv.org/pdf/1004.4710.pdf
	 */

	if (rhs == 0_bui)
	{
		throw std::runtime_error("Division by zero.");
	}
	if (lhs == rhs)
	{
		out_quotient = 1_bui;
		out_remainder = 0_bui;
	}
	if (lhs < rhs)
	{
		out_quotient = 0_bui;
		out_remainder = lhs;
	}

	//Normalization step.
	//The resulting remainder must divided by this multiplier.
	auto remainder_multiplier = 0;
	while(rhs.digits_.back() <= std::numeric_limits<unsigned long long>::max() / 2 + 1)
	{
		rhs <<= 1;
		lhs <<= 1;
		++remainder_multiplier;
	}

	//Divisor digits count.
	const auto n = rhs.digits_.size();

	//Digit count difference. This is also a maximum digit index of the quotient.
	const auto m = lhs.digits_.size() - n;

	//Dividend digits array.
	//auto &a = lhs.digits_;

	//Divisor digits array.
	//const auto &b = rhs.digits_;

	//Quotient digits array.
	auto& q = out_quotient.digits_;
	if (const auto betaPowMTimesB = BigUInt{ 1ull, m } * rhs; lhs >= betaPowMTimesB)
	{
		q.resize(m + 1);
		q[m] = 1;
		lhs -= betaPowMTimesB;
	}
	else
	{
		q.resize(m);
	}
	for (long long int j = m - 1; j >=0 ; --j)
	{
		//unsigned long long remainder_unused;
		const auto qStar = _udiv128(lhs.digits_[n + j], lhs.digits_[n + j - 1], rhs.digits_[n - 1], /*&remainder_unused*/ nullptr);
		q[j] = std::min(qStar, std::numeric_limits<unsigned long long>::max() /* Beta-1 */);
		auto qjBetaJTimesB = BigUInt(q[j], j) * rhs;
		//lhs = lhs - qjBetaJ * rhs;
		while (lhs < qjBetaJTimesB)
		{
			--q[j];
			lhs += BigUInt(1ull, j) * rhs;
		}
		lhs -= qjBetaJTimesB;
	}
	out_remainder = lhs >>= remainder_multiplier;
	assert(out_remainder.digits_.empty() || out_remainder.digits_.back() != 0);
	assert(out_quotient.digits_.empty() || out_quotient.digits_.back() != 0);
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
