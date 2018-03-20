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

MyStringAnsi::MyStringAnsi()
	: str(local), hashCode(std::numeric_limits<uint32_t>::max())
{
	this->Ctor(nullptr);
}

MyStringAnsi::MyStringAnsi(size_t bufferSize)
	: str(local), hashCode(std::numeric_limits<uint32_t>::max())
{
	if (bufferSize > BUFFER_SIZE)
	{
		this->SetBufferSizeInternal(bufferSize);		
	}
	this->Ctor(nullptr);
}

MyStringAnsi::MyStringAnsi(char * str)
	: str(local), hashCode(std::numeric_limits<uint32_t>::max())
{
	this->Ctor(str);
}

MyStringAnsi::MyStringAnsi(const char * str)
	: str(local), hashCode(std::numeric_limits<uint32_t>::max())
{
	this->Ctor(str);
}

MyStringAnsi::MyStringAnsi(const char * str, size_t length)
	: str(local), hashCode(std::numeric_limits<uint32_t>::max())
{
	size_t bufferSize = length + 1;

	if (bufferSize > BUFFER_SIZE)
	{			
		this->str = new char[bufferSize];
		this->SetBufferSizeInternal(bufferSize);		
	}

	memcpy(this->str, str, length);
	this->str[length] = 0;

	this->SetLengthInternal(length);
}

MyStringAnsi::MyStringAnsi(const std::string & str)
	: str(local), hashCode(std::numeric_limits<uint32_t>::max())
{
	this->Ctor(str.c_str());
}

MyStringAnsi::MyStringAnsi(const MyStringAnsi &other)
	: str(local), hashCode(std::numeric_limits<uint32_t>::max())
{
	this->Ctor(other.str);
	this->hashCode = other.hashCode;
}

MyStringAnsi::MyStringAnsi(MyStringAnsi && other)  /* noexcept needed to enable optimizations in containers */
	: str(other.str), hashCode(other.hashCode)
{
	//http://blog.smartbear.com/c-plus-plus/c11-tutorial-introducing-the-move-constructor-and-the-move-assignment-operator/

	memcpy(local, other.local, BUFFER_SIZE);
	if (other.str == other.local)
	{
		this->str = this->local;
	}

	other.str = nullptr;
	other.hashCode = std::numeric_limits<uint32_t>::max();
}



MyStringAnsi::~MyStringAnsi()
{
	this->Release();
}

/// <summary>
/// Manually release current data
/// </summary>
void MyStringAnsi::Release()
{
	if (this->str != this->local)
	{
		delete[] this->str;
	}
	this->str = local;
	memset(this->local, 0, BUFFER_SIZE + 1);	
	this->hashCode = std::numeric_limits<uint32_t>::max();
}



void MyStringAnsi::Ctor(const char * str)
{
	if (str == nullptr)
	{			
		this->str[0] = 0;
		this->SetLengthInternal(0);
		return;
	}

	size_t strLength = strlen(str);

	//if strlen < 16 -> use local buffer, [15] = 0
	//if strlen >= 16 -> use "heap"

	if (strLength >= BUFFER_SIZE)
	{		
		size_t bufferSize = strLength + 1;
		this->SetBufferSizeInternal(bufferSize);

		this->str = new char[bufferSize];
	}
		
	memcpy(this->str, str, strLength + 1);	//copy with "null-termination"	
	this->hashCode = std::numeric_limits<uint32_t>::max();
	this->SetLengthInternal(strLength);
}


/// <summary>
/// Delete actual string and create new one, if NULL str is passed,
/// new string will only consist of terminating char '\0'
/// str does not have to be null terminated if lenth is passed
/// </summary>
/// <param name="str"></param>
/// <param name="length"></param> 
void MyStringAnsi::CreateNew(const char * str, size_t length)
{
	if (str == nullptr)
	{		
		this->str[0] = 0;
		this->SetLengthInternal(0);		
		return;
	}

	size_t strLength = length;
	if (length == 0)
	{
		strLength = strlen(str);
	}	

	size_t curBufSize = this->capacity();
	if (curBufSize < strLength + 1)
	{		
		//buffer size is always at least BUFFER_SIZE
		//so here we go only if buffer is on "heap", because size > BUFFER_SIZE

		curBufSize = this->CalcNewBufferSize(curBufSize, strLength);
		
		//delete actuall buffer only if it is on "heap"
		delete[] this->str;
		this->str = new char[curBufSize];

		this->SetBufferSizeInternal(curBufSize);
	}

	memcpy(this->str, str, strLength);	
	this->str[strLength] = 0;
	this->hashCode = std::numeric_limits<uint32_t>::max();

	this->SetLengthInternal(strLength);
}

