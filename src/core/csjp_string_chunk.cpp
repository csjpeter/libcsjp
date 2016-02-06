/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012 Csaszar, Peter
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <regex.h>
#include <string.h>
#include <errno.h>

#include "csjp_string_chunk.h"

namespace csjp {

StringChunk::StringChunk() :
	ref(0),
	len(0),
	length(len),
	str(ref)
{
}

StringChunk::StringChunk(const char * str, size_t _length) :
	ref(str),
	len(_length),
	length(len),
	str(ref)
{
	ENSURE(str || !_length,  InvalidArgument);
}

StringChunk::StringChunk(const StringChunk & str, size_t from, size_t until) :
	ref(str.ref),
	len(str.len),
	length(len),
	str(ref)
{
	ENSURE(until <= str.len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);

	ref += from;
	len = until - from;
}

StringChunk::StringChunk(const StringChunk & str, size_t from) :
	ref(str.ref),
	len(str.len),
	length(len),
	str(ref)
{
	ENSURE(from <= str.len,  InvalidArgument);

	ref += from;
	len -= from;
}

StringChunk::StringChunk(const StringChunk & str) :
	ref(str.ref),
	len(str.len),
	length(len),
	str(ref)
{
}

StringChunk::StringChunk(StringChunk && temp) :
	ref(temp.ref),
	len(temp.len),
	length(len),
	str(ref)
{
}

StringChunk::~StringChunk()
{
}

const char& StringChunk::operator[](size_t i) const
{
	ENSURE(i < len, IndexOutOfRange);

	return ref[i];
}

void StringChunk::assign(const char * str, size_t length)
{
	ref = str;
	len = length;
}

void StringChunk::assign(const char * str)
{
	if(!str){
		clear();
		return;
	}

	assign(str, strlen(str));
}

void StringChunk::assign(const StringChunk & chunk)
{
	assign(chunk.str, chunk.length);
}

int StringChunk::compare(const char * str, size_t _length) const
{
	ENSURE(str || !_length,  InvalidArgument);

	DBG("'%' (%) == '%' (%)",  ref, len, str, _length);

	if(!_length)
		return (len == 0) ? 0 : 1;
	if(!len)
		return -1;

	const char * ptr = ref;
	const char * const endPtr = ref + len;
	const char * const strEndPtr = str + _length;

	for(; ptr < endPtr && str < strEndPtr; str++, ptr++){
		if(*ptr == *str)
			continue;
		if(*ptr < *str)
			return -1;
		else
			return 1;
	}

	if(len < _length)
		return -1;
	else if(_length < len)
		return 1;

	return 0;
}

int StringChunk::compare(const char * str) const
{
	if(!str)
		return (len == 0) ? 0 : 1;

	size_t _length = strlen(str);

	return compare(str, _length);
}

int StringChunk::compare(const StringChunk & str) const
{
	return compare(str.ref, str.len);
}

bool StringChunk::isEqual(const char * str, size_t _length) const
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return len == 0;
	if(len != _length)
		return false;

	return !compare(str, _length);
}

bool StringChunk::isEqual(const char * str) const
{
	if(!str)
		return len == 0;

	size_t _length = strlen(str);

	return !compare(str, _length);
}

bool StringChunk::isEqual(const StringChunk & str) const
{
	return isEqual(str.ref, str.len);
}

void StringChunk::clear()
{
	len = 0;
	ref = NULL;
}

bool StringChunk::findFirst(size_t & pos, const char * str, size_t _length, size_t from, size_t until) const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);
	ENSURE(str,  InvalidArgument);
	ENSURE(_length,  InvalidArgument);

	if(until - from < _length)
		return false;
	if(!ref)
		return false;

	size_t p = 0;

	const char * refPtr = ref + from;
	const char * const refEndPtr = ref + until;
	const char * strPtr = str;
	const char * const strEndPtr = str + _length;

	for(; refPtr < refEndPtr; refPtr++){
		if(*refPtr != *strPtr){
			if(strPtr == str)
				continue;
			strPtr = str;
		}
		if(*refPtr == *strPtr){
			if(strPtr == str)
				p = refPtr - ref;
			strPtr++;
			if(strPtr == strEndPtr){
				pos = p;
				return true;
			}
		}
	}

	return false;
}

bool StringChunk::findFirst(size_t & pos, const char * str, size_t from, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findFirst(pos, str, _length, from, until);
}

bool StringChunk::findFirst(size_t & pos, const char * str, size_t from) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findFirst(pos, str, _length, from, len);
}

bool StringChunk::findFirst(size_t & pos, const StringChunk & str, size_t from, size_t until) const
{
	return findFirst(pos, str.ref, str.len, from, until);
}

