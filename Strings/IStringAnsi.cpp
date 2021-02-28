#include "./IStringAnsi.h"


#include <cstdio>
#include <cstdlib>
#include <cctype>


#include <string.h>
#include <locale>
#include <cmath>


#include "./MurmurHash3.h"

#include "./MyStringAnsi.h"
#include "./MySmallStringAnsi.h"
#include "./MyStringView.h"

//====================================================================
// ctors / dtors
//====================================================================


template <typename Type>
IStringAnsi<Type>::IStringAnsi() : 
	hashCode(std::numeric_limits<uint32_t>::max())
{
	//static_cast<Type *>(this)->CtorInternal(nullptr);
	static_cast<Type *>(this)->DefaultInit();
}


template <typename Type>
IStringAnsi<Type>::IStringAnsi(size_t bufferSize) : 
	hashCode(std::numeric_limits<uint32_t>::max())
{
	if (bufferSize > Type::BUFFER_SIZE)
	{
		static_cast<Type *>(this)->SetBufferSizeInternal(bufferSize);

		char * str = new char[bufferSize];
		str[0] = 0;

		static_cast<Type *>(this)->SetLengthInternal(0);
		static_cast<Type *>(this)->SetStrInternal(str);
	}
	else
	{
		static_cast<Type *>(this)->CtorInternal(nullptr, 0);
	}
}

template <typename Type>
IStringAnsi<Type>::IStringAnsi(char * str) : 
	hashCode(std::numeric_limits<uint32_t>::max())
{
	static_cast<Type *>(this)->CtorInternal(str, 0);
}

template <typename Type>
IStringAnsi<Type>::IStringAnsi(const char * str) : 
	hashCode(std::numeric_limits<uint32_t>::max())
{
	static_cast<Type *>(this)->CtorInternal(str, 0);
}



template <typename Type>
IStringAnsi<Type>::IStringAnsi(const std::string & str) : 
	hashCode(std::numeric_limits<uint32_t>::max())
{
	static_cast<Type *>(this)->CtorInternal(str.c_str(), str.length());
}



template <typename Type>
IStringAnsi<Type>::~IStringAnsi()
{
	static_cast<Type *>(this)->ReleaseInternal();
}

/// <summary>
/// Manually release current data
/// and allocate back to "empty" string
/// </summary>
template <typename Type>
void IStringAnsi<Type>::Release()
{
	static_cast<Type *>(this)->ReleaseInternal();
	static_cast<Type *>(this)->CtorInternal(nullptr, 0);
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
		if (str == nullptr)
		{
			//both strings are null
			return;
		}
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

	char * newStr = new char[newBufferSize];

	char * str = static_cast<Type *>(this)->str();	
	if (str != nullptr)
	{
		size_t strLength = static_cast<const Type *>(this)->length();
		if (strLength != 0)
		{
			memcpy(newStr, str, strLength + 1);	//copy with "null-termination"
		}

		if (static_cast<const Type *>(this)->IsLocal() == false)
		{
			delete[] str;
		}
	}

	static_cast<Type *>(this)->SetStrInternal(newStr);
	static_cast<Type *>(this)->SetBufferSizeInternal(newBufferSize);
	
}



//====================== Methods ===============================

template <typename Type>
uint32_t IStringAnsi<Type>::GetHashCode() const noexcept
{
	if (this->hashCode != std::numeric_limits<uint32_t>::max())
	{
		return this->hashCode;
	}

	this->hashCode = MurmurHash3_x86_32(static_cast<const Type *>(this)->c_str(), 
		static_cast<uint32_t>(static_cast<const Type *>(this)->length()));
	
	//const volatile uint32_t * ptr = &this->hashCode;
	//(*const_cast<uint32_t*>(ptr)) = hash;

	return this->hashCode;
}



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

