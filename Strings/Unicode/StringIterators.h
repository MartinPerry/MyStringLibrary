#ifndef STRING_ITERATORS_H
#define STRING_ITERATORS_H

#include <string>
#include <string_view>
#include <limits>


#ifdef USE_ICU_LIBRARY
#	include <unicode/unistr.h>
#	include <unicode/schriter.h>
#endif

#if __has_include("./utf8.h")
#	include "./utf8.h"
#endif

//========================================================================

/// <summary>
/// Input string is ASCII string
/// Will return uint32_t -> ASCII id
/// </summary>
struct CustomAsciiIterator 
{	
	static inline uint32_t DONE = std::numeric_limits<uint32_t>::max();

	CustomAsciiIterator(const std::string & str) :
		CustomAsciiIterator(std::string_view(str))
	{}

	CustomAsciiIterator(const std::string_view & view) :
		v(view),
		index(0)
	{}

	CustomAsciiIterator(const CustomAsciiIterator& other) :
		v(other.v),
		index(other.index)
	{}

	void SetOffsetFromStart(uint32_t offset) { index = offset; }
	void SetOffsetFromCurrent(uint32_t offset) { index += offset; }

	uint32_t GetFirst() { return this->v[0]; }
	uint32_t GetCurrent() { return this->v[index]; }
	uint32_t GetNext() { this->index++; return this->GetCurrent(); }
	uint32_t GetCurrentAndAdvance() 
	{ 
		if (this->HasNext() == false)
		{
			return CustomAsciiIterator::DONE;
		}
		uint32_t c = this->v[this->index];
		this->index++; 		
		return c;
	}
	bool HasNext() { return index < v.length(); }

protected:
	std::string_view v;
	size_t index;
};

//========================================================================

#if __has_include("./utf8.h")

/// <summary>
/// Input string is unicode string
/// Will return uint32_t -> unicode code point
/// 
/// Example:
/// std::u8string testUnicode = u8"ABC"
///		u8"\u010D"          // è
///		u8"\u017E"          // ž
///		u8"\u03A9"          // "omega"
///		u8"\u6C34"          // "chinese"
///		u8"\U0001F600"      // :)
///		u8"\U0001F63A"      // "cat face"
///		u8"a\u0301";        // a + combining acute
/// 
/// </summary>
struct CustomU8Iterator
{
	static inline uint32_t DONE = std::numeric_limits<uint32_t>::max();

	CustomU8Iterator(const std::u8string& str) :
		CustomU8Iterator(std::u8string_view(str))		
	{
	}

	CustomU8Iterator(const std::u8string_view& view) :
		v(view),		
		it(v.data()),
		itEnd(v.data() + v.size())
	{
	}

	CustomU8Iterator(const CustomU8Iterator& other) :
		v(other.v),
		it(other.it),
		itEnd(other.itEnd)
	{
	}

	void SetOffsetFromStart(uint32_t offset) 
	{ 
		it = v.data(); 
		this->SetOffsetFromCurrent(offset);
	}
	void SetOffsetFromCurrent(uint32_t offset) 
	{ 
		for (uint32_t i = 0; i < offset; i++)
		{
			utf8::unchecked::next(it);
		}
	}

	uint32_t GetFirst() 
	{ 
		it = v.data();
		return utf8::unchecked::peek_next(it); 
	}
	uint32_t GetCurrent() 
	{ 
		if (this->HasNext() == false)
		{
			return CustomU8Iterator::DONE;
		}
		return utf8::unchecked::peek_next(it); 
	}

	uint32_t GetNext() 
	{ 
		if (this->HasNext() == false)
		{
			return CustomU8Iterator::DONE;
		}
		utf8::unchecked::next(it); 

		return this->GetCurrent();
	}
	uint32_t GetCurrentAndAdvance()
	{
		if (this->HasNext() == false)
		{
			return CustomU8Iterator::DONE;
		}
		uint32_t c = utf8::unchecked::next(it);		
		return c;
	}
	bool HasNext() { return it < itEnd; }

protected:
	std::u8string_view v;
	
	const char8_t* it;
	const char8_t const* itEnd;
};

#endif

//========================================================================


#ifdef USE_ICU_LIBRARY

struct CustomUnicodeIterator : public icu::StringCharacterIterator
{		

	CustomUnicodeIterator(const icu::UnicodeString & str) :
		icu::StringCharacterIterator(str)
	{}

