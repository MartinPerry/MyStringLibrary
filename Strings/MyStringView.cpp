#include "./MyStringView.h"

#include "./MyStringAnsi.h"
#include "./MySmallStringAnsi.h"
#include "./MurmurHash3.h"

MyStringView::MyStringView(const MyStringAnsi & str) noexcept :
	str(str.c_str()),
	len(str.length()),
	hash(&str.hashCode)
{
}

MyStringView::MyStringView(const MySmallStringAnsi & str) noexcept :
	str(str.c_str()),
	len(str.length()),
	hash(&str.hashCode)
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

MyStringView::MyStringView(const std::vector<char>& v) noexcept :
	str(v.data()),
	len(v.size()),
	hash(std::numeric_limits<uint32_t>::max())
{
}

MyStringView & MyStringView::operator = (const char * str) noexcept
{
	this->str = str;
	this->len = StringLengthCExpr(str);
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
	return this->str[len - 1];
}

void MyStringView::RemoveFromStart(int count)
{
	this->str++;
	this->len--;
}

void MyStringView::RemoveFromEnd(int count)
{	
	this->len--;
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

	return MyStringView::npos;
}