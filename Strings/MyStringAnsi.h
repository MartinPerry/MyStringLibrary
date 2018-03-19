#ifndef MY_STRING_ANSI_H
#define MY_STRING_ANSI_H


#include <vector>

#include "./MyStringUtils.h"
#include "./MyStringMacros.h"

#define BUFFER_SIZE 16


class MyStringAnsi
{
public:
	typedef enum SearchAlgorithm { BM = 0, KMP = 1, BF = 2, C_LIB = 3 } SearchAlgorithm;

	static const int npos = -1;

	MyStringAnsi();
	MyStringAnsi(char * str);
	MyStringAnsi(const char * str);
	MyStringAnsi(const char * str, size_t length);
	MyStringAnsi(const std::string & str);
	MyStringAnsi(const MyStringAnsi & other); //copy ctor
	MyStringAnsi(MyStringAnsi && other);	//move ctor
	MyStringAnsi(size_t bufferSize);
	
	~MyStringAnsi();

	static MyStringAnsi LoadFromFile(const MyStringAnsi & fileName);
	static MyStringAnsi CreateFormated(const char * str, ...);

	//MyStringAnsi CreateReplaced(const char * src, const char * dest) const;

	//======================================================================
	void Release();
	bool SaveToFile(const MyStringAnsi & fileName) const;

	
	const char * c_str() const { return this->str; };	
	size_t length() const { return this->strLength; };
	size_t capacity() const 
	{ 
		if (this->str == local) return BUFFER_SIZE;		
		size_t bufferSize = 0;
		memcpy(&bufferSize, local, sizeof(size_t));
		return bufferSize;
	};


	void Trim();
	void Reverse();
	void RemoveMultipleChars(char t);

	std::vector<MyStringAnsi> Split(char delimeter, bool keepEmptyValues = false) const;

	/*
	uint32_t GetHashCode() const;
	uint32_t GetRawHashCode() const;
	char GetCharAt(int index) const;
	char GetLastChar() const;

	void FillWideString(wchar_t * &str) const;
	void FillString(char * &str) const;
	void FillString(const char * &str) const;
	bool IsEmpty() const;

	void Append(const char * str);
	void AppendFormat(const char * str, ...);	
	void Replace(const MyStringAnsi & src, const MyStringAnsi & dest);
	void Replace(const char * src, const char * dest);
	void Replace(const char * src, const char * dest, int offset);
	void Replace(const char * search, const char * replace, const std::vector<int> & searchStartPos);
	
	
	

	int Find(const MyStringAnsi & str, SearchAlgorithm algo = C_LIB) const;
	int Find(const char * str, SearchAlgorithm algo = C_LIB) const;
	int Find(const char * str, int offset) const;
	std::vector<int> FindAll(const char * str) const;
	
	

	MyStringAnsi SubString(int start) const;
	MyStringAnsi SubString(int start, int length) const;
	void CopySubstring(int start, char * destination) const;
	void CopySubstring(int start, int length, char * destination) const;
	

	int Count(const MyStringAnsi & str) const;
	int Count(const char * str) const;
	
	void ToUpper();
	void ToLower();		
	int Compare(const MyStringAnsi & str) const;

	int LevenshteinDistance(const MyStringAnsi & str2) const;
	int LevenshteinDistance(const char * str2) const;

	MyStringAnsi & operator = (const MyStringAnsi & str);
	MyStringAnsi & operator = (const char * str);
	MyStringAnsi & operator = (const std::string & str);

	MyStringAnsi & operator = (MyStringAnsi&& other);

	void operator += (const MyStringAnsi & str);
	void operator += (const char * str);
	void operator += (const char letter);
	*/

	template <typename T,
		typename = typename std::enable_if<std::is_signed<T>::value>::type,
		typename = typename std::enable_if<std::is_integral<T>::value>::type>
	void operator += (T number);
		
	template <typename T,
		typename = typename std::enable_if<std::is_unsigned<T>::value>::type>
	void operator += (T number);
	
	inline char operator [](const int index) const;

	/*
	void operator += (const float number);
	void operator += (const double number);

	char & operator [](const int index);
	
	MyStringAnsi operator + (const MyStringAnsi & str) const;
	MyStringAnsi operator + (const char * str) const;	
	MyStringAnsi operator + (const double number) const;
	*/
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>	
	MyStringAnsi operator + (T number) const;
	
	//operator uint32_t() const { return GetHashCode(); }


	//conversion
	/*
	int ToInt() const;
	double ToReal() const;
	bool IsNumber() const;
	bool IsIntNumber() const;
	bool IsFloatNumber() const;
	std::vector<double> GetAllNumbers() const;
	

	//friends
	friend MyStringAnsi operator +(int value, const MyStringAnsi & right);
	*/
	
private:


