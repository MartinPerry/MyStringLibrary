#include "StringBenchmarks.h"

#include <string>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <random>

#include "MyString.h"

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
	std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << std::endl;
}

void StringBenchmarks::TestStringToInt()
{
	
	std::cout << __func__ << std::endl;

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
		res[i] = MyStringUtils::ToNumber<long long>(rnd[i].c_str());
	}
	this->Finish();

	this->Start("atoll");
	for (int i = 0; i < COUNT; i++)
	{
		res[i] = atoll(rnd[i].c_str());
	}
	this->Finish();
	
}

void StringBenchmarks::TestStringToDouble()
{
	
	std::cout << __func__ << std::endl;

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
		res[i] = MyStringUtils::ToNumber<long long>(rnd[i].c_str());
	}
	this->Finish();

	this->Start("std::strtod");
	for (int i = 0; i < COUNT; i++)
	{
		res[i] = std::strtod(rnd[i].c_str(), nullptr);
	}
	this->Finish();
	
}

template <typename T>
void StringBenchmarks::TestAppendIntNumber()
{
	std::cout << __func__ << std::endl;

	std::uniform_int_distribution<T> uniform_dist(std::numeric_limits<T>::min(),
		std::numeric_limits<T>::max());

	std::vector<T> rnd;
	for (int i = 0; i < COUNT; i++)
	{
		rnd.push_back(uniform_dist(e));
	}
	
	MyStringAnsi tmp(COUNT * 20);
	std::string tmp2;
	tmp2.reserve(COUNT * 20);
	
	
	this->Start("MyString +=");
	for (int i = 0; i < COUNT; i++)
	{
		tmp += rnd[i];		
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
		printf("Number conversion not working");
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