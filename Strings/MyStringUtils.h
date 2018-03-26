#ifndef MY_STRING_UTILS
#define MY_STRING_UTILS


#include <stdint.h>
#include <type_traits>

#define RET_VAL(T, enable_cond) \
	typename std::enable_if<enable_cond<T>::value, T>::type


struct MyStringUtils 
{
		
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
	static RET_VAL(T, std::is_floating_point) ToNumber(const char * str, const char * ret = nullptr)
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
		while ((*str >= '0') && (*str <= '9'))
		{
			value = value * 10 + (*str - '0');
			++str;
		}

		if ((*str != '.') && (*str != 'e'))
		{
			if (ret) ret = str;
			return static_cast<T>(value) * sign;
		}

		uint64_t fractValue = 0;
		uint64_t fractLen = 1;
		int64_t expon = 0;
		int exponSign = 1;

		T val = static_cast<T>(value) * sign;

		if (*str == '.')
		{
			++str;
			
			while ((*str >= '0') && (*str <= '9'))
			{
				fractValue = fractValue * 10 + (*str - '0');
				fractLen *= 10;
				++str;
			}

			val = (static_cast<T>(value) + static_cast<T>(fractValue) / fractLen) * sign;
		}
		
		if (*str == 'e')
		{
			//not fully working the same as std::strtod

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

			while ((*str >= '0') && (*str <= '9'))
			{
				expon = expon * 10 + (*str - '0');
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
		
		if (ret) ret = str;
		return val;
	}

	/// <summary>
	/// Convert string to integral number
	/// </summary>
	/// <param name="str">input string</param>
	/// <param name="ret">out: if not null, pointer where we ended</param>
	/// <returns>converte number</returns>
	template <typename T>
	static RET_VAL(T, std::is_integral) ToNumber(const char * str, const char * ret = nullptr)
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
				
		while ((*str >= '0') && (*str <= '9'))
		{
			value = value * 10 + (*str - '0');
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
	static int GetNumDigits(T number)
	{
		int digits = 0;
		if (number < 0) digits = 1; // remove this line if '-' counts as a digit
		while (number) {
			number /= 10;
			digits++;
		}
		return digits;
	}

	/// <summary>
	/// Calculate number of digits in uint8_t
	/// </summary>
	/// <param name="number"></param>
	/// <returns></returns>
	template <>
	static int GetNumDigits(uint8_t x)
	{		
		if (x >= 10)
		{
			if (x >= 100) return 3;
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
	static int GetNumDigits(uint16_t x)
	{		
		if (x >= 1000)
		{
			if (x >= 10000) return 5;
			return 4;
		}
		
		if (x >= 10)
		{
			if (x >= 100) return 3;
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
	static int GetNumDigits(uint32_t x)
	{		
		if (x >= 10000) {
			if (x >= 10000000) {
				if (x >= 100000000) {
					if (x >= 1000000000) return 10;
					return 9;
				}
				return 8;
			}
			if (x >= 100000) {
				if (x >= 1000000) return 7;
				return 6;
			}
			return 5;
		}
		if (x >= 100) {
			if (x >= 1000) return 4;
			return 3;
		}
		if (x >= 10) return 2;
		return 1;
	}

	/// <summary>
	/// Calculate number of digits in uint64_t
	/// </summary>
	/// <param name="number"></param>
	/// <returns></returns>
	template <>
	static int GetNumDigits(uint64_t x)
	{
		if (x >= 10000000000) { //11
			if (x >= 100000000000000) { //15
				if (x >= 100000000000000000) { //18
					if (x >= 1000000000000000000) { //19
						if (x >= 10000000000000000000) return 20;
						return 19;
					}
					return 18;
				}
				if (x >= 1000000000000000) { //16
					if (x >= 10000000000000000) return 17;
					return 16;
				}
				return 15;
			}
			if (x >= 1000000000000) { //13
				if (x >= 10000000000000) return 14;
				return 13;
			}
			if (x >= 100000000000) return 12;
			return 11;
		}

		if (x >= 10000) { //5
			if (x >= 10000000) { //8
				if (x >= 100000000) { //9
					if (x >= 1000000000) return 10;
					return 9;
				}
				return 8;
			}
			if (x >= 100000) { //6
				if (x >= 1000000) return 7;
				return 6;
			}
			return 5;
		}
		if (x >= 100) { //3
			if (x >= 1000) return 4;
			return 3;
		}
		if (x >= 10) return 2;
		return 1;
	}

	

};

#endif