bool StringChunk::findFirst(size_t & pos, const StringChunk & str, size_t from) const
{
	return findFirst(pos, str.ref, str.len, from, len);
}

bool StringChunk::findFirst(size_t & pos, char c, size_t from) const
{
	return findFirstOf(pos, &c, 1, from, len);
}

bool StringChunk::findFirstOf(size_t & pos, const char * str, size_t _length, size_t from, size_t until) const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);
	ENSURE(str,  InvalidArgument);
	ENSURE(_length,  InvalidArgument);

	if(until - from < _length)
		return false;
	if(!ref)
		return false;

	const char * refPtr = ref + from;
	const char * const refEndPtr = ref + until;
	const char * strPtr = str;
	const char * const strEndPtr = str + _length;

	for(; refPtr < refEndPtr; refPtr++){
		for(strPtr = str; strPtr < strEndPtr; strPtr++){
			if(*refPtr == *strPtr){
				pos = refPtr - ref;
				return true;
			}
		}
	}

	return false;
}

bool StringChunk::findFirstOf(size_t & pos, const char * str, size_t from, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findFirstOf(pos, str, _length, from, until);
}

bool StringChunk::findFirstOf(size_t & pos, const char * str, size_t from) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findFirstOf(pos, str, _length, from, len);
}

bool StringChunk::findFirstOf(size_t & pos, const StringChunk & str, size_t from, size_t until) const
{
	return findFirstOf(pos, str.ref, str.len, from, until);
}

bool StringChunk::findFirstOf(size_t & pos, const StringChunk & str, size_t from) const
{
	return findFirstOf(pos, str.ref, str.len, from, len);
}

bool StringChunk::findFirstNotOf(size_t & pos, const char * str, size_t _length, size_t from, size_t until) const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);
	ENSURE(str,  InvalidArgument);
	ENSURE(_length,  InvalidArgument);

	if(!ref)
		return false;

	const char * refPtr = ref + from;
	const char * const refEndPtr = ref + until;
	const char * strPtr = str;
	const char * const strEndPtr = str + _length;

	for(; refPtr < refEndPtr; refPtr++){
		bool stranger = true;
		for(strPtr = str; strPtr < strEndPtr; strPtr++){
			if(*refPtr == *strPtr){
				stranger = false;
				break;
			}
		}
		if(stranger){
			pos = refPtr - ref;
			return true;
		}
	}

	return false;
}

bool StringChunk::findFirstNotOf(size_t & pos, const char * str, size_t from, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findFirstNotOf(pos, str, _length, from, until);
}

bool StringChunk::findFirstNotOf(size_t & pos, const char * str, size_t from) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findFirstNotOf(pos, str, _length, from, len);
}

bool StringChunk::findFirstNotOf(size_t & pos, const StringChunk & str, size_t from, size_t until) const
{
	return findFirstNotOf(pos, str.ref, str.len, from, until);
}

bool StringChunk::findFirstNotOf(size_t & pos, const StringChunk & str, size_t from) const
{
	return findFirstNotOf(pos, str.ref, str.len, from, len);
}

bool StringChunk::findLast(size_t & pos, const char * str, size_t _length, size_t until) const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(str,  InvalidArgument);
	ENSURE(_length,  InvalidArgument);

	if(!ref)
		return false;

	const char * refPtr = ref + until - 1;
	const char * strLastPtr = str + _length - 1;
	const char * strPtr = strLastPtr;

	for(; ref <= refPtr; refPtr--){
		if(*refPtr != *strPtr){
			if(strPtr == strLastPtr)
				continue;
			strPtr = strLastPtr;
		}
		if(*refPtr == *strPtr){
			strPtr--;
			if(strPtr < str){
				pos = refPtr - ref;
				return true;
			}
		}
	}

	return false;
}

bool StringChunk::findLast(size_t & pos, const char * str, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findLast(pos, str, _length, until);
}

bool StringChunk::findLast(size_t & pos, const char * str) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findLast(pos, str, _length, len);
}

bool StringChunk::findLast(size_t & pos, const StringChunk & str, size_t until) const
{
	return findLast(pos, str.ref, str.len, until);
}

bool StringChunk::findLast(size_t & pos, const StringChunk & str) const
{
	return findLast(pos, str.ref, str.len, len);
}

bool StringChunk::findLast(size_t & pos, char c, size_t until) const
{
	return findLastOf(pos, &c, 1, until);
}

bool StringChunk::findLast(size_t & pos, char c) const
{
	return findLastOf(pos, &c, 1, len);
}

