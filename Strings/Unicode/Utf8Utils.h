#ifndef UTF8_UTILS_H
#define UTF8_UTILS_H

#include <string>

#include "./StringIterators.h"



using StringUtf8 = std::u8string;

static inline StringUtf8 AsStringUtf8(const char* str)
{
	return StringUtf8(reinterpret_cast<const char8_t*>(str));
}

static inline StringUtf8 AsStringUtf8(const std::string& str)
{
	return StringUtf8(reinterpret_cast<const char8_t*>(str.c_str()), str.length());
}

static inline StringUtf8 AsStringUtf8(const mystrlib::String& str)
{
	return StringUtf8(reinterpret_cast<const char8_t*>(str.c_str()), str.length());
}



class Utf8Utils
{
public:
	
	static int GetPackSize(const StringUtf8& str)
	{
		return static_cast<int>(sizeof(int) + sizeof(char8_t) * str.size());
	};

	static uint8_t* PackToMemory(const StringUtf8& str, uint8_t* memory)
	{
		//store unicode string raw length
		int strBufferSize = static_cast<int>(sizeof(char8_t) * str.size());
		memcpy(memory, &strBufferSize, sizeof(int));
		memory += sizeof(int);

		//store unicode string
		memcpy(memory, str.data(), strBufferSize);
		memory += strBufferSize;

		return memory;
	};

	static const uint8_t* UnpackFromMemory(const uint8_t* memory, StringUtf8& str)
	{
		//restore unicode string
		int strBufferSize = 0;
		memcpy(&strBufferSize, memory, sizeof(int));
		memory += sizeof(int);

		str = StringUtf8((const char8_t*)memory, strBufferSize / sizeof(char8_t));
		memory += (strBufferSize);

		return memory;
	};


	static bool AllInRange(const StringUtf8& str, char32_t start, char32_t end)
	{
		CustomU8Iterator it(str);
		char32_t c;
		while ((c = it.GetCurrentAndAdvance()) != it.DONE)
		{
			if (c < 'A')
			{
				//ignore non-letters before 'A' in ASCII
				continue;
			}
			if (c < start) return false;
			if (c > end) return false;
		}

		return true;
	};

};


#endif