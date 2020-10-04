#include "./MyStringUtils.h"

#include <locale.h>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cfloat>

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

uint64_t MyStringUtils::GetFractPartReversed(double val, uint64_t intPart, int fractPlaces)
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
uint64_t MyStringUtils::GetFractPartAsBuffer(double val, uint64_t intPart, int fractPlaces)
{
	double decimalMult = std::pow(10, fractPlaces);

	double fractPart = val - intPart;
	uint64_t fractInt = (uint64_t)(fractPart * decimalMult);

	uint64_t res = 0x3030303030303030; //fill buffer with '0'
	uint8_t * buf = reinterpret_cast<uint8_t *>(&res);

	buf[fractPlaces] = 0; //set terminating NULL to last fractPlace

	uint64_t rev_num = 0;
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
uint64_t MyStringUtils::ReversDigits(uint64_t num)
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

