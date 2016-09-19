/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2016 Csaszar, Peter
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <regex.h>
#include <string.h>
#include <errno.h>

#include "csjp_str.h"

namespace csjp {

Str::Str() :
	AStr(),
	length(len)
{
}

Str::Str(const Str & str, size_t from, size_t until) :
	AStr(),
	length(len)
{
	ENSURE(until <= str.len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);

	assign(str.data + from, until - from);
}

Str::Str(const Str & str, size_t from) :
	AStr(),
	length(len)
{
	ENSURE(from <= str.len,  InvalidArgument);

	assign(str.data + from, str.len - from);
}

Str::Str(const Str & str) :
	AStr(),
	length(len)
{
	assign(str.data, str.len);
}

Str::Str(Str && temp) :
	length(len)
{
	assign(temp.data, temp.len);
}

Str::~Str()
{
}

void Str::assign(const char * str, size_t length)
{
	data = (char*)str;
	len = length;
}

void Str::assign(const char * str)
{
	if(!str){
		clear();
		return;
	}

	assign(str, strlen(str));
}

void Str::assign(const Str & chunk)
{
	assign(chunk.c_str(), chunk.length);
}

Str Str::read(size_t from, size_t until) const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);

	Str str;

	if(from != until)
		str.assign(data + from, until - from);

	return str;
}

void Str::clear()
{
	len = 0;
	data = NULL;
}

bool Str::findFirst(size_t & pos, const char * str, size_t _length, size_t from, size_t until) const
{
	return AStr::findFirst(pos, str, _length, from, until);
}

bool Str::findFirst(size_t & pos, const char * str, size_t from, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findFirst(pos, str, _length, from, until);
}

bool Str::findFirst(size_t & pos, const char * str, size_t from) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findFirst(pos, str, _length, from, len);
}

bool Str::findFirst(size_t & pos, const Str & str, size_t from, size_t until) const
{
	return findFirst(pos, str.data, str.len, from, until);
}

bool Str::findFirst(size_t & pos, const Str & str, size_t from) const
{
	return findFirst(pos, str.data, str.len, from, len);
}

bool Str::findFirst(size_t & pos, char c, size_t from) const
{
	return findFirstOf(pos, &c, 1, from, len);
}

bool Str::findFirstOf(size_t & pos, const char * str, size_t _length, size_t from, size_t until)
	const
{
	return AStr::findFirstOf(pos, str, _length, from, until);
}

bool Str::findFirstOf(size_t & pos, const char * str, size_t from, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findFirstOf(pos, str, _length, from, until);
}

bool Str::findFirstOf(size_t & pos, const char * str, size_t from) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findFirstOf(pos, str, _length, from, len);
}

bool Str::findFirstOf(size_t & pos, const Str & str, size_t from, size_t until) const
{
	return findFirstOf(pos, str.data, str.len, from, until);
}

bool Str::findFirstOf(size_t & pos, const Str & str, size_t from) const
{
	return findFirstOf(pos, str.data, str.len, from, len);
}

bool Str::findFirstNotOf(size_t & pos, const char * str, size_t _length, size_t from, size_t until)
	const
{
	return AStr::findFirstNotOf(pos, str, _length, from, until);
}

bool Str::findFirstNotOf(size_t & pos, const char * str, size_t from, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findFirstNotOf(pos, str, _length, from, until);
}

bool Str::findFirstNotOf(size_t & pos, const char * str, size_t from) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findFirstNotOf(pos, str, _length, from, len);
}

bool Str::findFirstNotOf(size_t & pos, const Str & str, size_t from, size_t until) const
{
	return findFirstNotOf(pos, str.data, str.len, from, until);
}

bool Str::findFirstNotOf(size_t & pos, const Str & str, size_t from) const
{
	return findFirstNotOf(pos, str.data, str.len, from, len);
}

