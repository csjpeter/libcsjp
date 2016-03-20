/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012 Csaszar, Peter
 */

#ifndef CSJP_STRING_CHUNK_H
#define CSJP_STRING_CHUNK_H

#include <csjp_array.h>

namespace csjp {

/** This string class is to process already existing or static string datas as constant.
 * It is the developer's responsibility, not to change the string given for such an
 * object.
 */
/** The inner data might contain 0 byte. Methods are using length property instead of
 * the 0 byte as sentinel.
 */
class StringChunk
{
protected:
	const char * ref; /** As reference to existing char * pointed memory area. */
	size_t len;
public:
	const size_t & length;
	/** Terminating zero is not garanteed. */
	const char * const & str;

public:
	const StringChunk & operator=(StringChunk && temp)
	{
		ref = temp.ref;
		temp.ref = 0;
		len = temp.len;
		temp.len = 0;
		return *this;
	}

	explicit StringChunk();
	explicit StringChunk(const char * str, size_t length);
	inline StringChunk(const char * str) : ref(str), len(0),
			length(len), str(ref)
		{ if(str){ const char *c = str;
				 while(*c != 0) c++; len = c - str; } }
	explicit StringChunk(const StringChunk & str, size_t from, size_t until);
	explicit StringChunk(const StringChunk & str, size_t from);
	explicit StringChunk(const StringChunk & str);
	StringChunk(StringChunk && temp);
	virtual ~StringChunk();

	void assign(const char * str, size_t length);
	void assign(const char * str);
	void assign(const StringChunk & chunk);

	const char& operator[](size_t i) const;
	const StringChunk & operator=(const char * str) { assign(str); return *this; }
	const StringChunk & operator=(const StringChunk & chunk) { assign(chunk); return *this;}

	operator const char * () const { return ref; }

	int compare(const char * str, size_t _length) const;
	int compare(const char * str) const;
	int compare(const StringChunk & str) const;

	bool isEqual(const char * str, size_t _length) const;
	bool isEqual(const char * str) const;
	bool isEqual(const StringChunk & str) const;

	StringChunk read(size_t from, size_t until) const;

	void clear();

	bool findFirst(size_t &, const char *, size_t _length, size_t from, size_t until) const;
	bool findFirst(size_t &, const char *, size_t from, size_t until) const;
	bool findFirst(size_t &, const char *, size_t from = 0) const;
	bool findFirst(size_t &, const StringChunk &, size_t from, size_t until) const;
	bool findFirst(size_t &, const StringChunk &, size_t from = 0) const;
	bool findFirst(size_t &, char, size_t from = 0) const;

	bool findFirstOf(size_t &, const char *, size_t _length, size_t from, size_t until) const;
	bool findFirstOf(size_t &, const char *, size_t from, size_t until) const;
	bool findFirstOf(size_t &, const char *, size_t from = 0) const;
	bool findFirstOf(size_t &, const StringChunk &, size_t from, size_t until) const;
	bool findFirstOf(size_t &, const StringChunk &, size_t from = 0) const;

	bool findFirstNotOf(size_t &, const char *, size_t _length, size_t from, size_t until) const;
	bool findFirstNotOf(size_t &, const char *, size_t from, size_t until) const;
	bool findFirstNotOf(size_t &, const char *, size_t from = 0) const;
	bool findFirstNotOf(size_t &, const StringChunk &, size_t from, size_t until) const;
	bool findFirstNotOf(size_t &, const StringChunk &, size_t from = 0) const;

	bool findLast(size_t &, const char *, size_t _length, size_t until) const;
	bool findLast(size_t &, const char *, size_t until) const;
	bool findLast(size_t &, const char *) const;
	bool findLast(size_t &, const StringChunk &, size_t until) const;
	bool findLast(size_t &, const StringChunk &) const;
	bool findLast(size_t &, char, size_t until) const;
	bool findLast(size_t &, char) const;

	bool findLastOf(size_t &, const char *, size_t _length, size_t until) const;
	bool findLastOf(size_t &, const char *, size_t until) const;
	bool findLastOf(size_t &, const char *) const;
	bool findLastOf(size_t &, const StringChunk &, size_t until) const;
	bool findLastOf(size_t &, const StringChunk &) const;

	bool findLastNotOf(size_t &, const char *, size_t _length, size_t until) const;
	bool findLastNotOf(size_t &, const char *, size_t until) const;
	bool findLastNotOf(size_t &, const char *) const;
	bool findLastNotOf(size_t &, const StringChunk &, size_t until) const;
	bool findLastNotOf(size_t &, const StringChunk &) const;

	bool startsWith(const char *, size_t length) const;
	bool startsWith(const char *) const;
	bool endsWith(const char *, size_t length) const;
	bool endsWith(const char *) const;

	size_t count(const char *, size_t _length, size_t from, size_t until) const;
	size_t count(const char *, size_t from, size_t until) const;
	size_t count(const char *, size_t from = 0) const;
	size_t count(const StringChunk & str, size_t from, size_t until) const;
	size_t count(const StringChunk & str, size_t from = 0) const;

	bool chopFront(const char * str, size_t _length);
	bool chopFront(const char * str);
	bool chopFront(const StringChunk & str);
	void chopFront(size_t _length);
	void chopBack(size_t _length);
	void cutAt(size_t);

	void trim(const char * toTrim, size_t _length);
	void trim(const char * toTrim);
	void trim(const StringChunk &);
	void trimFront(const char * toTrim, size_t _length);
	void trimFront(const char * toTrim);
	void trimFront(const StringChunk &);
	void trimBack(const char * toTrim, size_t _length);
	void trimBack(const char * toTrim);
	void trimBack(const StringChunk &);

	Array<StringChunk> split(const char * delimiters, bool avoidEmptyResults = true) const;
};

inline bool operator==(const StringChunk & a, const char * b) { return a.isEqual(b); }
inline bool operator!=(const StringChunk & a, const char * b) { return !a.isEqual(b); }
inline bool operator==(const StringChunk & a, const StringChunk & b) { return a.isEqual(b); }
inline bool operator!=(const StringChunk & a, const StringChunk & b) { return !a.isEqual(b); }
inline bool operator<(const StringChunk & a, const StringChunk & b) { return a.compare(b) == -1; }
inline bool operator<(const StringChunk & a, const char * b) { return a.compare(b) == -1; }
inline bool operator<(const char * a, const StringChunk & b) { return b.compare(a) == 1; }

inline bool operator<(const StringChunk & a, const String & b)
	{ StringChunk chunk(b, b.length); return a < chunk; }
inline bool operator<(const String & a, const StringChunk & b)
	{ StringChunk chunk(a, a.length); return chunk < b; }

inline String & operator<<=(String & lhs, const StringChunk & rhs)
	{ lhs.assign(rhs, rhs.length); return lhs; }
inline String & operator<<(String & lhs, const StringChunk & rhs)
	{ lhs.append(rhs, rhs.length); return lhs; }

Array<StringChunk> split(const String & str,
		const char * delimiters, bool avoidEmptyResults = true);

bool subStringByRegexp(
		const String & str,
		Array<StringChunk> & result,
		const char * regexp);
Array<StringChunk> subStringByRegexp(
		const String & str,
		const char * regexp,
		unsigned numOfExpectedMatches = 100);


}

#endif
