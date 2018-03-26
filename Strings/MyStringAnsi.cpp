#include "./MyStringAnsi.h"

#include "./MyString.h"
#include "./MurmurHash3.h"



#include <cstdio>
#include <cstdlib>
#include <climits>
#include <wctype.h>
#include <cctype>
#include <stdarg.h>

#include <cstring>
#include <locale>
#include <cmath>



//====================================================================
// ctors / dtors
//====================================================================


template <typename Type>
IStringAnsi<Type>::IStringAnsi()
	: hashCode(std::numeric_limits<uint32_t>::max())
{
	static_cast<Type *>(this)->CtorInternal(nullptr);
}


template <typename Type>
IStringAnsi<Type>::IStringAnsi(size_t bufferSize)
	: hashCode(std::numeric_limits<uint32_t>::max())
{
	if (bufferSize > BUFFER_SIZE)
	{
		static_cast<Type *>(this)->SetBufferSizeInternal(bufferSize);
	}
	static_cast<Type *>(this)->CtorInternal(nullptr);
}

template <typename Type>
IStringAnsi<Type>::IStringAnsi(char * str)
	: hashCode(std::numeric_limits<uint32_t>::max())
{
	static_cast<Type *>(this)->CtorInternal(str);
}

template <typename Type>
IStringAnsi<Type>::IStringAnsi(const char * str)
	: hashCode(std::numeric_limits<uint32_t>::max())
{
	static_cast<Type *>(this)->CtorInternal(str);
}



template <typename Type>
IStringAnsi<Type>::IStringAnsi(const std::string & str)
	: hashCode(std::numeric_limits<uint32_t>::max())
{
	static_cast<Type *>(this)->CtorInternal(str.c_str());
}




template <typename Type>
IStringAnsi<Type>::~IStringAnsi()
{
	this->Release();
}

/// <summary>
/// Manually release current data
/// </summary>
template <typename Type>
void IStringAnsi<Type>::Release()
{
	static_cast<Type *>(this)->ReleaseInternal();	
}



/// <summary>
/// Delete actual string and create new one, if NULL str is passed,
/// new string will only consist of terminating char '\0'
/// str does not have to be null terminated if lenth is passed
/// </summary>
/// <param name="str"></param>
/// <param name="length"></param> 
template <typename Type>
void IStringAnsi<Type>::CreateNew(const char * newStr, size_t length)
{
	char * str = static_cast<Type *>(this)->str();
	if (newStr == nullptr)
	{		
		str[0] = 0;
		static_cast<Type *>(this)->SetLengthInternal(0);
		return;
	}

	size_t strLength = length;
	if (length == 0)
	{
		strLength = strlen(newStr);
	}	

	size_t curBufSize = static_cast<const Type *>(this)->capacity();
	if (curBufSize < strLength + 1)
	{		
		//buffer size is always at least BUFFER_SIZE
		//so here we go only if buffer is on "heap", because size > BUFFER_SIZE

		curBufSize = this->CalcNewBufferSize(curBufSize, strLength);
		
		//delete actuall buffer only if it is on "heap"
		if (static_cast<const Type *>(this)->IsLocal() == false)
		{
			delete[] str;
		}
		str = new char[curBufSize];

		static_cast<Type *>(this)->SetBufferSizeInternal(curBufSize);
	}
	static_cast<Type *>(this)->SetStrInternal(str);

	memcpy(str, newStr, strLength);
	str[strLength] = 0;
	this->hashCode = std::numeric_limits<uint32_t>::max();

	static_cast<Type *>(this)->SetLengthInternal(strLength);
	
}

/// <summary>
/// Resize internal string buffer to length
/// Current string is coppied to the resized one
/// </summary>
/// <param name="newBufferSize"></param>
template <typename Type>
void IStringAnsi<Type>::ResizeBuffer(size_t newBufferSize)
{
	size_t curBufSize = static_cast<const Type *>(this)->capacity();

	if (newBufferSize <= curBufSize)
	{
		//resize to lower size not permitted
		return;
	}

	if (curBufSize >= newBufferSize)
	{
		//no resize needed
		return;
	}

	size_t strLength = static_cast<const Type *>(this)->length();
	char * str = static_cast<Type *>(this)->str();

	char * newStr = new char[newBufferSize];
	memcpy(newStr, str, strLength + 1);	//copy with "null-termination"
	if (static_cast<const Type *>(this)->IsLocal() == false)
	{
		delete[] str;
	}
	
	static_cast<Type *>(this)->SetStrInternal(newStr);
	static_cast<Type *>(this)->SetBufferSizeInternal(newBufferSize);
	
}


