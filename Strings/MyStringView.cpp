#include "./MyStringView.h"

#include "./MyStringAnsi.h"
#include "./MySmallStringAnsi.h"
#include "./MurmurHash3_constexpr.inl"

MyStringView::MyStringView(const char * str, size_t len) : 
	str(str),
	len((len == 0) ? StringLengthCExpr(str) : len),
	hash(static_cast<uint32_t>(MurmurHash3_32(str, len)))
{		
}

MyStringView::MyStringView(const MyStringAnsi & str) : 
	str(str.c_str()),
	len(str.length()),
	hash(&str.hashCode)
{
}

MyStringView::MyStringView(const MySmallStringAnsi & str) :
	str(str.c_str()),
	len(str.length()),
	hash(&str.hashCode)
{
}

MyStringView::MyStringView(const MyStringView & v) : 
	str(v.str),
	len(v.len),
	hash(v.hash)
{
}

MyStringView & MyStringView::operator = (const char * str)
{
	this->str = str;
	this->len = StringLengthCExpr(str);
	this->hash = static_cast<uint32_t>(MurmurHash3_32(str, len));	
	return *this;
}

const char * MyStringView::c_str() const
{
	return this->str;
};

size_t MyStringView::length() const
{
	return this->len;
};

uint32_t MyStringView::GetHashCode() const
{
	return (this->hash.isPtr) ? *this->hash.valuePtr : this->hash.value;
}

char MyStringView::GetLastChar() const
{	
	return this->str[len - 1];
};
