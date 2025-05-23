#ifndef MY_STRING_UTILS
#define MY_STRING_UTILS

class MyStringAnsi;

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <vector>

#include <array>
#include <unordered_map>

#include "./MyStringView.h"
#include "./MyStringMacros.h"

struct MyStringUtils 
{
	static const size_t npos = static_cast<size_t>(-1);

	//https://tombarta.wordpress.com/2008/04/23/specializing-atoi/

	/// <summary>
	/// Convert string to floating point number
	/// Not working same for "e" notation 10e125
	/// -> result differ fropm std::strtod
	/// </summary>
	/// <param name="str">input string</param>
	/// <param name="ret">out: if not null, pointer where we ended</param>
	/// <returns>converte number</returns>
	template <typename T>
	static RET_VAL_STR(T, (std::is_floating_point<T>::value)) ToNumber(const char * str, const char ** ret = nullptr)
	{
		//skip leading whitespace
		while ((*str <= ' ') && (*str != 0))
		{
			++str;
		}

		//check sign
		int sign = 1;
		if (*str == '-') // handle negative
		{
			sign = -1;
			++str;
		}

		uint64_t value = 0;
		char c;
		while ((c = *str) >= '0' && c <= '9')
		{
			value = value * 10 + (c - '0');
			++str;
		}
		
		if ((*str != '.') && (*str != 'e'))
		{
			if (ret) *ret = str;
			return static_cast<T>(value) * sign;
		}
				
		T val;

		if (*str == '.')
		{
			static const uint64_t pow10[15] = {
				1, 10, 100, 1000, 10000,
				100000, 1000000, 10000000, 100000000, 1000000000,
				10000000000, 100000000000, 1000000000000, 10000000000000, 100000000000000
			};

			uint64_t fractValue = 0;
			uint64_t fractLen = 1;

			++str;
			
			const char * startStr = str;
			
			while ((c = *str) >= '0' && c <= '9')
			{
				fractValue = fractValue * 10 + (c - '0');
				++str;
			}
			
			size_t len = str - startStr;
			if (len <= 14)
			{
				fractLen = pow10[len];
			}
			else 
			{
				while (len--)
				{
					fractLen *= 10;
				}
			}

			val = (static_cast<T>(value) + static_cast<T>(fractValue) / fractLen) * sign;
		}
		else 
		{
			val = static_cast<T>(value) * sign;
		}
		


		if (*str == 'e')
		{
			//not fully working the same as std::strtod

			int64_t expon = 0;
			int exponSign = 1;

			++str;			
			if (*str == '-') // handle negative
			{
				exponSign = -1;
				++str;
			}
			else if (*str == '+') // handle positive
			{
				exponSign = +1;
				++str;
			}

			while ((c = *str) >= '0' && c <= '9')
			{
				expon = expon * 10 + (c - '0');
				++str;
			}
								
			
			//pow 10^exp
			if (exponSign == -1)
			{
				while (expon--)
				{
					val /= 10.0;
				}
			}
			else 
			{
				while (expon--)
				{
					val *= 10.0;
				}
			}			
		}
		
		if (ret) *ret = str;
		return val;
	}

	/// <summary>
	/// Convert string to integral number
	/// </summary>
	/// <param name="str">input string</param>
	/// <param name="ret">out: if not null, pointer where we ended</param>
	/// <returns>converte number</returns>
	template <typename T>
	static RET_VAL_STR(T, (std::is_integral<T>::value)) ToNumber(const char * str, const char * ret = nullptr)
	{
		char c;

		//skip leading whitespace		
		while (((c = *str) <= ' ') && (c != 0))
		{
			++str;
		}
		
		//check sign
		int sign = 1;
		if (*str == '-') // handle negative
		{
			sign = -1;
			++str;
		}

		uint64_t value = 0;
						
		while (((c = *str) >= '0') && (c <= '9'))
		{
			value = value * 10 + (c - '0');
			++str;
		}

		if (ret) ret = str;
		return static_cast<T>(value) * sign;
	};
	
	/// <summary>
	/// Universal fallback for number of digits in number
	/// </summary>
	/// <param name="number"></param>
	/// <returns></returns>
	template <typename T>	
	static int GetNumDigits(T number) noexcept
	{
		int digits = 0;
		if (number <= 0) digits = 1; // remove this line if '-' counts as a digit
		while (number) {
			number /= 10;
			digits++;
		}
		return digits;
	}		


	static MyStringAnsi ToStringSimple(double val, int fractPlaces);
	static uint64_t GetFractPartReversed(double val, uint64_t intPart, int fractPlaces) noexcept;
	static uint64_t GetFractPartAsBuffer(double val, uint64_t intPart, int fractPlaces) noexcept;
	static uint64_t ReversDigits(uint64_t num) noexcept;

	static std::vector<std::vector<MyStringAnsi>> LoadCsv(const char* fileName, char delim);


	static size_t SearchBruteForce(MyStringView haystack, MyStringView needle, size_t start = 0);
	static size_t SearchBoyerMoore(MyStringView haystack, MyStringView needle, size_t*& lookUp, size_t start = 0);
	static size_t SearchBoyerMooreHorspool(MyStringView haystack, MyStringView needle, size_t*& lookUp, size_t start = 0);
	static size_t SearchKnuthMorisPrat(MyStringView haystack, MyStringView needle, size_t*& lookUp, size_t start = 0);
	
