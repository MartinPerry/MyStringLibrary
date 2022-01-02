#ifndef TINY_UTF8_WRAPPER_H
#define TINY_UTF8_WRAPPER_H

#if __has_include("./tinyutf8.h")

#include "./tinyutf8.h"


struct TinyUtf8Wrapper
{
	constexpr static size_t npos = -1;
	tiny_utf8::string str;

	TinyUtf8Wrapper() :
		str(tiny_utf8::string())
	{
	}

	TinyUtf8Wrapper(const tiny_utf8::string& str) :
		str(str)
	{
	}

	TinyUtf8Wrapper(tiny_utf8::string&& str) :
		str(std::move(str))
	{
	}
	
	TinyUtf8Wrapper(const char * str) :
		str(str)
	{
	}

	TinyUtf8Wrapper(const char* str, size_t length) :
		str(str, length)
	{
	}

	template <typename S>
	S getAs() const
	{				
		return S(str.c_str());
	}
	
	

	size_t length() const
	{
		return str.length();
	}
		
	size_t find(char ch, size_t startOffset = 0)
	{
		return str.find(ch, startOffset);
	}

	void replace(size_t startIndex, size_t replacedChars, const TinyUtf8Wrapper& replacement)
	{
		str.replace(startIndex, replacedChars, replacement.str);
	}

	/*
	size_t raw_find(char ch, size_t startByte) const
	{
		return str.raw_find(ch, startByte);
	}

	void raw_replace(size_t startIndex, size_t replacedBytes, const TinyUtf8Wrapper& replacement)
	{
		str.raw_replace(startIndex, replacedBytes, replacement.str);
	}
	*/

	TinyUtf8Wrapper substr(size_t startIndex, size_t len = TinyUtf8Wrapper::npos) const
	{
		return TinyUtf8Wrapper(str.substr(startIndex, len));
	}

};

#endif

#endif
