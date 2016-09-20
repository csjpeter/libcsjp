/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#ifndef CSJP_ASTR_H
#define CSJP_ASTR_H

#include <csjp_logger.h>

namespace csjp {

class Str;
class String;
template <typename DataType> class Array;

class AStr
{
	friend class String;
	friend class Str;
protected:
	char * data;
	size_t len;
protected:
	AStr() : data(0), len(0), length(len) {}
	virtual ~AStr() {}

public:
	const size_t & length;
	/** Terminating zero is not garanteed. */
	const char * c_str() const { return data; }

public:
	int compare(const char * str, size_t _length) const;
	int compare(const char * str) const;
	bool isEqual(const char * str, size_t _length) const;
	bool isEqual(const char * str) const;
	bool findFirst(size_t & pos, const char * str, size_t _length, size_t from, size_t until)
		const;
	bool findFirstOf(size_t & pos, const char * str, size_t _length, size_t from, size_t until)
		const;
	bool findFirstNotOf(size_t & pos, const char * str, size_t _length,
			size_t from, size_t until) const;
	bool findLast(size_t & pos, const char * str, size_t _length, size_t until) const;
	bool findLastOf(size_t & pos, const char * str, size_t _length, size_t until) const;
	bool findLastNotOf(size_t & pos, const char * str, size_t _length, size_t until) const;
	bool startsWith(const char * str, size_t _length) const;
	bool endsWith(const char * str, size_t _length) const;
	size_t count(const char * str, size_t _length, size_t from, size_t until) const;
	Array<Str> split(const char * delimiters, bool avoidEmptyResults) const;
	String encodeBase64() const;
	String decodeBase64() const;
};

}

#endif
