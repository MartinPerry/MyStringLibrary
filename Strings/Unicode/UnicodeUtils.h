#ifndef UNICODE_UTILS_H
#define UNICODE_UTILS_H

#if (defined(ENABLE_ICU) && (__has_include(<unicode/unistr.h>)))
#	include "./IcuUnicodeStringWrapper.h"

#	define HAS_UNICODE

using UnicodeStringWrapper = IcuUnicodeStringWrapper;

#elif __has_include("./tinyutf8.h")
#	include "./TinyUtf8Wrapper.h"

#	define HAS_UNICODE

using UnicodeStringWrapper = TinyUtf8Wrapper;

#endif


#ifdef _WIN32

#include <Windows.h>

//https://stackoverflow.com/questions/45575863/how-to-print-utf-8-strings-to-stdcout-on-windows
inline void EnableWinConsoleUtf8()
{
    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    //setvbuf(stdout, nullptr, _IOFBF, 1000);
};

#endif

#endif
