#include "StringBenchmarks.h"

#include <string>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <random>
#include <unordered_map>
#include <vector>

#include "./StringTests.h"
#include "../MyStringLib.h"

#ifdef _WIN32
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif
#	include <windows.h>   // WinApi header
#endif

using namespace mystrlib;

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

//===============================================================================

StringBenchmarks::StringBenchmarks(int count) : 
	COUNT(count),
	isReferenceRun(false),
	lastDt(0)
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

void StringBenchmarks::FinishReferenceRun()
{
	isReferenceRun = true;
	this->Finish();
	isReferenceRun = false;
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
	auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	
	if ((dt < lastDt) && (isReferenceRun))
	{
		StartColor(COLOR_RED);
	}
	else
	{
		StartColor(COLOR_YELLOW);
	}
	std::cout << "Time: " << dt << " ms" << std::endl;
	EndColor(COLOR_WHITE);
	std::cout << " ======= " << std::endl;

	lastDt = dt;
}

void StringBenchmarks::RunExternalTest(std::function<void(int c, double *)> f)
{
	LogTestStart(__func__);

	this->Start("");
	f(COUNT, res);
	this->Finish();

}

void StringBenchmarks::RunAll()
{
	TestShortStrAllocation();
	TestStringToInt();
	TestStringToDouble();
	TestAppendNumberAll();
	TestAppendSmallString();
	TestAppendString();
	TestHashing();
	TestTrim();
	TestReverse();
	TestRemoveMultipleChars();
	TestReplace();
	TestFind();
	TestSplit();
	TestSubstring();
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

	this->Start("MyString (literal)");
	for (int i = 0; i < COUNT; i++)
	{
		String x = "xxxxxxx";
		res[i] += x.length();
	}
	this->Finish();

	this->Start("MySmallString (literal)");
	for (int i = 0; i < COUNT; i++)
	{
		SmallString x = "xxxxxxx";
		res[i] += x.length();
	}
	this->Finish();

	this->Start("std::string (literal)");
	for (int i = 0; i < COUNT; i++)
	{
		std::string x = "xxxxxxx";
		res[i] += x.length();
	}
	this->FinishReferenceRun();


	this->Start("MyString");
	for (int i = 0; i < COUNT; i++)
	{
		String x = rnd[i].c_str();
		res[i] += x.length();
	}
	this->Finish();

	this->Start("MySmallString");
	for (int i = 0; i < COUNT; i++)
	{
		SmallString x = rnd[i].c_str();
		res[i] += x.length();
	}
	this->Finish();

	this->Start("std::string");
	for (int i = 0; i < COUNT; i++)
	{
		std::string x = rnd[i].c_str();
		res[i] += x.length();
	}
	this->FinishReferenceRun();
}

void StringBenchmarks::TestStringToInt()
{
	LogTestStart(__func__);

	std::uniform_int_distribution<long long> uniform_dist(std::numeric_limits<long long>::min(), 
		std::numeric_limits<long long>::max());

	std::vector<String> rnd;
	for (int i = 0; i < COUNT; i++)
	{
		rnd.push_back(std::to_string(uniform_dist(e)).c_str());
	}

	this->Start("MyStringUtils::ToNumber");
	for (int i = 0; i < COUNT; i++)
	{
		res[i] = (double)StringUtils::ToNumber<long long>(rnd[i].c_str());
	}
	this->Finish();

	this->Start("atoll");
	for (int i = 0; i < COUNT; i++)
	{
		res[i] = (double)atoll(rnd[i].c_str());
	}
	this->FinishReferenceRun();
	
}

void StringBenchmarks::TestStringToDouble()
{
	LogTestStart(__func__);

	std::uniform_real_distribution<double> uniform_dist2(-1.0, 1.0);

	std::vector<String> rnd;
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
		res[i] = (double)StringUtils::ToNumber<double>(rnd[i].c_str());
	}
	this->Finish();

	this->Start("std::strtod");
	for (int i = 0; i < COUNT; i++)
	{
		res[i] = (double)std::strtod(rnd[i].c_str(), nullptr);
	}
	this->FinishReferenceRun();
	
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
	
	String tmp = "";
	SmallString tmp1 = "";
	std::string tmp2 = "";
	
	
	this->Start("MyStringAnsi +=");
	for (int i = 0; i < COUNT; i++)
	{
		tmp += rnd[i];		
	}
	this->End();
	this->PrintTime();

	this->Start("MySmallString +=");
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
	isReferenceRun = true;
	this->PrintTime();
	isReferenceRun = false;


	if (strcmp(tmp.c_str(), tmp2.c_str()) != 0)
	{
		printf("Number conversion failed");
	}
	if (strcmp(tmp1.c_str(), tmp2.c_str()) != 0)
	{
		printf("Number conversion failed");
	}
	
}

