#include "StringTests.h"

#include <string>
#include <random>
#include <assert.h> 
#include <vector>
#include <time.h>


#include "./MyStringAnsi.h"
#include "./MySmallStringAnsi.h"
#include "./md5.h"
#include "./MyStringUtils.h"

template <typename T>
std::string StringTests<T>::CreateRandomString(int len)
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

template <typename T>
void StringTests<T>::TestCtors()
{
	printf("==== Ctors (%s) ==== ", __func__);

	T m14 = "abcdefghijklmo"; //use local
	T m15 = "abcdefghijklmop"; //use local
	T m16 = "abcdefghijklmopq"; //use "heap"


	std::string s14 = "abcdefghijklmo";
	std::string s15 = "abcdefghijklmop";
	std::string s16 = "abcdefghijklmopq";

	if (strcmp(m14.c_str(), s14.c_str()) != 0)
	{
		StringTests<T>::error("ctor not working");
	}

	if (strcmp(m15.c_str(), s15.c_str()) != 0)
	{
		StringTests<T>::error("ctor not working");
	}

	if (strcmp(m16.c_str(), s16.c_str()) != 0)
	{
		StringTests<T>::error("ctor not working");
	}

	printf(" OK \n");
}

template <typename T>
void StringTests<T>::TestStringToIntNumber()
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
			StringTests<T>::error("Number conversion not working");
		}
	}


	for (long long i = -500; i < 500; i++)
	{
		std::string tmp = std::to_string(i);

		long long r1 = MyStringUtils::ToNumber<long long>(tmp.c_str());
		long long r2 = atoll(tmp.c_str());

		if (r1 != r2)
		{
			StringTests<T>::error("Number conversion not working");
		}
	}

	printf(" OK \n");
}

template <typename T>
void StringTests<T>::TestStringToRealNumber()
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
			StringTests<T>::error("Number conversion not working");
		}
	}


	for (long long i = -500; i < 500; i++)
	{
		std::string tmp = std::to_string(i);

		double r1 = MyStringUtils::ToNumber<double>(tmp.c_str());
		double r2 = std::strtod(tmp.c_str(), nullptr);

		if (r1 != r2)
		{
			StringTests<T>::error("Number conversion not working");
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
			StringTests<T>::error("Number conversion not working");
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
			StringTests<T>::error("Number conversion not working");
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
			StringTests<T>::error("Number conversion not working");
		}
	}
	*/


	printf(" OK \n");
}

template <typename T>
template <typename N>
void StringTests<T>::TestAppendIntNumber()
{
	printf("==== Append number (%s) ==== ", __func__);

	// Seed with a real random value, if available
	std::random_device r;
	std::default_random_engine e(r());

	std::uniform_int_distribution<N> uniform_dist(std::numeric_limits<N>::min(),
		std::numeric_limits<N>::max());

	std::vector<N> rnd;
	//for (int i = 0; i < 500; i++)
	for (int i = 0; i < 500; i++)
	{
		rnd.push_back(uniform_dist(e));
	}
	for (int i = -250; i < 250; i++)
	{
		rnd.push_back(static_cast<N>(i));
	}
	rnd.push_back(std::numeric_limits<N>::min());
	rnd.push_back(std::numeric_limits<N>::max());
	rnd.push_back(N(100001));
	rnd.push_back(N(10001));

	//if it fails - index is in x
	std::vector<N> x;
	for (size_t i = 0; i < rnd.size(); i++)
	{
		int j = MyStringUtils::GetNumDigits(rnd[i]);
		for (int k = 0; k < j; k++)
		{
			x.push_back(rnd[i]);
		}
	}

	T r1 = "";
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
		StringTests<T>::error("Number conversion not working");
	}

	for (size_t i = 0; i < r1.length(); i++)
	{		
		if (r1[i] != r2[i])
		{			
			//printf("Failed: %ll", x[i]);
			StringTests<T>::error("Number conversion not working");
		}
	}
	
	printf(" OK \n");
}