bool Str::findLast(size_t & pos, const char * str, size_t _length, size_t until) const
{
	return AStr::findLast(pos, str, _length, until);
}

bool Str::findLast(size_t & pos, const char * str, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findLast(pos, str, _length, until);
}

bool Str::findLast(size_t & pos, const char * str) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findLast(pos, str, _length, len);
}

bool Str::findLast(size_t & pos, const Str & str, size_t until) const
{
	return findLast(pos, str.data, str.len, until);
}

bool Str::findLast(size_t & pos, const Str & str) const
{
	return findLast(pos, str.data, str.len, len);
}

bool Str::findLast(size_t & pos, char c, size_t until) const
{
	return findLastOf(pos, &c, 1, until);
}

bool Str::findLast(size_t & pos, char c) const
{
	return findLastOf(pos, &c, 1, len);
}

bool Str::findLastOf(size_t & pos, const char * str, size_t _length, size_t until) const
{
	return AStr::findLastOf(pos, str, _length, until);
}

bool Str::findLastOf(size_t & pos, const char * str, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findLastOf(pos, str, _length, until);
}

bool Str::findLastOf(size_t & pos, const char * str) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findLastOf(pos, str, _length, len);
}

bool Str::findLastOf(size_t & pos, const Str & str, size_t until) const
{
	return findLastOf(pos, str.data, str.len, until);
}

bool Str::findLastOf(size_t & pos, const Str & str) const
{
	return findLastOf(pos, str.data, str.len, len);
}

bool Str::findLastNotOf(size_t & pos, const char * str, size_t _length, size_t until) const
{
	return AStr::findLastNotOf(pos, str, _length, until);
}

bool Str::findLastNotOf(size_t & pos, const char * str, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findLastNotOf(pos, str, _length, until);
}

bool Str::findLastNotOf(size_t & pos, const char * str) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return findLastNotOf(pos, str, _length, len);
}

bool Str::findLastNotOf(size_t & pos, const Str & str, size_t until) const
{
	return findLastNotOf(pos, str.data, str.len, until);
}

bool Str::findLastNotOf(size_t & pos, const Str & str) const
{
	return findLastNotOf(pos, str.data, str.len, len);
}

bool Str::startsWith(const char * str, size_t _length) const
{
	return AStr::startsWith(str, _length);
}

bool Str::startsWith(const char * str) const
{
	if(!str)
		return true;

	size_t _length = strlen(str);

	return startsWith(str, _length);
}

bool Str::endsWith(const char * str, size_t _length) const
{
	return AStr::endsWith(str, _length);
}

bool Str::endsWith(const char * str) const
{
	if(!str)
		return true;

	size_t _length = strlen(str);

	return endsWith(str, _length);
}

size_t Str::count(const char * str, size_t _length, size_t from, size_t until) const
{
	return AStr::count(str, _length, from, until);
}

size_t Str::count(const char * str, size_t from, size_t until) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return count(str, _length, from, until);
}

size_t Str::count(const char * str, size_t from) const
{
	size_t _length = 0;
	if(str)
		_length = strlen(str);

	return count(str, _length, from, len);
}

size_t Str::count(const Str & str, size_t from, size_t until) const
{
	return count(str.data, str.len, from, until);
}

size_t Str::count(const Str & str, size_t from) const
{
	return count(str.data, str.len, from, len);
}

bool Str::chopFront(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return true;
	if(!len)
		return false;
	if(len < _length)
		return false;

	if(startsWith(str, _length)){
		data += _length;
		len -= _length;
		return true;
	}

	return false;
}

bool Str::chopFront(const char * str)
{
	if(!str)
		return true;

	size_t _length = strlen(str);

	return chopFront(str, _length);
}

bool Str::chopFront(const Str & str)
{
	return chopFront(str.c_str(), str.length);
}

void Str::chopFront(size_t _length)
{
	ENSURE(_length <= len,  InvalidArgument);

	data += _length;
	len -= _length;
}

