
/* Copyright (c) 2014-2015, Human Brain Project
 *                          Stefan.Eilemann@epfl.ch
 */

#include <servus/uint128_t.h>

#ifndef ZEROEQ_DETAIL_BYTESWAP_H
#define ZEROEQ_DETAIL_BYTESWAP_H

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4985) // inconsistent decl of ceil
#include <intrin.h>
#pragma warning(pop)
#elif defined __xlC__
#include <builtins.h>
#include <byteswap.h>
#elif defined(LB_GCC_4_3_OR_OLDER) && !defined(__clang__) && !defined(__APPLE__)
#include <byteswap.h>
#define USE_GCC_BSWAP_FUNCTION
#endif

namespace zeroeq
{
namespace detail
{
inline void byteswap(uint64_t& value)
{
#ifdef _MSC_VER
    value = _byteswap_uint64(value);
#elif defined __xlC__
    value = __bswap_constant_64(value);
#elif defined USE_GCC_BSWAP_FUNCTION
    value = bswap_64(value);
#else
    value = __builtin_bswap64(value);
#endif
}

inline void byteswap(uint128_t& value)
{
    byteswap(value.low());
    byteswap(value.high());
}

#undef USE_GCC_BSWAP_FUNCTION
}
}
#endif