/// <summary>
/// Resize internal string buffer to length
/// Current string is coppied to the resized one
/// </summary>
/// <param name="newBufferSize"></param>
void MyStringAnsi::ResizeBuffer(size_t newBufferSize)
{
	size_t curBufSize = this->capacity();

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

	size_t strLength = this->length();

	char * newStr = new char[newBufferSize];
	memcpy(newStr, str, strLength + 1);	//copy with "null-termination"
	if (this->str != this->local)
	{
		delete[] this->str;
	}
	
	this->SetBufferSizeInternal(newBufferSize);	
	this->str = newStr;
}


//====================================================================
// Static methods
//====================================================================

/// <summary>
/// Load string from file "fileName"
/// </summary>
/// <param name="fileName"></param>
/// <returns></returns>
MyStringAnsi MyStringAnsi::LoadFromFile(const MyStringAnsi & fileName)
{
	FILE *f = nullptr;  //pointer to file we will read in
	my_fopen(&f, fileName.c_str(), "rb");
	if (f == nullptr)
	{
		printf("Failed to open file: \"%s\"\n", fileName.c_str());
		return "";
	}

	fseek(f, 0L, SEEK_END);
	long size = ftell(f);
	fseek(f, 0L, SEEK_SET);

	char * data = new char[size + 1];
	fread(data, sizeof(char), size, f);
	fclose(f);

	data[size] = 0;
	MyStringAnsi tmp = MyStringAnsi(data);
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
MyStringAnsi MyStringAnsi::CreateFormated(const char * str, ...)
{
	if (str == nullptr)
	{
		return "";
	}

	va_list vl;
	
	//calculate length of new string
	std::vector<char> localBuffer;
	int appendLength = -1;
	while (appendLength < 0)
	{
		va_start(vl, str);
		localBuffer.resize(localBuffer.size() + 256);
		appendLength = my_vsnprintf(&localBuffer[0], localBuffer.size(), localBuffer.size() - 1, str, vl);
		va_end(vl);
	}
			
	
	//always store in heap
	size_t bufferSize = appendLength + 16;
	MyStringAnsi newStr = MyStringAnsi(bufferSize);
	
	va_start(vl, str);
	int written = my_vsnprintf(newStr.str, bufferSize, bufferSize - 1, str, vl);
	va_end(vl);
	
	if (written == -1)
	{
		return "";
	}


	size_t strLength = strlen(newStr.str);
	newStr.str[strLength] = 0;
	newStr.hashCode = std::numeric_limits<uint32_t>::max();
	newStr.SetLengthInternal(strLength);
	return newStr;
}

//====================== Methods ===============================

//====================================================================
// Helper methods
//====================================================================

bool MyStringAnsi::SaveToFile(const MyStringAnsi & fileName) const
{

	FILE *f = nullptr;  //pointer to file we will read in
	my_fopen(&f, fileName.c_str(), "wb");
	if (f == nullptr)
	{
		return false;
	}

	fwrite(this->c_str(), sizeof(char), this->length(), f);
	fclose(f);

	return true;
}


//====================================================================
// Methods for string manipulation
//====================================================================

/// <summary>
/// Trim string from left and right
/// </summary>
void MyStringAnsi::Trim()
{
	size_t newLength = this->length();
	char * start = this->str;
	while ((*this->str > 0) && isspace(*this->str))
	{
		this->str++;
		newLength--;
	}

	char * end = this->str + newLength - 1;
	while ((end > this->str) && (*end > 0) && isspace(*end))
	{
		end--;
		newLength--;
	}
	
	memmove(start, this->str, newLength);

	this->str = start;
	this->str[newLength] = 0;

	this->SetLengthInternal(newLength);
	this->hashCode = std::numeric_limits<uint32_t>::max();
}

/// <summary>
/// Reverse string
/// </summary>
void MyStringAnsi::Reverse()
{
	size_t length = this->length();
	size_t halfLen = length >> 1;
	for (int i = 0; i < halfLen; i++)
	{
		SWAP(this->str[i], this->str[length - i - 1]);
	}
	this->hashCode = std::numeric_limits<uint32_t>::max();
}


/// <summary>
/// Remove same characters from sequence
/// Eg: remove x -> xxxaaxx => xaax
/// 
/// </summary>
/// <param name="t"></param>
void MyStringAnsi::RemoveMultipleChars(char t)
{
	size_t j = 1;
	char lastC = this->str[0];
	char * start = this->str + 1;
	char c = 0;
	while ((c = *start) != 0)
	{		
		start++;

		if ((c == lastC) && (c == t))
		{
			continue;
		}
		
		this->str[j] = c;
		j++;

		lastC = c;
	}
	this->str[j] = 0;
		
	this->SetLengthInternal(j - 1);
	this->hashCode = std::numeric_limits<uint32_t>::max();
}

//====================================================================
// Methods for obtaining new data from string
//====================================================================

/// <summary>
/// Split string by a char delimeter
/// Eg: 11  11  22 -> ' ' => {11,11,22}
/// Eg: 11  11  22 -> ' ', keepEmptyValues => {11,,11,,22}
/// 
/// </summary>
/// <param name="delimeter"></param>
/// <param name="keepEmptyValues">if we want to keep empty values (default: false)</param>
/// <returns></returns>
std::vector<MyStringAnsi> MyStringAnsi::Split(char delimeter, bool keepEmptyValues) const
{
	std::vector<MyStringAnsi> splited;

	char * end = this->str;
	char * start = end;
	char c = 0;
	while ( (c = *end) != 0)
	{		
		if (c == delimeter)
		{
			//splitting
			size_t len = end - start;
			if (len == 0)
			{
				if (keepEmptyValues)
				{
					MyStringAnsi s;
					splited.push_back(s);
				}
			}
			else
			{
				MyStringAnsi s(start, len);
				splited.push_back(s);
			}

			start = end + 1;
		}

		end++;
	}


	size_t len = end - start;
	if (len == 0)
	{
		if (keepEmptyValues)
		{
			MyStringAnsi s;
			splited.push_back(s);
		}
	}
	else
	{
		MyStringAnsi s(start, len);
		splited.push_back(s);
	}
	
	return splited;
}

/// <summary>
/// Count number of char occurences in string
/// </summary>
/// <param name="f"></param>
/// <returns></returns>
size_t MyStringAnsi::Count(const char f) const
{
	size_t count = 0;

	char * end = this->str;
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
int MyStringAnsi::BoyerMoore(const char * needle, int * &last, int start) const
{
	int needleLen = static_cast<int>(strlen(needle));

	size_t strLength = this->length();

	if (last == nullptr)
	{
		last = new int[static_cast<int>(std::numeric_limits<uint8_t>::max()) + 1];
		memset(last, -1, std::numeric_limits<uint8_t>::max());
		for (size_t i = 0; i < strLength; i++)
		{
			last[static_cast<unsigned char>(this->str[i])] = i;
		}
	}
	int index = needleLen - 1;
	int cmpIndex = index;
	index += start;
	while (index < strLength)
	{
		if (this->str[index] == needle[cmpIndex])
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
			int offset = last[static_cast<int>(this->str[index])];
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
int MyStringAnsi::KnuthMorisPrat(const char * needle, int * &last, int start) const
{
	int index = 1;
	int cmpIndex = 0;
	int needleLen = static_cast<int>(strlen(needle));
	int * failFce = last;
	size_t strLen = this->length();

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
		if (this->str[index] == needle[cmpIndex])
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
int MyStringAnsi::BruteForce(const char * needle, int start) const
{
	int needleLen = static_cast<int>(strlen(needle));
	int i = start;
	int j = 0;
	int lastPos = -1;
	size_t strLen = this->length();

	while (i < strLen)
	{
		j = 0;
		while (j < needleLen)
		{
			if (this->str[i] == needle[j])
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
int MyStringAnsi::CLib(const char * needle, int start) const
{
	char * found = strstr(this->str + start, needle);
	if (found == nullptr)
	{
		return -1;
	}
	return static_cast<int>(found - this->str);
}
