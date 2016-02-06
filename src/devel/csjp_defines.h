/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012 Csaszar, Peter
 */

#ifndef CSJP_DEFINES_H
#define CSJP_DEFINES_H

#ifdef NO_COLOR
#define VT_GRAPHICS_MODE(code) ""
#else
#define VT_GRAPHICS_MODE(code) "\e\["#code"m"
#endif

#define VT_NORMAL		VT_GRAPHICS_MODE(0)
#define VT_TA_BOLD		VT_GRAPHICS_MODE(1)
#define VT_TA_UNDERSCORE	VT_GRAPHICS_MODE(4)
#define VT_TA_BLINK		VT_GRAPHICS_MODE(5)
#define VT_TA_REVERSE		VT_GRAPHICS_MODE(7)
#define VT_TA_CONCEALED		VT_GRAPHICS_MODE(8)

#define VT_BLACK	VT_GRAPHICS_MODE(30)
#define VT_RED		VT_GRAPHICS_MODE(31)
#define VT_GREEN	VT_GRAPHICS_MODE(32)
#define VT_YELLOW	VT_GRAPHICS_MODE(33)
#define VT_BLUE		VT_GRAPHICS_MODE(34)
#define VT_MAGENTA	VT_GRAPHICS_MODE(35)
#define VT_CYAN		VT_GRAPHICS_MODE(36)
#define VT_WHITE	VT_GRAPHICS_MODE(37)

#define VT_BG_BLACK	VT_GRAPHICS_MODE(40)
#define VT_BG_RED	VT_GRAPHICS_MODE(41)
#define VT_BG_GREEN	VT_GRAPHICS_MODE(42)
#define VT_BG_YELLOW	VT_GRAPHICS_MODE(43)
#define VT_BG_BLUE	VT_GRAPHICS_MODE(44)
#define VT_BG_MAGENTA	VT_GRAPHICS_MODE(45)
#define VT_BG_CYAN	VT_GRAPHICS_MODE(46)
#define VT_BG_WHITE	VT_GRAPHICS_MODE(47)

#ifndef NULL
#define NULL 0
#endif

#undef TEMP_FAILURE_RETRY /* for android, but may be would be wiser anyway */
#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(call) \
	{long int ret; do ret = call; while (ret == -1 && errno == EINTR);}
#define TEMP_FAILURE_RETRY_RESULT(result, call) \
	do result = call; while (result == -1 && errno == EINTR);
#endif

#define STRING_HELPER(expr) #expr
#define STRING(expr) STRING_HELPER(expr)

#include <csjp_utility.h>

namespace csjp {

template<typename Type>
Type && move_cast(Type & t)
{
	return (Type &&)t;
}

template<typename Type>
void swap(Type & a, Type & b)
{
	Type tmp(move_cast(a));
	a = move_cast(b);
	b = move_cast(tmp);
}

}

//inline void* operator new(long unsigned, void* p) { return p; }

#endif
