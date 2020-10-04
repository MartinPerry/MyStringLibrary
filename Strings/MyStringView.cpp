#include "./MyStringView.h"

#include "./MyStringAnsi.h"
#include "./MySmallStringAnsi.h"
#include "./MurmurHash3_constexpr.inl"
#include "./MurmurHash3.h"

MyStringView::MyStringView(StringLiteral l) noexcept :
	str(l),
	len(StringLengthCExpr(str)),
	hash(static_cast<uint32_t>(MurmurHash3_32(str, len)))	
{
}


MyStringView::MyStringView(const char * str, size_t len) noexcept :
	str(str),
	len((len == 0) ? StringLengthCExpr(str) : len),
	//hash(static_cast<uint32_t>(MurmurHash3_32(str, len)))
	hash(std::numeric_limits<uint32_t>::max())
{		
}

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

MyStringView & MyStringView::operator = (const char * str) noexcept
{
	this->str = str;
	this->len = StringLengthCExpr(str);
	this->hash = static_cast<uint32_t>(MurmurHash3_x86_32(str, len));
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

/*
uint32_t MyStringView::GetHashCode() const
{
	return (this->hash.isPtr) ? *this->hash.valuePtr : this->hash.value;
}
*/

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
};
