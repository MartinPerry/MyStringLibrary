#ifndef STRING_BENCHMARKS_H
#define STRING_BENCHMARKS_H

#include <chrono>
#include <random>
#include <functional>

namespace mystrlib
{

	struct StringBenchmarks
	{

		StringBenchmarks(int count);
		~StringBenchmarks();
		
		void RunAll();

		void TestShortStrAllocation();
		void TestStringToInt();
		void TestStringToDouble();

		template <typename T>
		void TestAppendIntNumber();
		void TestAppendNumberAll();

		void TestAppendWithDigitsCount();

		void TestAppendSmallString();
		void TestAppendString();

		void TestHashing();

		void TestTrim();
		void TestReverse();
		void TestRemoveMultipleChars();
		void TestReplace();
		void TestFind();
		void TestSplit();
		void TestSubstring();

		void RunExternalTest(std::function<void(int, double*)> f);


	private:
		const int COUNT;
		double* res;

		std::random_device r;
		std::default_random_engine e;

		std::chrono::high_resolution_clock::time_point t1;
		std::chrono::high_resolution_clock::time_point t2;

		bool isReferenceRun;
		int64_t lastDt;

		void ResetArray();
		void AntiOptimization();
		void Start(const std::string& desc);
		void End();
		void PrintTime();
		void Finish();
		void FinishReferenceRun();
		void LogTestStart(const char* name);

		static std::string TrimStd(const std::string& input);
		static std::string RemoveMultipleCharsStd(const std::string& input, char c);
		static std::string ReplaceAllStd(const std::string& input, const std::string& what, const std::string& with);
		static std::vector<std::string> SplitStd(const std::string& input, char delim, bool keepEmpty);

	};

}

#endif // !_STRING_BENCHMARKS_H_


