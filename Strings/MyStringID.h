#ifndef MY_STRING_ID_H
#define MY_STRING_ID_H

//ID_DEBUG - in this mode, string is stored together with ID
//-> this mode can not be used together with constexpr !!!
//#define ID_DEBUG



//================================
//includes

#ifdef ID_DEBUG
#	include <unordered_map>
#	include <set>
#	include <cassert>
#endif


#include "./MurmurHash3.h"

#include "./MyStringAnsi.h"
#include "./MySmallStringAnsi.h"
#include "./MyStringView.h"


//================================
//class declaration

class MyStringId
{
public:
#if defined(ID_DEBUG) || defined(CONSTEXPR_DISABLE)
	MyStringId(const char * key);
#else
	constexpr MyStringId(const char * key);
#endif
	MyStringId(const MyStringAnsi & key);
	MyStringId(const MySmallStringAnsi & key);
	MyStringId(const MyStringView & key);
	MyStringId(const MyStringId & key);
	MyStringId(const uint32_t & key);

	MyStringId& operator=(const MyStringId & other)
	{
		// check for self-assignment
		if (&other == this)
		{
			return *this;
		}
#ifdef ID_DEBUG
		this->debugKey = other.debugKey;
#endif
		this->hashId = other.hashId;

		return *this;
	}

	inline uint32_t GetHashId() const { return this->hashId; };
	inline uint32_t GetHashCode() const { return this->hashId; };
	inline operator uint32_t() { return this->hashId; }
	inline operator uint32_t() const { return this->hashId; }


private:
#ifdef ID_DEBUG
	MyStringAnsi debugKey;
	static void Check(const MyStringAnsi & key, uint32_t hash);
#endif
	uint32_t hashId;
};

//================================
// add operator() to std namepespace
//this will enabled to use our ID in hash_map

namespace std
{
	template <>
	struct hash <MyStringId>
	{
		size_t operator()(const MyStringId & k) const
		{
			return k.GetHashId();
		};
	};
};



//================================
//declaration of inline ctor
//this is used mainly for constexpr

#if defined(ID_DEBUG) || defined(CONSTEXPR_DISABLE)
inline MyStringId::MyStringId(const char * key)
	: hashId(CALC_HASH(key))
{
#ifdef ID_DEBUG
	this->debugKey = key;
	MyStringId::Check(debugKey, hashID);
#endif
}
#else
constexpr inline MyStringId::MyStringId(const char * key)
	: hashId(CALC_HASH(key))
{

}
#endif

//other ctors
//those are not constexpr

inline MyStringId::MyStringId(const MyStringAnsi & key)
	: hashId(key.GetHashCode())
{
#ifdef ID_DEBUG
	this->debugKey = key;
	MyStringId::Check(debugKey, hashID);
#endif	
}

inline MyStringId::MyStringId(const MySmallStringAnsi & key)
	: hashId(key.GetHashCode())
{
#ifdef ID_DEBUG
	this->debugKey = key;
	MyStringId::Check(debugKey, hashID);
#endif	
}

inline MyStringId::MyStringId(const MyStringView & key)
	: hashId(key.GetHashCode())
{
#ifdef ID_DEBUG
	this->debugKey = key;
	MyStringId::Check(debugKey, hashID);
#endif	
}


inline MyStringId::MyStringId(const MyStringId & key)
	: hashId(key.GetHashId())
{
#ifdef ID_DEBUG
	this->debugKey = key.debugKey;
	MyStringId::Check(debugKey, hashID);
#endif	
}

inline MyStringId::MyStringId(const uint32_t & key)
	: hashId(key)
{
#ifdef ID_DEBUG
	this->debugKey = "direct_hash_value_";
	this->debugKey += key;
	MyStringId::Check(debugKey, hashId);
#endif

}


#ifdef ID_DEBUG
inline void MyStringId::Check(const MyStringAnsi & key, uint32_t hashId)
{
	static std::unordered_map<uint32_t, std::set<MyStringAnsi> > collisions;

	if (collisions.find(hashId) == collisions.end())
	{
		//hashID not exist yet
		collisions[hashId].insert(key);
		return;
	}

	if (collisions[hashId].find(key) == collisions[hashId].end())
	{
		//hashID already exist... only one key can be set for this hashID
		assert(collisions[hashId].size() == 0);
	}

	collisions[hashId].insert(key);
}
#endif
//================================

