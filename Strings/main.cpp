
#if __has_include(<vld.h>)
#	define VLD_ENABLED
#	include <vld.h>
#endif

#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <array>
#include <string>
#include <random>
#include <unordered_map>
#include <map>

#include "MyString.h"
#include "./Testing/StringTests.h"
#include "./Testing/StringBenchmarks.h"

#include "./Unicode/UnicodeUtils.h"

#include "./SimpleLocalization/Localization.h"

using namespace std;
using namespace mystrlib;

template <class T>
constexpr void test_helper(T&&) {}

#define IS_CONSTEXPR(...) noexcept(test_helper(__VA_ARGS__))


template <typename T>
FORCE_INLINE std::array<uint8_t, sizeof(T)> FastUnpack(uint8_t * data, size_t offset)
{
	static const uint64_t MASK = 0x00000000000000ff;

	T val = 0;
	memcpy(&val, data + offset, sizeof(T));


	std::array<uint8_t, sizeof(T)> v;
	v[0] = (val & MASK);
	v[1] = (val & MASK << 8) >> 8;
	v[2] = (val & MASK << 16) >> 16;
	v[3] = (val & MASK << 24) >> 24;
	v[4] = (val & MASK << 32) >> 32;
	v[5] = (val & MASK << 40) >> 40;
	v[6] = (val & MASK << 48) >> 48;
	v[7] = (val & MASK << 56) >> 56;

	return std::move(v);
};

//===========================================================

void RunAllTest()
{
	StringTests<SmallString>::TestCtors();
	StringTests<SmallString>::TestMethods();
	StringTests<SmallString>::TestStringToIntNumber();
	StringTests<SmallString>::TestStringToRealNumber();
	StringTests<SmallString>::TestAppendNumberAll();
	StringTests<SmallString>::TestAppendString();
	StringTests<SmallString>::TestSubstring();

	StringTests<String>::TestCtors();
	StringTests<String>::TestMethods();
	StringTests<String>::TestStringToIntNumber();
	StringTests<String>::TestStringToRealNumber();
	StringTests<String>::TestAppendNumberAll();
	StringTests<String>::TestAppendString();
	StringTests<String>::TestSubstring();

	StringTests<String>::TestLocalization();
}

//===========================================================

int main(int argc, char ** argv)
{
#ifdef VLD_ENABLED
	VLDSetReportOptions(VLD_OPT_REPORT_TO_DEBUGGER | VLD_OPT_REPORT_TO_FILE, L"leaks.txt");
	//VLDSetOptions(VLD_OPT_SAFE_STACK_WALK, 1024, 1024);	
#endif	
#ifdef _WIN32
	EnableWinConsoleUtf8();
#endif
	
	
	//RunAllTest();
	

	StringBenchmarks sb(1000000);
	

	std::random_device r;
	std::default_random_engine e(r());
	std::uniform_int_distribution<uint64_t> uniform_dist(std::numeric_limits<uint64_t>::min(),
		std::numeric_limits<uint64_t>::max());

		
	/*
	sb.RunExternalTest([&](int count, double * r) -> void {
		for (int i = 0; i < count; i++)
		{
			constexpr MyStringID id = "toto je muj dlouhy retezec pro ID";
			r[i] += id.GetHashCode();
		}
	});

	sb.RunExternalTest([&](int count, double * r) -> void {
		for (int i = 0; i < count; i++)
		{
			MyStringAnsi id = "toto je muj dlouhy retezec pro ID";
			r[i] += id.GetHashCode();
		}
	});
	*/

	sb.RunAll();


	
	return 0;
}
