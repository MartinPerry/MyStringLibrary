#ifndef MY_STRING_VIEW_H
#define MY_STRING_VIEW_H

class MyStringAnsi;
class MySmallStringAnsi;

#include <type_traits>
#include <unordered_map>
#include <stdint.h>
#include <limits>
#include <string.h>
#include <vector>

#include "./MurmurHash3_constexpr.inl"
#include "./MyStringMacros.h"

//========================================================================

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

//========================================================================

/// <summary>
/// Simple string view class
/// </summary>
class MyStringView 
{
public:

	static const size_t npos = static_cast<size_t>(-1);

	constexpr MyStringView() noexcept;
	constexpr MyStringView(StringLiteral l) noexcept;
	constexpr MyStringView(const char * str, size_t len = 0) noexcept;
	MyStringView(const MyStringAnsi & str) noexcept;
	MyStringView(const MySmallStringAnsi & str) noexcept;
	MyStringView(const MyStringView & v) noexcept;

	MyStringView(const std::string& str) noexcept;
	MyStringView(const std::vector<char>& v) noexcept;


	~MyStringView() = default;

	uint32_t GetHashCode() const noexcept;
	const char * c_str() const noexcept;
	size_t length() const noexcept;

	char GetLastChar() const;

	void RemoveFromStart(int count);
	void RemoveFromEnd(int count);
	MyStringView SubString(int start, size_t length) const;

	size_t Find(const char c) const noexcept;

	MyStringView & operator = (const char * str) noexcept;

	template <typename T>
	RET_VAL_STR(char, (std::is_integral<T>::value))
	operator [](const T index) const;

	/// <summary>
	/// Used for comparison os views that are substring 
	/// non-null terminated
	/// For example:
	/// std::unordered_map<MyStringView, ...,
	/// std::hash<MyStringView>, MyStringView>
	/// </summary>
	/// <param name="t1"></param>
	/// <param name="t2"></param>
	/// <returns></returns>
	bool operator() (MyStringView const& t1, MyStringView const& t2) const
	{
		if (t1.length() != t2.length()) return false;
		return (memcmp(t1.c_str(), t2.c_str(), t1.length()) == 0);
	};


private:
	struct stringHash
	{
		bool isPtr;
		union
		{
			mutable uint32_t value;
			uint32_t * valuePtr;
		};

		constexpr stringHash(uint32_t val) :
			isPtr(false),
			value(val) 
		{}

		constexpr stringHash(uint32_t * val) :
			isPtr(true),
			valuePtr(val)
		{}
	};

	const char * str;
	size_t len;
	stringHash hash;	

};



constexpr MyStringView::MyStringView() noexcept :
	str(nullptr),
	len(0),
	hash(std::numeric_limits<uint32_t>::max())
{
}

constexpr MyStringView::MyStringView(StringLiteral l) noexcept :
	str(l),
	len(StringLengthCExpr(str)),
	hash(MurmurHash3_32CExpr(str, static_cast<uint32_t>(len), MURMUR_HASH_DEF_SEED))
{
}


constexpr MyStringView::MyStringView(const char* str, size_t len) noexcept :
	str(str),
	len((len == 0) ? StringLengthCExpr(str) : len),
	hash(std::numeric_limits<uint32_t>::max())
{
}


template <typename T>
RET_VAL_STR(char, (std::is_integral<T>::value))
MyStringView::operator [](const T index) const
{
	return str[index];
}

//For use in std::unordered_map
//http://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key
namespace std
{
	template <>
	struct hash<MyStringView>
	{
		std::size_t operator()(const MyStringView & k) const
		{
			return static_cast<size_t>(k.GetHashCode());
		};
	};
};

#endif
