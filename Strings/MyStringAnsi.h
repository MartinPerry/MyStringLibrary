#ifndef MY_STRING_ANSI_H
#define MY_STRING_ANSI_H

#include <string.h>
#include <vector>
#include <limits>

#include "./MyStringUtils.h"
#include "./MyStringMacros.h"

#include "./IStringAnsi.h"


class MyStringAnsi : public IStringAnsi<MyStringAnsi>
{
public:
	static const size_t BUFFER_SIZE = 0;

	using IStringAnsi<MyStringAnsi>::IStringAnsi;
	using IStringAnsi<MyStringAnsi>::operator=;

	MyStringAnsi(const char * newStr, size_t length)	
		: bufferCapacity(length + 1),
		strLength(length)
	{				
		this->strPtr = new char[this->bufferCapacity];
				
		memcpy(this->strPtr, newStr, length);
		this->strPtr[length] = 0;
				
		hashCode = std::numeric_limits<uint32_t>::max();
	}

	MyStringAnsi(const MyStringAnsi &other)
	{
		this->CtorInternal(other.c_str());		
		this->hashCode = other.hashCode;
	};
	
	MyStringAnsi(MyStringAnsi && other)  :
		strPtr(other.strPtr), 
		bufferCapacity(other.bufferCapacity),
		strLength(other.strLength)		
	{				
		this->hashCode = other.hashCode;

		other.strPtr = nullptr;
		other.bufferCapacity = 0;
		other.strLength = 0;
		other.hashCode = std::numeric_limits<uint32_t>::max();
	};

	~MyStringAnsi() = default;



	const char * c_str() const
	{
		return this->strPtr;
	};


	size_t length() const
	{
		return this->strLength;
	};

	size_t capacity() const
	{
		return this->bufferCapacity;
	};

	MyStringAnsi & operator = (const MyStringAnsi & str)
	{
		return IStringAnsi<MyStringAnsi>::operator=(str);
	};
			
	friend class IStringAnsi<MyStringAnsi>;

protected:
	
	char * strPtr;
	size_t bufferCapacity;
	size_t strLength;



	void ReleaseInternal()
	{
		delete[] this->strPtr;
		this->strPtr = nullptr;
		
		this->bufferCapacity = 0;
		this->strLength = 0;
				
		this->hashCode = std::numeric_limits<uint32_t>::max();		
	};

	void CtorInternal(const char * newStr)
	{		
		if (newStr == nullptr)
		{
			this->strPtr = nullptr;
			//this->strPtr[0] = 0;
			this->strLength = 0;
			this->bufferCapacity = 0;
			return;
		}

		this->strLength = strlen(newStr);
		this->bufferCapacity = this->strLength + 1;
		
		this->strPtr = new char[this->bufferCapacity];
				
		memcpy(this->strPtr, newStr, this->bufferCapacity);	//copy with "null-termination"				
	};


	char * str()
	{
		return this->strPtr;
	};

	bool IsLocal() const
	{
		return false;
	};

	void SetBufferSizeInternal(size_t s)
	{
		this->bufferCapacity = s;
	};

	void SetLengthInternal(size_t s)
	{
		this->strLength = s;
	};

	void SetStrInternal(char * s)
	{
		this->strPtr = s;

	};
};

//For use in std::unordered_map
//http://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key
namespace std
{
	template <>
	struct hash<MyStringAnsi>
	{
		std::size_t operator()(const MyStringAnsi & k) const
		{
			return k.GetHashCode();
		};
	};
};





#endif