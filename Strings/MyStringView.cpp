#include "./MyStringView.h"

#include "./MyString.h"
#include "./MySmallString.h"
#include "./hashing/MurmurHash3.h"

using namespace mystrlib;

StringView::StringView(const char* str, size_t len) noexcept :
	str(str),
	len((len == 0) ? strlen(str) : len),
	hash(std::numeric_limits<uint32_t>::max())
{
}


StringView::StringView(const String & str) noexcept :
	str(str.c_str()),
	len(str.length()),
	hash(&str.hashCode)
{
}

StringView::StringView(const String* str) noexcept :
	str(str->c_str()),
	len(str->length()),
	hash(&str->hashCode)
{
}

StringView::StringView(const SmallString& str) noexcept :
	str(str.c_str()),
	len(str.length()),
	hash(&str.hashCode)
{
}

StringView::StringView(const SmallString* str) noexcept :
	str(str->c_str()),
	len(str->length()),
	hash(&str->hashCode)
{
}

StringView::StringView(const StringView & v) noexcept :
	str(v.str),
	len(v.len),
	hash(v.hash)
{
}


StringView::StringView(const std::string& str) noexcept :
	str(str.c_str()),
	len(str.length()),
	hash(std::numeric_limits<uint32_t>::max())
{
}

StringView::StringView(const std::string_view& str) noexcept :
	str(str.data()),
	len(str.length()),
	hash(std::numeric_limits<uint32_t>::max())
{
}


StringView::StringView(const std::vector<char>& v) noexcept :
	str(v.data()),
	len(v.size()),
	hash(std::numeric_limits<uint32_t>::max())
{
}

StringView & StringView::operator = (const char * str) noexcept
{
	this->str = str;
	this->len = strlen(str);
	this->hash = std::numeric_limits<uint32_t>::max();// static_cast<uint32_t>(MurmurHash3_x86_32(str, len));
	return *this;
}

const char * StringView::c_str() const noexcept
{
	return this->str;
};

size_t StringView::length() const noexcept
{
	return this->len;
};

bool StringView::empty() const noexcept
{
	return ((this->len == 0) || (this->str == nullptr));
};


uint32_t StringView::GetHashCode() const noexcept
{
	if (this->hash.isPtr)
	{
		return *this->hash.valuePtr;
	}
	
	if (this->hash.value != std::numeric_limits<uint32_t>::max())
	{
		return this->hash.value;
	}

	this->hash.value = MurmurHash3_x86_32(this->str, static_cast<uint32_t>(this->len));
	
	return this->hash.value;
}

char StringView::GetLastChar() const
{	
	if (len == 0)
	{
		return 0;
	}

	return this->str[len - 1];
}

char StringView::GetFirstChar() const
{
	if (len == 0)
	{
		return 0;
	}

	return this->str[0];
}


void StringView::Trim()
{
	size_t oldLength = this->length();
	
	while ((*str > 0) && isspace(*str))
	{
		str++;
		len--;
	}

	const char* end = str + this->len - 1;
	while ((end > str) && (*end > 0) && isspace(*end))
	{
		end--;
		len--;
	}

	if (oldLength != len)
	{
		this->hash = std::numeric_limits<uint32_t>::max();
	}
}

void StringView::RemoveFromStart(size_t count)
{
	if (len < count)
	{
		count = len;
	}

	this->str += count;
	this->len -= count;
	this->hash = std::numeric_limits<uint32_t>::max();
}

void StringView::RemoveFromEnd(size_t count)
{	
	if (len < count)
	{
		count = len;
	}

	this->len -= count;
	this->hash = std::numeric_limits<uint32_t>::max();
}

StringView StringView::SubString(int start) const
{
	return this->SubString(start, std::numeric_limits<size_t>::max());
}


StringView StringView::SubString(int start, size_t length) const
{	
	StringView v = *this;

	for (int i = 0; i < start; i++)
	{
		v.str++;
		v.len--;
	}

	if (v.len > length)
	{
		v.len = length;
	}
	return v;
}

StringView StringView::substr(int start, size_t length) const
{
	return this->SubString(start, length);
}

/// <summary>
/// Test if string ends with suffix
/// </summary>
/// <param name="needle"></param>
/// <returns></returns>
bool StringView::StartWith(StringView needle) const noexcept
{
	size_t strLen = this->length();
	const char* str = this->c_str();

	if (strLen < needle.length())
	{
		return false;
	}

	for (size_t i = 0; i < strLen; i++)
	{
		if (str[i] != needle[0])
		{
			return false;
		}

		needle.RemoveFromStart();

		if (needle.length() == 0)
		{
			break;
		}
	}

	return true;
}

/// <summary>
/// Test if string ends with suffix
/// </summary>
/// <param name="needle"></param>
/// <returns></returns>
bool StringView::EndWith(StringView needle) const noexcept
{
	size_t strLen = this->length();
	const char* str = this->c_str();

	if (strLen < needle.length())
	{
		return false;
	}

	// < strLen - not a bug
	// feature - i wraps around when it goes under zero
	//https://stackoverflow.com/questions/3623263/reverse-iteration-with-an-unsigned-loop-variable
	for (size_t i = strLen - 1; i < strLen; i--)
	{
		if (str[i] != needle.GetLastChar())
		{
			return false;
		}

		needle.RemoveFromEnd();

		if (needle.length() == 0)
		{
			break;
		}
	}

	return true;
}

/// <summary>
/// Find char within this string view and return first position of
/// occurence
/// </summary>
/// <param name="str">char to find</param>
/// <returns>position of occurence needle in haystack</returns>
size_t StringView::Find(const char c) const noexcept
{
	size_t strLen = this->length();
	const char* str = this->c_str();

	for (size_t i = 0; i < strLen; i++)
	{
		if (str[i] == c)
		{
			return i;
		}
	}

	return StringUtils::npos;
}

size_t StringView::Find(StringView needle) const noexcept
{
	size_t pos = StringUtils::npos;

	if (needle.length() == 0)
	{
		return pos;
	}

	return StringUtils::SearchBruteForce(*this, needle);
}

size_t StringView::Find(StringView needle, size_t offset) const noexcept
{
	size_t pos = StringUtils::npos;

	if (needle.length() == 0)
	{
		return pos;
	}

	size_t len = this->length();
	const char* str = this->c_str();
	str += offset;
	len -= offset;

	StringView strViewOffset = StringView(str, len);

	pos = StringUtils::SearchBruteForce(strViewOffset, needle);

	if (pos == StringUtils::npos)
	{
		return StringUtils::npos;
	}

	return offset + pos;
}


std::vector<size_t> StringView::FindAll(StringView needle) const
{
	size_t searchLength = needle.length();
	size_t* last = nullptr;
	size_t pos = 0;

	std::vector<size_t> startPos;
	int foundOffset = 0;
	
	while (1)
	{
		pos = StringUtils::SearchKnuthMorisPrat(*this, needle, last, pos); //better use this, because BM skipping
		//is calculated from haystack, not needle
		if (pos == StringUtils::npos)
		{
			//not found
			break;
		}
		startPos.push_back(pos);    //store found start positions of searched needle

		pos += searchLength;        //set new search start

		foundOffset++;
	}

	delete[] last;

	return startPos;
}
