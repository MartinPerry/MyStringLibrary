
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

	StringX oo = "xx";
	int ii = sizeof(oo);
	printf("%s\n", oo.c_str());


	MyStringAnsi str("xax");

	//double ix = MyStringUtils::ToNumber<double>("3.14159e+001");
	
	//StringTests::TestStringToIntNumber();
	//StringTests::TestStringToRealNumber();
	StringTests::TestAppendNumberAll();

	StringBenchmarks sb(1000'000);
	//sb.RunExternalTest([&](int count, double * r) -> void{});

	
	//sb.TestStringToInt();
	//sb.TestStringToDouble();
	sb.TestAppendNumberAll();

	return 0;
}