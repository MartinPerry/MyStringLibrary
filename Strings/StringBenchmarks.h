#ifndef _STRING_BENCHMARKS_H_
#define _STRING_BENCHMARKS_H_

#include <chrono>
#include <random>
#include <functional>


struct StringBenchmarks
{

	StringBenchmarks(int count);
	~StringBenchmarks();

	void TestShortStrAllocation();
	void TestStringToInt();
	void TestStringToDouble();

	template <typename T>
	void TestAppendIntNumber();
	void TestAppendNumberAll();

	void TestAppendSmallString();
	void TestAppendString();

	void TestHashing();

	void RunExternalTest(std::function<void(int, double *)> f);
		

private:
	const int COUNT;
	double * res;

	std::random_device r;
	std::default_random_engine e;

	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;

	void ResetArray();
	void AntiOptimization();
	void Start(const std::string & desc);
	void End();
	void PrintTime();
	void Finish();
	void LogTestStart(const char * name);
};

#endif // !_STRING_BENCHMARKS_H_


