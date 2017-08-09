/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace csjp {

class CString
{
public:
	const char * ptr;
	CString(const char * ptr = 0) : ptr(ptr) {}
	operator const char * () const { return ptr; }
};

inline bool			operator==(const CString a,		const char * b)
								{ return !strcmp(a.ptr, b); }
inline char &			operator<<=(char & c,			const CString str)
								{ c = atoi(str); return c; }
inline unsigned char &		operator<<=(unsigned char & c,		const CString str)
								{ c = atoi(str); return c; }
inline int &			operator<<=(int & i,			const CString str)
								{ i = atoi(str); return i; }
inline long int &		operator<<=(long int & i,		const CString str)
								{ i = atol(str); return i; }
inline long long int &		operator<<=(long long int & i,		const CString str)
								{ i = atoll(str); return i; }
inline unsigned &		operator<<=(unsigned & i,		const CString str)
								{ i = atol(str); return i; }
inline long unsigned &		operator<<=(long unsigned & i,		const CString str)
								{ i = atoll(str); return i; }
inline long long unsigned &	operator<<=(long long unsigned & i,	const CString str)
								{ i = atoll(str); return i; }
inline float &			operator<<=(float & i,			const CString str)
								{ i = atof(str); return i; }
inline double &			operator<<=(double & i,			const CString str)
								{ i = strtod(str, 0); return i; }
inline long double &		operator<<=(long double & i,		const CString str)
								{ i = strtold(str, 0); return i; }

struct YNBool
{
	bool val;
	YNBool() : val(false) {}
	YNBool & operator=(bool b) { val = b; return *this; }
	YNBool & operator=(const char * s)
	{
		val = false;
		if(s != 0 && s[1] == 0 && s[0] == 'Y')
			val = true;
		return *this;
	}
	operator bool() const { return val; }
};

inline bool operator==(const YNBool & lhs, const YNBool & rhs) { return lhs.val == rhs.val; }
inline bool operator!=(const YNBool & lhs, const YNBool & rhs) { return lhs.val != rhs.val; }
inline bool operator==(const YNBool & lhs, bool rhs) { return lhs.val == rhs; }
inline bool operator!=(const YNBool & lhs, bool rhs) { return lhs.val != rhs; }
inline bool operator==(bool lhs, const YNBool & rhs) { return lhs == rhs.val; }
inline bool operator!=(bool lhs, const YNBool & rhs) { return lhs != rhs.val; }
inline bool operator<(const YNBool & lhs, const YNBool & rhs)
		{ return (lhs.val == false) && (rhs.val == true); }
inline YNBool & operator<<=(YNBool & lhs, const char * rhs) { lhs = rhs; return lhs; }

struct Char
{
	char val;
	Char() : val(0) {}
	Char(char u) : val(u) {}
	Char & operator=(char u) { val = u; return *this; }
	Char & operator=(const char * s) { val <<= CString(s); return *this; }
	Char operator++(int) { Char u(val); ++val; return u; }
	operator char() const { return val; }
};

inline bool operator==(const Char & lhs, const Char & rhs) { return lhs.val == rhs.val; }
inline bool operator!=(const Char & lhs, const Char & rhs) { return lhs.val != rhs.val; }
inline bool operator==(const Char & lhs, char rhs) { return lhs.val == rhs; }
inline bool operator!=(const Char & lhs, char rhs) { return lhs.val != rhs; }
inline bool operator==(char lhs, const Char & rhs) { return lhs == rhs.val; }
inline bool operator!=(char lhs, const Char & rhs) { return lhs != rhs.val; }
inline bool operator<(const Char & lhs, const Char & rhs) { return lhs.val < rhs.val; }
inline Char & operator<<=(Char & lhs, const CString & rhs) { lhs.val <<= rhs; return lhs; }

struct UInt
{
	unsigned val;
	UInt() : val(0) {}
	UInt(unsigned u) : val(u) {}
	UInt & operator=(unsigned u) { val = u; return *this; }
	UInt & operator=(const char * s) { val <<= CString(s); return *this; }
	UInt operator++(int) { UInt u(val); ++val; return u; }
	operator unsigned() const { return val; }
};

inline bool operator==(const UInt & lhs, const UInt & rhs) { return lhs.val == rhs.val; }
inline bool operator!=(const UInt & lhs, const UInt & rhs) { return lhs.val != rhs.val; }
inline bool operator==(const UInt & lhs, unsigned rhs) { return lhs.val == rhs; }
inline bool operator!=(const UInt & lhs, unsigned rhs) { return lhs.val != rhs; }
inline bool operator==(const UInt & lhs, int rhs)
		{ return 0 <= rhs && (lhs.val == (unsigned)rhs); }
inline bool operator!=(const UInt & lhs, int rhs)
		{ return rhs  < 0 || (lhs.val != (unsigned)rhs); }
inline bool operator==(unsigned lhs, const UInt & rhs) { return lhs == rhs.val; }
inline bool operator!=(unsigned lhs, const UInt & rhs) { return lhs != rhs.val; }
inline bool operator==(int lhs, const UInt & rhs)
		{ return 0 <= lhs && ((unsigned)lhs == rhs.val); }
inline bool operator!=(int lhs, const UInt & rhs)
		{ return lhs < 0 || ((unsigned)lhs != rhs.val); }
inline bool operator<(const UInt & lhs, const UInt & rhs) { return lhs.val < rhs.val; }
inline UInt & operator<<=(UInt & lhs, const CString & rhs) { lhs.val <<= rhs; return lhs; }

struct Double
{
	double val;
	Double() : val(0) {}
	Double(double d) : val(d) {}
	Double(const Double & d) : val(d.val) {}
	Double & operator=(unsigned u) { val = u; return *this; }
	Double & operator=(const char * s) { val <<= CString(s); return *this; }
	const Double & operator+=(const Double & rhs) { val += rhs.val; return *this; }
	const Double & operator-=(const Double & rhs) { val -= rhs.val; return *this; }
	const Double & operator-=(const double & rhs) { val -= rhs; return *this; }
	const Double & operator/=(const Double & rhs) { val /= rhs.val; return *this; }
	operator unsigned() const { return val; }
	Double & abs() { val = fabs(val); return *this; }
	void nan() { val = NAN; }
};

inline Double operator/(const Double & lhs, const Double & rhs) { Double d(lhs); d/=rhs; return d; }
inline Double operator-(const Double & lhs, const Double & rhs) { Double d(lhs); d-=rhs; return d; }
inline Double operator-(const Double & lhs, const double rhs) { Double d(lhs); d-=rhs; return d; }
inline bool operator==(const Double & lhs, const Double & rhs) { return lhs.val == rhs.val; }
inline bool operator!=(const Double & lhs, const Double & rhs) { return lhs.val != rhs.val; }
inline bool operator==(const Double & lhs, double rhs) { return lhs.val == rhs; }
inline bool operator!=(const Double & lhs, double rhs) { return lhs.val != rhs; }
inline bool operator==(double lhs, const Double & rhs) { return lhs == rhs.val; }
inline bool operator!=(double lhs, const Double & rhs) { return lhs != rhs.val; }
inline bool operator<(const Double & lhs, const Double & rhs) { return lhs.val < rhs.val; }
inline bool operator<(const Double & lhs, double rhs) { return lhs.val < rhs; }
inline bool operator<=(const double & lhs, const Double & rhs) { return lhs <= rhs.val; }
inline Double & operator<<=(Double & lhs, const CString & rhs) { lhs.val <<= rhs; return lhs; }

}

#endif
