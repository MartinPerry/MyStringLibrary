#ifndef INTERFACE_STRING_ANSI_H
#define INTERFACE_STRING_ANSI_H

class MyStringAnsi;

#include <vector>

#include "./MyStringUtils.h"
#include "./MyStringMacros.h"

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

typedef enum SearchAlgorithm { BM = 0, KMP = 1, BF = 2, C_LIB = 3 } SearchAlgorithm;
typedef enum StringConstants { REPLACE_ALL = -1 } StringConstants;

template <typename Type>
class IStringAnsi
{
public:
		
	static const int npos = -1;

	IStringAnsi();
	IStringAnsi(char * str);
	IStringAnsi(const char * str);
	IStringAnsi(const std::string & str);

	IStringAnsi(size_t bufferSize);

	virtual ~IStringAnsi();

	template <typename RetVal>
	static RetVal LoadFromFile(const char * fileName);

	template <typename RetVal>
	static RetVal CreateFormated(const char * str, ...);

	//IStringAnsi CreateReplaced(const char * src, const char * dest) const;

	//======================================================================
	void Release();
	bool SaveToFile(const char * fileName) const;




	void Trim();
	void Reverse();
	void RemoveMultipleChars(char t);

	template <typename RetVal>
	std::vector<RetVal> Split(char delimeter, bool keepEmptyValues = false) const;

	size_t Count(const char str) const;

	void Append(const char * str, size_t len = 0);

	template<typename... Args>
	void AppendFormat(const char * str, Args... args);




	void Replace(const MyStringAnsi & oldValue, const MyStringAnsi & newValue);
	void Replace(const char * oldValue, const char * newValue);
	void Replace(const char * oldValue, const char * newValue, int replaceOffset);
	void Replace(const char * oldValue, const char * newValue, const std::vector<int> & searchStartPos);


	Type SubString(int start) const;
	Type SubString(int start, size_t length) const;
	void CopySubstring(int start, char ** destination) const;
	void CopySubstring(int start, size_t length, char ** destination) const;

	std::vector<double> GetAllNumbers() const;

	bool IsNumber() const;
	bool IsIntNumber() const;
	bool IsFloatNumber() const;


	template <typename T>	
	RET_VAL(void, std::is_integral<T>::value && std::is_signed<T>) operator += (T number);

	template <typename T>	
	RET_VAL(void, std::is_unsigned<T>) operator += (T number);

	void operator += (const MyStringAnsi & str);
	void operator += (const std::string & str);
	void operator += (const char * str);
	void operator += (const char letter);


	inline char operator [](const int index) const;
	inline char & operator [](const int index);


	//template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>	
	//IStringAnsi operator + (T number) const;

	//operator uint32_t() const { return GetHashCode(); }


protected:

	mutable uint32_t hashCode;

	size_t CalcNewBufferSize(size_t forLen) const
	{
		return CalcNewBufferSize(static_cast<const Type *>(this)->capacity(), forLen);
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

	int CLib(const char * str, size_t start = 0) const;
	int BruteForce(const char * str, size_t start = 0) const;
	int BoyerMoore(const char * str, int * &lookUp, size_t start = 0) const;
	int KnuthMorisPrat(const char * str, int * &lookUp, size_t start = 0) const;


};



/*
//For use in std::unordered_map
//http://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key
namespace std
{
	template <>
	struct hash<IStringAnsi>
	{
		std::size_t operator()(const IStringAnsi & k) const
		{
			return k.GetHashCode();
		};
	};
};
*/




/// <summary>
/// Append signed number
/// </summary>
template <typename Type>
template <typename T>
RET_VAL(void, std::is_integral<T>::value && std::is_signed<T>) IStringAnsi<Type>::operator += (T number)
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

	size_t curSize = static_cast<Type *>(this)->capacity();
	size_t strLength = static_cast<Type *>(this)->length();
	if (curSize <= strLength + len)
	{
		curSize = CalcNewBufferSize(curSize, strLength + len);
		this->ResizeBuffer(curSize);
	}

	char * str = static_cast<Type *>(this)->str();

	if (len == 1)
	{
		str[strLength++] = static_cast<char>(number + '0');
		str[strLength] = 0;
		static_cast<Type *>(this)->SetLengthInternal(strLength);
		this->hashCode = std::numeric_limits<uint32_t>::max();
		return;
	}


	size_t i = strLength + len - 1;
	while (input > 9)
	{
		std::make_unsigned<T>::type x = input / 100;
		const char * t = conversions[input - 100 * x];
		str[i--] = t[1];
		str[i--] = t[0];
		input = x;
	};

	if (input != 0)
	{
		str[i--] = static_cast<char>(input + '0');
	}
	if (sign)
	{
		str[i] = '-';
	}

	strLength += len;

