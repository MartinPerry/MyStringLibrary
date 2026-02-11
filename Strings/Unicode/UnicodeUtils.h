#ifndef UNICODE_UTILS_H
#define UNICODE_UTILS_H

#include <string>
#include "../MyString.h"

#ifdef _WIN32

#ifndef NOMINMAX
#   define NOMINMAX
#endif
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


using StringUtf8 = std::u8string;

static inline StringUtf8 AsStringUtf8(const char* str)
{    
    return StringUtf8(reinterpret_cast<const char8_t*>(str));
}

static inline StringUtf8 AsStringUtf8(const std::string& str)
{
    return StringUtf8(reinterpret_cast<const char8_t*>(str.c_str()), str.length());
}

static inline StringUtf8 AsStringUtf8(const mystrlib::MyString& str)
{
    return StringUtf8(reinterpret_cast<const char8_t*>(str.c_str()), str.length());
}

#endif