/// <summary>
/// Transform every character of string with a transfrom callback
/// </summary>
/// <param name="t"></param>
template <typename Type>
void IStringAnsi<Type>::Transform(std::function<char(char)> t)
{
	//size_t count = 0;

	char * end = static_cast<Type *>(this)->str();
	char c = 0;
	while ((c = *end) != 0)
	{
		*end = t(c);

		end++;
	}

	this->hashCode = std::numeric_limits<uint32_t>::max();
}


template <typename Type>
void IStringAnsi<Type>::Append(const char * appendStr, size_t len)
{
	if (appendStr == nullptr)
	{
		return;
	}

	if (len == 0)
	{
		len = strlen(appendStr);
	}
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


/// <summary>
/// Clear string, but keep allocated memory
/// Just set [0] = 0
/// </summary>
template <typename Type>
void IStringAnsi<Type>::Clear()
{
	char * start = static_cast<Type *>(this)->str();
	start[0] = 0;

	static_cast<Type *>(this)->SetLengthInternal(0);
	this->hashCode = std::numeric_limits<uint32_t>::max();
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
	
	if (tmp != start)
	{
		//move only if trimming from begining is needed
		memmove(start, tmp, newLength);
	}

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



template <typename Type>
void IStringAnsi<Type>::RemoveChar(char t)
{
	char * str = static_cast<Type *>(this)->str();
	size_t j = 0;	
	char * start = str;
	char c = 0;
	while ((c = *start) != 0)
	{		
		start++;

		if (c == t)
		{
			continue;
		}
		
		str[j] = c;
		j++;
	}
	str[j] = 0;
		
	static_cast<Type *>(this)->SetLengthInternal(j);
	this->hashCode = std::numeric_limits<uint32_t>::max();
}

/// <summary>
/// Remove same characters given by parameter t from sequence
/// Eg: t = x => remove x -> xxxaaxx => xaax
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

	static_cast<Type *>(this)->SetLengthInternal(j);
	this->hashCode = std::numeric_limits<uint32_t>::max();
}


/// <summary>
/// Remove all ASCII characters that are not printable (<= 31)
/// (but do not remove 0 as it is string end)
/// </summary>
template <typename Type>
void IStringAnsi<Type>::RemoveNonPrintableChars()
{
	char* str = static_cast<Type*>(this)->str();
	size_t j = 0;
	char* start = str;
	char c = 0;
	while ((c = *start) != 0)
	{
		start++;

		if ((c > 0) && (c <= 31))
		{
			continue;
		}

		str[j] = c;
		j++;
	}
	str[j] = 0;

	static_cast<Type*>(this)->SetLengthInternal(j);
	this->hashCode = std::numeric_limits<uint32_t>::max();
}

template <typename Type>
void IStringAnsi<Type>::PopBack()
{
	char * str = static_cast<Type *>(this)->str();
	size_t length = static_cast<const Type *>(this)->length();

	str[length - 1] = 0;
		
	static_cast<Type *>(this)->SetLengthInternal(length - 1);
	this->hashCode = std::numeric_limits<uint32_t>::max();
}


template <typename Type>
void IStringAnsi<Type>::Replace(MyStringView oldValue, MyStringView newValue)
{
	this->Replace(oldValue, newValue, StringConstants::REPLACE_ALL);
}

/// <summary>
/// Replace all "oldValue" with "newValue" Use KMP for finding "oldValue" in text
/// </summary>
/// <param name="oldValue"></param>
/// <param name="newValue"></param>
/// <param name="replaceOffset">if multiple occurence of "oldValue", which one to replace (size_t max = all)</param>
template <typename Type>
void IStringAnsi<Type>::Replace(MyStringView oldValue, MyStringView newValue, size_t replaceOffset)
{
	size_t oldValueLen = oldValue.length();
	size_t * last = nullptr;
	size_t pos = 0;

	std::vector<size_t> startPos;
	size_t foundOffset = 0;

	MyStringView thisView = MyStringView(static_cast<const Type*>(this));


	while (1)
	{
		pos = MyStringUtils::SearchKnuthMorisPrat(thisView, oldValue, last, pos); //better use this, because BM skipping
													   //is calculated from haystack, not needle
		if (pos == IStringAnsi<Type>::npos)
		{ 
			break;  //not found
		}     

		if (replaceOffset == StringConstants::REPLACE_ALL)
		{
			startPos.push_back(pos);    //store all found start positions of search words
		}
		else if (foundOffset == replaceOffset)
		{
			startPos.push_back(pos);    //store found start positions of search words
			break;
		}
		pos += oldValueLen;        //set new search start

		foundOffset++;
	}

	delete[] last;

	this->Replace(oldValue, newValue, startPos);
}

template <typename Type>
void IStringAnsi<Type>::Replace(MyStringView oldValue, MyStringView newValue, const std::vector<size_t> & searchStartPos)
{
	if (searchStartPos.size() == 0)
	{
		//nothing found
		return;
	}

	size_t oldValueLen = oldValue.length();
	size_t newValueLen = newValue.length();
	
	if (oldValueLen == newValueLen)
	{
		char * str = static_cast<Type *>(this)->str();
		for (auto foundPos : searchStartPos)
		{			
			memcpy(str + foundPos, newValue.c_str(), newValueLen);			
		}

		return;
	}



	//new length can be smaller or larger than current length
	size_t newLength = static_cast<const Type *>(this)->length() -
		searchStartPos.size() * oldValueLen + 
		searchStartPos.size() * newValueLen;

	size_t curSize = static_cast<const Type *>(this)->capacity();
	size_t curLen = static_cast<const Type *>(this)->length();
	
	char * str = nullptr;
	char * tmpStr = nullptr;

	char * newStr = nullptr;
	char * tmpNewStr = nullptr;

	bool enlarged = false;
	if (curSize <= newLength)
	{
		curSize = CalcNewBufferSize(curSize, newLength);

		str = static_cast<Type *>(this)->str();
		tmpStr = str;

		newStr = new char[curSize];
		tmpNewStr = newStr;

		enlarged = true;						
	}
	else 
	{
		newStr = static_cast<Type *>(this)->str();
		tmpNewStr = newStr;

		str = new char[curSize];
		tmpStr = str;
		memcpy(str, newStr, curSize);						
	}

	size_t lastPos = 0;

	for (auto foundPos : searchStartPos)
	{
		size_t copyLen = foundPos - lastPos;

		memcpy(newStr, str, copyLen);
		newStr += copyLen;
		str += copyLen;

		memcpy(newStr, newValue.c_str(), newValueLen);
		newStr += newValueLen;
		str += oldValueLen;

		lastPos = foundPos + oldValueLen;
	}

	size_t copyLen = curLen - lastPos;
	memcpy(newStr, str, copyLen);
	newStr[copyLen] = 0;


	if (enlarged)
	{
		if (static_cast<const Type *>(this)->IsLocal() == false)
		{
			delete[] tmpStr;
		}
	}
	else 
	{
		delete[] tmpStr;
	}


	static_cast<Type *>(this)->SetLengthInternal(newLength);
	static_cast<Type *>(this)->SetStrInternal(tmpNewStr);
	if (curSize > Type::BUFFER_SIZE)
	{
		static_cast<Type *>(this)->SetBufferSizeInternal(curSize);
	}

	this->hashCode = std::numeric_limits<uint32_t>::max();
}

template <typename Type>
Type IStringAnsi<Type>::CreateReplaced(MyStringView src, MyStringView dest) const
{
	Type newStr = Type(static_cast<const Type *>(this)->c_str(),
		static_cast<const Type *>(this)->length());
	newStr.Replace(src, dest);
	return newStr;
}

//====================================================================
// Methods for obtaining new data from string
//====================================================================

template <typename Type>
std::vector<double> IStringAnsi<Type>::GetAllNumbers() const
{
	std::vector<double> s;
	s.reserve(100);

	const char * str = static_cast<const Type *>(this)->c_str();
	const char * start = str;
	while (*str)
	{		
		while (*str && ((*str < '0') || (*str > '9')))
		{
			str++;
		}

		if (!*str)
		{
			break;
		}
		else if ((str != start) && (*(str - 1) == '-'))
		{
			str--;
		}

		double n = MyStringUtils::ToNumber<double>(str, &str);
		s.push_back(n);		
	}

	return s;
}

template <typename Type>
bool IStringAnsi<Type>::IsNumber() const
{
	return this->IsFloatNumber();
}

template <typename Type>
bool IStringAnsi<Type>::IsIntNumber() const
{
	size_t length = static_cast<const Type *>(this)->length();
	if (length == 0)
	{
		return false;
	}

	const char * str = static_cast<const Type *>(this)->c_str();

	size_t startPos = 0;
	if (str[0] == '-')
	{
		startPos = 1;
	}

	for (size_t i = startPos; i < length; i++)
	{
		if ((str[i] > '9') || (str[i] < '0'))
		{
			return false;
		}
	}

	return true;
}

template <typename Type>
bool IStringAnsi<Type>::IsFloatNumber() const
{
	size_t length = static_cast<const Type *>(this)->length();
	if (length == 0)
	{
		return false;
	}

	const char * str = static_cast<const Type *>(this)->c_str();

	size_t startPos = 0;
	if (str[0] == '-')
	{
		startPos = 1;
	}

	int decCount = 0;
	for (size_t i = startPos; i < length; i++)
	{
		if ((str[i] == '.') && (decCount == 0))
		{
			decCount++;
			continue;
		}

		if ((str[i] < '0') || (str[i] > '9'))
		{
			return false;
		}
	}

	return true;
}



//====================================================================
// Finding
//====================================================================

/// <summary>
/// Test if string ends with suffix
/// </summary>
/// <param name="needle"></param>
/// <returns></returns>
template <typename Type>
bool IStringAnsi<Type>::EndsWith(MyStringView needle) const noexcept
{
	size_t strLen = static_cast<const Type*>(this)->length();
	const char* str = static_cast<const Type*>(this)->c_str();

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
/// Find char within this string and return first position of
/// occurence
/// </summary>
/// <param name="str">char to find</param>
/// <returns>position of occurence needle in haystack</returns>
template <typename Type>
size_t IStringAnsi<Type>::Find(const char c) const noexcept
{
	size_t strLen = static_cast<const Type *>(this)->length();
	const char * str = static_cast<const Type *>(this)->c_str();

	for (size_t i = 0; i < strLen; i++)
	{
		if (str[i] == c) 
		{
			return i;
		}
	}

	return IStringAnsi<Type>::npos;
}

/// <summary>
/// Find char within this string and return last position of
/// occurence
/// </summary>
/// <param name="str">char to find</param>
/// <returns>position of occurence needle in haystack</returns>
template <typename Type>
size_t IStringAnsi<Type>::FindLast(const char c) const noexcept
{
	size_t strLen = static_cast<const Type *>(this)->length();
	const char * str = static_cast<const Type *>(this)->c_str();

	if (strLen == 0)
	{
		return IStringAnsi<Type>::npos;
	}

	// < strLen - not a bug
	// feature - i wraps around when it goes under zero
	//https://stackoverflow.com/questions/3623263/reverse-iteration-with-an-unsigned-loop-variable
	for (size_t i = strLen - 1; i < strLen; i--)
	{
		if (str[i] == c)
		{
			return i;
		}
	}

	return IStringAnsi<Type>::npos;
}


/// <summary>
/// Find str within this string and return first position of
/// occurence
/// </summary>
/// <param name="str">text to find</param>
/// <param name="algo">specify searching algorithm (default: CLib)</param>
/// <returns>position of occurence needle in haystack</returns>
template <typename Type>
size_t IStringAnsi<Type>::Find(const Type& needle, SearchAlgorithm algo) const
{
	if (needle.c_str() == nullptr)
	{
		return IStringAnsi<Type>::npos;
	}

	if (algo == SearchAlgorithm::DEFAULT)
	{		
		return this->CLib(needle.c_str());
	}

	return this->Find(MyStringView(needle), algo);
}


/// <summary>
/// Find str within this string and return first position of
/// occurence
/// </summary>
/// <param name="str">text to find</param>
/// <param name="algo">specify searching algorithm (default: CLib)</param>
/// <returns>position of occurence needle in haystack</returns>
template <typename Type>
size_t IStringAnsi<Type>::Find(const char* needle, SearchAlgorithm algo) const
{
	if (needle == nullptr)
	{
		return IStringAnsi<Type>::npos;
	}

	if (algo == SearchAlgorithm::DEFAULT)
	{		
		return this->CLib(needle);
	}

	return this->Find(MyStringView(needle), algo);
}


/// <summary>
/// Find str within this string and return first position of
/// occurence
/// The string view does not support SearchAlgorithm::C_LIB
/// because string view does not have to be NULL-terminated
/// and C_LIB requires NULL-terminated needle
/// </summary>
/// <param name="str">text to find</param>
/// <param name="algo">specify searching algorithm (default: BF)</param>
/// <returns>position of occurence needle in haystack</returns>
template <typename Type>
size_t IStringAnsi<Type>::Find(MyStringView needle, SearchAlgorithm algo) const
{
	size_t pos = MyStringUtils::npos;

	if (needle.c_str() == nullptr)
	{
		return pos;
	}

	size_t* last = nullptr;
	
	if (algo == SearchAlgorithm::BM)
	{	
		pos = MyStringUtils::SearchBoyerMoore(static_cast<const Type*>(this), needle, last);
	}
	else if (algo == SearchAlgorithm::KMP)
	{	
		pos = MyStringUtils::SearchKnuthMorisPrat(static_cast<const Type*>(this), needle, last);
	}
	else if (algo == SearchAlgorithm::BF)
	{		
		pos = MyStringUtils::SearchBruteForce(static_cast<const Type*>(this), needle);
	}
	else if (algo == SearchAlgorithm::DEFAULT)
	{				
		//non-null terminated string are not supported with CLib
		//and needle may not bet null terminated
		pos = MyStringUtils::SearchBruteForce(static_cast<const Type*>(this), needle);
	}

	if (last != nullptr)
	{
		delete[] last;
	}

	return pos;
}



template <typename Type>
size_t IStringAnsi<Type>::Find(MyStringView needle, size_t offset) const
{
	size_t count = 0;
	size_t searchLength = needle.length();
	size_t * last = nullptr;
	size_t pos = 0;

	MyStringView thisView = MyStringView(static_cast<const Type*>(this));

	while (1)
	{
		pos = MyStringUtils::SearchKnuthMorisPrat(thisView, needle, last, pos); //better use this, because BM skipping
													//is calculated from haystack, not needle
		if (pos == MyStringUtils::npos)
		{ 
			//not found
			break; 
		}       

		if (count == offset)
		{
			break;
		}

		count++;
		pos += searchLength;        //set new search start
	}

	delete[] last;

	return pos;
}

template <typename Type>
std::vector<size_t> IStringAnsi<Type>::FindAll(MyStringView needle) const
{
	size_t searchLength = needle.length();
	size_t * last = nullptr;
	size_t pos = 0;

	std::vector<size_t> startPos;
	int foundOffset = 0;
	
	MyStringView thisView = MyStringView(static_cast<const Type*>(this));

	while (1)
	{
		pos = MyStringUtils::SearchKnuthMorisPrat(thisView, needle, last, pos); //better use this, because BM skipping
													//is calculated from haystack, not needle
		if (pos == MyStringUtils::npos)
		{ 
			//not found
			break; 
		}        
		startPos.push_back(pos);    //store found start positions of serach words

		pos += searchLength;        //set new search start

		foundOffset++;
	}

	delete[] last;

	return startPos;
}



//====================================================================
// Substrings
//====================================================================

/// <summary>
/// Get substring within string
/// </summary>
/// <param name="start">start position</param>
/// <returns>substring from string</returns>
template <typename Type>
Type IStringAnsi<Type>::SubString(int start) const
{
	size_t length = static_cast<const Type *>(this)->length() - start;

	return this->SubString(start, length);
}


/// <summary>
/// Get substring within string
/// </summary>
/// <param name="start">start position</param>
/// <param name="length">length of substring</param>
/// <returns>substring from string</returns>
template <typename Type>
Type IStringAnsi<Type>::SubString(int start, size_t length) const
{	
	if (start + length > static_cast<const Type *>(this)->length())
	{
		//to do ... hodit vyjimku
	}

	const char * str = static_cast<const Type *>(this)->c_str();

	Type s(length + 1);

	char * newStr = s.str();
	memcpy(newStr, str + start, length);
	newStr[length] = 0;

	s.SetLengthInternal(length);

	return s;
}

/// <summary>
/// Get substring within string
/// String IS null terminated
/// Destination string is allocated with new char[length + 1]
/// </summary>
/// <param name="start">start position</param>
/// <param name="destination">destination, where substring is copied. Must be cleared with delete[]</param>
template <typename Type>
void IStringAnsi<Type>::CopySubstring(int start, char ** destination) const
{
	size_t length = static_cast<const Type *>(this)->length() - start;

	this->CopySubstring(start, length, destination);
}

/// <summary>
/// Get substring within string. 
/// String IS null terminated
/// Destination string is allocated with new char[length + 1]
/// </summary>
/// <param name="start">start position</param>
/// <param name="destination">destination, where substring is copied. Must be cleared with delete[]</param>
template <typename Type>
void IStringAnsi<Type>::CopySubstring(int start, size_t length, char ** destination) const
{
	/*
	if (start + length > this->strLength)
	{
		//to do ... hodit vyjimku
	}
	*/
	*destination = new char[length + 1];

	memcpy(*destination, static_cast<const Type *>(this)->c_str() + start, length);
	(*destination)[length] = 0;
}

/// <summary>
/// Get substring within string
/// String IS NOT null terminated !
/// </summary>
/// <param name="start">start position</param>
/// <param name="destination">destination, where substring is copied</param>
template <typename Type>
void IStringAnsi<Type>::CopySubstring(int start, char * destination) const
{	
	size_t length = static_cast<const Type *>(this)->length() - start;

	this->CopySubstring(start, length, destination);
}

/// <summary>
/// Get substring within string. 
/// String IS NOT null terminated !
/// </summary>
/// <param name="start">start position</param>
/// <param name="destination">destination, where substring is copied</param>
template <typename Type>
void IStringAnsi<Type>::CopySubstring(int start, size_t length, char * destination) const
{
	/*
	if (start + length > this->strLength)
	{
		//to do ... hodit vyjimku
	}
	*/

	memcpy(destination, static_cast<const Type *>(this)->c_str() + start, length);	
}



/// <summary>
/// Count number of char occurences in string
/// </summary>
/// <param name="f"></param>
/// <returns></returns>
template <typename Type>
size_t IStringAnsi<Type>::Count(const char f) const noexcept
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


/// <summary>
/// Perfrom searching using strstr - standard C library
/// !!!! str AND needle must be null terminated !!!
/// </summary>
/// <param name="needle">text to find</param>
/// <param name="start">start position of searching (default: 0)</param>
/// <returns>position of occurence needle in haystack</returns>
template <typename Type>
size_t IStringAnsi<Type>::CLib(const char * needle, size_t start) const
{
	const char * str = static_cast<const Type *>(this)->c_str();
	const char * found = strstr(str + start, needle);
	if (found == nullptr)
	{
		return IStringAnsi<Type>::npos;
	}
	return static_cast<size_t>(found - str);
}



template class IStringAnsi<MyStringAnsi>;
template class IStringAnsi<MySmallStringAnsi>;
//template std::vector<MyStringAnsi> IStringAnsi<MyStringAnsi>::Split(char delimeter, bool keepEmptyValues) const;