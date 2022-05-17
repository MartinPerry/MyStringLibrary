#ifndef UCI_UNICODE_STRING_WRAPPER_H
#define UCI_UNICODE_STRING_WRAPPER_H

#if (defined(ENABLE_ICU) && (__has_include(<unicode/unistr.h>)))

#include <unicode/unistr.h>


struct IcuUnicodeStringWrapper
{
	constexpr static size_t npos = -1;
	icu::UnicodeString str;

	IcuUnicodeStringWrapper() :
		str(icu::UnicodeString::fromUTF8(""))
	{
	}

	IcuUnicodeStringWrapper(const icu::UnicodeString & str) :
		str(str)
	{
	}

	IcuUnicodeStringWrapper(icu::UnicodeString && str) :
		str(std::move(str))
	{
	}

	IcuUnicodeStringWrapper(const char * str) :
		str(icu::UnicodeString::fromUTF8(str))
	{
	}

	IcuUnicodeStringWrapper(const char* str, size_t length) :
		str(icu::UnicodeString::fromUTF8(icu::StringPiece(str, length)))
	{
	}

	template <typename S>
	S getAs() const
	{
		S tmp;
		str.toUTF8String(tmp);
		return tmp;
	}

	size_t length() const
	{
		return str.length();
	}
		
	size_t find(char ch, size_t startOffset = 0)
	{		
		return str.indexOf(ch, startOffset);
	}

	void replace(size_t startIndex, size_t replacedChars, const IcuUnicodeStringWrapper& replacement)
	{
		str.replace(startIndex, replacedChars, replacement.str);
	}
	
	IcuUnicodeStringWrapper substr(size_t startIndex, size_t len = INT32_MAX) const
	{
		return IcuUnicodeStringWrapper(str.tempSubString(startIndex, len));
	}
	
};

#endif

#endif
