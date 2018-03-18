
#include <vld.h>


#include <cstdio>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <array>
#include <string>
#include <random>

#include "MyString.h"
#include "StringTests.h"
#include "StringBenchmarks.h"

using namespace std;


/*
//Exponents range from -1022 to +1023
//an exponent value of +1023 represents the actual zero

typedef union {
double d;
uint64_t i;
} dtoi;

dtoi q;
q.d = 3.0e10;

int64_t s = q.i >> 63;

uint64_t oldE = q.i & uint64_t(0x7ff0000000000000);
oldE >>= 52;

int64_t e = (int64_t)(oldE) - 1023;

uint64_t newE = uint64_t(1 + 1023) << 52;

dtoi q2;
q2.i = (q.i & uint64_t(0x800fffffffffffff)) | (newE & uint64_t(0x7ff0000000000000));

val = q2.d;

*/


int main(int argc, char ** argv)
{
	VLDSetReportOptions(VLD_OPT_REPORT_TO_DEBUGGER | VLD_OPT_REPORT_TO_FILE, L"leaks.txt");
	//VLDSetOptions(VLD_OPT_SAFE_STACK_WALK, 1024, 1024);

	/*
	std::string xx = "";
	for (int i = 0; i < 100; i++)
	{
		xx += "\"";
		xx += std::to_string(i);
		xx += "\",";
	}
	*/

	MyStringAnsi s = "";
	//s += (int64_t)(-954564564654564);
	//s += int32_t(-112310);
	
	s += int16_t(std::numeric_limits<int16_t>::min());
	s += " ";
	s += int16_t(std::numeric_limits<int16_t>::max());

	std::string t = "";
	t += std::to_string(int16_t(std::numeric_limits<int16_t>::min()));
	t += " ";
	t += std::to_string(int16_t(std::numeric_limits<int16_t>::max()));

	s += uint64_t(123);
	s += size_t(123);

	//double ix = MyStringUtils::ToNumber<double>("3.14159e+001");
	
	//StringTests::TestStringToIntNumber();
	//StringTests::TestStringToRealNumber();
	StringTests::TestAppendNumberAll();

	StringBenchmarks sb(1'000'000);
	//sb.TestStringToInt();
	//sb.TestStringToDouble();
	sb.TestAppendNumberAll();

	return 0;
}