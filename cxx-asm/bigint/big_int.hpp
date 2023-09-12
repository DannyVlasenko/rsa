#pragma once
#include <vector>
#include <span>
#include <string>
#include <iosfwd>

class BigUInt
{
public:
	template<std::unsigned_integral T>
	explicit BigUInt(T v) : digits_(1ull, static_cast<unsigned long long>(v)) {}

	explicit BigUInt(std::span<unsigned char> data);

	BigUInt& operator+=(const BigUInt& rhs);
	BigUInt& operator-=(const BigUInt& rhs);
	BigUInt& operator*=(const BigUInt& rhs);
	BigUInt& operator/=(const BigUInt& rhs);

	friend BigUInt operator+(BigUInt lhs, const BigUInt &rhs)
	{
		lhs += rhs;
		return lhs;
	}

	friend BigUInt operator-(BigUInt lhs, const BigUInt &rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	friend BigUInt operator*(BigUInt lhs, const BigUInt &rhs)
	{
		lhs *= rhs;
		return lhs;
	}

	friend BigUInt operator/(BigUInt lhs, const BigUInt &rhs)
	{
		lhs /= rhs;
		return lhs;
	}

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

private:
	std::vector<unsigned long long> digits_;
};

inline BigUInt operator""_bui(unsigned long long value)
{
	return BigUInt(value);
}

[[nodiscard]]
std::string to_string(const BigUInt& value);

