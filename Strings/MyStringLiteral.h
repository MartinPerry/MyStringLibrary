#ifndef MY_STRING_LITERAL_H
#define MY_STRING_LITERAL_H

#include <cstdint>

#include "./MyStringMacros.h"

namespace mystrlib
{

	/// <summary>
	/// https://stackoverflow.com/questions/2041355/c-constructor-accepting-only-a-string-literal/2041408#2041408
	/// </summary>
	class StringLiteral
	{
	public:
		constexpr explicit StringLiteral(const char* literal) : literal_(literal) {}
		// The constructor is public to allow explicit conversion of external string
		// literals to `_L` literals. If there is no such need, then move constructor
		// to private section.

		constexpr uint32_t length() const { return StringLengthCExpr(literal_); }

		constexpr operator const char* () { return literal_; }

	private:
		constexpr friend StringLiteral operator"" _L(const char*, size_t);
		// Helps, when constructor is moved to private section.

		const char* literal_;
	};

	constexpr inline StringLiteral operator"" _L(const char* str, size_t)
	{
		return StringLiteral(str);
	}
}

#endif
