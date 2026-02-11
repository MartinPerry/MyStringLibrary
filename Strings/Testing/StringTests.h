#ifndef STRING_TESTS_H
#define STRING_TESTS_H

#include <string>
#include <stdexcept>

namespace mystrlib
{

	template <typename T>
	struct StringTests
	{
		static std::string CreateRandomString(int len);

		static void TestCtors();
		static void TestStringToIntNumber();
		static void TestStringToRealNumber();

		template <typename N>
		static void TestAppendIntNumber();
		static void TestAppendNumberAll();

		static void TestAppendString();
		static void TestSubstring();

		static void TestMethods();

		static void TestLocalization();

	protected:
		inline static void error(const std::string& s)
		{
			printf("%s\n", s.c_str());
			throw std::runtime_error(s);
		};
	};
	
}

#endif