void StringBenchmarks::TestAppendWithDigitsCount()
{
	LogTestStart(__func__);

	this->Start("AppendWithDigitsCount");

	for (int i = 0; i < COUNT; i++)
	{
		String x = "";
		x.AppendWithDigitsCount(-123, 5);
		res[i] += x.length();
	}
	
	this->Finish();

	this->Start("AppendFormat");

	for (int i = 0; i < COUNT; i++)
	{
		String x = "";
		x.AppendFormat("%05d", -123);
		res[i] += x.length();
	}

	this->Finish();
	
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
		rnd.push_back(StringTests<String>::CreateRandomString(uniform_dist(e)));
	}

	rnd.push_back("");
	rnd.push_back(" ");
		

	this->Start("MyString +=");
	for (int i = 0; i < COUNT; i++)
	{
		String tmp = "";
		tmp += rnd[i];
		res[i] = static_cast<double>(tmp.length());
	}
	this->Finish();

	this->Start("MySmallString +=");
	for (int i = 0; i < COUNT; i++)
	{
		SmallString tmp1 = "";
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
	this->FinishReferenceRun();


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
		rnd.push_back(StringTests<String>::CreateRandomString(uniform_dist(e)));
	}

	rnd.push_back("");
	rnd.push_back(" ");

	String tmp = "";
	SmallString tmp1 = "";
	std::string tmp2 = "";


	this->Start("MyString +=");
	for (int i = 0; i < COUNT; i++)
	{
		tmp += rnd[i];
	}
	this->End();
	this->PrintTime();

	this->Start("MySmallString +=");
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
	isReferenceRun = true;
	this->PrintTime();
	isReferenceRun = false;


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

	std::unordered_map<StringId, int> dataId;
	std::unordered_map<String, int> dataString;

	std::vector<String> keys;
	std::vector<StringId> keysId;
	for (int i = 0; i < COUNT; i++)
	{
		String key = StringTests<String>::CreateRandomString(10);
		StringId keyId = key;

		keys.push_back(key);
		keysId.push_back(keyId);

		dataId[keyId] = i;
		dataString[key] = i;
	}



	this->Start("Hash MyString");
	for (int i = 0; i < COUNT; i++)
	{		
		res[i] = dataString[keys[i]];
	}
	this->Finish();


	this->Start("Hash MyStringId");
	for (int i = 0; i < COUNT; i++)
	{
		res[i] = dataId[keysId[i]];
	}
	this->Finish();

}

std::string StringBenchmarks::TrimStd(const std::string& input)
{
	const size_t first = input.find_first_not_of(' ');
	if (first == std::string::npos)
	{
		return "";
	}
	const size_t last = input.find_last_not_of(' ');
	return input.substr(first, last - first + 1);
}

std::string StringBenchmarks::RemoveMultipleCharsStd(const std::string& input, char c)
{
	if (input.empty())
	{
		return input;
	}

	std::string out;
	out.reserve(input.size());
	out.push_back(input[0]);

	for (size_t i = 1; i < input.size(); i++)
	{
		if (!(input[i] == c && out.back() == c))
		{
			out.push_back(input[i]);
		}
	}

	return out;
}

std::string StringBenchmarks::ReplaceAllStd(const std::string& input, const std::string& what, const std::string& with)
{
	if (what.empty())
	{
		return input;
	}

	std::string out = input;
	size_t pos = 0;
	while ((pos = out.find(what, pos)) != std::string::npos)
	{
		out.replace(pos, what.length(), with);
		pos += with.length();
	}
	return out;
}

