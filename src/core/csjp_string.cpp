/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011 Csaszar, Peter
 */

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <regex.h>

#include "csjp_string.h"

namespace csjp {

void String::setCapacity(size_t _cap)
{
	char *dst = (char *)realloc(data, _cap + 1);
	if(!dst)
		throw OutOfMemory("No enough memory for string allocation with size of % bytes.",
				size);

	size = _cap + 1;
	data = dst;
	if(_cap < len)
		len = _cap;
	data[len] = 0;
}

void String::extendCapacity(size_t _cap)
{
	ENSURE(len <= _cap,  InvalidArgument);

	/* Preallocate 10% more bytes (or 100% more if 10% was less than 2048 bytes). */
	size_t inc = _cap / 10;
	if(inc < 2048)
		inc = _cap;
	_cap += inc;

	setCapacity(_cap);
}

size_t String::capacity() const
{
	return (0 < size) ? size - 1 : 0;
}

void String::setLength(size_t length)
{
	if(size <= length)
		throw InvalidArgument(
				"String capacity is %, requested length to set is %.",
				size ? size-1 : 0, length);
	len = length;
}

bool String::findFirst(size_t & pos, const char * str, size_t _length, size_t from, size_t until)
	const
{
	return AStr::findFirst(pos, str, _length, from, until);
}

bool String::findFirst(size_t & pos, const Str & str, size_t from, size_t until) const
{
	return findFirst(pos, str.data, str.len, from, until);
}

bool String::findFirst(size_t & pos, const Str & str, size_t from) const
{
	return findFirst(pos, str.data, str.len, from, len);
}

bool String::findFirst(size_t & pos, char c, size_t from) const
{
	return findFirstOf(pos, &c, 1, from, len);
}

bool String::findFirstOf(size_t & pos, const char * str, size_t _length, size_t from, size_t until) const
{
	return AStr::findFirstOf(pos, str, _length, from, until);
}

bool String::findFirstOf(size_t & pos, const Str & str, size_t from, size_t until) const
{
	return findFirstOf(pos, str.data, str.len, from, until);
}

bool String::findFirstOf(size_t & pos, const Str & str, size_t from) const
{
	return findFirstOf(pos, str.data, str.len, from, len);
}

bool String::findFirstNotOf(size_t & pos, const char * str, size_t _length, size_t from, size_t until) const
{
	return AStr::findFirstNotOf(pos, str, _length, from, until);
}

bool String::findFirstNotOf(size_t & pos, const Str & str, size_t from, size_t until) const
{
	return findFirstNotOf(pos, str.data, str.len, from, until);
}

bool String::findFirstNotOf(size_t & pos, const Str & str, size_t from) const
{
	return findFirstNotOf(pos, str.data, str.len, from, len);
}

bool String::findLast(size_t & pos, const char * str, size_t _length, size_t until) const
{
	return AStr::findLast(pos, str, _length, until);
}

bool String::findLast(size_t & pos, const Str & str, size_t until) const
{
	return findLast(pos, str.data, str.len, until);
}

bool String::findLast(size_t & pos, const Str & str) const
{
	return findLast(pos, str.data, str.len, len);
}

bool String::findLast(size_t & pos, char c, size_t until) const
{
	return findLastOf(pos, &c, 1, until);
}

bool String::findLast(size_t & pos, char c) const
{
	return findLastOf(pos, &c, 1, len);
}

bool String::findLastOf(size_t & pos, const char * str, size_t _length, size_t until) const
{
	return AStr::findLastOf(pos, str, _length, until);
}

bool String::findLastOf(size_t & pos, const Str & str, size_t until) const
{
	return findLastOf(pos, str.data, str.len, until);
}

bool String::findLastOf(size_t & pos, const Str & str) const
{
	return findLastOf(pos, str.data, str.len, len);
}

bool String::findLastNotOf(size_t & pos, const char * str, size_t _length, size_t until) const
{
	return AStr::findLastNotOf(pos, str, _length, until);
}

bool String::findLastNotOf(size_t & pos, const Str & str, size_t until) const
{
	return findLastNotOf(pos, str.data, str.len, until);
}

bool String::findLastNotOf(size_t & pos, const Str & str) const
{
	return findLastNotOf(pos, str.data, str.len, len);
}

bool String::startsWith(const char * str, size_t _length) const
{
	return AStr::startsWith(str, _length);
}

bool String::startsWith(const char * str) const
{
	if(!str)
		return true;

	size_t _length = strlen(str);

	return startsWith(str, _length);
}

bool String::endsWith(const char * str, size_t _length) const
{
	return AStr::endsWith(str, _length);
}

bool String::endsWith(const char * str) const
{
	if(!str)
		return true;

	size_t _length = strlen(str);

	return endsWith(str, _length);
}

size_t String::count(const char * str, size_t _length, size_t from, size_t until) const
{
	return AStr::count(str, _length, from, until);
}

size_t String::count(const Str & str, size_t from, size_t until) const
{
	return count(str.data, str.len, from, until);
}

size_t String::count(const Str & str, size_t from) const
{
	return count(str.data, str.len, from, len);
}

void String::assign(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	if(size <= _length)
		extendCapacity(_length);

	len = _length;

	memcpy(data, str, _length);

	data[len] = 0;
}

void String::assign(const AStr & astr)
{
	if(!astr.data){
		clear();
		return;
	}

	assign(astr.data, astr.len);
}

void String::assign(const Str & str)
{
	if(!str.data){
		clear();
		return;
	}

	assign(str.data, str.len);
}

void String::fill(char _fill, size_t _length)
{
	if(len <= _length)
		if(size <= _length)
			extendCapacity(_length);

	if(_length)
		memset(data, _fill, _length);
	len = _length;
	data[len] = 0;
}

void String::fill(char _fill)
{
	fill(_fill, len);
}

/**
 * Move a part of the string to a position before the original position.
 */
void String::shiftForward(size_t from, size_t until, size_t offset)
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);
	ENSURE(offset <= from,  InvalidArgument);

	if(!data)
		return;

	const char * const srcUntil = data + until; /* points to the first not to move */
	const char * srcPtr = data + from;
	char * dstPtr = data + from - offset;

	size_t bs = sizeof(unsigned);
	if(1 < bs){
		const char * const srcUntilBS = srcUntil - bs; /* points to the first not to move */
		for(;
				srcPtr < srcUntilBS;
				srcPtr += bs, dstPtr += bs)
			*(unsigned*)(dstPtr) = *(unsigned*)(srcPtr);
	}

	/* Move the remaining chunk bytes. */
	for(; srcPtr < srcUntil; srcPtr++, dstPtr++)
		*dstPtr = *srcPtr;

	if(len == until)
		len -= offset;
	data[len] = 0;
}

