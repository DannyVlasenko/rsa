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

	BigUInt& operator<<=(int shift);

	BigUInt& operator>>=(int shift);

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

	friend BigUInt operator/(const BigUInt& lhs, const BigUInt& rhs);

	friend BigUInt operator%(const BigUInt& lhs, const BigUInt& rhs);

	friend bool operator==(const BigUInt& lhs, const BigUInt& rhs)
	{
		return lhs.digits_ == rhs.digits_;
	}

	friend auto operator<=>(const BigUInt& lhs, const BigUInt& rhs)
	{
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

struct DivisionResult
{
	BigUInt quotient;
	BigUInt remainder;
};

DivisionResult div(const BigUInt &lhs, const BigUInt& rhs);

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
