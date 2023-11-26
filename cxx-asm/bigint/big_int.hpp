#pragma once
#include <vector>
#include <span>
#include <string>
#include <iosfwd>

class BigUInt
{
public:
	BigUInt() noexcept = default;

	template<std::unsigned_integral T>
	explicit BigUInt(T v) : digits_(1ull, static_cast<unsigned long long>(v))
	{
		if(digits_.back() == 0)
		{
			digits_.resize(0);
		}
	}

	template<std::unsigned_integral T>
	explicit BigUInt(T v, size_t pos) : digits_(pos + 1)
	{
		digits_.back() = v;
		if(digits_.back() == 0)
		{
			digits_.resize(0);
		}
	}

	explicit BigUInt(std::span<unsigned char> data);

	friend void swap(BigUInt& lhs, BigUInt& rhs) noexcept
	{
		using std::swap;
		swap(lhs.digits_, rhs.digits_);
	}

	BigUInt& operator+=(const BigUInt& rhs);
	BigUInt& operator-=(const BigUInt& rhs);
	BigUInt& operator*=(const BigUInt& rhs);

	BigUInt& operator/=(const BigUInt& rhs)
	{
		auto res = *this / rhs;
		swap(*this, res);
		return *this;
	}

	BigUInt& operator%=(const BigUInt& rhs)
	{
		auto res = *this % rhs;
		swap(*this, res);
		return *this;
	}

	BigUInt& operator++();

	BigUInt operator++(int);

	BigUInt& operator<<=(int shift);

	BigUInt& operator>>=(int shift);

	BigUInt& operator&=(const BigUInt& rhs);

	BigUInt& operator|=(const BigUInt& rhs);

	friend BigUInt operator+(BigUInt lhs, const BigUInt& rhs)
	{
		lhs += rhs;
		return lhs;
	}

	friend BigUInt operator-(BigUInt lhs, const BigUInt& rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	friend BigUInt operator*(BigUInt lhs, BigUInt rhs);

	friend void div(BigUInt lhs, BigUInt rhs, BigUInt &out_quotient, BigUInt &out_remainder);

	friend auto div(const BigUInt &lhs, const BigUInt &rhs)
	{
		struct DivisionResult
		{
			BigUInt quotient;
			BigUInt remainder;
		};

		DivisionResult res;
		div(lhs, rhs, res.quotient, res.remainder);
		return res;
	}

	friend BigUInt operator/(const BigUInt& lhs, const BigUInt& rhs);

	friend BigUInt operator%(const BigUInt& lhs, const BigUInt& rhs);

	friend BigUInt operator&(BigUInt lhs, const BigUInt& rhs)
	{
		lhs &= rhs;
		return lhs;
	}

	friend BigUInt operator|(BigUInt lhs, const BigUInt& rhs)
	{
		lhs |= rhs;
		return lhs;
	}

	friend bool operator==(const BigUInt& lhs, const BigUInt& rhs)
	{
		return lhs.digits_ == rhs.digits_;
	}

	friend auto operator<=>(const BigUInt& lhs, const BigUInt& rhs)
	{
		if (lhs.digits_.size() != rhs.digits_.size())
		{
			return lhs.digits_.size() <=> rhs.digits_.size();
		}
		return std::lexicographical_compare_three_way(lhs.digits_.rbegin(), lhs.digits_.rend(),
			rhs.digits_.rbegin(), rhs.digits_.rend());
	}

	friend std::ostream& operator<<(std::ostream& out, const BigUInt& value);

	friend std::size_t hash_value(const BigUInt& obj)
	{
		std::size_t seed = 0x7ED9EC52;
		constexpr std::hash<unsigned long long> hasher{};
		for (auto digit : obj.digits_){
			seed ^= hasher(digit) + 0x9e3779b9 + (seed<<6) + (seed>>2);
		}
		return seed;
	}

private:
	std::vector<unsigned long long> digits_;
};

BigUInt pow(BigUInt base, BigUInt exp);

inline BigUInt operator""_bui(unsigned long long value)
{
	return BigUInt(value);
}

[[nodiscard]]
std::string to_string(const BigUInt& value);

template<>
struct std::hash<BigUInt>
{
	std::size_t operator()(const BigUInt& value) const noexcept
	{
		return hash_value(value);
	}
};