bool StringChunk::findLastOf(size_t & pos, const char * str, size_t _length, size_t until) const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(str,  InvalidArgument);
	ENSURE(_length,  InvalidArgument);

	if(!ref)
		return false;

	const char * refPtr = ref + until - 1;
	const char * strPtr = str;
	const char * const strEndPtr = str + _length;

	for(; ref <= refPtr; refPtr--){
		for(strPtr = str; strPtr < strEndPtr; strPtr++){
			if(*refPtr == *strPtr){
				pos = refPtr - ref;
				return true;
			}
		}
	}

	return false;
}

bool StringChunk::findLastOf(size_t & pos, const char * str, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findLastOf(pos, str, _length, until);
}

bool StringChunk::findLastOf(size_t & pos, const char * str) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findLastOf(pos, str, _length, len);
}

bool StringChunk::findLastOf(size_t & pos, const StringChunk & str, size_t until) const
{
	return findLastOf(pos, str.ref, str.len, until);
}

bool StringChunk::findLastOf(size_t & pos, const StringChunk & str) const
{
	return findLastOf(pos, str.ref, str.len, len);
}

bool StringChunk::findLastNotOf(size_t & pos, const char * str, size_t _length, size_t until) const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(str || !_length,  InvalidArgument);

	if(!until)
		return false;
	if(!ref)
		return false;
	if(!str){
		/* len != 0 ; if len was 0, until is either greater (invalid)
		 * or 0 what is already handled by returning false. */
		pos = len - 1;
		return true;
	}

	const char * refPtr = ref + until - 1;
	const char * strPtr = str;
	const char * const strEndPtr = str + _length;

	for(; ref <= refPtr; refPtr--){
		bool stranger = true;
		for(strPtr = str; strPtr < strEndPtr; strPtr++){
			if(*refPtr == *strPtr){
				stranger = false;
				break;
			}
		}
		if(stranger){
			pos = refPtr - ref;
			return true;
		}
	}

	return false;
}

bool StringChunk::findLastNotOf(size_t & pos, const char * str, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findLastNotOf(pos, str, _length, until);
}

bool StringChunk::findLastNotOf(size_t & pos, const char * str) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findLastNotOf(pos, str, _length, len);
}

bool StringChunk::findLastNotOf(size_t & pos, const StringChunk & str, size_t until) const
{
	return findLastNotOf(pos, str.ref, str.len, until);
}

bool StringChunk::findLastNotOf(size_t & pos, const StringChunk & str) const
{
	return findLastNotOf(pos, str.ref, str.len, len);
}

bool StringChunk::startsWith(const char * str, size_t _length) const
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return true;
	if(!len)
		return false;
	if(len < _length)
		return false;

	const char * ptr = ref;
	const char * const endPtr = ref + len;
	const char * const strEndPtr = str + _length;

	for(; ptr < endPtr && str < strEndPtr; str++, ptr++){
		if(*ptr == *str)
			continue;
		return false;
	}

	return true;
}

bool StringChunk::startsWith(const char * str) const
{
	if(!str)
		return true;

	size_t _length = strlen(str);

	return startsWith(str, _length);
}

bool StringChunk::endsWith(const char * str, size_t _length) const
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return true;
	if(!len)
		return false;
	if(len < _length)
		return false;

	const char * refPtr = ref + len - 1;
	const char * strPtr = str + _length - 1;

	for(; ref <= refPtr && str <= strPtr; refPtr--, strPtr--){
		if(*refPtr == *strPtr)
			continue;
		return false;
	}

	return true;
}

bool StringChunk::endsWith(const char * str) const
{
	if(!str)
		return true;

	size_t _length = strlen(str);

	return endsWith(str, _length);
}

size_t StringChunk::count(const char * str, size_t _length, size_t from, size_t until) const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);
	ENSURE(str,  InvalidArgument);

	if(until - from < _length)
		return 0;

	size_t pos = from;
	size_t matches = 0;

	while(findFirst(pos, str, _length, pos, until)){
		pos += _length;
		matches++;
	}

	return matches;
}

size_t StringChunk::count(const char * str, size_t from, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return count(str, _length, from, until);
}

size_t StringChunk::count(const char * str, size_t from) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return count(str, _length, from, len);
}

size_t StringChunk::count(const StringChunk & str, size_t from, size_t until) const
{
	return count(str.ref, str.len, from, until);
}

size_t StringChunk::count(const StringChunk & str, size_t from) const
{
	return count(str.ref, str.len, from, len);
}

bool StringChunk::chopFront(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return true;
	if(!len)
		return false;
	if(len < _length)
		return false;

	if(startsWith(str, _length)){
		ref += _length;
		len -= _length;
		return true;
	}

	return false;
}

