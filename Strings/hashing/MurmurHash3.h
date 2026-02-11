//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef MURMURHASH3_H
#define MURMURHASH3_H

#ifndef MURMUR_HASH_DEF_SEED
#	define MURMUR_HASH_DEF_SEED 0
#endif


#ifdef CONSTEXPR_DISABLE
#	define CALC_HASH(x) MurmurHash3_x86_32(x)
//#	define CALC_HASH(x) strlen(x)
#else
#	define CALC_HASH(x) MurmurHash3_32CExpr(x)
//#	define CALC_HASH(x) StringLengthCExpr(x)
#endif

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER) && (_MSC_VER < 1600)
	typedef unsigned char uint8_t;
	typedef unsigned int uint32_t;
	typedef unsigned __int64 uint64_t;
#else	// defined(_MSC_VER)
#	include <stdint.h>
#endif

//-----------------------------------------------------------------------------

uint32_t MurmurHash3_x86_32(const void * key);

uint32_t MurmurHash3_x86_32(const void * key, int len);

uint32_t MurmurHash3_x86_32(const void * key, int len, uint32_t seed);



void MurmurHash3_x86_128(const void * key, int len, uint32_t seed, void * out);

void MurmurHash3_x64_128(const void * key, int len, uint32_t seed, void * out);

//-----------------------------------------------------------------------------


#include "MurmurHash3_constexpr.inl"

#endif // _MURMURHASH3_H_
