#include "./MyStringUtils.h"

#include <locale.h>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cfloat>
#include <cmath>

#include "./MyStringAnsi.h"


/// <summary>
/// Simple conversion of double value to string
/// with given number of fractional places
/// (Does not support "e" notation, output number is always XX.YY)
/// </summary>
/// <param name="val"></param>
/// <param name="fractPlaces"></param>
/// <returns></returns>
MyStringAnsi MyStringUtils::ToStringSimple(double val, int fractPlaces)
{
	bool negative = val < 0;

	if (negative) val *= -1;
	uint64_t intPart = static_cast<uint64_t>(val);

	MyStringAnsi res = (negative) ? "-" : "";
	res += intPart;

	if (fractPlaces > 0)
	{
		if (fractPlaces <= 7)
		{
			uint64_t fractPartReverse = MyStringUtils::GetFractPartAsBuffer(val, intPart, fractPlaces);
			if (fractPartReverse)
			{
				res += '.';
				char * buf = reinterpret_cast<char *>(&fractPartReverse);
				res.Append(buf, fractPlaces);
			}
		}
		else
		{
			uint64_t fractPartReverse = MyStringUtils::GetFractPartReversed(val, intPart, fractPlaces);
			if (fractPartReverse)
			{
				res += '.';

				while (fractPartReverse)
				{
					int cc = (fractPartReverse % 10);
					fractPartReverse /= 10;
					res += (char)(cc + '0');
				}
			}
		}
	}

	return res;
}

/// <summary>
/// Get fractional part of number in reversed order
/// Eg: for 45.789 => 987
/// </summary>
/// <param name="val">number</param>
/// <param name="intPart">integer part of number</param>
/// <param name="fractPlaces">how many places we want</param>
/// <returns></returns>
uint64_t MyStringUtils::GetFractPartReversed(double val, uint64_t intPart, int fractPlaces) noexcept
{
	double decimalMult = std::pow(10, fractPlaces);

	double fractPart = val - intPart;

	//reversed number is without leading zeroes
	//eg: 0.0157 => will reverse to 751
	//but we need one leading zero
	uint64_t fractPartReverse = MyStringUtils::ReversDigits((uint64_t)(fractPart * decimalMult));
	if (fractPartReverse == 0)
	{
		return fractPartReverse;
	}

	//calculate number of leading zeroes
	int fractLeadingZeros = 0;
	while (fractPart < 1)
	{
		fractLeadingZeros++;
		fractPart *= 10;
	}
	fractLeadingZeros--;

	//move reversed part 
	//751 => 7510
	while (fractLeadingZeros > 0)
	{
		fractPartReverse *= 10;
		fractLeadingZeros--;
	}

	return fractPartReverse;
}

/// <summary>
/// Get fractional part directly as a buffer (stored inside uint64_t)
/// This can support maximal 7 fractPlaces
/// </summary>
/// <param name="val"></param>
/// <param name="intPart"></param>
/// <param name="fractPlaces"></param>
/// <returns></returns>
uint64_t MyStringUtils::GetFractPartAsBuffer(double val, uint64_t intPart, int fractPlaces) noexcept
{
	double decimalMult = std::pow(10, fractPlaces);

	double fractPart = val - intPart;
	uint64_t fractInt = (uint64_t)(fractPart * decimalMult);

	uint64_t res = 0x3030303030303030; //fill buffer with '0'
	uint8_t * buf = reinterpret_cast<uint8_t *>(&res);

	buf[fractPlaces] = 0; //set terminating NULL to last fractPlace
	
	while (fractInt > 0)
	{
		fractPlaces--;
		buf[fractPlaces] = (uint8_t)(fractInt % 10 + '0');
		fractInt = fractInt / 10;
	}

	return res;
}

/// <summary>
/// Reverse digits in input number
/// Input: 123
/// Output: 321
/// </summary>
/// <param name="num"></param>
/// <returns></returns>
uint64_t MyStringUtils::ReversDigits(uint64_t num) noexcept
{
	if (num < 10) return num;

	uint64_t rev_num = 0;
	while (num > 0)
	{
		rev_num = rev_num * 10 + num % 10;
		num = num / 10;
	}
	return rev_num;
}

/// <summary>
/// Simple load for CSV
/// CSV is splitted with delimeter, but this method
/// does not handle delimeter inside strings ""
/// </summary>
/// <param name="fileName"></param>
/// <param name="delim"></param>
/// <returns></returns>
std::vector<std::vector<MyStringAnsi>> MyStringUtils::LoadCsv(const char* fileName, char delim)
{
	std::vector<std::vector<MyStringAnsi>> res;

	MyStringAnsi tmp = MyStringAnsi::LoadFromFile(fileName);

	auto lines = tmp.Split({ '\n', '\r' });

	for (auto & line : lines)
	{
		res.emplace_back(line.Split(delim));
	}

	return res;
}