template <typename T>
void StringTests<T>::TestAppendNumberAll()
{
	//this->TestAppendNumber<int8_t>();
	StringTests<T>::TestAppendIntNumber<int16_t>();	
	StringTests<T>::TestAppendIntNumber<int32_t>();
	StringTests<T>::TestAppendIntNumber<int64_t>();
	
	//this->TestAppendNumber<uint8_t>();
	StringTests<T>::TestAppendIntNumber<uint16_t>();
	StringTests<T>::TestAppendIntNumber<uint32_t>();
	StringTests<T>::TestAppendIntNumber<uint64_t>();
}

template <typename T>
void StringTests<T>::TestAppendString()
{
	printf("==== Append string (%s) ==== ", __func__);

	// Seed with a real random value, if available
	std::random_device r;
	std::default_random_engine e(r());

	std::uniform_int_distribution<int> uniform_dist(1, 1000);

	std::vector<std::string> rnd;	
	for (int i = 0; i < 500; i++)
	{
		rnd.push_back(StringTests<T>::CreateRandomString(uniform_dist(e)));
	}
	
	rnd.push_back("");
	rnd.push_back(" ");
	
	T r1 = "";
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
		StringTests<T>::error("String append not working");
	}

	printf(" OK \n");
}

template <typename T>
void StringTests<T>::TestSubstring()
{
	printf("==== Substring (%s) ==== ", __func__);

	// Seed with a real random value, if available
	std::random_device r;
	std::default_random_engine e(r());

	std::uniform_int_distribution<int> uniform_dist(1, 1000);

	std::vector<std::string> rnd;
	std::vector<int> starts;
	std::vector<size_t> lens;
	for (int i = 0; i < 500; i++)
	{
		int len = uniform_dist(e);
		rnd.push_back(StringTests<T>::CreateRandomString(len));

		std::uniform_int_distribution<int> uniform_distStart(1, len);
		int start = uniform_distStart(e);
		starts.push_back(start);

		std::uniform_int_distribution<int> uniform_distLen(0, len - start);
		lens.push_back(uniform_distLen(e));
	}

	


	T r1 = "";
	std::string r2 = "";

	for (size_t i = 0; i < rnd.size(); i++)
	{
		T s = rnd[i];
		r1 += s.SubString(starts[i], lens[i]);
	}
	for (size_t i = 0; i < rnd.size(); i++)
	{
		std::string s = rnd[i];
		r2 += s.substr(starts[i], lens[i]);
	}


	if (strcmp(r1.c_str(), r2.c_str()) != 0)
	{
		StringTests<T>::error("String substring not working");
	}

	printf(" OK \n");
}

