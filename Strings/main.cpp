
#ifdef _WIN32
#include <vld.h>
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
#include "StringTests.h"
#include "StringBenchmarks.h"

#include "./SimpleLocalization/Localization.h"

using namespace std;

#ifndef _WIN32

//http://stackoverflow.com/questions/77005/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes

/// <summary>
/// https://github.com/JPNaude/dev_notes/wiki/Produce-a-stacktrace-when-something-goes-wrong-in-your-application
/// </summary>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>

/* This structure mirrors the one found in /usr/include/asm/ucontext.h */
typedef struct _sig_ucontext {
	unsigned long     uc_flags;
	struct ucontext   *uc_link;
	stack_t           uc_stack;
	struct sigcontext uc_mcontext;
	sigset_t          uc_sigmask;
} sig_ucontext_t;

void crit_err_hdlr(int sig_num, siginfo_t * info, void * ucontext) {
	void *             array[50];
	void *             caller_address;
	char **            messages;
	int                size, i;
	sig_ucontext_t *   uc;

	uc = (sig_ucontext_t *)ucontext;

	/* Get the address at the time the signal was raised */
#if defined(__i386__) // gcc specific
	caller_address = (void *)uc->uc_mcontext.eip; // EIP: x86 specific
#elif defined(__x86_64__) // gcc specific
	caller_address = (void *)uc->uc_mcontext.rip; // RIP: x86_64 specific
#else
#error Unsupported architecture. // TODO: Add support for other arch.
#endif

	fprintf(stderr, "\n");
	//FILE * backtraceFile;

	// In this example we write the stacktrace to a file. However, we can also just fprintf to stderr (or do both).
	//QString backtraceFilePath = "/home/my_user/stacktrace.txt;
	//backtraceFile = fopen(backtraceFilePath.toUtf8().data(), "w");

	if (sig_num == SIGSEGV)
		printf("signal %d (%s), address is %p from %p\n", sig_num, strsignal(sig_num), info->si_addr, (void *)caller_address);
	else
		printf("signal %d (%s)\n", sig_num, strsignal(sig_num));

	size = backtrace(array, 50);
	/* overwrite sigaction with caller's address */
	array[1] = caller_address;
	messages = backtrace_symbols(array, size);
	/* skip first stack frame (points here) */
	for (i = 1; i < size && messages != NULL; ++i) {
		printf("[bt]: (%d) %s\n", i, messages[i]);
	}

	//fclose(backtraceFile);
	free(messages);

	exit(EXIT_FAILURE);
}

void installSignal(int __sig) {
	struct sigaction sigact;
	sigact.sa_sigaction = crit_err_hdlr;
	sigact.sa_flags = SA_RESTART | SA_SIGINFO;
	if (sigaction(__sig, &sigact, (struct sigaction *)NULL) != 0) {
		fprintf(stderr, "error setting signal handler for %d (%s)\n", __sig, strsignal(__sig));
		exit(EXIT_FAILURE);
	}
}


#endif

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

struct IBar;

template <class T>
struct IFoo 
{
	IFoo() { printf("parent"); }
	IFoo(const char * x) {}
	

	template <typename U>
	typename std::enable_if<std::is_same<U, IBar>::value, IBar &>::type
	operator=(const U & x){ return *static_cast<T *>(this); }

	void Test() { static_cast<T *>(this)->TestInternal(); }
};


struct IBar : public IFoo<IBar>
{
	using IFoo<IBar>::IFoo;
	
	using IFoo<IBar>::operator=;
	
	IBar() : IFoo() {}
	IBar(const IBar & b) {}
	IBar(const IBar && b) {}
	
	IBar & operator=(const IBar & x) { return IFoo<IBar>::operator=(x); }

	void TestInternal(){}
};




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


uint64_t GetFractPartAsBuffer(double val, uint64_t intPart, int fractPlaces)
{
	double decimalMult = std::pow(10, fractPlaces);

	double fractPart = val - intPart;
	uint64_t fractInt = (uint64_t)(fractPart * decimalMult);

	uint64_t res = 0;
	uint8_t * buf = reinterpret_cast<uint8_t *>(&res);
	
	uint64_t rev_num = 0;
	while (fractInt > 0)
	{
		fractPlaces--;
		buf[fractPlaces] = (uint8_t)(fractInt % 10 + '0');				
		fractInt = fractInt / 10;
	}

	return 0;
}

template <class T>
constexpr void test_helper(T&&) {}

