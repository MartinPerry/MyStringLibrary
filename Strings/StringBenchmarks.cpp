#include "StringBenchmarks.h"

#include <string>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <random>
#include <unordered_map>
#include <vector>

#include "StringTests.h"
#include "MyString.h"

#ifndef NOMINMAX
#	define NOMINMAX
#endif

#ifdef _WIN32
#include <windows.h>   // WinApi header
#endif

static const int COLOR_RED = 12;
static const int COLOR_GREEN = 10;
static const int COLOR_YELLOW = 14;
static const int COLOR_WHITE = 15;

void StartColor(int colorID)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, colorID);
#endif
}

void EndColor(int colorID)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, colorID);
#endif
}

StringBenchmarks::StringBenchmarks(int count)
	: COUNT(count)
{
	res = new double[COUNT + 1];

	e = std::default_random_engine(r());
}

StringBenchmarks::~StringBenchmarks()
{
	delete[] res;
}

void StringBenchmarks::LogTestStart(const char * name)
{
	StartColor(COLOR_GREEN);
	std::cout << name << std::endl;
	EndColor(COLOR_WHITE);
}

void StringBenchmarks::ResetArray()
{
	for (int i = 0; i < COUNT; i++) 
	{ 
		res[i] = 0; 
	}
}

void StringBenchmarks::AntiOptimization()
{
	double sum = 0; 
	for (int i = 0; i < COUNT; i++) 
	{ 
		sum += res[i]; 
	} 
	std::cout << "Array sum = " << sum << std::endl;
}

void StringBenchmarks::Start(const std::string & desc)
{
	std::cout << "Starting " << desc << std::endl;
	t1 = std::chrono::high_resolution_clock::now();
}

void StringBenchmarks::End()
{
	t2 = std::chrono::high_resolution_clock::now();
}

void StringBenchmarks::Finish()
{
	this->End();
	this->AntiOptimization();
	this->ResetArray();
	this->PrintTime();
}

void StringBenchmarks::PrintTime()
{
	StartColor(COLOR_YELLOW);
	std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << std::endl;
	EndColor(COLOR_WHITE);
	std::cout << " ======= " << std::endl;
}

void StringBenchmarks::RunExternalTest(std::function<void(int c, double *)> f)
{
	LogTestStart(__func__);

	this->Start("");
	f(COUNT, res);
	this->Finish();

}

void StringBenchmarks::TestShortStrAllocation()
{
	LogTestStart(__func__);

	std::uniform_int_distribution<short> uniform_dist(std::numeric_limits<short>::min(),
		std::numeric_limits<short>::max());

	std::vector<std::string> rnd;
	for (int i = 0; i < COUNT; i++)
	{
		rnd.push_back(std::to_string(uniform_dist(e)).c_str());
	}

	this->Start("MyStringAnsi (literal)");
	for (int i = 0; i < COUNT; i++)
	{
		MyStringAnsi x = "xxxxxxx";
		res[i] += x.length();
	}
	this->Finish();

	this->Start("MySmallStringAnsi (literal)");
	for (int i = 0; i < COUNT; i++)
	{
		MySmallStringAnsi x = "xxxxxxx";
		res[i] += x.length();
	}
	this->Finish();

	this->Start("std::string (literal)");
	for (int i = 0; i < COUNT; i++)
	{
		std::string x = "xxxxxxx";
		res[i] += x.length();
	}
	this->Finish();


	this->Start("MyStringAnsi");
	for (int i = 0; i < COUNT; i++)
	{
		MyStringAnsi x = rnd[i].c_str();
		res[i] += x.length();
	}
	this->Finish();

	this->Start("MySmallStringAnsi");
	for (int i = 0; i < COUNT; i++)
	{
		MySmallStringAnsi x = rnd[i].c_str();
		res[i] += x.length();
	}
	this->Finish();

	this->Start("std::string");
	for (int i = 0; i < COUNT; i++)
	{
		std::string x = rnd[i].c_str();
		res[i] += x.length();
	}
	this->Finish();
}