/**
 * Move a part of the string to a position after the original position.
 */
void String::shiftBackward(size_t from, size_t until, size_t offset)
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);

	if(size <= until + offset)
		extendCapacity(len + offset);

	const char * const srcFrom = data + from;
	const char * srcPtr = data + until - 1;
	char * dstPtr = data + until - 1 + offset;

	size_t bs = sizeof(unsigned);
	if(1 < bs){
		for(srcPtr -= (bs - 1), dstPtr -= (bs - 1);
				srcFrom <= srcPtr;
				srcPtr -= bs, dstPtr -= bs)
			*(unsigned*)(dstPtr) = *(unsigned*)(srcPtr);
		/* Calculate the remaining bytes less then bs; reposition pointers. */
		size_t chunk = (until - from) % bs;
		srcPtr = data + from + chunk - 1;
		dstPtr = data + from + chunk - 1 + offset;
	}

	for(; srcFrom <= srcPtr; srcPtr--, dstPtr--)
		*dstPtr = *srcPtr;

	if(len <= until + offset)
		len = until + offset;
	data[len] = 0;
}

void String::prepend(char c)
{
	shiftBackward(0, len, 1);
	data[0] = c;
}

void String::prepend(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	shiftBackward(0, len, _length);
	memcpy(data, str, _length);
}

void String::append(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return;

	if(size <= len + _length)
		extendCapacity(len + _length);

	memcpy(data + len, str, _length);

	len += _length;
	data[len] = 0;
}

void String::append(const char * str)
{
	if(!str)
		return;

	size_t _length = strlen(str);

	append(str, _length);
}

void String::append(char c)
{
	if(size <= len + 1)
		extendCapacity(len + 1);

	data[len] = c;
	len++;
	data[len] = 0;
}

void String::append(unsigned char n)
{
	append((char)n);
}

void String::append(unsigned n)
{
	append((long long unsigned)n);
}

void String::append(long unsigned n)
{
	append((long long unsigned)n);
}

void String::append(long long unsigned n)
{
	long long unsigned number = n;
	char buf[sizeof(long long unsigned) * 3 + 1];
	memset(buf, ' ', sizeof(buf));
	unsigned s = sizeof(buf) - 1;
	buf[s--] = 0;
	do {
		buf[s--] = '0' + number % 10;
		number /= 10;
	} while(number);

	append(buf + s + 1);
}

void String::append(int n)
{
	append((long long int)n);
}

void String::append(long int n)
{
	append((long long int)n);
}

