#ifndef MY_STRING_ANSI_H
#define MY_STRING_ANSI_H

#include <cstring>
#include <vector>
#include <limits>
#include <utility>

#include "./MyStringUtils.h"
#include "./MyStringMacros.h"

#include "./IStringAnsi.h"
#include "./MyStringView.h"


class MyStringAnsi : public IStringAnsi<MyStringAnsi>
{
public:
	static const size_t BUFFER_SIZE = 0;

	using IStringAnsi<MyStringAnsi>::IStringAnsi;
	using IStringAnsi<MyStringAnsi>::operator=;

	/// <summary>
	/// Create new string from input memory by "move" memory ownership
	/// to the string. So, mem does not have to be released manually
	/// Example:
	/// char * m = new char[10];
	/// fread(...)
	/// MyStringAnsi str = MyStringAnsi::CreateFromMoveMemory(m, 10);
	/// 
	/// </summary>
	/// <param name="mem"></param>
	/// <param name="memSize"></param>
	/// <param name="strLength"></param>
	/// <returns></returns>
	static MyStringAnsi CreateFromMoveMemory(char * mem, size_t memSize, size_t strLength = 0)
	{
		MyStringAnsi str;
		if (mem == nullptr)
		{
			return str;
		}		

		delete[] str.strPtr;

		str.strPtr = mem;
		str.bufferCapacity = memSize;

		if (strLength == 0)
		{
			strLength = strlen(mem);
		}
		str.strLength = strLength;

		return str;
	}

	/// <summary>
	/// Get size of string after it will be packed with 
	/// PackToMemory
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	static int GetPackSize(const MyStringAnsi& str)
	{
		return static_cast<int>(sizeof(int) + sizeof(uint8_t) * str.length());
	}

	/// <summary>
	/// Pack string to memory for e.g. serialization
	/// </summary>
	/// <param name="str"></param>
	/// <param name="memory"></param>
	/// <returns></returns>
	static uint8_t * PackToMemory(const MyStringAnsi & str, uint8_t * memory)
	{
		//store unicode string raw length
		int strBufferSize = static_cast<int>(sizeof(char) * str.length());
		memcpy(memory, &strBufferSize, sizeof(int));
		memory += sizeof(int);

		//store unicode string
		memcpy(memory, str.c_str(), strBufferSize);
		memory += strBufferSize;

		return memory;
	};

	/// <summary>
	/// Unpack string, packed with PackToMemory
	/// </summary>
	/// <param name="memory"></param>
	/// <param name="str"></param>
	/// <returns></returns>
	static uint8_t * UnpackFromMemory(uint8_t * memory, MyStringAnsi & str)
	{
		// restore string
		int tmp = 0;
		memcpy(&tmp, memory, sizeof(int));
		memory += sizeof(int);

		size_t strBufferSize = static_cast<size_t>(tmp);

		if (strBufferSize > 0)
		{
			if (str.bufferCapacity < strBufferSize + 1)
			{
				delete[] str.strPtr;
				str.bufferCapacity = strBufferSize + 1;
				str.strPtr = new char[str.bufferCapacity];
			}
			memcpy(str.strPtr, memory, strBufferSize);
			str.strPtr[strBufferSize] = 0;

			str.strLength = strBufferSize;
		}
		else
		{
			if (str.bufferCapacity > 0)
			{
				str.strPtr[0] = 0;
				str.strLength = 0;
			}
		}

		memory += (strBufferSize);

		return memory;
	};

	MyStringAnsi() : 
		strPtr(nullptr),
		bufferCapacity(0),
		strLength(0)
	{
		this->CtorInternal(nullptr, 0);
	}

	MyStringAnsi(const char * newStr, size_t length) : 
		bufferCapacity(length + 1),
		strLength(length)
	{				
		this->strPtr = new char[this->bufferCapacity];
				
		memcpy(this->strPtr, newStr, length);
		this->strPtr[length] = 0;
	}
	
	MyStringAnsi(const MyStringView & str) : 
		MyStringAnsi(str.c_str(), str.length())
	{
	}

	MyStringAnsi(const MyStringAnsi &other) : 
		strPtr(nullptr),
		bufferCapacity(0),
		strLength(0)		
	{
		this->CtorInternal(other.c_str(), other.length());		
		this->hashCode = other.hashCode;
	};
	
	MyStringAnsi(MyStringAnsi && other) noexcept  :
		strPtr(std::exchange(other.strPtr, nullptr)),
		bufferCapacity(std::exchange(other.bufferCapacity, 0)),
		strLength(std::exchange(other.strLength, 0))
	{				
		this->hashCode = std::exchange(other.hashCode, std::numeric_limits<uint32_t>::max());
	};

	~MyStringAnsi() = default;


	/// <summary>
	/// Get read-only inner pointer
	/// </summary>
	/// <returns></returns>
	const char * c_str() const noexcept
	{
		return this->strPtr;
	};

	/// <summary>
	/// Get raw inner memory pointer
	/// data can be modified
	/// NOTE: Beware of removing terminating NULL !!!
	/// </summary>
	/// <returns></returns>
	char* data() const noexcept
	{
		return this->strPtr;
	};

	size_t length() const noexcept
	{
		return this->strLength;
	};

	size_t capacity() const noexcept
	{
		return this->bufferCapacity;
	};

	MyStringAnsi & operator = (const MyStringAnsi & other)
	{
		return IStringAnsi<MyStringAnsi>::operator=(other);
	};

	MyStringAnsi& operator = (MyStringAnsi&& other) noexcept
	{
		std::swap(strPtr, other.strPtr);
		std::swap(bufferCapacity, other.bufferCapacity);
		std::swap(strLength, other.strLength);
		std::swap(hashCode, other.hashCode);
		return *this;
	};
		
	/// <summary>
	/// Move internal string array
	/// char * str = (char *)(std::move(stringVar));
	/// </summary>
	/// <returns></returns>
	operator char *() && noexcept
	{
		char* m = this->strPtr;
		this->strPtr = nullptr;
		this->bufferCapacity = 0;
		this->strLength = 0;
		return m;
	};
	
	friend class IStringAnsi<MyStringAnsi>;
	friend class MyStringView;

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

	void DefaultInit() noexcept
	{
		this->strPtr = nullptr;
		this->bufferCapacity = 0;
		this->strLength = 0;
	}

	void CtorInternal(const char * newStr, size_t newStrLength)
	{		
		if (newStr == nullptr)
		{
			this->strPtr = new char[1];
			this->strPtr[0] = 0;

			this->strLength = 0;
			this->bufferCapacity = 1;
			return;
		}

		if (newStrLength == 0)
		{
			this->strLength = strlen(newStr);
		}
		else 
		{
			this->strLength = newStrLength;
		}
		this->bufferCapacity = this->strLength + 1;
		
		this->strPtr = new char[this->bufferCapacity];
				
		memcpy(this->strPtr, newStr, this->bufferCapacity);	//copy with "null-termination"				
	};


	char * str() noexcept
	{
		return this->strPtr;
	};

	bool IsLocal() const noexcept
	{
		return false;
	};

	void SetBufferSizeInternal(size_t s) noexcept
	{
		this->bufferCapacity = s;
	};

	void SetLengthInternal(size_t s) noexcept
	{
		this->strLength = s;
	};

	void SetStrInternal(char * s) noexcept
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
			return static_cast<size_t>(k.GetHashCode());
		};
	};
};





#endif