void Str::chopBack(size_t _length)
{
	ENSURE(_length <= len,  InvalidArgument);

	len -= _length;
}

void Str::cutAt(size_t _length)
{
	ENSURE(_length <= len,  InvalidArgument);

	len = _length;
}

void Str::trim(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return;

	size_t pos;

	if(findFirstNotOf(pos, str, _length, 0, len)){
		data += pos;
		len -= pos;
		if(findLastNotOf(pos, str, _length, len))
			cutAt(pos + 1);
	} else
		cutAt(0);
}

void Str::trim(const char * str)
{
	if(!str)
		return;

	size_t _length = strlen(str);

	trim(str, _length);
}

void Str::trim(const Str & str)
{
	trim(str.data, str.len);
}

void Str::trimFront(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return;

	size_t pos;

	if(findFirstNotOf(pos, str, _length, 0, len)){
		data += pos;
		len -= pos;
	} else
		cutAt(0);
}

void Str::trimFront(const char * str)
{
	if(!str)
		return;

	size_t _length = strlen(str);

	trimFront(str, _length);
}

void Str::trimFront(const Str & str)
{
	trimFront(str.data, str.len);
}

void Str::trimBack(const char * str, size_t _length)
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return;

	size_t pos;

	if(findLastNotOf(pos, str, _length, len))
		cutAt(pos + 1);
}

void Str::trimBack(const char * str)
{
	if(!str)
		return;

	size_t _length = strlen(str);

	trimBack(str, _length);
}

void Str::trimBack(const Str & str)
{
	trimBack(str.data, str.len);
}

Array<Str> Str::split(const char * delimiters, bool avoidEmptyResults) const
{
	return AStr::split(delimiters, avoidEmptyResults);
}

/** Tries to mine the matching groups and make
 * result to refer to it if matching was successfull.
 * FIXME : write standalone regexp class. */
bool subStringByRegexp(
		const String & str,
		Array<Str> & result,
		const char * regexp)
{
	regex_t regexpCompiled;
	unsigned matchesSize = result.capacity + 1;
	regmatch_t matches[matchesSize];

	memset(matches, 0, sizeof(matches));
	if(regcomp(&regexpCompiled, regexp, 0))
		throw ParseError(errno,
				"Failed to compile regular expression: '%'.",
				regexp);
	int res = regexec(&regexpCompiled, str.c_str(),
			matchesSize, matches, 0);
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
			throw InvalidArgument(
			"Regular expression contains more groups than "
			"the capacity of the result array given as parameter.");
		result.add(str.c_str() + matches[i].rm_so, matches[i].rm_eo -
				matches[i].rm_so);
		//DBG("Match: '%' by using regexp: '%'.", result[i-1], regexp);
	}

	return true;
}

Array<Str> subStringByRegexp(
		const String & str, const char * regexp,
		unsigned numOfExpectedMatches)
{
	Array<Str> result;

	regex_t regexpCompiled;
	unsigned matchesSize = numOfExpectedMatches + 1;
	regmatch_t matches[matchesSize];

	memset(matches, 0, sizeof(matches));
	if(regcomp(&regexpCompiled, regexp, 0))
		throw ParseError(errno,
				"Failed to compile regular expression: '%'.",
				regexp);
	regexec(&regexpCompiled, str.c_str(), matchesSize, matches, 0);
	regfree(&regexpCompiled);

	result.clear();
        for(unsigned i = 1; i < matchesSize; i++){
                if(matches[i].rm_eo == -1 || matches[i].rm_so == -1 ||
                                // avoid posible regex bug :
                                matches[i].rm_eo < matches[i].rm_so)
			break;
		result.add(str.c_str() + matches[i].rm_so,
				matches[i].rm_eo - matches[i].rm_so);
		//DBG("Match: '%' by using regexp: '%'.", result[i-1], regexp);
	}

	return result;
}

}