//====================================================================
// Static methods
//====================================================================

/// <summary>
/// Load string from file "fileName"
/// </summary>
/// <param name="fileName"></param>
/// <returns></returns>
template <typename Type>
template <typename RetVal>
RetVal IStringAnsi<Type>::LoadFromFile(const char * fileName)
{
	FILE *f = nullptr;  //pointer to file we will read in
	my_fopen(&f, fileName, "rb");
	if (f == nullptr)
	{
		printf("Failed to open file: \"%s\"\n", fileName);
		return "";
	}

	fseek(f, 0L, SEEK_END);
	long size = ftell(f);
	fseek(f, 0L, SEEK_SET);

	char * data = new char[size + 1];
	fread(data, sizeof(char), size, f);
	fclose(f);

	data[size] = 0;
	RetVal tmp = RetVal(data);
	SAFE_DELETE_ARRAY(data);

	return tmp;

}

/// <summary>
/// Create new string based on formatted string
/// ("%s %i", "kuk", 145)
/// This is quite slow
/// </summary>
/// <param name="str"></param>
/// <param name=""></param>
/// <returns></returns>
template <typename Type>
template <typename RetVal>
RetVal IStringAnsi<Type>::CreateFormated(const char * newStrFormat, ...)
{
	if (newStrFormat == nullptr)
	{
		return RetVal("");
	}

	va_list vl;
	
	//calculate length of new string
	std::vector<char> localBuffer;
	int appendLength = -1;
	while (appendLength < 0)
	{
		va_start(vl, newStrFormat);
		localBuffer.resize(localBuffer.size() + 256);
		appendLength = my_vsnprintf(&localBuffer[0], localBuffer.size(), localBuffer.size() - 1, newStrFormat, vl);
		va_end(vl);
	}
			
	
	//always store in heap
	size_t bufferSize = appendLength + 16;
	RetVal newStr = RetVal(bufferSize);
	
	char * str = newStr.str();
	
	va_start(vl, newStrFormat);
	int written = my_vsnprintf(str, bufferSize, bufferSize - 1, newStrFormat, vl);
	va_end(vl);
	
	if (written == -1)
	{
		return "";
	}


	size_t strLength = strlen(str);
	str[strLength] = 0;	

	newStr.hashCode = std::numeric_limits<uint32_t>::max();
	newStr.SetLengthInternal(strLength);	
	return newStr;
}

//====================== Methods ===============================

//====================================================================
// Helper methods
//====================================================================
template <typename Type>
bool IStringAnsi<Type>::SaveToFile(const char * fileName) const
{

	FILE *f = nullptr;  //pointer to file we will read in
	my_fopen(&f, fileName, "wb");
	if (f == nullptr)
	{
		return false;
	}

	fwrite(static_cast<const Type *>(this)->c_str(), 
		sizeof(char), 
		static_cast<const Type *>(this)->length(), f);
	fclose(f);

	return true;
}


//====================================================================
// Methods for string manipulation
//====================================================================

template <typename Type>
void IStringAnsi<Type>::Append(const char * appendStr)
{
	if (appendStr == nullptr)
	{
		return;
	}

	size_t len = strlen(appendStr);
	size_t curSize = static_cast<const Type *>(this)->capacity();
	size_t strLength = static_cast<const Type *>(this)->length();

	if (curSize <= strLength + len)
	{
		curSize = CalcNewBufferSize(curSize, strLength + len);
		this->ResizeBuffer(curSize);
	}

	char * str = static_cast<Type *>(this)->str();

	memcpy(str + strLength, appendStr, len);

	strLength += len;
	str[strLength] = 0;
	static_cast<Type *>(this)->SetLengthInternal(strLength);
	
	this->hashCode = std::numeric_limits<uint32_t>::max();
}

