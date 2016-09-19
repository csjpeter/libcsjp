/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2016 Csaszar, Peter
 */

#ifndef CSJP_STR_H
#define CSJP_STR_H

#include <csjp_array.h>

namespace csjp {

/** This string class is to process already existing or static string datas as constant.
 * It is the developer's responsibility, not to change the string given for such an
 * object.
 */
/** The inner data might contain 0 byte. Methods are using length property instead of
 * the 0 byte as sentinel.
 */
class Str : public AStr
{
public:
	const size_t & length;
	/** Terminating zero is not garanteed. */
	const char * c_str() const { return data; }

public:
	const Str & operator=(Str && temp);

	explicit Str();
	explicit Str(const char * str, size_t length);
	explicit Str(const char * str);
	explicit Str(const String & str);
	explicit Str(const Str & str, size_t from, size_t until);
	explicit Str(const Str & str, size_t from);
	explicit Str(const Str & str);
	Str(Str && temp);
	virtual ~Str();

	void assign(const char * str, size_t length);
	void assign(const char * str);
	void assign(const Str & chunk);

	const char& operator[](size_t i) const { return data[i];}
	const Str & operator=(const char * str);
	const Str & operator=(const Str & chunk);

	int compare(const char * str, size_t _length) const;
	int compare(const char * str) const;
	int compare(const Str & str) const;

	bool isEqual(const char * str, size_t _length) const { return AStr::isEqual(str, _length); }
	bool isEqual(const char * str) const { return AStr::isEqual(str); }
	bool isEqual(const Str & str) const { return isEqual(str.data, str.len); }

	Str read(size_t from, size_t until) const;

	void clear();

	bool findFirst(size_t &, const char *, size_t _length,
					size_t from, size_t until) const;
	bool findFirst(size_t &, const char *, size_t from, size_t until) const;
	bool findFirst(size_t &, const char *, size_t from = 0) const;
	bool findFirst(size_t &, const Str &, size_t from, size_t until) const;
	bool findFirst(size_t &, const Str &, size_t from = 0) const;
	bool findFirst(size_t &, char, size_t from = 0) const;

	bool findFirstOf(size_t &, const char *, size_t _length, size_t from, size_t until) const;
	bool findFirstOf(size_t &, const char *, size_t from, size_t until) const;
	bool findFirstOf(size_t &, const char *, size_t from = 0) const;
	bool findFirstOf(size_t &, const Str &, size_t from, size_t until) const;
	bool findFirstOf(size_t &, const Str &, size_t from = 0) const;

	bool findFirstNotOf(size_t &, const char *, size_t _length, size_t from, size_t until)const;
	bool findFirstNotOf(size_t &, const char *, size_t from, size_t until) const;
	bool findFirstNotOf(size_t &, const char *, size_t from = 0) const;
	bool findFirstNotOf(size_t &, const Str &, size_t from, size_t until) const;
	bool findFirstNotOf(size_t &, const Str &, size_t from = 0) const;

	bool findLast(size_t &, const char *, size_t _length, size_t until) const;
	bool findLast(size_t &, const char *, size_t until) const;
	bool findLast(size_t &, const char *) const;
	bool findLast(size_t &, const Str &, size_t until) const;
	bool findLast(size_t &, const Str &) const;
	bool findLast(size_t &, char, size_t until) const;
	bool findLast(size_t &, char) const;

	bool findLastOf(size_t &, const char *, size_t _length, size_t until) const;
	bool findLastOf(size_t &, const char *, size_t until) const;
	bool findLastOf(size_t &, const char *) const;
	bool findLastOf(size_t &, const Str &, size_t until) const;
	bool findLastOf(size_t &, const Str &) const;

	bool findLastNotOf(size_t &, const char *, size_t _length, size_t until) const;
	bool findLastNotOf(size_t &, const char *, size_t until) const;
	bool findLastNotOf(size_t &, const char *) const;
	bool findLastNotOf(size_t &, const Str &, size_t until) const;
	bool findLastNotOf(size_t &, const Str &) const;

