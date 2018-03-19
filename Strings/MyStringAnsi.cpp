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
	: str(local), strLength(0), hashCode(std::numeric_limits<uint32_t>::max())
{
	this->Ctor(nullptr);
}

MyStringAnsi::MyStringAnsi(size_t bufferSize)
	: str(local), strLength(0),	hashCode(std::numeric_limits<uint32_t>::max())
{
	if (bufferSize > BUFFER_SIZE)
	{
		memcpy(this->local, &bufferSize, sizeof(size_t));
	}
	this->Ctor(nullptr);
}

MyStringAnsi::MyStringAnsi(char * str)
	: str(local), strLength(0), hashCode(std::numeric_limits<uint32_t>::max())
{
	this->Ctor(str);
}

MyStringAnsi::MyStringAnsi(const char * str)
	: str(local), strLength(0), hashCode(std::numeric_limits<uint32_t>::max())
{
	this->Ctor(str);
}

MyStringAnsi::MyStringAnsi(const char * str, size_t length)
	: str(local), strLength(length), hashCode(std::numeric_limits<uint32_t>::max())
{
	size_t bufferSize = length + 1;

	if (bufferSize > BUFFER_SIZE)
	{			
		this->str = new char[bufferSize];
		memcpy(this->local, &bufferSize, sizeof(size_t));
	}

	memcpy(this->str, str, length);
	this->str[length] = 0;
}

MyStringAnsi::MyStringAnsi(const std::string & str)
	: str(local), strLength(0), hashCode(std::numeric_limits<uint32_t>::max())
{
	this->Ctor(str.c_str());
}

MyStringAnsi::MyStringAnsi(const MyStringAnsi &other)
	: str(local), strLength(0), hashCode(std::numeric_limits<uint32_t>::max())
{
	this->Ctor(other.str);
	this->hashCode = other.hashCode;
}

MyStringAnsi::MyStringAnsi(MyStringAnsi && other)  /* noexcept needed to enable optimizations in containers */
	: str(other.str), strLength(other.strLength), hashCode(other.hashCode)
{
	//http://blog.smartbear.com/c-plus-plus/c11-tutorial-introducing-the-move-constructor-and-the-move-assignment-operator/

	memcpy(local, other.local, BUFFER_SIZE);

	other.str = nullptr;
	other.strLength = 0;
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
	this->strLength = 0;	
	this->hashCode = std::numeric_limits<uint32_t>::max();
}



void MyStringAnsi::Ctor(const char * str)
{
	if (str == nullptr)
	{			
		this->str[0] = 0;
		return;
	}

	this->strLength = strlen(str);

	//if strlen < 16 -> use local buffer, [15] = 0
	//if strlen >= 16 -> use "heap"

	if (this->strLength >= BUFFER_SIZE)
	{		
		size_t bufferSize = strLength + 1;
		memcpy(this->local, &bufferSize, sizeof(size_t));

		this->str = new char[bufferSize];
	}
		
	memcpy(this->str, str, strLength + 1);	//copy with "null-termination"	
	this->hashCode = std::numeric_limits<uint32_t>::max();
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
		this->strLength = 0;	
		this->str[0] = 0;
		return;
	}

	this->strLength = length;
	if (length == 0)
	{
		this->strLength = strlen(str);
	}	

	size_t curBufSize = this->capacity();
	if (curBufSize < this->strLength + 1)
	{		
		//buffer size is always at least BUFFER_SIZE
		//so here we go only if buffer is on "heap", because size > BUFFER_SIZE

		curBufSize = this->CalcNewBufferSize(curBufSize, strLength);
		
		//delete actuall buffer only if it is on "heap"
		delete[] this->str;
		this->str = new char[curBufSize];

		memcpy(this->local, &curBufSize, sizeof(size_t));
	}

	memcpy(this->str, str, this->strLength);	
	this->str[this->strLength] = 0;
	this->hashCode = std::numeric_limits<uint32_t>::max();

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



	char * newStr = new char[newBufferSize];
	memcpy(newStr, str, this->strLength + 1);	//copy with "null-termination"
	if (this->str != this->local)
	{
		delete[] this->str;
	}
	
	memcpy(this->local, &newBufferSize, sizeof(size_t));		
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


	newStr.strLength = strlen(newStr.str);
	newStr.str[newStr.strLength] = 0;
	newStr.hashCode = std::numeric_limits<uint32_t>::max();
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
	size_t newLength = this->strLength;
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

	this->strLength = newLength;
	this->hashCode = std::numeric_limits<uint32_t>::max();
}

/// <summary>
/// Reverse string
/// </summary>
void MyStringAnsi::Reverse()
{
	size_t halfLen = this->strLength >> 1;
	for (int i = 0; i < halfLen; i++)
	{
		SWAP(this->str[i], this->str[this->strLength - i - 1]);
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
	while (*start != 0)
	{
		char c = *start;
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
		
	this->strLength = j - 1;
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
	while (*end != 0)
	{
		char c = *end;
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

	MyStringAnsi s(start, end - start);
	splited.push_back(s);

	return splited;
}