bool StringChunk::chopFront(const char * str)
{
	if(!str)
		return true;

	size_t _length = strlen(str);

	return chopFront(str, _length);
}

bool StringChunk::chopFront(const StringChunk & str)
{
	return chopFront(str.str, str.length);
}

void StringChunk::chopFront(size_t _length)
{
	ENSURE(_length <= len,  InvalidArgument);

	ref += _length;
	len -= _length;
}

void StringChunk::chopBack(size_t _length)
{
	ENSURE(_length <= len,  InvalidArgument);

	len -= _length;
}

void StringChunk::cutAt(size_t _length)
{
	ENSURE(_length <= len,  InvalidArgument);

	len = _length;
}

void StringChunk::trim(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return;

	size_t pos;

	if(findFirstNotOf(pos, str, _length, 0, len)){
		ref += pos;
		len -= pos;
		if(findLastNotOf(pos, str, _length, len))
			cutAt(pos + 1);
	} else
		cutAt(0);
}

void StringChunk::trim(const char * str)
{
	if(!str)
		return;

	size_t _length = strlen(str);

	trim(str, _length);
}

void StringChunk::trim(const StringChunk & str)
{
	trim(str.ref, str.len);
}

void StringChunk::trimFront(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return;

	size_t pos;

	if(findFirstNotOf(pos, str, _length, 0, len)){
		ref += pos;
		len -= pos;
	} else
		cutAt(0);
}

void StringChunk::trimFront(const char * str)
{
	if(!str)
		return;

	size_t _length = strlen(str);

	trimFront(str, _length);
}

void StringChunk::trimFront(const StringChunk & str)
{
	trimFront(str.ref, str.len);
}

void StringChunk::trimBack(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return;

	size_t pos;

	if(findLastNotOf(pos, str, _length, len))
		cutAt(pos + 1);
}

void StringChunk::trimBack(const char * str)
{
	if(!str)
		return;

	size_t _length = strlen(str);

	trimBack(str, _length);
}

void StringChunk::trimBack(const StringChunk & str)
{
	trimBack(str.ref, str.len);
}

Array<StringChunk> StringChunk::split(const char * delimiters, bool avoidEmptyResults) const
{
	Array<StringChunk> result;

	if(!delimiters || delimiters[0] == 0){
		if(avoidEmptyResults && len == 0)
			return result;
		result.setCapacity(1);
		result.add(ref, len);
		return result;
	}

	const char * until = ref + len;
	size_t d_len = strlen(delimiters);
	const char * d_until = delimiters + d_len;

	const char * res_start = ref;
	size_t res_len = 0;
	const char * iter = ref;

	while(iter < until){
		const char * d_iter = delimiters;
		for(; d_iter < d_until; d_iter++)
			if(*iter == *d_iter)
				break;

		if(d_iter == d_until){
			iter++;
			res_len++;
			continue;
		}

		if(!avoidEmptyResults || res_len)
			result.add(res_start, res_len);
		iter++;
		res_start = iter;
		res_len = 0;
	}
	if(!avoidEmptyResults || res_len)
		result.add(res_start, res_len);

	return result;
}

Array<StringChunk> split(const String & str,
		const char * delimiters, bool avoidEmptyResults)
{
	StringChunk chunk(str.str, str.length);
	return chunk.split(delimiters, avoidEmptyResults);
}

/** Tries to mine the first matching group and make
 * result to refer to it if matching was successfull.
 * FIXME : write standalone regexp class. */
bool subStringByRegexp(const String & str, Array<StringChunk> & result, const char * regexp)
{
	regex_t regexpCompiled;
	unsigned matchesSize = result.capacity + 1;
	regmatch_t matches[matchesSize];

	memset(matches, 0, sizeof(matches));
	if(regcomp(&regexpCompiled, regexp, 0))
		throw ParseError(errno, "Failed to compile regular expression: '%'.", regexp);
	int res = regexec(&regexpCompiled, str.str, matchesSize, matches, 0);
	regfree(&regexpCompiled);
	if(res)
		return false;

	result.clear();
        for(unsigned i = 1; i < matchesSize; i++){
                if(matches[i].rm_eo == -1 || matches[i].rm_so == -1 ||
                                // avoid posible regex bug :
                                matches[i].rm_eo < matches[i].rm_so)
			break;
		if(result.capacity < i)
			throw new InvalidArgument("Regular expression contains more groups than "
					"the capacity of the result array given as parameter.");
		result.add(str.str + matches[i].rm_so, matches[i].rm_eo - matches[i].rm_so);
		DBG("Got substring: '%' by using regexp: '%'.", result[i-1], regexp);
	}

	return true;
}

}