void String::append(long long int n)
{
	bool negative = (n < 0);
	long long int number = n;
	if(negative)
		number *= -1;
	char buf[sizeof(long long int) * 3 + 1];
	memset(buf, ' ', sizeof(buf));
	unsigned s = sizeof(buf) - 1;
	buf[s--] = 0;
	do {
		buf[s--] = '0' + number % 10;
		number /= 10;
	} while(number);

	if(negative)
		buf[s--] = '-';

	append(buf + s + 1);
}

void String::append(double n)
{
	append((long double)n);
}

void String::append(long double n, unsigned precision)
{
	//appendPrintf("%Lf", n);
	if(isnan(n)){
		append("nan");
		return;
	}
	if(isinf(n)){
		append("inf");
		return;
	}
	//if(fpclassify(n) != FP_NORMAL){
	//	throw InvalidArgument("Invalid double number");
	//}
	//printf("n: %Lf\n", n);
	bool negative = n < 0.0;
	n = fabsl(n);
	//printf("n: %Lf\n", n);
	long double number = truncl(n);
	//printf("number: %Lf\n", number);
	//long double m = pow(10.0, precision);
	long double part = n - number;
	//printf("part: %Lf\n", part);
	char buf[1024];
	//memset(buf, ' ', sizeof(buf));
	unsigned s = sizeof(buf) - 1;
	unsigned i = s;
	buf[i] = 0;
	unsigned u;
	for(i -= precision; i < s; i++){
		part *= 10;
	//printf("part: %Lf\n", part);
		u = part;
		buf[i] = '0' + u;
	//printf("buf: '%s'\n", buf+(s-precision));
		part -= u;
	//printf("part: %Lf\n", part);
	}
	i -= precision;
	i--;
	buf[i] = '.';
	//printf("buf: '%s'\n", buf+i);
	unsigned digits = log10l(number) + 1;
	number /= pow(10, digits);
	//printf("number: %Lf\n", number);
	for(i -= digits, s-=precision+1; i < s; i++){
		number *= 10;
	//printf("number: %Lf\n", number);
		u = number;
		buf[i] = '0' + u;
	//printf("buf: '%s'\n", buf+(s-digits));
		number -= u;
	//printf("number: %Lf\n", number);
	}
	i -= digits;
	if(buf[i] == '.'){
		i--;
		buf[i] = '0';
	}
	if(negative){
		i--;
		buf[i] = '-';
	}
	//printf("buf: %s\n", buf+i);
	//printf("n: %Lf\n", n);

	append(buf + i, sizeof(buf) - i - 1);
	//printf("result: '%s'\n", data);
}

void String::appendVPrintf(const char * format, va_list args, size_t _length)
{
	if(format == NULL)
		return;

#if ! _ISOC99_SOURCE
#error We need to use C99 for calculating printed number of characters with vsnprintf().
#endif

	if(size <= len + _length)
		extendCapacity(len + _length);

	int _written = vsnprintf(data + len, size - len, format, args);

	if(0 <= _written && len + _written < size){
		len += _length;
		return;
	}

	throw ShouldNeverReached(EXCLI);
}

void String::appendPrintf(const char * format, ...)
{
	if(format == NULL)
		return;

	va_list args;

	va_start(args, format);
	int _length = vsnprintf(NULL, 0, format, args);
	va_end(args);
	if(_length < 0)
		throw InvalidArgument(errno, "Libc vsnprintf() failed in %.",
				__PRETTY_FUNCTION__);

	va_start(args, format);
	appendVPrintf(format, args, _length);
	va_end(args);
}

void String::insert(size_t pos, const char * str, size_t _length)
{
	ENSURE(pos <= len,  InvalidArgument);
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return;

	shiftBackward(pos, len, _length);
	memcpy(data + pos, str, _length);
}

void String::erase(size_t from, size_t until)
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);

	shiftForward(until, len, until - from);
}

/** The str to write must fit in the length of the existing string. */
void String::write(size_t pos, const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);
	ENSURE(pos + _length <= len,  InvalidArgument);

	if(!data)
		return;

	memcpy(data + pos, str, _length);
}

String String::read(size_t from, size_t until) const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);

	String str;

	if(from != until)
		str.assign(data + from, until - from);

	return str;
}