//define forward declaration
#define GENERATE_ID_FORWARD_OPERATORS_REF(p1, p2) \
	bool operator ==(const p1 & str1, const p2 & str2); \
	bool operator !=(const p1 & str1, const p2 & str2); \
	bool operator >(const p1 & str1, const p2 & str2); \
	bool operator <(const p1 & str1, const p2 & str2);


#define GENERATE_ID_BODY_OPERATORS_REF(p1, p2) \
	inline bool operator ==(const p1 & str1, const p2 & str2){ \
		return (str1.GetHashCode() == str2.GetHashCode()); \
	}; \
	inline bool operator !=(const p1 & str1, const p2 & str2){ \
		return (str1.GetHashCode() != str2.GetHashCode()); \
	}; \
	inline bool operator >(const p1 & str1, const p2 & str2){ \
		return (str1.GetHashCode() > str2.GetHashCode()); \
	}; \
	inline bool operator <(const p1 & str1, const p2 & str2) { \
		return (str1.GetHashCode() < str2.GetHashCode()); \
	};

#define GENERATE_BODY2(p1, p2, op) \
	inline bool operator op(const p1 &id1, const p2 &id2){ \
		return (id1 op id2.GetHashCode());  \
	}; 

#define GENERATE_BODY3(p1, p2, op) \
	inline bool operator op(const p1 &id1, const p2 &id2){ \
		return (id1.GetHashCode() op id2);  \
	}; 

//================================

//declare friend operators to compare our ID with different
//classes and structures directly

GENERATE_ID_FORWARD_OPERATORS_REF(MyStringId, MyStringId);
GENERATE_ID_FORWARD_OPERATORS_REF(MyStringId, MyStringAnsi);
GENERATE_ID_FORWARD_OPERATORS_REF(MyStringId, MySmallStringAnsi);
GENERATE_ID_FORWARD_OPERATORS_REF(MyStringId, MyStringView);
GENERATE_ID_FORWARD_OPERATORS_REF(MyStringId, uint32_t);
GENERATE_ID_FORWARD_OPERATORS_REF(MyStringAnsi, MyStringId);
GENERATE_ID_FORWARD_OPERATORS_REF(MySmallStringAnsi, MyStringId);
GENERATE_ID_FORWARD_OPERATORS_REF(MyStringView, MyStringId);
GENERATE_ID_FORWARD_OPERATORS_REF(uint32_t, MyStringId);

//================================

GENERATE_ID_BODY_OPERATORS_REF(MyStringId, MyStringId);
GENERATE_ID_BODY_OPERATORS_REF(MyStringId, MyStringAnsi);
GENERATE_ID_BODY_OPERATORS_REF(MyStringId, MySmallStringAnsi);
GENERATE_ID_BODY_OPERATORS_REF(MyStringId, MyStringView);
GENERATE_ID_BODY_OPERATORS_REF(MyStringAnsi, MyStringId);
GENERATE_ID_BODY_OPERATORS_REF(MySmallStringAnsi, MyStringId);
GENERATE_ID_BODY_OPERATORS_REF(MyStringView, MyStringId);


GENERATE_BODY2(uint32_t, MyStringId, == );
GENERATE_BODY3(MyStringId, uint32_t, == );


GENERATE_BODY2(uint32_t, MyStringId, != );
GENERATE_BODY3(MyStringId, uint32_t, != );


GENERATE_BODY2(uint32_t, MyStringId, < );
GENERATE_BODY3(MyStringId, uint32_t, < );

GENERATE_BODY2(uint32_t, MyStringId, > );
GENERATE_BODY3(MyStringId, uint32_t, > );


//================================
//undef defined values

#ifdef GENERATE_ID_FORWARD_OPERATORS_REF
#	undef GENERATE_ID_FORWARD_OPERATORS_REF
#endif

#ifdef GENERATE_ID_BODY_OPERATORS_REF
#	undef GENERATE_ID_BODY_OPERATORS_REF
#endif

#ifdef GENERATE_BODY2
#	undef GENERATE_BODY2
#endif

#ifdef GENERATE_BODY3
#	undef GENERATE_BODY3
#endif

#ifdef ID_DEBUG
#	undef ID_DEBUG
#endif



#endif //MY_STRING_ID_H
