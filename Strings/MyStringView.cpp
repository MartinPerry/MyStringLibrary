#include "./MyStringView.h"

#include "./MyStringAnsi.h"
#include "./MySmallStringAnsi.h"
#include "./MurmurHash3.h"

MyStringView::MyStringView(const char* str, size_t len) noexcept :
	str(str),
	len((len == 0) ? strlen(str) : len),
	hash(std::numeric_limits<uint32_t>::max())
{
}


MyStringView::MyStringView(const MyStringAnsi & str) noexcept :
	str(str.c_str()),
	len(str.length()),
	hash(&str.hashCode)
{
}

MyStringView::MyStringView(const MyStringAnsi* str) noexcept :
	str(str->c_str()),
	len(str->length()),
	hash(&str->hashCode)
{
}

MyStringView::MyStringView(const MySmallStringAnsi & str) noexcept :
	str(str.c_str()),
	len(str.length()),
	hash(&str.hashCode)
{
}

MyStringView::MyStringView(const MySmallStringAnsi* str) noexcept :
	str(str->c_str()),
	len(str->length()),
	hash(&str->hashCode)
{
}

MyStringView::MyStringView(const MyStringView & v) noexcept :
	str(v.str),
	len(v.len),
	hash(v.hash)
{
}


MyStringView::MyStringView(const std::string& str) noexcept :
	str(str.c_str()),
	len(str.length()),
	hash(std::numeric_limits<uint32_t>::max())
{
}

MyStringView::MyStringView(const std::string_view& str) noexcept :
	str(str.data()),
	len(str.length()),
	hash(std::numeric_limits<uint32_t>::max())
{
}


MyStringView::MyStringView(const std::vector<char>& v) noexcept :
	str(v.data()),
	len(v.size()),
	hash(std::numeric_limits<uint32_t>::max())
{
}

MyStringView & MyStringView::operator = (const char * str) noexcept
{
	this->str = str;
	this->len = strlen(str);
	this->hash = std::numeric_limits<uint32_t>::max();// static_cast<uint32_t>(MurmurHash3_x86_32(str, len));
	return *this;
}

const char * MyStringView::c_str() const noexcept
{
	return this->str;
};

size_t MyStringView::length() const noexcept
{
	return this->len;
};

bool MyStringView::empty() const noexcept
{
	return ((this->len == 0) || (this->str == nullptr));
};


uint32_t MyStringView::GetHashCode() const noexcept
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

char MyStringView::GetLastChar() const
{	
	if (len == 0)
	{
		return 0;
	}

	return this->str[len - 1];
}

char MyStringView::GetFirstChar() const
{
	if (len == 0)
	{
		return 0;
	}

	return this->str[0];
}


void MyStringView::Trim()
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

void MyStringView::RemoveFromStart(size_t count)
{
	if (len < count)
	{
		count = len;
	}

	this->str += count;
	this->len -= count;
	this->hash = std::numeric_limits<uint32_t>::max();
}

void MyStringView::RemoveFromEnd(size_t count)
{	
	if (len < count)
	{
		count = len;
	}

	this->len -= count;
	this->hash = std::numeric_limits<uint32_t>::max();
}

MyStringView MyStringView::SubString(int start) const
{
	return this->SubString(start, std::numeric_limits<size_t>::max());
}


MyStringView MyStringView::SubString(int start, size_t length) const
{	
	MyStringView v = *this;

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

MyStringView MyStringView::substr(int start, size_t length) const
{
	return this->SubString(start, length);
}

/// <summary>
/// Test if string ends with suffix
/// </summary>
/// <param name="needle"></param>
/// <returns></returns>
bool MyStringView::StartWith(MyStringView needle) const noexcept
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
bool MyStringView::EndWith(MyStringView needle) const noexcept
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
size_t MyStringView::Find(const char c) const noexcept
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

	return MyStringUtils::npos;
}

size_t MyStringView::Find(MyStringView needle) const noexcept
{
	size_t pos = MyStringUtils::npos;

	if (needle.length() == 0)
	{
		return pos;
	}

	return MyStringUtils::SearchBruteForce(*this, needle);
}

size_t MyStringView::Find(MyStringView needle, size_t offset) const noexcept
{
	size_t pos = MyStringUtils::npos;

	if (needle.length() == 0)
	{
		return pos;
	}

	size_t len = this->length();
	const char* str = this->c_str();
	str += offset;
	len -= offset;

	MyStringView strViewOffset = MyStringView(str, len);

	pos = MyStringUtils::SearchBruteForce(strViewOffset, needle);

	if (pos == MyStringUtils::npos)
	{
		return MyStringUtils::npos;
	}

	return offset + pos;
}


std::vector<size_t> MyStringView::FindAll(MyStringView needle) const
{
	size_t searchLength = needle.length();
	size_t* last = nullptr;
	size_t pos = 0;

	std::vector<size_t> startPos;
	int foundOffset = 0;
	
	while (1)
	{
		pos = MyStringUtils::SearchKnuthMorisPrat(*this, needle, last, pos); //better use this, because BM skipping
		//is calculated from haystack, not needle
		if (pos == MyStringUtils::npos)
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