void String::replace(const char * what, size_t whatLength, const char * to, size_t toLength, size_t from, size_t until, size_t maxNumOfRepl)
{
	ENSURE(what,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(to || !toLength,  InvalidArgument);

	if (from == until)
		return;
	if(until - from < whatLength)
		return;
	if(!to)
		to = "";

	size_t pos = from;
	size_t matches = 0;
	size_t diffLength = 0;

	/* Three main cases:
	 * 1) whatLength == toLength -- do replace at once
	 * 2) whatLength < toLength -- count matches, allocate needed memory before any change on the input
	 * 3) whatLength > toLength -- count matches, do replaces with shrinking
	 */

	if(whatLength == toLength) {
		while(findFirst(pos, what, whatLength, pos, until)){
			write(pos, to, toLength);
			pos += whatLength;
			matches++;
			if(matches == maxNumOfRepl)
				break;
		}
		return;
	}

	/* Lets count matches. */
	while(findFirst(pos, what, whatLength, pos, until)){
		pos += whatLength;
		matches++;
		if(matches == maxNumOfRepl)
			break;
	}

	if(whatLength < toLength) {
		diffLength = toLength - whatLength;
		if(size <= len + matches * diffLength)
			extendCapacity(len + matches * diffLength);

		size_t chunkEndPos = len;
		for(; 0 < matches; matches--) {
			size_t offset = matches * (diffLength);
			//DBG("pos: %, chunkEndPos: %, offset: %", pos, chunkEndPos, offset);
			//DBG("Before shiftBackward from: %, until: %, offset: %, str: '%'",
			//		pos, chunkEndPos, offset, data);
			shiftBackward(pos, chunkEndPos, offset);
			//DBG("After shiftBackward from: %, until: %, offset: %, str: '%'",
			//		pos, chunkEndPos, offset, data);
			write(pos + offset - toLength, to, toLength);
			//DBG("After write pos: %, to: %, str: '%'",
			//		pos + offset - toLength, to, data);
			if(matches == 1)
				break;
			pos -= whatLength;
			chunkEndPos = pos;
			pos -= 1; // step one before the last string we found
			findLast(pos, what, whatLength, pos); // could fail only in last cycle
			pos += whatLength;
		}
	} else {
		diffLength = whatLength - toLength;
		size_t offset = diffLength;
		size_t lastPos = 0;
		pos = 0;
		findFirst(pos, what, whatLength, pos, until);
		for(; 0 < matches; offset += diffLength, matches--) {
			//DBG("Before write pos: %, to: %, str: '%'", pos , to, data);
			write(pos, to, toLength);
			//DBG("After  write pos: %, to: %, str: '%'", pos , to, data);
			lastPos = pos + toLength;
			findFirst(pos, what, whatLength, pos + toLength + offset, until); // the last call might not find anything
			if(matches == 1)
				pos = len; // for the last replace, we will move the rest of the string
			//DBG("After findfirst: pos: %, lastPos: %", pos, lastPos);
			//DBG("Before shiftForward from: %, until: %, offset: %, str: '%'",
			//		lastPos + offset, pos, offset, data);
			shiftForward(lastPos + offset, pos, offset);
			//DBG("After  shiftForward from: %, until: %, offset: %, str: '%'",
			//		lastPos + offset, pos, offset, data);
			pos -= offset;
		}
	}
}

void String::replace(const char * what, size_t whatLength, const char * to, size_t toLength, size_t from, size_t until)
{
	replace(what, whatLength, to, toLength, from, until, len);
}

void String::replace(const char * what, size_t whatLength, const char * to, size_t toLength, size_t from)
{
	replace(what, whatLength, to, toLength, from, len, len);
}

void String::replace(const char * what, const char * to, size_t from, size_t until, size_t maxNumOfRepl)
{
	size_t whatLength = 0;
	if(what)
		whatLength = strlen(what);
	size_t toLength = 0;
	if(to)
		toLength = strlen(to);
	replace(what, whatLength, to, toLength, from, until, maxNumOfRepl);
}

void String::replace(const char * what, const char * to, size_t from, size_t until)
{
	size_t whatLength = 0;
	if(what)
		whatLength = strlen(what);
	size_t toLength = 0;
	if(to)
		toLength = strlen(to);
	replace(what, whatLength, to, toLength, from, until, len);
}

void String::replace(const char * what, const char * to, size_t from)
{
	size_t whatLength = 0;
	if(what)
		whatLength = strlen(what);
	size_t toLength = 0;
	if(to)
		toLength = strlen(to);
	replace(what, whatLength, to, toLength, from, len, len);
}

void String::replace(const Str & what, const Str & to,
						size_t from, size_t until, size_t maxNumOfRepl)
{
	replace(what.data, what.len, to.data, to.len, from, until, maxNumOfRepl);
}

void String::replace(const Str & what, const Str & to, size_t from, size_t until)
{
	replace(what.data, what.len, to.data, to.len, from, until, len);
}

void String::replace(const Str & what, const Str & to, size_t from)
{
	replace(what.data, what.len, to.data, to.len, from, len, len);
}

void String::clear()
{
	if(!data)
		return;

	free(data);
	data = 0;
	len = 0;
	size = 0;
}

void String::chopFront(size_t _length)
{
	ENSURE(_length <= len,  InvalidArgument);

	shiftForward(_length, len, _length);
}

void String::chopBack(size_t _length)
{
	ENSURE(_length <= len,  InvalidArgument);

	len -= _length;
	data[len] = 0;
}

void String::cutAt(size_t _length)
{
	ENSURE(_length <= len,  InvalidArgument);

	if(!data)
		return;

	len = _length;
	data[len] = 0;
}

void String::chop()
{
	if(!data)
		return;

	len = 0;
	data[len] = 0;
}

void String::trim(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return;

	size_t pos;

	if(findLastNotOf(pos, str, _length, len))
		cutAt(pos + 1);

	if(findFirstNotOf(pos, str, _length, 0, len))
		chopFront(pos);
	else
		cutAt(0);
}

void String::trimFront(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return;

	size_t pos;

	if(findFirstNotOf(pos, str, _length, 0, len))
		chopFront(pos);
	else
		cutAt(0);
}

void String::trimBack(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return;

	size_t pos;

	if(findLastNotOf(pos, str, _length, len))
		cutAt(pos + 1);
}

Array<Str> String::split(const char * delimiters, bool avoidEmptyResults) const
{
	return AStr::split(delimiters, avoidEmptyResults);
}

void String::adopt(char *& str, size_t _length, size_t _size)
{
	ENSURE(_length <= _size, InvalidArgument);

	if(!str || !_length){
		clear();
		return;
	}

	if(_size == _length){
		_size += 1;
		char * str_;
		if((str_ = (char *)realloc(str, _size)) == NULL)
			throw OutOfMemory("No enough memory for string allocation with size of "
					"% bytes.", _size);
		str = str_;
	}

	if(data)
		free(data);

	data = str;
	str = NULL;

	len = _length;
	size = _size;
	data[len] = 0;
}

void String::adopt(char *& str, size_t _size)
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	adopt(str, _length, _size);
}

