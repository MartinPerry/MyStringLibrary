#include "StringTests.h"

#include <string>
#include <random>
#include <assert.h> 

#include "MyString.h"
#include "MyStringUtils.h"



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
			printf("Failed: %ll", x[i]);
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

