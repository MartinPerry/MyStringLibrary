
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

/*
void X_aligned_memcpy_sse2(char* dest, const char* src, const unsigned long s)
{
//must be aligned to 128bit
	while (uintptr_t(dest) % 16 != 0)
	{
		dest++;
	}

	while (uintptr_t(src) % 16 != 0)
	{
		src++;
	}

	__asm
	{
		mov esi, src;    //src pointer
		mov edi, dest;   //dest pointer

		mov ebx, s;   //ebx is our counter 
		shr ebx, 7;      //divide by 128 (8 * 128bit registers)

	loop_copy:
		prefetchnta 128[esi]; //SSE2 prefetch
		prefetchnta 160[esi];
		prefetchnta 192[esi];
		prefetchnta 224[esi];

		movdqa xmm0, 0[esi]; //move data from src to registers
		movdqa xmm1, 16[esi];
		movdqa xmm2, 32[esi];
		movdqa xmm3, 48[esi];
		movdqa xmm4, 64[esi];
		movdqa xmm5, 80[esi];
		movdqa xmm6, 96[esi];
		movdqa xmm7, 112[esi];

		movntdq 0[edi], xmm0; //move data from registers to dest
		movntdq 16[edi], xmm1;
		movntdq 32[edi], xmm2;
		movntdq 48[edi], xmm3;
		movntdq 64[edi], xmm4;
		movntdq 80[edi], xmm5;
		movntdq 96[edi], xmm6;
		movntdq 112[edi], xmm7;

		add esi, 128;
		add edi, 128;
		dec ebx;

		jnz loop_copy; //loop please
	loop_copy_end:
	}
}
*/

//https://akrzemi1.wordpress.com/2014/04/14/common-optimizations/
class StringX 
{
public:
	StringX(const char * s) 
	{
		buffer.begin = nullptr;


		length = strlen(s);
		if (length < 15)
		{
			ptr = buffer.local;
			capacity = 16;
		}
		else 
		{
			buffer.begin = new char[length + 1];			
			ptr = buffer.begin;
			capacity = length + 1;
		}

		memcpy(ptr, s, length);
		ptr[length] = 0;
	}
	
	char * c_str() { return ptr; }

	~StringX() = default;

private:
	union Buffer
	{
		char * begin;
		char  local[16];
	};

	Buffer buffer;
	char * ptr;
	size_t length;
	size_t capacity;

};

MyStringAnsi tmpFunc()
{
	MyStringAnsi o = "ahoj xxxxxxxxxxxxxxxx";

	return o;
}


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

//vyhozeni sfinae z headeru
//https://stackoverflow.com/questions/48480469/function-implementation-with-enable-if-outside-of-class-definition

int main(int argc, char ** argv)
{
	VLDSetReportOptions(VLD_OPT_REPORT_TO_DEBUGGER | VLD_OPT_REPORT_TO_FILE, L"leaks.txt");
	//VLDSetOptions(VLD_OPT_SAFE_STACK_WALK, 1024, 1024);

	

	/*
	std::string xx = "";
	size_t lastC = 0;
	for (int i = 0; i < 500000; i++)
	{
		xx += std::to_string(i);
		if (lastC != xx.capacity())
		{
			printf("%d\n", xx.capacity());
			lastC = xx.capacity();
		}
	}

	printf("=====\n");

	MyStringAnsi xx2 = "";
	lastC = 0;
	for (int i = 0; i < 500000; i++)
	{
		xx2 += i;
		if (lastC != xx2.capacity())
		{
			printf("%d\n", xx2.capacity());
			lastC = xx2.capacity();
		}
	}
	*/
	
	int oioi = sizeof(MyStringAnsi);

	char m[16];
	memset(m, 0, 16);
	uint64_t v = 456456456;
	memcpy(m, &v, sizeof(uint64_t));
	
	size_t u = 0;
	memcpy(&u, m, sizeof(size_t));

	size_t u2 = (uint8_t(m[3]) << 24) + (uint8_t(m[2]) << 16) + (uint8_t(m[1]) << 8) + uint8_t(m[0]);

	if (v != u)
	{
		printf("x");
	}

	MyStringAnsi oosx = tmpFunc();

	MyStringAnsi oo1t = "1  1  axxxxxxxx xxaaxx";
	//oo1t.RemoveMultipleChars('x');
	auto ss1 = oo1t.Split<MyStringAnsi>(' ');
	auto ss = oo1t.Split<MyStringAnsi>(' ', true);

	MyStringAnsi oo1 = "1";
	MyStringAnsi oo14 = "abcdefghijklmo";
	MyStringAnsi oo15 = "abcdefghijklmop";
	MyStringAnsi oo16 = "abcdefghijklmopq";

	oo1 += "ahoj";
	oo1 += "ahojahoj";
	oo1 += "ahojahoj";

	auto x = oo1.c_str();

	oo1.capacity();
	oo16.capacity();

	//oo12 += 1234567890123;
	//oo12 += 445645646545456;

	//std::string x[50000];
	//std::string po = "xxxxxxxxxxxxxxx xxxxx";
	std::string po = "xxxxxxxxxxxxxx";
	po += "x";
	int oox = po.capacity();
	po += "x";

	StringX oo = "xx";
	int ii = sizeof(po);
	printf("%s\n", oo.c_str());

	

	MyStringAnsi str("xax");

	//double ix = MyStringUtils::ToNumber<double>("3.14159e+001");
	
	/*
	StringTests::TestCtors();
	StringTests::TestMethods();
	StringTests::TestStringToIntNumber();
	StringTests::TestStringToRealNumber();
	StringTests::TestAppendNumberAll();	
	StringTests::TestAppendString();
	*/

	StringBenchmarks sb(1000'000);
	/*
	sb.RunExternalTest([&](int count, double * r) -> void{
		for (int i = 0; i < count; i++)
		{
			MyStringAnsi x = "xxxxxxxxxxxxxxx";
			r[i] += x.length();
		}
	});

	sb.RunExternalTest([&](int count, double * r) -> void {
		for (int i = 0; i < count; i++)
		{
			MyStringAnsi x = "xxxxxxxxxxxxxxxx";
			r[i] += x.length();
		}
	});
	*/

	std::random_device r;
	std::default_random_engine e(r());
	std::uniform_int_distribution<uint64_t> uniform_dist(std::numeric_limits<uint64_t>::min(),
		std::numeric_limits<uint64_t>::max());

	/*
	std::vector<std::array<char, 16>> d;
	for (int i = 0; i < 10000'000; i++)
	{
		std::array<char, 16> a;
		
		uint64_t v = uniform_dist(e);
		memcpy(a.data(), &v, sizeof(uint64_t));


		d.push_back(a);
	}

	sb.RunExternalTest([&](int count, double * r) -> void {
		for (int i = 0; i < count; i++)
		{			
			size_t u = 0;
			memcpy(&u, d[i].data(), sizeof(size_t));
			
			r[i] += u;
		}
	});

	sb.RunExternalTest([&](int count, double * r) -> void {
		for (int i = 0; i < count; i++)
		{
			const char * m = d[i].data();
			
			size_t u = (uint8_t(m[3]) << 24) + (uint8_t(m[2]) << 16) + (uint8_t(m[1]) << 8) + uint8_t(m[0]);

			
			r[i] += u;
		}
	});
	*/

	//sb.TestShortStrAllocation();
	//sb.TestStringToInt();
	//sb.TestStringToDouble();
	//sb.TestAppendNumberAll();
	//sb.TestAppendSmallString();
	//sb.TestAppendString();

	return 0;
}