	friend class AhoCorsick;
	
protected:
	static size_t* BuildBoyerMooreHorspoolLookup(MyStringView needle);
	static size_t* BuildKnuthMorisPratBuildFailLookup(MyStringView needle);
	static bool IsSame(const char* str1, const char* str2, size_t len);
};

//===========================================================================================
//===========================================================================================
//===========================================================================================

/// <summary>
/// https://blog.kelynnjeri.me/aho-corasick-algorithm-efficient-string-matching-for-text-processing
/// </summary>
class AhoCorasick
{
public:
	AhoCorasick();
	~AhoCorasick();

	void Release();

	void AddPattern(const std::string& pattern);
	void AddPattern(const char* pattern, size_t patternLength);

	void SearchPatterns(const std::string& haystack);
	bool ContainsPatterns(const std::string& haystack);

private:
	struct TrieNode
	{
		std::unordered_map<uint8_t, TrieNode*> children;

		//flag used for fast check if char is in children
		//each bit represent group of four values
		//so if bit is 1, we must heck map, if it is 0, we dont need to
		uint64_t flag = 0;

		//std::vector<std::string> output;

		TrieNode* fail = nullptr;
		bool hasOutput = false;

		//==================

		TrieNode* try_emplace_child(uint8_t c)
		{
			uint64_t bit = c & (256 - 1); //c % 256
			flag |= (uint64_t(1) << bit);

			auto it = children.find(c);
			if (it != children.end())
			{
				return it->second;
			}


			auto n = new TrieNode();
			children.emplace(c, n);
			return n;
		}

		bool has_child(uint8_t c) const
		{
			uint64_t bit = c & (256 - 1); //c % 256

			if (flag & uint64_t(uint64_t(1) << bit))
			{
				return children.contains(c);
			}

			return false;
		}

		TrieNode* find_child(uint8_t c) const
		{
			auto it = children.find(c);
			return (it == children.end()) ? nullptr : it->second;
		}

		//==================
	};

	TrieNode* root;
	bool failCreated;

	std::unordered_map<TrieNode*, std::vector<std::string>> nodePatterns;
	
	void BuildFailTransitions();
};


//===========================================================================================
//===========================================================================================
//===========================================================================================

/// <summary>
/// Calculate number of digits in uint8_t
/// </summary>
/// <param name="number"></param>
/// <returns></returns>
template <>
inline int MyStringUtils::GetNumDigits(uint8_t x) noexcept
{
	if (x >= 10U)
	{
		if (x >= 100U) return 3;
		return 2;
	}
	return 1;
}

/// <summary>
/// Calculate number of digits in uint16_t
/// </summary>
/// <param name="number"></param>
/// <returns></returns>
template <>
inline int MyStringUtils::GetNumDigits(uint16_t x) noexcept
{
	if (x >= 1000U)
	{
		if (x >= 10000U) return 5;
		return 4;
	}

	if (x >= 10U)
	{
		if (x >= 100U) return 3;
		return 2;
	}

	return 1;
}

/// <summary>
/// Calculate number of digits in uint32_t
/// </summary>
/// <param name="number"></param>
/// <returns></returns>
template <>
inline int MyStringUtils::GetNumDigits(uint32_t x) noexcept
{
	if (x >= 10000U) {
		if (x >= 10000000U) {
			if (x >= 100000000U) {
				if (x >= 1000000000U) return 10;
				return 9;
			}
			return 8;
		}
		if (x >= 100000U) {
			if (x >= 1000000U) return 7;
			return 6;
		}
		return 5;
	}
	if (x >= 100U) {
		if (x >= 1000U) return 4;
		return 3;
	}
	if (x >= 10U) return 2;
	return 1;
}

/// <summary>
/// Calculate number of digits in uint64_t
/// </summary>
/// <param name="number"></param>
/// <returns></returns>
template <>
inline int MyStringUtils::GetNumDigits(uint64_t x) noexcept
{
	if (x >= 10000000000ULL) { //11
		if (x >= 100000000000000ULL) { //15
			if (x >= 100000000000000000ULL) { //18
				if (x >= 1000000000000000000ULL) { //19
					if (x >= 10000000000000000000ULL) return 20;
					return 19;
				}
				return 18;
			}
			if (x >= 1000000000000000ULL) { //16
				if (x >= 10000000000000000ULL) return 17;
				return 16;
			}
			return 15;
		}
		if (x >= 1000000000000ULL) { //13
			if (x >= 10000000000000ULL) return 14;
			return 13;
		}
		if (x >= 100000000000ULL) return 12;
		return 11;
	}

	if (x >= 10000ULL) { //5
		if (x >= 10000000ULL) { //8
			if (x >= 100000000ULL) { //9
				if (x >= 1000000000ULL) return 10;
				return 9;
			}
			return 8;
		}
		if (x >= 100000ULL) { //6
			if (x >= 1000000ULL) return 7;
			return 6;
		}
		return 5;
	}
	if (x >= 100ULL) { //3
		if (x >= 1000ULL) return 4;
		return 3;
	}
	if (x >= 10ULL) return 2;
	return 1;
}

#endif