/*-----------------------------------------------------------
Function:    AppendFormat
Parameters:
[in] str - text to find
[in] ... - input elements

Append new formated string
Really SLOW !!!! - because of used vsnprintf
eg ("Formated %d %d", 10, 20) => "Formated 10, 20"
-------------------------------------------------------------*/
template <typename Type>
template<typename... Args>
void IStringAnsi<Type>::AppendFormat(const char * appendStr, Args... args)
{
	IStringAnsi<Type> tmp = IStringAnsi<Type>::CreateFormated(appendStr, args...);
	this->Append(tmp.c_str());	
}




/// <summary>
/// Trim string from left and right
/// </summary>
template <typename Type>
void IStringAnsi<Type>::Trim()
{
	size_t newLength = static_cast<const Type *>(this)->length();
	char * start = static_cast<Type *>(this)->str();
	char * tmp = start;

	while ((*tmp > 0) && isspace(*tmp))
	{
		tmp++;
		newLength--;
	}

	char * end = tmp + newLength - 1;
	while ((end > tmp) && (*end > 0) && isspace(*end))
	{
		end--;
		newLength--;
	}
	
	memmove(start, tmp, newLength);

	start[newLength] = 0;	
	

	static_cast<Type *>(this)->SetLengthInternal(newLength);
	this->hashCode = std::numeric_limits<uint32_t>::max();
}

/// <summary>
/// Reverse string
/// </summary>
template <typename Type>
void IStringAnsi<Type>::Reverse()
{
	char * str = static_cast<Type *>(this)->str();
	size_t length = static_cast<const Type *>(this)->length();
	size_t halfLen = length >> 1;
	for (size_t i = 0; i < halfLen; i++)
	{
		SWAP(str[i], str[length - i - 1]);
	}
	this->hashCode = std::numeric_limits<uint32_t>::max();
}


/// <summary>
/// Remove same characters from sequence
/// Eg: remove x -> xxxaaxx => xaax
/// 
/// </summary>
/// <param name="t"></param>
template <typename Type>
void IStringAnsi<Type>::RemoveMultipleChars(char t)
{
	char * str = static_cast<Type *>(this)->str();
	size_t j = 1;
	char lastC = str[0];
	char * start = str + 1;
	char c = 0;
	while ((c = *start) != 0)
	{		
		start++;

		if ((c == lastC) && (c == t))
		{
			continue;
		}
		
		str[j] = c;
		j++;

		lastC = c;
	}
	str[j] = 0;
		
	static_cast<Type *>(this)->SetLengthInternal(j - 1);
	this->hashCode = std::numeric_limits<uint32_t>::max();
}

//====================================================================
// Methods for obtaining new data from string
//====================================================================




/// <summary>
/// Count number of char occurences in string
/// </summary>
/// <param name="f"></param>
/// <returns></returns>
template <typename Type>
size_t IStringAnsi<Type>::Count(const char f) const
{
	size_t count = 0;

	const char * end = static_cast<const Type *>(this)->c_str();
	char c = 0;
	while ((c = *end) != 0)
	{
		if (c == f)
		{
			count++;
		}
		end++;
	}

	return count;
}



//====================================================================
// Searching
//====================================================================

/*-----------------------------------------------------------
Function:    BoyerMoore
Parameters:
[in] needle - text to find
[in/out] last - pointer to array of last function
[in] start - start position of searching (default: 0)
Returns:
position of occurence needle in haystack

Perfrom Boyer-Moore searching. Last function can be passed in
in "last". If "last" is NULL, last function is calculated
and filled to "last" array
!Important! "last" array must be freed outside this method
-------------------------------------------------------------*/
template <typename Type>
int IStringAnsi<Type>::BoyerMoore(const char * needle, int * &last, size_t start) const
{
	const char * str = static_cast<const Type *>(this)->c_str();
	int needleLen = static_cast<int>(strlen(needle));

	size_t strLength = static_cast<const Type *>(this)->length();

	if (last == nullptr)
	{
		last = new int[static_cast<int>(std::numeric_limits<uint8_t>::max()) + 1];
		memset(last, -1, std::numeric_limits<uint8_t>::max());
		for (size_t i = 0; i < strLength; i++)
		{
			last[static_cast<unsigned char>(str[i])] = i;
		}
	}
	int index = needleLen - 1;
	int cmpIndex = index;
	index += start;
	while (index < strLength)
	{
		if (str[index] == needle[cmpIndex])
		{
			index--;
			cmpIndex--;
			if (cmpIndex < 0)
			{

				return index + 1;
			}
		}
		else
		{
			int offset = last[static_cast<int>(str[index])];
			index = index + needleLen - ((cmpIndex < offset + 1) ? cmpIndex : offset + 1);
			cmpIndex = needleLen - 1;
		}
	}


	return -1;

}

