#ifndef MY_STRING_ANSI_H
#define MY_STRING_ANSI_H


#include <vector>

#include "./MyStringUtils.h"
#include "./MyStringMacros.h"

#include "./IStringAnsi.h"


class MyStringAnsi : public IStringAnsi<MyStringAnsi>
{
public:
	static const size_t BUFFER_SIZE = 0;

	using IStringAnsi<MyStringAnsi>::IStringAnsi;
	
	MyStringAnsi::MyStringAnsi(const char * newStr, size_t length)	
		: bufferCapacity(length + 1),
		strLength(length)
	{				
		this->strPtr = new char[this->bufferCapacity];
				
		memcpy(this->strPtr, newStr, length);
		this->strPtr[length] = 0;
				
		hashCode = std::numeric_limits<uint32_t>::max();
	}

	MyStringAnsi::MyStringAnsi(const MyStringAnsi &other)
	{
		this->CtorInternal(other.c_str());		
		this->hashCode = other.hashCode;
	};
	
	MyStringAnsi::MyStringAnsi(MyStringAnsi && other)  :
		strPtr(other.strPtr), 
		bufferCapacity(other.bufferCapacity),
		strLength(other.strLength)
	{				
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
			this->strPtr = new char[1];
			this->strPtr[0] = 0;
			this->strLength = 0;
			this->bufferCapacity = 1;
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



#include "./ComparisonOperators.inl"


#endif