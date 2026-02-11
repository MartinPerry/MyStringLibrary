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

	GENERATE_FORWARD_OPERATORS_REF(MyString, MyString);
	GENERATE_FORWARD_OPERATORS_REF(MySmallString, MySmallString);
	GENERATE_FORWARD_OPERATORS_REF(MyString, MySmallString);
	GENERATE_FORWARD_OPERATORS_REF(MySmallString, MyString);
	GENERATE_FORWARD_OPERATORS_REF(MyString, std::string);
	GENERATE_FORWARD_OPERATORS_REF(std::string, MyString);
	GENERATE_FORWARD_OPERATORS_REF(std::string, MySmallString);
	GENERATE_FORWARD_OPERATORS_REF(MySmallString, std::string);
	GENERATE_FORWARD_OPERATORS_FULL_TYPE(char*, MyString&);
	GENERATE_FORWARD_OPERATORS_FULL_TYPE(char*, MySmallString&);
	GENERATE_FORWARD_OPERATORS_FULL_TYPE(MyString&, char*);
	GENERATE_FORWARD_OPERATORS_FULL_TYPE(MySmallString&, char*);

	GENERATE_FORWARD_OPERATORS_REF(MyStringView, MyStringView);
	GENERATE_FORWARD_OPERATORS_REF(MyStringView, MyString);
	GENERATE_FORWARD_OPERATORS_REF(MyString, MyStringView);
	GENERATE_FORWARD_OPERATORS_REF(MyStringView, MySmallString);
	GENERATE_FORWARD_OPERATORS_REF(MySmallString, MyStringView);
	GENERATE_FORWARD_OPERATORS_REF(MyStringView, std::string);
	GENERATE_FORWARD_OPERATORS_REF(std::string, MyStringView);
	GENERATE_FORWARD_OPERATORS_FULL_TYPE(char*, MyStringView&);
	GENERATE_FORWARD_OPERATORS_FULL_TYPE(MyStringView&, char*);

	//=====================================================================

	GENERATE_BODY_OPERATORS_REF(MyString, MyString);
	GENERATE_BODY_OPERATORS_REF(MySmallString, MySmallString);
	GENERATE_BODY_OPERATORS_REF(MyString, MySmallString);
	GENERATE_BODY_OPERATORS_REF(MySmallString, MyString);
	GENERATE_BODY_OPERATORS_REF(MyString, std::string);
	GENERATE_BODY_OPERATORS_REF(std::string, MyString);
	GENERATE_BODY_OPERATORS_REF(std::string, MySmallString);
	GENERATE_BODY_OPERATORS_REF(MySmallString, std::string);

	GENERATE_BODY_OPERATORS_REF(MyStringView, MyStringView);
	GENERATE_BODY_OPERATORS_REF(MyStringView, MyString);
	GENERATE_BODY_OPERATORS_REF(MyString, MyStringView);
	GENERATE_BODY_OPERATORS_REF(MyStringView, MySmallString);
	GENERATE_BODY_OPERATORS_REF(MySmallString, MyStringView);
	GENERATE_BODY_OPERATORS_REF(MyStringView, std::string);
	GENERATE_BODY_OPERATORS_REF(std::string, MyStringView);

	//==========================================================



	inline bool operator ==(const char* str1, const MyString& str2)
	{
		if (str1 == nullptr) return false;
		return (strcmp(str1, str2.c_str()) == 0);
	};

	inline bool operator ==(const MyString& str1, const char* str2)
	{
		return str2 == str1;
	};


	inline bool operator ==(const char* str1, const MySmallString& str2)
	{
		if (str1 == nullptr) return false;
		return (strcmp(str1, str2.c_str()) == 0);
	};

	inline bool operator ==(const MySmallString& str1, const char* str2)
	{
		return str2 == str1;
	};

	inline bool operator ==(const char* str1, const MyStringView& str2)
	{
		//https://stackoverflow.com/questions/28997095/comparing-null-terminated-string-with-a-non-null-terminated-string-in-c
		if (str1 == nullptr) return false;
		return (strncmp(str1, str2.c_str(), str2.length()) == 0 && str1[str2.length()] == 0);
	};

	inline bool operator ==(const MyStringView& str1, const char* str2)
	{
		return str2 == str1;
	};


	inline bool operator !=(const char* str1, const MyString& str2)
	{
		return !(str1 == str2);
	};

	inline bool operator !=(const  MyString& str1, const char* str2)
	{
		return !(str1 == str2);
	};

	inline bool operator !=(const char* str1, const MySmallString& str2)
	{
		return !(str1 == str2);
	};

	inline bool operator !=(const  MySmallString& str1, const char* str2)
	{
		return !(str1 == str2);
	};

	inline bool operator !=(const char* str1, const MyStringView& str2)
	{
		return !(str1 == str2);
	};

	inline bool operator !=(const MyStringView& str1, const char* str2)
	{
		return !(str1 == str2);
	};


	inline bool operator <(const char* str1, const MyString& str2)
	{
		if (str1 == nullptr) return true;
		return (strcmp(str1, str2.c_str()) < 0);
	};

	inline bool operator <(const MyString& str1, const char* str2)
	{
		return str2 < str1;
	};

	inline bool operator <(const char* str1, const MySmallString& str2)
	{
		if (str1 == nullptr) return true;
		return (strcmp(str1, str2.c_str()) < 0);
	};

	inline bool operator <(const MySmallString& str1, const char* str2)
	{
		return str2 < str1;
	};

	inline bool operator <(const char* str1, const MyStringView& str2)
	{
		//not tested
		if (str1 == nullptr) return false;
		return (strncmp(str1, str2.c_str(), str2.length()) < 0);
	};

	inline bool operator <(const MyStringView& str1, const char* str2)
	{
		return str2 < str1;
	};



	inline bool operator >(const char* str1, const MyString& str2)
	{
		if (str1 == nullptr) return false;
		return (strcmp(str1, str2.c_str()) > 0);
	};

	inline bool operator >(const MyString& str1, const char* str2)
	{
		return str2 > str1;
	};

	inline bool operator >(const char* str1, const MySmallString& str2)
	{
		//not tested
		if (str2 == nullptr) return false;
		return (strcmp(str1, str2.c_str()) > 0);
	};

	inline bool operator >(const MySmallString& str1, const char* str2)
	{
		return str2 > str1;
	};

	inline bool operator >(const char* str1, const MyStringView& str2)
	{
		if (str1 == nullptr) return false;
		return (strncmp(str1, str2.c_str(), str2.length()) > 0);
	};

	inline bool operator >(const MyStringView& str1, const char* str2)
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