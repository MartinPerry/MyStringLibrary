#ifndef _COMPARISON_OPERATORS_H_
#define _COMPARISON_OPERATORS_H_

#include "./MyStringAnsi.h"

#include <cstring>

//================================================================

//define forward declaration
#define GENERATE_OPERATOR_FORWARD(p1, p2, op) \
		bool operator op(const p1 &str1, const p2 &str2);

#define GENERATE_BODY_EQUALS(p1, p2) \
	inline bool operator ==(const p1 &str1, const p2 &str2){ \
		if (str1.length() != str2.length()) return false; \
		return (strcmp(str1.c_str(), str2.c_str()) == 0); \
	};


#define GENERATE_BODY_NOT_EQUALS(p1, p2) \
	inline bool operator !=(const p1 &str1, const p2 &str2){ \
		if (str1.length() == str2.length()) return false; \
		return (strcmp(str1.c_str(), str2.c_str()) != 0); \
	};
	
#define GENERATE_BODY_GREATER(p1, p2, op) \
	inline bool operator op(const p1 &str1, const p2 &str2){ \
		return (strcmp(str1.c_str(), str2.c_str()) op 0); \
	};

//================================================================

GENERATE_OPERATOR_FORWARD(MyStringAnsi, MyStringAnsi, == );


GENERATE_OPERATOR_FORWARD(MyStringAnsi, MyStringAnsi, != );

GENERATE_OPERATOR_FORWARD(MyStringAnsi, MyStringAnsi, < );

GENERATE_OPERATOR_FORWARD(MyStringAnsi, MyStringAnsi, > );

//=====================================================================

GENERATE_BODY_EQUALS(MyStringAnsi, MyStringAnsi);


GENERATE_BODY_NOT_EQUALS(MyStringAnsi, MyStringAnsi);


GENERATE_BODY_GREATER(MyStringAnsi, MyStringAnsi, <);

GENERATE_BODY_GREATER(MyStringAnsi, MyStringAnsi, >);


//==========================================================


bool operator ==(const char * str1, const MyStringAnsi & str2);
bool operator ==(const MyStringAnsi & str1, const char * str2);

bool operator !=(const char * str1, const MyStringAnsi &str2);
bool operator !=(const MyStringAnsi &str1, const char * str2);


inline bool operator ==(const char * str1, const MyStringAnsi & str2)
{
	if (str1 == nullptr) return false;
	return (strcmp(str1, str2.c_str()) == 0);
};

inline bool operator ==(const MyStringAnsi &str1, const char * str2)
{
	if (str2 == nullptr) return false;
	return (strcmp(str1.c_str(), str2) == 0);
};




inline bool operator !=(const char * str1, const MyStringAnsi &str2)
{
	if (str1 == nullptr) return true;
	return (strcmp(str1, str2.c_str()) != 0);
};

inline bool operator !=(const  MyStringAnsi &str1, const char * str2)
{
	if (str2 == nullptr) return true;
	return (strcmp(str1.c_str(), str2) != 0);
};




//================================
//undef defined values

#ifdef GENERATE_OPERATOR_FORWRAD
	#undef GENERATE_OPERATOR_FORWRAD
#endif

#ifdef GENERATE_BODY_EQUALS
	#undef GENERATE_BODY_EQUALS
#endif

#ifdef GENERATE_BODY_NOT_EQUALS
	#undef GENERATE_BODY_NOT_EQUALS
#endif

#ifdef GENERATE_BODY_GREATER
	#undef GENERATE_BODY_GREATER
#endif



#endif