/*-----------------------------------------------------------
Function:    KnuthMorisPrat
Parameters:
[in] needle - text to find
[in/out] last - pointer to array of last function
[in] start - start position of searching (default: 0)
Returns:
position of occurence needle in haystack

Perfrom KMP searching. Last function can be passed in
in "last". If "last" is NULL, last function is calculated
and filled to "last" array
!Important! "last" array must be freed outside this method
-------------------------------------------------------------*/
template <typename Type>
int IStringAnsi<Type>::KnuthMorisPrat(const char * needle, int * &last, size_t start) const
{
	int index = 1;
	int cmpIndex = 0;
	int needleLen = static_cast<int>(strlen(needle));
	int * failFce = last;
	size_t strLen = static_cast<const Type *>(this)->length();
	const char * str = static_cast<const Type *>(this)->c_str();

	if (failFce == nullptr)
	{
		failFce = new int[needleLen];
		last = failFce;
		//buil Fail fce
		failFce[0] = 0;
		index = 1;
		cmpIndex = 0;
		while (index < needleLen)
		{
			if (needle[index] == needle[cmpIndex])
			{
				failFce[index] = failFce[index - 1] + 1;
				cmpIndex++;
			}
			else
			{
				failFce[index] = 0;
				if ((failFce[index - 1] != 0) && (cmpIndex != 0))
				{
					cmpIndex = 0;
					index--;
				}
			}
			index++;
		}

	}
	index = start;
	cmpIndex = 0;
	while (index < strLen)
	{
		if (str[index] == needle[cmpIndex])
		{
			index++;
			cmpIndex++;
			if (cmpIndex == needleLen)
			{

				return (index - cmpIndex);
			}
		}
		else
		{
			if (cmpIndex == 0)
			{
				index++;
			}
			else
			{
				char offset = failFce[cmpIndex - 1];
				cmpIndex = offset;
			}
		}
	}


	return -1;
}

/*-----------------------------------------------------------
Function:    BruteForce
Parameters:
[in] needle - text to find
[in] start - start position of searching (default: 0)
Returns:
position of occurence needle in haystack

Perfrom BF searching. Suitable for small strings
-------------------------------------------------------------*/
template <typename Type>
int IStringAnsi<Type>::BruteForce(const char * needle, size_t start) const
{
	int needleLen = static_cast<int>(strlen(needle));
	size_t i = start;
	int j = 0;
	int lastPos = -1;
	size_t strLen = static_cast<const Type *>(this)->length();
	const char * str = static_cast<const Type *>(this)->c_str();

	while (i < strLen)
	{
		j = 0;
		while (j < needleLen)
		{
			if (str[i] == needle[j])
			{
				i++;
				j++;
				lastPos = i;
			}
			else
			{
				i++;
				lastPos = -1;
				break;
			}
		}
		if (lastPos != -1)
		{
			return (lastPos - needleLen);
		}
	}

	return -1;
}

/*-----------------------------------------------------------
Function:    CLib
Parameters:
[in] needle - text to find
[in] start - start position of searching (default: 0)
Returns:
position of occurence needle in haystack

Perfrom searching using strstr - standard C library
-------------------------------------------------------------*/
template <typename Type>
int IStringAnsi<Type>::CLib(const char * needle, size_t start) const
{
	const char * str = static_cast<const Type *>(this)->c_str();
	const char * found = strstr(str + start, needle);
	if (found == nullptr)
	{
		return -1;
	}
	return static_cast<int>(found - str);
}



template class IStringAnsi<MyStringAnsi>;
//template std::vector<MyStringAnsi> IStringAnsi<MyStringAnsi>::Split(char delimeter, bool keepEmptyValues) const;