#define IS_CONSTEXPR(...) noexcept(test_helper(__VA_ARGS__))

double bar(double x) { return x; }

constexpr double foo(double x, bool b) {
	if (b) return x;
	else return bar(x);
}

int main(int argc, char ** argv)
{
#ifdef _WIN32
	VLDSetReportOptions(VLD_OPT_REPORT_TO_DEBUGGER | VLD_OPT_REPORT_TO_FILE, L"leaks.txt");
	//VLDSetOptions(VLD_OPT_SAFE_STACK_WALK, 1024, 1024);	
#endif	

#ifndef _WIN32
	// For crashes, SIGSEV should be enough.
	installSignal(SIGSEGV);
#endif

	auto xxgf = MyStringAnsi::CreateFormated("xxx %d", 45);

	Localization l("en", "en", "../test_data/");
	auto res = l.Localize("searchCoords");
	auto res2 = res.getAs<Localization::String>();


	printf("%s\n", (l.Localize("searchCoords").getAs<Localization::String>()).c_str());
	printf("%s\n", (l.Localize("temperature", "layers").getAs<Localization::String>()).c_str());
	printf("%s\n", (l.Localize("searchCoords", { "kccc", "x", "xxcxc7" }).getAs<Localization::String>()).c_str());



	MyStringAnsi xxs = "ahoj";
	xxs[0] = 'x';
	double d = 3.0;// +xxs.length();

	constexpr auto xx = IS_CONSTEXPR(foo(3.0, true));
	constexpr auto yy = IS_CONSTEXPR(foo(3.0, false));
	constexpr auto zz = IS_CONSTEXPR(foo(d, true));
	
	
	std::cerr << xx << yy << zz;

	auto rer = GetFractPartAsBuffer(45.1, 45, 7);

	auto ss7 = MyStringUtils::ToStringSimple(-789.00999178, 7);

	ss7.GetHashCode();
	MyStringView viewx = ss7;
	ss7[0] = 'x';	
	ss7[0] = '-';

	MyStringAnsi w789 = ss7;
	MyStringView viewx2 = w789;
	w789[0] = '-';
	
	char xdsd = viewx2[0];

	MyStringId sid;
	sid = w789;

	if (viewx == viewx2)
	{
		printf("x");
	}

	std::unordered_map<MyStringView, int> xxa;
	xxa[viewx] = 789;

	MyStringView view = "-789.0099917"_L;

	//static_assert(view.length() == 12, "Literal failed");

	if (view == ss7)
	{
		printf("x");
	}

	view = "xxx";

	double rrd = MyStringUtils::ToNumber<double>("45.45");
	char * xMove = nullptr;
	{
		MyStringAnsi moveS = "xxxxx";

		xMove = (char *)(std::move(moveS));
	}

	MyStringAnsi tmpFind = "";
	tmpFind.AppendWithDigitsCount(-123, 5);


	StringBenchmarks sb11(1000000);
	sb11.RunExternalTest([&](int count, double * r) -> void{
		for (int i = 0; i < count; i++)
		{
			MyStringAnsi x = "";
			x.AppendWithDigitsCount(-123, 5);
			r[i] += x.length();
		}
	});

	sb11.RunExternalTest([&](int count, double * r) -> void {
		for (int i = 0; i < count; i++)
		{
			MyStringAnsi x = "";
			x.AppendFormat("%05d", -123);
			r[i] += x.length();
		}
	});

	return 0;

	tmpFind += "ahoj vojle ahoj kluku";
	auto all = tmpFind.FindAll("ahoj");
	auto ik1 = tmpFind.Find("klukuj", SearchAlgorithm::KMP);
	auto ik2 = tmpFind.Find("klukuk", SearchAlgorithm::C_LIB);
	auto ik3 = tmpFind.Find("klukuk", SearchAlgorithm::BM);
	auto ik4 = tmpFind.Find("klukuk", SearchAlgorithm::BF);

	

	MyStringAnsi ex;

	MyStringAnsi ee;

	ee = ex;

	unsigned long iip = -0;
	MyStringAnsi ioi = "x";
	ioi += iip;

	MyStringAnsi cislo = "45.789uuu";
	double c = static_cast<int>(cislo.SubString(0));

	cislo.Clear();
	cislo += 456;

	//MyStringAnsi ooxxx = MyStringAnsi::CreateFormated("xx %i km", 40);
	MyStringAnsi ooxxx = "1saxxxda-sd-1sadasd0xxxx";
	
	char data[5];
	ooxxx.CopySubstring(0, 2, data);

	char * data2 = new char[5];
	ooxxx.CopySubstring(0, 2, data2);

	char * data3 = nullptr;
	ooxxx.CopySubstring(0, 2, &data3);

	ooxxx.RemoveMultipleChars('x');
	ooxxx.RemoveChar('x');

	auto nm = ooxxx.GetAllNumbers();

	

	IBar ibar = IBar("x");
	IBar ibar2 = IBar();

	//MyStringAnsi ooxxx;

	ooxxx += "ahjoj";
	
	ooxxx += 5.1;

	std::map<MyStringAnsi, int> ooo;

	ooo.insert(std::make_pair(MyStringAnsi("xxx"), 0));
	ooo[MyStringAnsi("xxxxx")] = 1;

	IBar bar0 = "bar";

	IBar bar = "bar";
	bar = bar0;
	bar.Test();

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
	
	/*
	int oioi = sizeof(MyStringAnsi);

	std::string xx = "1231231321231231231";
	xx += "xxx";

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
	*/

	const MyStringAnsi kuk = "kuk";
	MyStringAnsi baf = "baf kuk x haf";
	
	auto rr = baf.Split<MyStringAnsi>(' ');

	MyStringAnsi kukbaf = kuk + baf;	
	//kukbaf = kuk + "ahoj";

	if (kuk == "ahh")
	{
	}

	kukbaf.Transform([&](char x) -> char {
		return x + 1;
	});

	kukbaf.AppendFormat("%s %s", kuk.c_str(), baf.c_str());
	MyStringAnsi::CreateFormated("%s %s", kuk.c_str(), baf.c_str());

	std::string hh = "xxx";
	
	MyStringAnsi oxoxo = hh;
	MySmallStringAnsi oxoxos = hh;
	
	//std::vector<MyStringAnsi> xxx789 = oxoxo.Split('*');
	MyStringAnsi oosx = tmpFunc();
	//oosx = std::move(oxoxo);
	oosx = hh;
	oosx = oxoxo;
	oosx = oxoxos;
	oosx += oxoxo;
	oosx += oxoxos;

	MyStringAnsi iin = "123 456.4";
	auto numbr = iin.GetAllNumbers();

	MySmallStringAnsi tmpSmall = "ahoj vojle";
	tmpSmall.Replace("oj", "voj");

	tmpSmall.PopBack();
	char last = tmpSmall.GetLastChar();

	MyStringAnsi tmpReplace = "ahoj babi ahoh ahoj baf ahoj";	
	tmpReplace.Replace("ahoj", "vole");

	tmpReplace.PopBack();

	//MyStringAnsi tmpReplace = "ahoj babi ahoh ahoj baf";	
	//tmpReplace.Replace("ahoj", "volej");

	MyStringAnsi oo1t = "1  1  axxxxxxxx xxaaxx";

	oo1t += 1567;
	oo1t += 'a';
	
	//oo1t.RemoveMultipleChars('x');
	auto ss1 = oo1t.Split<std::string>(' ');
	auto ss = oo1t.Split<MyStringAnsi>(' ', true);

	//return 0;

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
	size_t oox = po.capacity();
	po += "x";

	StringX oo = "xx";
	int ii = sizeof(po);
	printf("%s\n", oo.c_str());

	

	MyStringAnsi str("xax");

	//double ix = MyStringUtils::ToNumber<double>("3.14159e+001");
	
	
	StringTests<MySmallStringAnsi>::TestCtors();
	StringTests<MySmallStringAnsi>::TestMethods();
	StringTests<MySmallStringAnsi>::TestStringToIntNumber();
	StringTests<MySmallStringAnsi>::TestStringToRealNumber();
	StringTests<MySmallStringAnsi>::TestAppendNumberAll();
	StringTests<MySmallStringAnsi>::TestAppendString();
	StringTests<MySmallStringAnsi>::TestSubstring();
	
	StringTests<MyStringAnsi>::TestCtors();
	StringTests<MyStringAnsi>::TestMethods();
	StringTests<MyStringAnsi>::TestStringToIntNumber();
	StringTests<MyStringAnsi>::TestStringToRealNumber();
	StringTests<MyStringAnsi>::TestAppendNumberAll();
	StringTests<MyStringAnsi>::TestAppendString();
	StringTests<MyStringAnsi>::TestSubstring();
	
	

	StringBenchmarks sb(1000000);
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