void String::adopt(char *& str)
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	adopt(str, _length, _length + 1);
}

void String::lower()
{
	char diff = 'a' - 'A';
	char * start =  data;
	const char * end =  start + len;
	for(char * iter = start; iter < end; iter++)
		if('A' <= *iter && *iter <= 'Z')
			*iter += diff;
}

void String::upper()
{
	char diff = 'a' - 'A';
	char * start =  data;
	const char * end =  start + len;
	for(char * iter = start; iter < end; iter++)
		if('a' <= *iter && *iter <= 'z')
			*iter -= diff;
}

String String::toLower() const
{
	String result(*this);
	result.lower();
	return result;
}

String String::toUpper() const
{
	String result(*this);
	result.upper();
	return result;
}

bool String::isRegexpMatch(const char * regexp)
{
	regex_t regexpCompiled;
	unsigned matchesSize = 2;
	regmatch_t matches[matchesSize];

	memset(matches, 0, sizeof(matches));
	if(regcomp(&regexpCompiled, regexp, 0))
		throw ParseError(errno, "Failed to compile regular expression: '%'.", regexp);

	int res = regexec(&regexpCompiled, data, matchesSize, matches, 0);
	regfree(&regexpCompiled);
	if(res)
		return false;

        return matchesSize;
}

/** Tries to mine the matching groups and make
 * result to refer to it if matching was successfull.
 * FIXME : write standalone regexp class. */
Array<Str> String::regexpMatches(const char * regexp, unsigned numOfExpectedMatches)
{
	regex_t regexpCompiled;
	unsigned matchesSize = numOfExpectedMatches + 1;
	regmatch_t matches[matchesSize];

	memset(matches, 0, sizeof(matches));
	if(regcomp(&regexpCompiled, regexp, 0))
		throw ParseError(errno, "Failed to compile regular expression: '%'.", regexp);

	regexec(&regexpCompiled, data, matchesSize, matches, 0);
	regfree(&regexpCompiled);

	Array<Str> result;
        for(unsigned i = 1; i < matchesSize; i++){
                if(matches[i].rm_eo == -1 || matches[i].rm_so == -1 ||
                                // avoid posible regex bug :
                                matches[i].rm_eo < matches[i].rm_so)
			break;
		result.add(data + matches[i].rm_so, matches[i].rm_eo - matches[i].rm_so);
		//DBG("Match: '%' by using regexp: '%'.", result[i-1], regexp);
	}

	return result;
}

}
