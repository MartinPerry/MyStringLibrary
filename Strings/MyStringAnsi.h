#ifndef MY_STRING_ANSI_H
#define MY_STRING_ANSI_H

class MyStringAnsi;

#include <vector>

#include "./MyStringUtils.h"
#include "./MyStringMacros.h"

#define BUFFER_SIZE 19

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

template <typename Type>
class IStringAnsi
{
public:
	typedef enum SearchAlgorithm { BM = 0, KMP = 1, BF = 2, C_LIB = 3 } SearchAlgorithm;

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

	void Append(const char * str);

	template<typename... Args>
	void AppendFormat(const char * str, Args... args);


	template <typename T>
	typename std::enable_if <std::is_integral<T>::value && std::is_signed<T>::value>::type
	operator += (T number)
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
		
	template <typename T>
	typename std::enable_if <std::is_unsigned<T>::value>::type
	operator += (T number)
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
	
	void operator += (const MyStringAnsi & str)
	{
		this->Append(str.c_str());
	};

	void operator += (const std::string & str)
	{
		this->Append(str.c_str());
	};



	inline char operator [](const int index) const;
	inline char & operator [](const int index);
	
	inline void operator += (const char * str);
	inline void operator += (const char letter);


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

class MyStringAnsi : public IStringAnsi<MyStringAnsi>
{
public:
	using IStringAnsi<MyStringAnsi>::IStringAnsi;
	
	MyStringAnsi::MyStringAnsi(const char * newStr, size_t length)		
	{
		memset(local, 0, sizeof(local));
		size_t bufferSize = length + 1;

		char * str = local;
		if (bufferSize > BUFFER_SIZE)
		{
			str = new char[bufferSize];
			this->SetBufferSizeInternal(bufferSize);
		}

		memcpy(str, newStr, length);
		str[length] = 0;

		this->SetStrInternal(str);
		this->SetLengthInternal(length);

		hashCode = std::numeric_limits<uint32_t>::max();
	}

	MyStringAnsi::MyStringAnsi(const MyStringAnsi &other)
	{
		memcpy(local, other.local, sizeof(local));
		if (this->IsLocal() == false)
		{
			this->CtorInternal(other.c_str());
		}
		this->hashCode = other.hashCode;
	};
	
	MyStringAnsi::MyStringAnsi(MyStringAnsi && other)  /* noexcept needed to enable optimizations in containers */		
	{
		//http://blog.smartbear.com/c-plus-plus/c11-tutorial-introducing-the-move-constructor-and-the-move-assignment-operator/
		memcpy(local, other.local, sizeof(local));
		hashCode = other.hashCode;

		memset(other.local, 0, sizeof(other.local));
		other.hashCode = std::numeric_limits<uint32_t>::max();
	};

	~MyStringAnsi() = default;

	const char * c_str() const
	{
		if (this->IsLocal()) return local;

		uintptr_t addr = 0;
		memcpy(&addr, local + 8, sizeof(uintptr_t));
		return reinterpret_cast<char *>(addr);
	};


	size_t length() const
	{
		if (this->IsLocal()) return local[BUFFER_SIZE];
		return (uint8_t(local[7]) << 24) +
			(uint8_t(local[6]) << 16) +
			(uint8_t(local[5]) << 8) +
			uint8_t(local[4]);
	};

	size_t capacity() const
	{
		if (this->IsLocal()) return BUFFER_SIZE;

		return (uint8_t(local[3]) << 24) +
			(uint8_t(local[2]) << 16) +
			(uint8_t(local[1]) << 8) +
			uint8_t(local[0]);
	};

	friend class IStringAnsi<MyStringAnsi>;

protected:

	//minimal size must be: 
	//uint32_t | uint32_t | uintptr_t | uint8_t
	//capacity | length (if "heap") | address (if "heap") | length (if local), < 0 (if "heap")  
	char local[BUFFER_SIZE + 1];



	void ReleaseInternal()
	{
		char * str = this->str();
		if (str != this->local)
		{
			delete[] str;
			str = nullptr;
		}

		memset(this->local, 0, sizeof(local));
		this->hashCode = std::numeric_limits<uint32_t>::max();
		this->SetStrInternal(str);
	};

	void CtorInternal(const char * newStr)
	{
		memset(local, 0, sizeof(local));
		if (newStr == nullptr)
		{
			this->local[0] = 0;
			this->SetLengthInternal(0);
			return;
		}

		size_t strLength = strlen(newStr);

		//if strlen < 16 -> use local buffer, [15] = 0
		//if strlen >= 16 -> use "heap"

		char * str = this->local;

		if (strLength >= BUFFER_SIZE)
		{
			size_t bufferSize = strLength + 1;
			this->SetBufferSizeInternal(bufferSize);

			str = new char[bufferSize];
		}
		this->SetStrInternal(str);

		memcpy(str, newStr, strLength + 1);	//copy with "null-termination"		
		this->SetLengthInternal(strLength);
	};


	char * str()
	{
		if (this->IsLocal()) return local;

		uintptr_t addr = 0;
		memcpy(&addr, local + 8, sizeof(uintptr_t));
		return reinterpret_cast<char *>(addr);
		//return this->strp;
	};

	bool IsLocal() const
	{
		return local[BUFFER_SIZE] >= 0;
	};

	void SetBufferSizeInternal(size_t s)
	{
		memcpy(this->local, &s, sizeof(size_t));
	};

	void SetLengthInternal(size_t s)
	{
		if (this->IsLocal()) local[BUFFER_SIZE] = static_cast<char>(s);
		else memcpy(this->local + 4, &s, sizeof(size_t));
	};

	void SetStrInternal(const char * s)
	{
		if (s == local) return;

		uintptr_t addr = reinterpret_cast<uintptr_t>(s);
		memcpy(local + 8, &addr, sizeof(uintptr_t));

		local[BUFFER_SIZE] = std::numeric_limits<char>::min();

	};
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
#include "./ComparisonOperators.inl"



/*
/// <summary>
/// Append signed number
/// </summary>
template <typename Type>
template <typename T>
inline void IStringAnsi<Type>::operator += (T number)
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
inline void IStringAnsi<Type>::operator += (T number)
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
*/

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

/*
template <typename Type>
template <typename T>
inline void IStringAnsi<Type>::operator+= (const T & str)
{
	this->Append(str.c_str());
}
*/

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

#undef _CRT_SECURE_NO_WARNINGS    //for MSVC - disable MSVC warnings on C functions
#undef _CRT_SECURE_NO_DEPRECATE

#endif