template <typename T>
void StringTests<T>::TestMethods()
{	
	//========================================================================
	//for trim

	printf("==== Trim (%s) ==== ", __func__);

	std::vector<const char *> inputs = {" 1111 111 ", "xxxx    ", "   ", " 11", 
		"xxxxxxxxxxxxxxxx", " xxxxxxxxxxxxxxxx   "};
	std::vector<const char *> outputs = { "1111 111", "xxxx", "", "11",
		"xxxxxxxxxxxxxxxx", "xxxxxxxxxxxxxxxx"};

	for (size_t i = 0; i < inputs.size(); i++)
	{		
		T x1 = inputs[i];
		x1.Trim();		

		if (strcmp(x1.c_str(), outputs[i]) != 0)
		{
			StringTests<T>::error("Trim not working");
		}
	}

	printf("OK\n");

	//========================================================================
	//for reverse

	printf("==== Reverse (%s) ==== ", __func__);

	inputs = { "kobylamamalybok", "hello world", "x", "xax", "pepa" };
	outputs = { "kobylamamalybok", "dlrow olleh", "x", "xax", "apep" };

	for (size_t i = 0; i < inputs.size(); i++)
	{
		T x1 = inputs[i];
		x1.Reverse();

		if (strcmp(x1.c_str(), outputs[i]) != 0)
		{
			StringTests<T>::error("Reverse not working");
		}
	}
	
	printf("OK\n");

	//========================================================================
	//for remove multiple chars
	
	printf("==== Remove multiple chars (%s) ==== ", __func__);

	inputs = { " xxx xxx ", "hellox hello", "x", "xax", "xxaxxxaxx" };
	outputs = { " x x ", "hellox hello", "x", "xax", "xaxax" };

	for (size_t i = 0; i < inputs.size(); i++)
	{
		T x1 = inputs[i];
		x1.RemoveMultipleChars('x');		

		if (strcmp(x1.c_str(), outputs[i]) != 0)
		{
			StringTests<T>::error("Reverse not working");
		}
	}

	printf("OK\n");

	//========================================================================
	//for split (not keep empty)
	
	printf("==== Split (not keep empty) (%s) ==== ", __func__);

	inputs = { " xxx xxx ", "hellox hello", "x", "xax", "xxaxxxaxx", " xx xx   xx" };
	std::vector<int> outputs2 = { 2, 2, 1, 1, 1, 3 };

	for (size_t i = 0; i < inputs.size(); i++)
	{
		T x1 = inputs[i];
		auto v = x1.template Split<T>(' ');

		if (v.size() != outputs2[i])
		{
			StringTests<T>::error("Reverse not working");
		}
	}

	printf("OK\n");

	//========================================================================
	//for split (keep empty)

	printf("==== Split (keep empty) (%s) ==== ", __func__);

	inputs = { " xxx xxx ", "hellox hello", "x", "xax", "xxaxxxaxx", " xx xx   xx" };
	outputs2 = { 4, 2, 1, 1, 1, 6 };

	for (size_t i = 0; i < inputs.size(); i++)
	{
		T x1 = inputs[i];
		auto v = x1.template Split<T>(' ', true);

		if (v.size() != outputs2[i])
		{
			StringTests<T>::error("Reverse not working");
		}
	}

	printf("OK\n");

	//========================================================================
	//string replace	

	printf("==== Replace (%s) ==== ", __func__);

	T tmpSmall = "ahoj vojle";
	tmpSmall.Replace("oj", "voj");

	if (strcmp(tmpSmall.c_str(), "ahvoj vvojle") != 0)
	{
		StringTests<T>::error("Replace not working");
	}

	T tmpReplace = "ahoj babi ahoh ahoj baf ahoj";
	tmpReplace.Replace("ahoj", "vole");

	if (strcmp(tmpReplace.c_str(), "vole babi ahoh vole baf vole") != 0)
	{
		StringTests<T>::error("Replace not working");
	}

	printf("OK\n");

	//========================================================================
	//string find

	printf("==== Find (%s) ==== ", __func__);

	T tmpFind = "ahoj vojle ahoj kluku";
	auto all = tmpFind.FindAll("ahoj");

	
	if (all.size() != 2)
	{
		StringTests<T>::error("Find not working");
	}
	if ((all[0] != 0) && (all[1] != 11))
	{
		StringTests<T>::error("Find not working");
	}

	auto ik1 = tmpFind.Find("kluk", SearchAlgorithm::KMP);
	auto ik2 = tmpFind.Find("kluk", SearchAlgorithm::C_LIB);
	auto ik3 = tmpFind.Find("kluk", SearchAlgorithm::BM);
	auto ik4 = tmpFind.Find("kluk", SearchAlgorithm::BF);

	if (ik1 != 16) StringTests<T>::error("Find not working");
	if (ik1 != ik2) StringTests<T>::error("Find not working");
	if (ik1 != ik3) StringTests<T>::error("Find not working");
	if (ik1 != ik4) StringTests<T>::error("Find not working");
	

	printf("OK\n");

	//========================================================================
	//MD5
#ifdef _WIN32
	printf("==== MD5 (%s) ==== ", __func__);

	MD5 md5;
	T md5Hash = md5.digestString("ahoj vole");
	if (strcmp(md5Hash.c_str(), "53a94a062714c8f28de90400642e142c") != 0)
	{
		StringTests<T>::error(std::string("MD5 not working ") + md5Hash.c_str());
	}
	

	md5Hash = md5.digestString("123456 lokomotiva");
	if (strcmp(md5Hash.c_str(), "8ac48676dae783798450238c37bebd93") != 0)
	{
		StringTests<T>::error(std::string("MD5 not working ") + md5Hash.c_str());
	}

	printf("OK\n");
#endif
}


template struct StringTests<MyStringAnsi>;
template struct StringTests<MySmallStringAnsi>;