/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2016 Csaszar, Peter
 */

#include <stdio.h>

#include "csjp_string.h"

namespace csjp {

Str::Str() :
	AStr()
{
}

Str::Str(const Str & str, size_t from, size_t until) :
	AStr()
{
	ENSURE(until <= str.len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);

	assign(str.data + from, until - from);
}

Str::Str(const Str & str, size_t from) :
	AStr()
{
	ENSURE(from <= str.len,  InvalidArgument);

	assign(str.data + from, str.len - from);
}

Str::Str(const Str & str) :
	AStr()
{
	assign(str.data, str.len);
}

Str::Str(Str && temp) :
	AStr()
{
	assign(temp.data, temp.len);
}

Str::~Str()
{
}

const Str & Str::operator=(Str && temp)
{
	data = temp.data;
	temp.data = 0;
	len = temp.len;
	temp.len = 0;
	return *this;
}

const Str & Str::operator=(const char * str)
	{ assign(str); return *this; }

const Str & Str::operator=(const Str & chunk)
	{ assign(chunk); return *this;}

Str::Str(const unsigned char * str, size_t length)
{
	ENSURE(str || !length,  InvalidArgument);
	assign((const char *)str, length);
}

Str::Str(const char * str, size_t length) : AStr()
{
	ENSURE(str || !length,  InvalidArgument);
	assign(str, length);
}

Str::Str(const char * str) : AStr()
{
	if(str){
		const char * c = str;
		while(*c != 0)
			c++;
		assign(str, c - str);
	}
}

Str::Str(const String & str) : AStr(str)
	{ assign(str.c_str(), str.length); }


int Str::compare(const char * str, size_t _length) const
	{ return AStr::compare(str, _length); }

int Str::compare(const char * str) const
	{ return AStr::compare(str); }

int Str::compare(const Str & str) const
	{ return AStr::compare(str.data, str.len); }

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

bool operator<(const Str & a, const String & b)
	{ Str chunk(b); return a < chunk; }
bool operator<(const String & a, const Str & b)
	{ Str chunk(a); return chunk < b; }

}

