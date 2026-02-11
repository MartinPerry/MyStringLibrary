#ifndef COMPARISON_OPERATORS_H
#define COMPARISON_OPERATORS_H

#include "./MyString.h"
#include "./MySmallString.h"
#include "./MyStringView.h"

#include <cstring>

//================================================================

	//define forward declaration
#define GENERATE_FORWARD_OPERATORS_REF(p1, p2) \
	bool operator ==(const p1 & str1, const p2 & str2); \
	bool operator !=(const p1 & str1, const p2 & str2); \
	bool operator >(const p1 & str1, const p2 & str2); \
	bool operator <(const p1 & str1, const p2 & str2);

#define GENERATE_FORWARD_OPERATORS_FULL_TYPE(p1, p2) \
	bool operator ==(const p1 str1, const p2 str2); \
	bool operator !=(const p1 str1, const p2 str2); \
	bool operator >(const p1 str1, const p2 str2); \
	bool operator <(const p1 str1, const p2 str2);

#define GENERATE_BODY_OPERATORS_REF(p1, p2) \
	inline bool operator ==(const p1 & str1, const p2 & str2){ \
		if (str1.length() != str2.length()) return false; \
		return (memcmp(str1.c_str(), str2.c_str(), str1.length()) == 0); \
	}; \
	inline bool operator !=(const p1 & str1, const p2 & str2){ \
		if (str1.length() != str2.length()) return true; \
			return (memcmp(str1.c_str(), str2.c_str(), str1.length()) != 0); \
	}; \
	inline bool operator >(const p1 & str1, const p2 & str2){ \
		return (strcmp(str1.c_str(), str2.c_str()) > 0); \
	}; \
	inline bool operator <(const p1 & str1, const p2 & str2) { \
			return (strcmp(str1.c_str(), str2.c_str()) < 0); \
	};

//================================================================

namespace mystrlib
{

	GENERATE_FORWARD_OPERATORS_REF(String, String);
	GENERATE_FORWARD_OPERATORS_REF(SmallString, SmallString);
	GENERATE_FORWARD_OPERATORS_REF(String, SmallString);
	GENERATE_FORWARD_OPERATORS_REF(SmallString, String);
	GENERATE_FORWARD_OPERATORS_REF(String, std::string);
	GENERATE_FORWARD_OPERATORS_REF(std::string, String);
	GENERATE_FORWARD_OPERATORS_REF(std::string, SmallString);
	GENERATE_FORWARD_OPERATORS_REF(SmallString, std::string);
	GENERATE_FORWARD_OPERATORS_FULL_TYPE(char*, String&);
	GENERATE_FORWARD_OPERATORS_FULL_TYPE(char*, SmallString&);
	GENERATE_FORWARD_OPERATORS_FULL_TYPE(String&, char*);
	GENERATE_FORWARD_OPERATORS_FULL_TYPE(SmallString&, char*);

	GENERATE_FORWARD_OPERATORS_REF(StringView, StringView);
	GENERATE_FORWARD_OPERATORS_REF(StringView, String);
	GENERATE_FORWARD_OPERATORS_REF(String, StringView);
	GENERATE_FORWARD_OPERATORS_REF(StringView, SmallString);
	GENERATE_FORWARD_OPERATORS_REF(SmallString, StringView);
	GENERATE_FORWARD_OPERATORS_REF(StringView, std::string);
	GENERATE_FORWARD_OPERATORS_REF(std::string, StringView);
	GENERATE_FORWARD_OPERATORS_FULL_TYPE(char*, StringView&);
	GENERATE_FORWARD_OPERATORS_FULL_TYPE(StringView&, char*);

	//=====================================================================

	GENERATE_BODY_OPERATORS_REF(String, String);
	GENERATE_BODY_OPERATORS_REF(SmallString, SmallString);
	GENERATE_BODY_OPERATORS_REF(String, SmallString);
	GENERATE_BODY_OPERATORS_REF(SmallString, String);
	GENERATE_BODY_OPERATORS_REF(String, std::string);
	GENERATE_BODY_OPERATORS_REF(std::string, String);
	GENERATE_BODY_OPERATORS_REF(std::string, SmallString);
	GENERATE_BODY_OPERATORS_REF(SmallString, std::string);