std::vector<std::string> StringBenchmarks::SplitStd(const std::string& input, char delim, bool keepEmpty)
{
	std::vector<std::string> tokens;
	std::string current;

	for (char ch : input)
	{
		if (ch == delim)
		{
			if (keepEmpty || !current.empty())
			{
				tokens.push_back(current);
			}
			current.clear();
		}
		else
		{
			current.push_back(ch);
		}
	}

	if (keepEmpty || !current.empty())
	{
		tokens.push_back(current);
	}

	return tokens;
}

void StringBenchmarks::TestTrim()
{
	LogTestStart(__func__);

	std::uniform_int_distribution<int> uniform_dist(1, 128);
	std::vector<std::string> rnd;
	rnd.reserve(COUNT);

	for (int i = 0; i < COUNT; i++)
	{
		const int len = uniform_dist(e);
		rnd.push_back("   " + StringTests<String>::CreateRandomString(len) + "   ");
	}

	Start("MyString::Trim");
	for (int i = 0; i < COUNT; i++)
	{
		String x = rnd[i].c_str();
		x.Trim();
		res[i] = static_cast<double>(x.length());
	}
	Finish();

	Start("std::string trim equivalent");
	for (int i = 0; i < COUNT; i++)
	{
		std::string x = TrimStd(rnd[i]);
		res[i] = static_cast<double>(x.length());
	}
	FinishReferenceRun();
}

void StringBenchmarks::TestReverse()
{
	LogTestStart(__func__);

	std::uniform_int_distribution<int> uniform_dist(1, 128);
	std::vector<std::string> rnd;
	rnd.reserve(COUNT);

	for (int i = 0; i < COUNT; i++)
	{
		rnd.push_back(StringTests<String>::CreateRandomString(uniform_dist(e)));
	}

	Start("MyString::Reverse");
	for (int i = 0; i < COUNT; i++)
	{
		String x = rnd[i].c_str();
		x.Reverse();
		res[i] = static_cast<double>(x.length());
	}
	Finish();

	Start("std::string + std::reverse");
	for (int i = 0; i < COUNT; i++)
	{
		std::string x = rnd[i];
		std::reverse(x.begin(), x.end());
		res[i] = static_cast<double>(x.length());
	}
	FinishReferenceRun();
}

void StringBenchmarks::TestRemoveMultipleChars()
{
	LogTestStart(__func__);

	std::uniform_int_distribution<int> uniform_dist(1, 128);
	std::vector<std::string> rnd;
	rnd.reserve(COUNT);

	for (int i = 0; i < COUNT; i++)
	{
		std::string base = StringTests<String>::CreateRandomString(uniform_dist(e));
		base += "xxxxxx";
		base += StringTests<String>::CreateRandomString(5);
		base += "xxx";
		rnd.push_back(base);
	}

	Start("MyString::RemoveMultipleChars");
	for (int i = 0; i < COUNT; i++)
	{
		String x = rnd[i].c_str();
		x.RemoveMultipleChars('x');
		res[i] = static_cast<double>(x.length());
	}
	Finish();

	Start("std::string equivalent");
	for (int i = 0; i < COUNT; i++)
	{
		std::string x = RemoveMultipleCharsStd(rnd[i], 'x');
		res[i] = static_cast<double>(x.length());
	}
	FinishReferenceRun();
}

void StringBenchmarks::TestReplace()
{
	LogTestStart(__func__);

	std::uniform_int_distribution<int> uniform_dist(8, 64);
	std::vector<std::string> rnd;
	rnd.reserve(COUNT);

	for (int i = 0; i < COUNT; i++)
	{
		std::string base = StringTests<String>::CreateRandomString(uniform_dist(e));
		base += " ahoj ";
		base += StringTests<String>::CreateRandomString(uniform_dist(e));
		base += " ahoj";
		rnd.push_back(base);
	}

	Start("MyString::Replace");
	for (int i = 0; i < COUNT; i++)
	{
		String x = rnd[i].c_str();
		x.Replace("ahoj", "vole");
		res[i] = static_cast<double>(x.length());
	}
	Finish();

	Start("std::string replace-all equivalent");
	for (int i = 0; i < COUNT; i++)
	{
		std::string x = ReplaceAllStd(rnd[i], "ahoj", "vole");
		res[i] = static_cast<double>(x.length());
	}
	FinishReferenceRun();
}