	bool startsWith(const char *, size_t length) const;
	bool startsWith(const char *) const;
	bool endsWith(const char *, size_t length) const;
	bool endsWith(const char *) const;

	size_t count(const char *, size_t _length, size_t from, size_t until) const;
	size_t count(const char *, size_t from, size_t until) const;
	size_t count(const char *, size_t from = 0) const;
	size_t count(const Str & str, size_t from, size_t until) const;
	size_t count(const Str & str, size_t from = 0) const;

	bool chopFront(const char * str, size_t _length);
	bool chopFront(const char * str);
	bool chopFront(const Str & str);
	void chopFront(size_t _length);
	void chopBack(size_t _length);
	void cutAt(size_t);

	void trim(const char * toTrim, size_t _length);
	void trim(const char * toTrim);
	void trim(const Str &);
	void trimFront(const char * toTrim, size_t _length);
	void trimFront(const char * toTrim);
	void trimFront(const Str &);
	void trimBack(const char * toTrim, size_t _length);
	void trimBack(const char * toTrim);
	void trimBack(const Str &);

	Array<Str> split(const char * delimiters, bool avoidEmptyResults = true) const;

	String encodeBase64() const { return AStr::encodeBase64(); }
	String decodeBase64() const { return AStr::decodeBase64(); }
};

inline bool operator==(const Str & a, const char * b) { return a.isEqual(b); }
inline bool operator!=(const Str & a, const char * b) { return !a.isEqual(b); }
inline bool operator==(const Str & a, const Str & b) { return a.isEqual(b); }
inline bool operator!=(const Str & a, const Str & b) { return !a.isEqual(b); }
inline bool operator<(const Str & a, const Str & b) { return a.compare(b) == -1; }
inline bool operator<(const Str & a, const char * b) { return a.compare(b) == -1; }
inline bool operator<(const char * a, const Str & b) { return b.compare(a) == 1; }

bool operator<(const Str & a, const String & b);
bool operator<(const String & a, const Str & b);

String & operator<<=(String & lhs, const Str & rhs);
String & operator<<(String & lhs, const Str & rhs);

bool subStringByRegexp(const String & str, Array<Str> & result, const char * regexp);
Array<Str> subStringByRegexp(const String & str, const char * regexp,
		unsigned numOfExpectedMatches = 100);


/* Inline implementations {{{ */

inline const Str & Str::operator=(Str && temp)
{
	data = temp.data;
	temp.data = 0;
	len = temp.len;
	temp.len = 0;
	return *this;
}
inline const Str & Str::operator=(const char * str)
	{ assign(str); return *this; }
inline const Str & Str::operator=(const Str & chunk)
	{ assign(chunk); return *this;}

inline Str::Str(const char * str, size_t length) : AStr(), length(len)
	{ ENSURE(str || !length,  InvalidArgument); assign(str, length); }
inline Str::Str(const char * str) : AStr(), length(len)
	{ if(str){ const char *c=str; while(*c!=0) c++; assign(str, c - str); } }

inline Str::Str(const String & str) : AStr(str), length(len)
	{ assign(str.c_str(), str.length); }


inline int Str::compare(const char * str, size_t _length) const
	{ return AStr::compare(str, _length); }
inline int Str::compare(const char * str) const
	{ return AStr::compare(str); }
inline int Str::compare(const Str & str) const
	{ return AStr::compare(str.data, str.len); }

inline bool operator<(const Str & a, const String & b)
	{ Str chunk(b); return a < chunk; }
inline bool operator<(const String & a, const Str & b)
	{ Str chunk(a); return chunk < b; }

inline String & operator<<=(String & lhs, const Str & rhs)
	{ lhs.assign(rhs.c_str(), rhs.length); return lhs; }
inline String & operator<<(String & lhs, const Str & rhs)
	{ lhs.append(rhs.c_str(), rhs.length); return lhs; }

}

/*}}}*/

#endif