//====================================================================
// Searching
//====================================================================

/// <summary>
/// Perfrom Boyer-Moore searching. Last function can be passed in
/// in "last". If "last" is NULL, last function is calculated
/// and filled to "last" array
/// !Important!"last" array must be freed outside this method
/// </summary>
/// <param name="haystack">where to find</param>
/// <param name="needle">text to find</param>
/// <param name="last">pointer to array of last function (in / out)</param>
/// <param name="start">start position of searching (default: 0)</param>
/// <returns>position of occurence needle in haystack</returns>
size_t MyStringUtils::SearchBoyerMoore(MyStringView haystack, MyStringView needle, size_t*& last, size_t start)
{
	const char* str = haystack.c_str();
	size_t needleLen = needle.length();


	if (needleLen == 0)
	{
		return MyStringUtils::npos;
	}

	size_t strLength = haystack.length();

	if (last == nullptr)
	{
		last = new size_t[static_cast<int>(std::numeric_limits<uint8_t>::max()) + 1];
		size_t val = MyStringUtils::npos;
		std::fill(last, last + std::numeric_limits<uint8_t>::max(), val);
		for (size_t i = 0; i < strLength; i++)
		{
			last[static_cast<uint8_t>(str[i])] = i;
		}
	}

	size_t index = needleLen - 1;
	size_t cmpIndex = index;
	index += start;
	while (index < strLength)
	{
		if (str[index] == needle[cmpIndex])
		{
			if (cmpIndex == 0)
			{
				return index;
			}

			index--;
			cmpIndex--;
		}
		else
		{
			size_t offset = last[static_cast<int>(str[index])];
			index = index + needleLen - ((cmpIndex < offset + 1) ? cmpIndex : offset + 1);
			cmpIndex = needleLen - 1;
		}
	}


	return MyStringUtils::npos;

}

/// <summary>
/// Calculate last function lookup
/// !Important! returned array must be freed outside this method
/// </summary>
/// <param name="needle">text to find</param>
/// <returns>pointer to array of last function</returns>
size_t* MyStringUtils::KnuthMorisPratBuildFailLookup(MyStringView needle)
{
	size_t needleLen = needle.length();

	size_t* failFce = new size_t[needleLen];
	
	//buil Fail fce
	failFce[0] = 0;
	size_t index = 1;
	size_t cmpIndex = 0;

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

	return failFce;
}

/// <summary>
/// Perfrom KMP (KnuthMorisPrat) searching. Last function can be passed in
/// in "last". If "last" is NULL, last function is calculated
/// and filled to "last" array
/// !Important!"last" array must be freed outside this method
/// </summary>
/// <param name="haystack">where to find</param>
/// <param name="needle">text to find</param>
/// <param name="last">pointer to array of last function (in / out)</param>
/// <param name="start">start position of searching (default: 0)</param>
/// <returns>position of occurence needle in haystack</returns>
size_t MyStringUtils::SearchKnuthMorisPrat(MyStringView haystack, MyStringView needle, size_t*& last, size_t start)
{
	size_t needleLen = needle.length();

	if (needleLen == 0)
	{
		return MyStringUtils::npos;
	}

	size_t index = 1;
	size_t cmpIndex = 0;
	size_t* failFce = last;
	size_t strLen = haystack.length();
	const char* str = haystack.c_str();

	if (failFce == nullptr)
	{
		failFce = MyStringUtils::KnuthMorisPratBuildFailLookup(needle);		
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
				cmpIndex = failFce[cmpIndex - 1];
			}
		}
	}


	return MyStringUtils::npos;
}

/// <summary>
/// Perfrom BF searching. Suitable for small strings
/// </summary>
/// <param name="haystack">where to find</param>
/// <param name="needle">text to find</param>
/// <param name="start">start position of searching (default: 0)</param>
/// <returns>position of occurence needle in haystack</returns>
size_t MyStringUtils::SearchBruteForce(MyStringView haystack, MyStringView needle, size_t start)
{
	size_t needleLen = needle.length();
	size_t i = start;
	size_t j = 0;
	size_t lastPos = MyStringUtils::npos;
	size_t strLen = haystack.length();
	const char* str = haystack.c_str();

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
				lastPos = MyStringUtils::npos;
				break;
			}
		}

		if (lastPos != MyStringUtils::npos)
		{
			return (lastPos - needleLen);
		}
	}

	return MyStringUtils::npos;
}