void StringBenchmarks::TestFind()
{
	LogTestStart(__func__);

	std::uniform_int_distribution<int> uniform_dist(24, 96);
	std::vector<std::string> rnd;
	rnd.reserve(COUNT);

	for (int i = 0; i < COUNT; i++)
	{
		std::string base = StringTests<String>::CreateRandomString(uniform_dist(e));
		base += " target ";
		base += StringTests<String>::CreateRandomString(uniform_dist(e));
		base += " target";
		rnd.push_back(base);
	}

	Start("MyString::Find + FindAll");
	for (int i = 0; i < COUNT; i++)
	{
		String x = rnd[i].c_str();
		const size_t first = x.Find("target", SearchAlgorithm::DEFAULT);
		const auto all = x.FindAll("target");
		res[i] = static_cast<double>(first + all.size());
	}
	Finish();

	Start("std::string::find loop");
	for (int i = 0; i < COUNT; i++)
	{
		const std::string& x = rnd[i];
		size_t first = x.find("target");
		size_t count = 0;
		size_t pos = first;
		while (pos != std::string::npos)
		{
			count++;
			pos = x.find("target", pos + 1);
		}

		if (first == std::string::npos)
		{
			first = 0;
		}
		res[i] = static_cast<double>(first + count);
	}
	FinishReferenceRun();
}

void StringBenchmarks::TestSplit()
{
	LogTestStart(__func__);

	std::uniform_int_distribution<int> uniform_dist(8, 24);
	std::vector<std::string> rnd;
	rnd.reserve(COUNT);

	for (int i = 0; i < COUNT; i++)
	{
		std::string base = StringTests<String>::CreateRandomString(uniform_dist(e));
		base += " ";
		base += StringTests<String>::CreateRandomString(uniform_dist(e));
		base += "  ";
		base += StringTests<String>::CreateRandomString(uniform_dist(e));
		rnd.push_back(base);
	}

	Start("MyString::Split keepEmpty=false");
	for (int i = 0; i < COUNT; i++)
	{
		String x = rnd[i].c_str();
		const auto parts = x.Split<String>(' ');
		res[i] = static_cast<double>(parts.size());
	}
	Finish();

	Start("std::string split keepEmpty=false");
	for (int i = 0; i < COUNT; i++)
	{
		const auto parts = SplitStd(rnd[i], ' ', false);
		res[i] = static_cast<double>(parts.size());
	}
	FinishReferenceRun();

	Start("MyString::Split keepEmpty=true");
	for (int i = 0; i < COUNT; i++)
	{
		String x = rnd[i].c_str();
		const auto parts = x.Split<String>(' ', true);
		res[i] = static_cast<double>(parts.size());
	}
	Finish();

	Start("std::string split keepEmpty=true");
	for (int i = 0; i < COUNT; i++)
	{
		const auto parts = SplitStd(rnd[i], ' ', true);
		res[i] = static_cast<double>(parts.size());
	}
	FinishReferenceRun();
}

void StringBenchmarks::TestSubstring()
{
	LogTestStart(__func__);

	std::uniform_int_distribution<int> uniform_len(20, 120);
	std::vector<std::string> rnd;
	std::vector<int> starts;
	std::vector<size_t> lens;
	rnd.reserve(COUNT);
	starts.reserve(COUNT);
	lens.reserve(COUNT);

	for (int i = 0; i < COUNT; i++)
	{
		const int len = uniform_len(e);
		rnd.push_back(StringTests<String>::CreateRandomString(len));

		std::uniform_int_distribution<int> uniform_start(0, len - 1);
		const int start = uniform_start(e);
		starts.push_back(start);

		std::uniform_int_distribution<int> uniform_sub_len(0, len - start);
		lens.push_back(static_cast<size_t>(uniform_sub_len(e)));
	}

	Start("MyString::SubString");
	for (int i = 0; i < COUNT; i++)
	{
		String x = rnd[i].c_str();
		String sub = x.SubString(starts[i], lens[i]);
		res[i] = static_cast<double>(sub.length());
	}
	Finish();

	Start("std::string::substr");
	for (int i = 0; i < COUNT; i++)
	{
		std::string sub = rnd[i].substr(static_cast<size_t>(starts[i]), lens[i]);
		res[i] = static_cast<double>(sub.length());
	}
	FinishReferenceRun();
}