	GENERATE_BODY_OPERATORS_REF(StringView, StringView);
	GENERATE_BODY_OPERATORS_REF(StringView, String);
	GENERATE_BODY_OPERATORS_REF(String, StringView);
	GENERATE_BODY_OPERATORS_REF(StringView, SmallString);
	GENERATE_BODY_OPERATORS_REF(SmallString, StringView);
	GENERATE_BODY_OPERATORS_REF(StringView, std::string);
	GENERATE_BODY_OPERATORS_REF(std::string, StringView);

	//==========================================================



	inline bool operator ==(const char* str1, const String& str2)
	{
		if (str1 == nullptr) return false;
		return (strcmp(str1, str2.c_str()) == 0);
	};

	inline bool operator ==(const String& str1, const char* str2)
	{
		return str2 == str1;
	};


	inline bool operator ==(const char* str1, const SmallString& str2)
	{
		if (str1 == nullptr) return false;
		return (strcmp(str1, str2.c_str()) == 0);
	};

	inline bool operator ==(const SmallString& str1, const char* str2)
	{
		return str2 == str1;
	};

	inline bool operator ==(const char* str1, const StringView& str2)
	{
		//https://stackoverflow.com/questions/28997095/comparing-null-terminated-string-with-a-non-null-terminated-string-in-c
		if (str1 == nullptr) return false;
		return (strncmp(str1, str2.c_str(), str2.length()) == 0 && str1[str2.length()] == 0);
	};

	inline bool operator ==(const StringView& str1, const char* str2)
	{
		return str2 == str1;
	};


	inline bool operator !=(const char* str1, const String& str2)
	{
		return !(str1 == str2);
	};

	inline bool operator !=(const  String& str1, const char* str2)
	{
		return !(str1 == str2);
	};

	inline bool operator !=(const char* str1, const SmallString& str2)
	{
		return !(str1 == str2);
	};

	inline bool operator !=(const  SmallString& str1, const char* str2)
	{
		return !(str1 == str2);
	};

	inline bool operator !=(const char* str1, const StringView& str2)
	{
		return !(str1 == str2);
	};

	inline bool operator !=(const StringView& str1, const char* str2)
	{
		return !(str1 == str2);
	};


	inline bool operator <(const char* str1, const String& str2)
	{
		if (str1 == nullptr) return true;
		return (strcmp(str1, str2.c_str()) < 0);
	};

	inline bool operator <(const String& str1, const char* str2)
	{
		return str2 < str1;
	};

	inline bool operator <(const char* str1, const SmallString& str2)
	{
		if (str1 == nullptr) return true;
		return (strcmp(str1, str2.c_str()) < 0);
	};

	inline bool operator <(const SmallString& str1, const char* str2)
	{
		return str2 < str1;
	};

	inline bool operator <(const char* str1, const StringView& str2)
	{
		//not tested
		if (str1 == nullptr) return false;
		return (strncmp(str1, str2.c_str(), str2.length()) < 0);
	};

	inline bool operator <(const StringView& str1, const char* str2)
	{
		return str2 < str1;
	};



	inline bool operator >(const char* str1, const String& str2)
	{
		if (str1 == nullptr) return false;
		return (strcmp(str1, str2.c_str()) > 0);
	};

	inline bool operator >(const String& str1, const char* str2)
	{
		return str2 > str1;
	};

	inline bool operator >(const char* str1, const SmallString& str2)
	{
		//not tested
		if (str2 == nullptr) return false;
		return (strcmp(str1, str2.c_str()) > 0);
	};

	inline bool operator >(const SmallString& str1, const char* str2)
	{
		return str2 > str1;
	};

	inline bool operator >(const char* str1, const StringView& str2)
	{
		if (str1 == nullptr) return false;
		return (strncmp(str1, str2.c_str(), str2.length()) > 0);
	};

	inline bool operator >(const StringView& str1, const char* str2)
	{
		return str2 > str1;
	};

}

//================================
//undef defined values

#ifdef GENERATE_FORWARD_OPERATORS_REF
#	undef GENERATE_FORWARD_OPERATORS_REF
#endif

#ifdef GENERATE_FORWARD_OPERATORS_FULL_TYPE
#	undef GENERATE_FORWARD_OPERATORS_FULL_TYPE
#endif

#ifdef GENERATE_BODY_OPERATORS_REF
#	undef GENERATE_BODY_OPERATORS_REF
#endif

#endif