#include "StringTests.h"

#include <string>
#include <random>
#include <assert.h> 
#include <vector>
#include <time.h>

#include "MyString.h"
#include "MyStringUtils.h"

std::string StringTests::CreateRandomString(int len)
{
	srand(static_cast<unsigned int>(time(0)));

	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	std::string r = "";
	for (int i = 0; i < len; ++i) {
		r += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return r;
}

void StringTests::TestCtors()
{
	printf("==== Ctors (%s) ==== ", __func__);

	MyStringAnsi m14 = "abcdefghijklmo"; //use local
	MyStringAnsi m15 = "abcdefghijklmop"; //use local
	MyStringAnsi m16 = "abcdefghijklmopq"; //use "heap"


	std::string s14 = "abcdefghijklmo";
	std::string s15 = "abcdefghijklmop";
	std::string s16 = "abcdefghijklmopq";

	if (strcmp(m14.c_str(), s14.c_str()) != 0)
	{
		StringTests::error("ctor not working");
	}

	if (strcmp(m15.c_str(), s15.c_str()) != 0)
	{
		StringTests::error("ctor not working");
	}

	if (strcmp(m16.c_str(), s16.c_str()) != 0)
	{
		StringTests::error("ctor not working");
	}

	printf(" OK \n");
}

void StringTests::TestStringToIntNumber()
{
	printf("==== String to number (%s) ==== ", __func__);
	
	// Seed with a real random value, if available
	std::random_device r;

	// Choose a random mean between 1 and 6
	std::default_random_engine e1(r());
	std::uniform_int_distribution<long long> uniform_dist(std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max());

	
	for (int i = 0; i < 500; i++)
	{
		std::string tmp = std::to_string(uniform_dist(e1));

		long long r1 = MyStringUtils::ToNumber<long long>(tmp.c_str());
		long long r2 = atoll(tmp.c_str());

		if (r1 != r2)
		{
			StringTests::error("Number conversion not working");
		}
	}


	for (long long i = -500; i < 500; i++)
	{
		std::string tmp = std::to_string(i);

		long long r1 = MyStringUtils::ToNumber<long long>(tmp.c_str());
		long long r2 = atoll(tmp.c_str());

		if (r1 != r2)
		{
			StringTests::error("Number conversion not working");
		}
	}

	printf(" OK \n");
}

void StringTests::TestStringToRealNumber()
{
	printf("==== String to number (%s) ==== ", __func__);

	// Seed with a real random value, if available
	std::random_device r;

	// Choose a random mean between 1 and 6
	std::default_random_engine e1(r());
	std::uniform_int_distribution<long long> uniform_dist(std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max());


	for (int i = 0; i < 500; i++)
	{
		std::string tmp = std::to_string(uniform_dist(e1));

		double r1 = MyStringUtils::ToNumber<double>(tmp.c_str());
		double r2 = std::strtod(tmp.c_str(), nullptr);

		if (r1 != r2)
		{
			StringTests::error("Number conversion not working");
		}
	}


	for (long long i = -500; i < 500; i++)
	{
		std::string tmp = std::to_string(i);

		double r1 = MyStringUtils::ToNumber<double>(tmp.c_str());
		double r2 = std::strtod(tmp.c_str(), nullptr);

		if (r1 != r2)
		{
			StringTests::error("Number conversion not working");
		}
	}


	// Choose a random mean between 1 and 6
	std::uniform_real_distribution<double> uniform_dist_double(-1.0, 1.0);
	for (int i = 0; i < 500; i++)
	{
		std::string tmp = std::to_string(uniform_dist_double(e1));

		double r1 = MyStringUtils::ToNumber<double>(tmp.c_str());
		double r2 = std::strtod(tmp.c_str(), nullptr);

		if (r1 != r2)
		{
			StringTests::error("Number conversion not working");
		}
	}


	/*
	std::vector<std::string> manualNegative = { "3.1e-1", "3.14159e-000", "3.14159e-001", "3e-000", "3e-010",
		"3e000", "3.14159e-025", "3.14159e-125" };

	for (auto tmp : manualNegative)
	{
		double r1 = MyStringUtils::ToNumber<double>(tmp.c_str());
		double r2 = std::strtod(tmp.c_str(), nullptr);

		if (r1 != r2)
		{
			StringTests::error("Number conversion not working");
		}
	}

	std::vector<std::string> manualPositive = { "3.1e1", "3.14159e+000", "3.14159e+001", "3e+000", "3e+010", 
		"3e000", "3.14159e+025", "3.14159e+125" };

	for (auto tmp : manualPositive)
	{
		double r1 = MyStringUtils::ToNumber<double>(tmp.c_str());
		double r2 = std::strtod(tmp.c_str(), nullptr);
		
		if (r1 != r2)
		{
			StringTests::error("Number conversion not working");
		}
	}
	*/


	printf(" OK \n");
}

template <typename T>
void StringTests::TestAppendIntNumber()
{
	printf("==== Append number (%s) ==== ", __func__);

	// Seed with a real random value, if available
	std::random_device r;
	std::default_random_engine e(r());

	std::uniform_int_distribution<T> uniform_dist(std::numeric_limits<T>::min(),
		std::numeric_limits<T>::max());

	std::vector<T> rnd;
	//for (int i = 0; i < 500; i++)
	for (int i = 0; i < 500; i++)
	{
		rnd.push_back(uniform_dist(e));
	}
	for (int i = -250; i < 250; i++)
	{
		rnd.push_back(static_cast<T>(i));
	}
	rnd.push_back(std::numeric_limits<T>::min());
	rnd.push_back(std::numeric_limits<T>::max());
	rnd.push_back(T(100001));
	rnd.push_back(T(10001));

	//if it fails - index is in x
	std::vector<T> x;
	for (size_t i = 0; i < rnd.size(); i++)
	{
		int j = MyStringUtils::GetNumDigits(rnd[i]);
		for (int k = 0; k < j; k++)
		{
			x.push_back(rnd[i]);
		}
	}

	MyStringAnsi r1 = "";
	std::string r2 = "";

	for (size_t i = 0; i < rnd.size(); i++)
	{
		r1 += rnd[i];
	}
	for (size_t i = 0; i < rnd.size(); i++)
	{
		r2 += std::to_string(rnd[i]);
	}


	if (r1.length() != r2.length())
	{
		StringTests::error("Number conversion not working");
	}

	for (size_t i = 0; i < r1.length(); i++)
	{		
		if (r1[i] != r2[i])
		{			
			//printf("Failed: %ll", x[i]);
			StringTests::error("Number conversion not working");
		}
	}
	
	printf(" OK \n");
}

void StringTests::TestAppendNumberAll()
{
	//this->TestAppendNumber<int8_t>();
	StringTests::TestAppendIntNumber<int16_t>();	
	StringTests::TestAppendIntNumber<int32_t>();
	StringTests::TestAppendIntNumber<int64_t>();
	
	//this->TestAppendNumber<uint8_t>();
	StringTests::TestAppendIntNumber<uint16_t>();
	StringTests::TestAppendIntNumber<uint32_t>();
	StringTests::TestAppendIntNumber<uint64_t>();
}

void StringTests::TestAppendString()
{
	printf("==== Append string (%s) ==== ", __func__);

	// Seed with a real random value, if available
	std::random_device r;
	std::default_random_engine e(r());

	std::uniform_int_distribution<int> uniform_dist(1, 1000);

	std::vector<std::string> rnd;	
	for (int i = 0; i < 500; i++)
	{
		rnd.push_back(StringTests::CreateRandomString(uniform_dist(e)));
	}
	
	rnd.push_back("");
	rnd.push_back(" ");
	
	MyStringAnsi r1 = "";
	std::string r2 = "";

	for (size_t i = 0; i < rnd.size(); i++)
	{
		r1 += rnd[i];
	}
	for (size_t i = 0; i < rnd.size(); i++)
	{
		r2 += rnd[i];
	}


	if (strcmp(r1.c_str(), r2.c_str()) != 0)
	{
		StringTests::error("String append not working");
	}

	printf(" OK \n");
}

void StringTests::TestMethods()
{
	//========================================================================
	//for trim
	std::vector<const char *> inputs = {" 1111 111 ", "xxxx    ", "   ", " 11", 
		"xxxxxxxxxxxxxxxx", " xxxxxxxxxxxxxxxx   "};
	std::vector<const char *> outputs = { "1111 111", "xxxx", "", "11",
		"xxxxxxxxxxxxxxxx", "xxxxxxxxxxxxxxxx"};

	for (size_t i = 0; i < inputs.size(); i++)
	{
		MyStringAnsi x1 = inputs[i];
		x1.Trim();

		if (strcmp(x1.c_str(), outputs[i]) != 0)
		{
			StringTests::error("Trim not working");
		}
	}

	//========================================================================
	//for reverse
	inputs = { "kobylamamalybok", "hello world", "x", "xax", "pepa" };
	outputs = { "kobylamamalybok", "dlrow olleh", "x", "xax", "apep" };

	for (size_t i = 0; i < inputs.size(); i++)
	{
		MyStringAnsi x1 = inputs[i];
		x1.Reverse();

		if (strcmp(x1.c_str(), outputs[i]) != 0)
		{
			StringTests::error("Reverse not working");
		}
	}
	
	//========================================================================
	//for remove multiple chars
	inputs = { " xxx xxx ", "hellox hello", "x", "xax", "xxaxxxaxx" };
	outputs = { " x x ", "hellox hello", "x", "xax", "xaxax" };

	for (size_t i = 0; i < inputs.size(); i++)
	{
		MyStringAnsi x1 = inputs[i];
		x1.RemoveMultipleChars('x');		

		if (strcmp(x1.c_str(), outputs[i]) != 0)
		{
			StringTests::error("Reverse not working");
		}
	}

	//========================================================================
	//for split (not keep empty)
	inputs = { " xxx xxx ", "hellox hello", "x", "xax", "xxaxxxaxx", " xx xx   xx" };
	std::vector<int> outputs2 = { 2, 2, 1, 1, 1, 3 };

	for (size_t i = 0; i < inputs.size(); i++)
	{
		MyStringAnsi x1 = inputs[i];
		auto v = x1.Split<MyStringAnsi>(' ');

		if (v.size() != outputs2[i])
		{
			StringTests::error("Reverse not working");
		}
	}

	//========================================================================
	//for split (keep empty)
	inputs = { " xxx xxx ", "hellox hello", "x", "xax", "xxaxxxaxx", " xx xx   xx" };
	outputs2 = { 4, 2, 1, 1, 1, 6 };

	for (size_t i = 0; i < inputs.size(); i++)
	{
		MyStringAnsi x1 = inputs[i];
		auto v = x1.Split<MyStringAnsi>(' ', true);

		if (v.size() != outputs2[i])
		{
			StringTests::error("Reverse not working");
		}
	}

	//========================================================================
	//
}

