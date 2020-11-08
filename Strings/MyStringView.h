#ifndef MY_STRING_VIEW_H
#define MY_STRING_VIEW_H

class MyStringAnsi;
class MySmallStringAnsi;

#include <type_traits>
#include <unordered_map>
#include <stdint.h>
#include <limits>
#include <string>


#include "./MyStringMacros.h"

//========================================================================

/// <summary>
/// https://stackoverflow.com/questions/2041355/c-constructor-accepting-only-a-string-literal/2041408#2041408
/// </summary>
class StringLiteral 
{
public:
	explicit StringLiteral(const char* literal) : literal_(literal) {}
	// The constructor is public to allow explicit conversion of external string
	// literals to `_L` literals. If there is no such need, then move constructor
	// to private section.

	operator const char* () { return literal_; }

private:
	friend StringLiteral operator"" _L(const char*, size_t);
	// Helps, when constructor is moved to private section.

	const char* literal_;
};

inline StringLiteral operator"" _L(const char* str, size_t)
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
	MyStringView() noexcept;
	MyStringView(StringLiteral l) noexcept;
	MyStringView(const char * str, size_t len = 0) noexcept;
	MyStringView(const MyStringAnsi & str) noexcept;
	MyStringView(const MySmallStringAnsi & str) noexcept;
	MyStringView(const MyStringView & v) noexcept;

	~MyStringView() = default;

	uint32_t GetHashCode() const noexcept;
	const char * c_str() const noexcept;
	size_t length() const noexcept;

	char GetLastChar() const;

	void RemoveFromStart(int count);
	void RemoveFromEnd(int count);

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

		stringHash(uint32_t val) :
			isPtr(false),
			value(val) 
		{}
		stringHash(uint32_t * val) :
			isPtr(true),
			valuePtr(val)
		{}
	};

	const char * str;
	size_t len;
	stringHash hash;	

};


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
