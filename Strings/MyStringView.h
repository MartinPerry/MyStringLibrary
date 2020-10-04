#ifndef MY_STRING_VIEW_H
#define MY_STRING_VIEW_H

#include <unordered_map>
#include <stdint.h>
#include <limits>

class MyStringAnsi;
class MySmallStringAnsi;

#include "./MyStringMacros.h"

class MyStringView 
{
public:
	MyStringView(const char * str, size_t len = 0);
	MyStringView(const MyStringAnsi & str);
	MyStringView(const MySmallStringAnsi & str);
	MyStringView(const MyStringView & v);

	~MyStringView() = default;

	uint32_t GetHashCode() const;
	const char * c_str() const;
	size_t length() const;

	char GetLastChar() const;

	MyStringView & operator = (const char * str);

private:
	struct stringHash
	{
		bool isPtr;
		union
		{
			uint32_t value;
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