void StringBenchmarks::TestStringToInt()
{
	LogTestStart(__func__);

	std::uniform_int_distribution<long long> uniform_dist(std::numeric_limits<long long>::min(), 
		std::numeric_limits<long long>::max());

	std::vector<MyStringAnsi> rnd;
	for (int i = 0; i < COUNT; i++)
	{
		rnd.push_back(std::to_string(uniform_dist(e)).c_str());
	}

	this->Start("MyStringUtils::ToNumber");
	for (int i = 0; i < COUNT; i++)
	{
		res[i] = (double)MyStringUtils::ToNumber<long long>(rnd[i].c_str());
	}
	this->Finish();

	this->Start("atoll");
	for (int i = 0; i < COUNT; i++)
	{
		res[i] = (double)atoll(rnd[i].c_str());
	}
	this->Finish();
	
}

void StringBenchmarks::TestStringToDouble()
{
	LogTestStart(__func__);

	std::uniform_real_distribution<double> uniform_dist2(-1.0, 1.0);

	std::vector<MyStringAnsi> rnd;
	for (int i = 0; i < COUNT / 2; i++)
	{
		rnd.push_back(std::to_string(uniform_dist2(e)).c_str());
	}

	std::uniform_int_distribution<long long> uniform_dist(std::numeric_limits<long long>::min(),
		std::numeric_limits<long long>::max());
	for (int i = COUNT / 2; i < COUNT; i++)
	{
		rnd.push_back(std::to_string(uniform_dist(e)).c_str());
	}

	this->Start("MyStringUtils::ToNumber");
	for (int i = 0; i < COUNT; i++)
	{
		res[i] = (double)MyStringUtils::ToNumber<long long>(rnd[i].c_str());
	}
	this->Finish();

	this->Start("std::strtod");
	for (int i = 0; i < COUNT; i++)
	{
		res[i] = (double)std::strtod(rnd[i].c_str(), nullptr);
	}
	this->Finish();
	
}

template <typename T>
void StringBenchmarks::TestAppendIntNumber()
{
	LogTestStart(__func__);
	

	std::uniform_int_distribution<T> uniform_dist(std::numeric_limits<T>::min(),
		std::numeric_limits<T>::max());

	std::vector<T> rnd;
	for (int i = 0; i < COUNT; i++)
	{
		rnd.push_back(uniform_dist(e));
	}
	
	//MyStringAnsi tmp(COUNT * 20);
	//std::string tmp2;
	//tmp2.reserve(COUNT * 20);
	
	MyStringAnsi tmp = "";
	MySmallStringAnsi tmp1 = "";
	std::string tmp2 = "";
	
	
	this->Start("MyStringAnsi +=");
	for (int i = 0; i < COUNT; i++)
	{
		tmp += rnd[i];		
	}
	this->End();
	this->PrintTime();

	this->Start("MySmallStringAnsi +=");
	for (int i = 0; i < COUNT; i++)
	{
		tmp1 += rnd[i];
	}
	this->End();
	this->PrintTime();

	this->Start("std::string += ");
	for (int i = 0; i < COUNT; i++)
	{
		tmp2 += std::to_string(rnd[i]);
	}
	this->End();
	this->PrintTime();


	if (strcmp(tmp.c_str(), tmp2.c_str()) != 0)
	{
		printf("Number conversion failed");
	}
	if (strcmp(tmp1.c_str(), tmp2.c_str()) != 0)
	{
		printf("Number conversion failed");
	}
	
}

void StringBenchmarks::TestAppendNumberAll()
{
	//this->TestAppendNumber<int8_t>();
	this->TestAppendIntNumber<int16_t>();	
	this->TestAppendIntNumber<int32_t>();
	this->TestAppendIntNumber<int64_t>();

	//this->TestAppendNumber<uint8_t>();
	this->TestAppendIntNumber<uint16_t>();
	this->TestAppendIntNumber<uint32_t>();
	this->TestAppendIntNumber<uint64_t>();
}