	str[strLength] = 0;
	static_cast<Type *>(this)->SetLengthInternal(strLength);
	this->hashCode = std::numeric_limits<uint32_t>::max();
};

/// <summary>
/// Append unsigned number
/// </summary>
template <typename Type>
template <typename T>
RET_VAL(void, std::is_unsigned<T>) IStringAnsi<Type>::operator += (T number)
{
	size_t len = MyStringUtils::GetNumDigits(number);
	//sizeof(T) = 1 => 4 (3 + sign)
	//sizeof(T) = 2 => 6 (5 + sign)
	//sizeof(T) = 4 => 11 (10 + sign)
	//sizeof(T) = 8 => 21 (20 + sign)

	size_t curSize = static_cast<Type *>(this)->capacity();
	size_t strLength = static_cast<Type *>(this)->length();

	if (curSize <= strLength + len)
	{
		curSize = CalcNewBufferSize(curSize, strLength + len);
		this->ResizeBuffer(curSize);
	}

	char * str = static_cast<Type *>(this)->str();

	if (len == 1)
	{
		str[strLength++] = static_cast<char>(number + '0');
		str[strLength] = 0;
		static_cast<Type *>(this)->SetLengthInternal(strLength);
		this->hashCode = std::numeric_limits<uint32_t>::max();
		return;
	}

	size_t i = strLength + len - 1;
	while (number > 9)
	{
		T x = number / 100;
		const char * t = conversions[number - 100 * x];
		str[i--] = t[1];
		str[i--] = t[0];
		number = x;
	};

	if (number != 0)
	{
		str[i] = static_cast<char>(number + '0');
	}

	strLength += len;

	str[strLength] = 0;
	static_cast<Type *>(this)->SetLengthInternal(strLength);
	this->hashCode = std::numeric_limits<uint32_t>::max();
};


/*
template <typename Type>
template <typename T>
IStringAnsi<Type> IStringAnsi<Type>::operator + (T number) const
{
IStringAnsi newStr = *this;
newStr += number;
return newStr;
}
*/

template <typename Type>
void IStringAnsi<Type>::operator += (const MyStringAnsi & str)
{
	this->Append(str.c_str(), str.length());
};

template <typename Type>
void IStringAnsi<Type>::operator += (const std::string & str)
{
	this->Append(str.c_str(), str.length());
};

template <typename Type>
inline void IStringAnsi<Type>::operator+= (const char * str)
{
	if (str == nullptr)
	{
		return;
	}
	this->Append(str);
}


/// <summary>
/// Add single char to string
/// </summary>
/// <param name="singleChar"></param>
template <typename Type>
inline void IStringAnsi<Type>::operator+= (const char singleChar)
{
	size_t curSize = static_cast<Type *>(this)->capacity();
	size_t strLength = static_cast<Type *>(this)->length();

	if (curSize <= strLength + 1)
	{
		curSize = CalcNewBufferSize(curSize, strLength + 1);
		this->ResizeBuffer(curSize);
	}

	char * str = static_cast<Type *>(this)->str();

	str[strLength++] = singleChar;
	str[strLength] = 0;
	static_cast<Type *>(this)->SetLengthInternal(strLength);

	this->hashCode = std::numeric_limits<uint32_t>::max();
}

template <typename Type>
inline char IStringAnsi<Type>::operator [](const int index) const
{
	return static_cast<const Type *>(this)->c_str()[index];
}

template <typename Type>
inline char & IStringAnsi<Type>::operator [](const int index)
{
	this->hashCode = std::numeric_limits<uint32_t>::max();
	return static_cast<Type *>(this)->str()[index];
}



/// <summary>
/// Split string by a char delimeter
/// Eg: 11  11  22 -> ' ' => {11,11,22}
/// Eg: 11  11  22 -> ' ', keepEmptyValues => {11,,11,,22}
/// 
/// </summary>
/// <param name="delimeter"></param>
/// <param name="keepEmptyValues">if we want to keep empty values (default: false)</param>
/// <returns></returns>
template <typename Type>
template <typename RetVal>
std::vector<RetVal> IStringAnsi<Type>::Split(char delimeter, bool keepEmptyValues) const
{
	std::vector<RetVal> splited;

	const char * end = static_cast<const Type *>(this)->c_str();
	const char * start = end;
	char c = 0;
	while ((c = *end) != 0)
	{
		if (c == delimeter)
		{
			//splitting
			size_t len = end - start;
			if (len == 0)
			{
				if (keepEmptyValues)
				{
					RetVal s("");
					splited.push_back(s);
				}
			}
			else
			{
				RetVal s(start, len);
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
			RetVal s("");
			splited.push_back(s);
		}
	}
	else
	{
		RetVal s(start, len);
		splited.push_back(s);
	}

	return splited;
}




#endif
