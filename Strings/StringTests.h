#ifndef _STRING_TESTS_H_
#define _STRING_TESTS_H_

#include <string>

struct StringTests 
{
	static void TestCtors();
	static void TestStringToIntNumber();
	static void TestStringToRealNumber();

	template <typename T>
	static void TestAppendIntNumber();

	static void TestAppendNumberAll();

	static void TestMethods();

protected:
	inline static void error(const std::string& s)
	{
		printf("%s\n", s.c_str());
		throw std::runtime_error(s);
	};
};

#endif