void StringBenchmarks::TestAppendSmallString()
{
	LogTestStart(__func__);

	// Seed with a real random value, if available
	std::random_device r;
	std::default_random_engine e(r());

	std::uniform_int_distribution<int> uniform_dist(1, 10);

	std::vector<std::string> rnd;
	for (int i = 0; i < COUNT; i++)
	{
		rnd.push_back(StringTests<MyStringAnsi>::CreateRandomString(uniform_dist(e)));
	}

	rnd.push_back("");
	rnd.push_back(" ");
		

	this->Start("MyStringAnsi +=");
	for (int i = 0; i < COUNT; i++)
	{
		MyStringAnsi tmp = "";
		tmp += rnd[i];
		res[i] = static_cast<double>(tmp.length());
	}
	this->Finish();

	this->Start("MySmallStringAnsi +=");
	for (int i = 0; i < COUNT; i++)
	{
		MySmallStringAnsi tmp1 = "";
		tmp1 += rnd[i];
		res[i] = static_cast<double>(tmp1.length());
	}
	this->Finish();

	this->Start("std::string += ");
	for (int i = 0; i < COUNT; i++)
	{
		std::string tmp2 = "";
		tmp2 += rnd[i];
		res[i] = static_cast<double>(tmp2.length());
	}
	this->Finish();


}

void StringBenchmarks::TestAppendString()
{
	LogTestStart(__func__);

	// Seed with a real random value, if available
	std::random_device r;
	std::default_random_engine e(r());

	std::uniform_int_distribution<int> uniform_dist(1, 100);

	std::vector<std::string> rnd;
	for (int i = 0; i < COUNT; i++)
	{
		rnd.push_back(StringTests<MyStringAnsi>::CreateRandomString(uniform_dist(e)));
	}

	rnd.push_back("");
	rnd.push_back(" ");

	MyStringAnsi tmp = "";
	MySmallStringAnsi tmp1 = "";
	std::string tmp2 = "";


	this->Start("MyStringAnsi +=");
	for (int i = 0; i < COUNT; i++)
	{
		tmp += rnd[i];
	}
	this->End();
	this->PrintTime();

	this->Start("MySmallStringAnsi +=");
	for (int i = 0; i < COUNT; i++)
	{
		tmp1 += rnd[i];
	}
	this->End();
	this->PrintTime();

	this->Start("std::string += ");
	for (int i = 0; i < COUNT; i++)
	{
		tmp2 += rnd[i];
	}
	this->End();
	this->PrintTime();


	if (strcmp(tmp.c_str(), tmp2.c_str()) != 0)
	{
		printf("String append failed");
	}
	if (strcmp(tmp1.c_str(), tmp2.c_str()) != 0)
	{
		printf("String append failed");
	}

}


void StringBenchmarks::TestHashing()
{
	LogTestStart(__func__);

	std::unordered_map<MyStringId, int> dataId;
	std::unordered_map<MyStringAnsi, int> dataString;

	std::vector<MyStringAnsi> keys;
	std::vector<MyStringId> keysId;
	for (int i = 0; i < COUNT; i++)
	{
		MyStringAnsi key = StringTests<MyStringAnsi>::CreateRandomString(10);
		MyStringId keyId = key;

		keys.push_back(key);
		keysId.push_back(keyId);

		dataId[keyId] = i;
		dataString[key] = i;
	}



	this->Start("Hash MyStringAnsi");
	for (int i = 0; i < COUNT; i++)
	{		
		res[i] = dataString[keys[i]];
	}
	this->Finish();


	this->Start("Hash MyStringID");
	for (int i = 0; i < COUNT; i++)
	{
		res[i] = dataId[keysId[i]];
	}
	this->Finish();

}