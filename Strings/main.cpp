
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


int main(int argc, char ** argv)
{
#ifdef VLD_ENABLED
	VLDSetReportOptions(VLD_OPT_REPORT_TO_DEBUGGER | VLD_OPT_REPORT_TO_FILE, L"leaks.txt");
	//VLDSetOptions(VLD_OPT_SAFE_STACK_WALK, 1024, 1024);	
#endif	

	
	/*
	Localization l("cs", "cs", "../test_data/");
	auto tmp0 = l.Localize("timeFormatWheel");

	l.ReplaceKeysByLang("en-us", { "timeFormatWheel", "timeFormatLine" });
	auto tmp1 = l.Localize("timeFormatWheel");

	std::unordered_map<Localization::String, Localization::Utf8StringWrapper> items;
	items["menuHelp"] = u8"xxx";
	items["lat"] = u8"yyy";

	auto res = l.Localize("searchCoords", "", items);
	auto res2 = res;


	printf("%s\n", (const char *)l.Localize("searchCoords").c_str());
	//printf("%s\n", (l.Localize("temperature", "layers").getAs<Localization::String>()).c_str());
	//printf("%s\n", (l.Localize("searchCoords", { "kccc", "x", "xxcxc7" }).getAs<Localization::String>()).c_str());
	*/
	

	
	
	StringTests<MySmallString>::TestCtors();
	StringTests<MySmallString>::TestMethods();
	StringTests<MySmallString>::TestStringToIntNumber();
	StringTests<MySmallString>::TestStringToRealNumber();
	StringTests<MySmallString>::TestAppendNumberAll();
	StringTests<MySmallString>::TestAppendString();
	StringTests<MySmallString>::TestSubstring();
	
	StringTests<MyString>::TestCtors();
	StringTests<MyString>::TestMethods();
	StringTests<MyString>::TestStringToIntNumber();
	StringTests<MyString>::TestStringToRealNumber();
	StringTests<MyString>::TestAppendNumberAll();
	StringTests<MyString>::TestAppendString();
	StringTests<MyString>::TestSubstring();
	
	

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

	
	sb.TestShortStrAllocation();
	sb.TestStringToInt();
	sb.TestStringToDouble();
	sb.TestAppendNumberAll();
	sb.TestAppendSmallString();
	sb.TestAppendString();	
	sb.TestHashing();
	

	
	return 0;
}