	void SetOffsetFromStart(uint32_t offset) { this->move32(offset, icu::CharacterIterator::EOrigin::kStart); }
	void SetOffsetFromCurrent(uint32_t offset) { this->move32(offset, icu::CharacterIterator::EOrigin::kCurrent); }
	uint32_t GetFirst() { return this->first32(); }
	uint32_t GetCurrent() { return this->current32(); }
	uint32_t GetNext() { return this->next32(); }
	uint32_t GetCurrentAndAdvance() { return this->next32PostInc(); }
	bool HasNext() { return this->hasNext(); }
		
};

#endif

//========================================================================

/// <summary>
/// Iterate Unicode sequence and return UTF8 bytes
/// 
/// Example:
/// std::u8string testUnicode = u8""
///		u8"ABC"
///		u8"\u010D"          // è
///		u8"\u017E"          // ž
///		u8"\u03A9"          // "omega"
///		u8"\u6C34"          // "chinese"
///		u8"\U0001F600"      // :)
///		u8"\U0001F63A"      // "cat face"
///		u8"a\u0301";        // a + combining acute
/// 
/// Will result in UTF-8 bytes:
/// 
///		"\x41\x42\x43"        // ABC
///		"\xC4\x8D"            // U+010D
///		"\xC5\xBE"            // U+017E
///		"\xCE\xA9"            // U+03A9
///		"\xE6\xB0\xB4"        // U+6C34
///		"\xF0\x9F\x98\x80"    // U+1F600
///		"\xF0\x9F\x98\xBA"    // U+1F63A 
///		"\x61\xCC\x81"       // a + combining acute
/// 
/// </summary>
/// <typeparam name="UnicodeIterator"></typeparam>
template <typename UnicodeIterator>
struct CustomUtf8BytesIterator
{
	static inline uint8_t DONE = 0;

	CustomUtf8BytesIterator(UnicodeIterator uniIt) :
		it(uniIt),
		curCodePoint(0),
		buf({ 0,0,0,0 }),
		bufIndex(4)
	{
	}
		
	uint8_t GetCurrent()
	{
		if ((bufIndex == 4) || (buf[bufIndex] == 0))
		{
			if (this->LoadUnicode() == false)
			{
				return DONE;
			}
			bufIndex = 0;
		}
		return buf[bufIndex];
	}

	uint8_t GetNext()
	{
		if ((bufIndex == 4) || (buf[bufIndex] == 0))
		{
			if (this->LoadUnicode() == false)
			{
				return DONE;
			}
			bufIndex = 0;
		}
		else 
		{
			bufIndex++;
		}

		return this->GetCurrent();
	}

	uint8_t GetCurrentAndAdvance()
	{
		if ((bufIndex == 4) || (buf[bufIndex] == 0))
		{
			if (this->LoadUnicode() == false)
			{
				return DONE;
			}
			bufIndex = 0;
		}

		return buf[bufIndex++];
	}
	
	bool HasNext() 
	{ 
		return (it.HasNext() && bufIndex < 4);
	}

protected:
	UnicodeIterator it;
	uint32_t curCodePoint;
	std::array<uint8_t, 4> buf;
	int bufIndex;

	bool LoadUnicode()
	{
		curCodePoint = it.GetCurrentAndAdvance();
		if (curCodePoint == it.DONE)
		{
			return false;
		}

		if (curCodePoint > 0x10FFFFu || (curCodePoint >= 0xD800u && curCodePoint <= 0xDFFFu))
		{
			// replace with U+FFFD (or throw)
			curCodePoint = 0xFFFDu;
		}

		buf = { 0,0,0,0 };
		char* it = reinterpret_cast<char*>(buf.data());
		utf8::append(static_cast<char32_t>(curCodePoint), it);		

		return true;
	}
};

//========================================================================

struct CustomIteratorCreator
{
	template <typename T>
	static auto Create(const T& str)
	{
		if constexpr (std::is_same<T, std::string>::value)
		{
			return CustomAsciiIterator(str);
		}

		if constexpr (std::is_same<T, std::string_view>::value)
		{
			return CustomAsciiIterator(str);
		}

#if __has_include("./utf8.h")
		if constexpr (std::is_same<T, std::u8string>::value)
		{
			return CustomU8Iterator(str);
		}

		if constexpr (std::is_same<T, std::u8string_view>::value)
		{
			return CustomU8Iterator(str);
		}
#endif

#ifdef USE_ICU_LIBRARY
		if constexpr (std::is_same<T, icu::UnicodeString>::value)
		{
			return CustomUnicodeIterator(str);
		}
#endif
	}
};


/*
Unicode -> UTF8
Example: "è"
Unicode: U+010D
UTF8 bytes: [0xC4, 0x8D] -> [196, 141]
*/


#endif