	char local[BUFFER_SIZE];	
	char * str;	

	//char * str;
	size_t strLength;	
	//size_t bufferSize;

	mutable uint32_t hashCode;
	

	size_t CalcNewBufferSize(size_t forLen) const
	{	
		return CalcNewBufferSize(this->capacity(), forLen);		
	};

	size_t CalcNewBufferSize(size_t curSize, size_t forLen) const
	{		
		size_t newSize = curSize + static_cast<size_t>(curSize * 0.6);
		if (newSize < forLen + 1)
		{
			newSize = forLen + 1;
		}

		return newSize;
	};
	
	void ResizeBuffer(size_t bufferSize);
	void CreateNew(const char * str, size_t length);
	void Ctor(const char * str);
	/*
	int CLib(const char * str, int start = 0) const;
	int BruteForce(const char * str, int start = 0) const;
	int BoyerMoore(const char * str, int * &lookUp, int start = 0) const;
	int KnuthMorisPrat(const char * str, int * &lookUp, int start = 0) const;

	char * strtok_single(char * str, char const * delims) const;
	*/

};


/*
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
*/
#include "./ComparisonOperators.inl"

static const char* const conversions[] = {
"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
"30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
"40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
"50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
"60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
"70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
"80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
"90", "91", "92", "93", "94", "95", "96", "97", "98", "99"
};


/// <summary>
/// Append signed number
/// </summary>
template <typename T, typename, typename>
void MyStringAnsi::operator += (T number)
{
	//uint64_t input = number;
	std::make_unsigned<T>::type input = number;

	int sign = 0;
	if (number < 0)
	{		
		sign = 1;
		input = -number;
	}

	size_t len = MyStringUtils::GetNumDigits(input) + sign;
	//sizeof(T) = 1 => 4 (3 + sign)
	//sizeof(T) = 2 => 6 (5 + sign)
	//sizeof(T) = 4 => 11 (10 + sign)
	//sizeof(T) = 8 => 21 (20 + sign)
	
	size_t curSize = this->capacity();
	if (curSize <= this->strLength + len)
	{
		curSize = CalcNewBufferSize(curSize, this->strLength + len);
		this->ResizeBuffer(curSize);
	}
	
	if (len == 1)
	{
		this->str[this->strLength++] = static_cast<char>(number + '0');
		this->str[this->strLength] = 0;
		return;
	}


	size_t i = this->strLength + len - 1;
	while (input > 9)
	{
		std::make_unsigned<T>::type x = input / 100;
		const char * t = conversions[input - 100 * x];
		this->str[i--] = t[1];
		this->str[i--] = t[0];
		input = x;
	};

	if (input != 0)
	{
		this->str[i--] = static_cast<char>(input + '0');
	}
	if (sign)
	{
		this->str[i] = '-';
	}

	this->strLength += len;

	this->str[this->strLength] = 0;
};

/// <summary>
/// Append unsigned number
/// </summary>
template <typename T, typename>
void MyStringAnsi::operator += (T number)
{
	size_t len = MyStringUtils::GetNumDigits(number);
	//sizeof(T) = 1 => 4 (3 + sign)
	//sizeof(T) = 2 => 6 (5 + sign)
	//sizeof(T) = 4 => 11 (10 + sign)
	//sizeof(T) = 8 => 21 (20 + sign)

	size_t curSize = this->capacity();
	if (curSize <= this->strLength + len)
	{
		curSize = CalcNewBufferSize(curSize, this->strLength + len);
		this->ResizeBuffer(curSize);
	}

	if (len == 1)
	{
		this->str[this->strLength++] = static_cast<char>(number + '0');
		this->str[this->strLength] = 0;
		return;
	}
	
	size_t i = this->strLength + len - 1;
	while (number > 9)
	{
		T x = number / 100;
		const char * t = conversions[number - 100 * x];
		this->str[i--] = t[1];
		this->str[i--] = t[0];
		number = x;
	};

	if (number != 0)
	{
		this->str[i] = static_cast<char>(number + '0');
	}
	
	
	this->strLength += len;

	this->str[this->strLength] = 0;
};



template <typename T, typename>
MyStringAnsi MyStringAnsi::operator + (T number) const
{
	MyStringAnsi newStr = *this;
	newStr += number;
	return newStr;
}


inline char MyStringAnsi::operator [](const int index) const
{
	return this->str[index];
}

#undef _CRT_SECURE_NO_WARNINGS    //for MSVC - disable MSVC warnings on C functions
#undef _CRT_SECURE_NO_DEPRECATE

#endif