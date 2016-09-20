/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2016 Csaszar, Peter
 */

#ifndef CSJP_TEXT_H
#define CSJP_TEXT_H

#include <csjp_string.h>
#include <csjp_unichar.h>

namespace csjp {

/** If you can, use String class instead. Using Text is extremely expensive. However on the
 * other hand Text is in unicode and is capable to handle different cultures, not just english.
 *
 * The inner implementation is working with 16 bit representation. Input char* and exported char*
 * variables are interpreted as they were in utf8 format. Thus both on assigning value or reading
 * out takes a full copy and expensive conversion works.
 */
class TextPrivate;
class Text {
/**
 * Do not inherit from this class!
 */
#define TextInitializer : \
	priv(0)
public:
	explicit Text(const Text & orig);
	const Text & operator=(const Text & orig);

	Text(Text && temp);
	const Text & operator=(Text && temp);

public:
	explicit Text();
	~Text();

private:
	TextPrivate *priv;

public:
	explicit Text(const char * utf8, size_t length);
	explicit Text(const char * utf8);
	explicit Text(const Str &);

	const Text& operator=(const char * utf8);
	const Text& operator=(const String &);

	const Text& operator+=(const Text &);
	const Text& operator+=(const char * utf8);
	const Text& operator+=(const String &);

	void assign(const char * utf8, size_t _length);
	void assign(const Str &);

	void uniCharAt(UniChar & c, size_t pos) const;
	UniChar uniCharAt(size_t pos) const
	{
		UniChar c;
		uniCharAt(c, pos);
		return c;
	}

public:
	const String & utf8() const;

	/* Number of codepoints in text. */
	size_t length() const;

	int compare(const Text &) const;
	int compare(const char * utf8) const;
	int compare(const char * utf8, size_t _length) const;

	int caseCompare(const Text &) const;
	int caseCompare(const char * utf8) const;
	int caseCompare(const char * utf8, size_t _length) const;

	bool isEqual(const Text &) const;
	bool isEqual(const char * utf8) const;
	bool isEqual(const char * utf8, size_t _length) const;

	void clear();

	void prepend(const Text&);
	void prepend(const char * utf8);
	void prepend(const char * utf8, size_t _length);

	void append(const Text&);
	void append(const char * utf8);
	void append(const char * utf8, size_t _length);

	void insert(size_t pos, const Text &);
	void insert(size_t pos, const char * utf8);
	void insert(size_t pos, const char * utf8, size_t _length);

	void erase(size_t from, size_t until);

	void write(size_t pos, const Text & text);

	void read(Text &, size_t from, size_t until) const;
	Text read(size_t from, size_t until) const
	{
		Text text;
		read(text, from, until);
		return text;
	}

	bool findFirst(size_t & pos, const Text & what, size_t from, size_t until) const;
	bool findFirst(size_t & pos, const Text & what, size_t from = 0) const;
	bool findFirst(size_t & pos, const char * what, size_t _length, size_t from, size_t until) const;
	bool findFirst(size_t & pos, const char * what, size_t from, size_t until) const;
	bool findFirst(size_t & pos, const char * what, size_t from = 0) const;
	bool findFirst(size_t & pos, const UniChar & c, size_t from, size_t until) const;
	bool findFirst(size_t & pos, const UniChar & c, size_t from = 0) const;

	bool findLast(size_t & pos, const Text & what, size_t from, size_t until) const;
	bool findLast(size_t & pos, const Text & what, size_t from = 0) const;
	bool findLast(size_t & pos, const char * what, size_t _length, size_t from, size_t until) const;
	bool findLast(size_t & pos, const char * what, size_t from, size_t until) const;
	bool findLast(size_t & pos, const char * what, size_t from = 0) const;
	bool findLast(size_t & pos, const UniChar & c, size_t from, size_t until) const;
	bool findLast(size_t & pos, const UniChar & c, size_t from = 0) const;

	bool startsWith(const Text & text) const;
	bool startsWith(const char * utf8) const;
	bool startsWith(const char * utf8, size_t _length) const;

	bool endsWith(const Text & text) const;
	bool endsWith(const char * utf8) const;
	bool endsWith(const char * utf8, size_t _length) const;

	void replace(const Text & what, const Text & to, size_t from, size_t until);
	void replace(const Text & what, const Text & to, size_t from = 0);
	void replace(const char * utf8What, size_t whatLength, const char * utf8To, size_t toLength, size_t from, size_t until);
	void replace(const char * utf8What, size_t whatLength, const char * utf8To, size_t toLength, size_t from = 0);
	void replace(const char * utf8What, const char * utf8To, size_t from, size_t until);
	void replace(const char * utf8What, const char * utf8To, size_t from = 0);

	void cutAt(size_t);

	void trim();
};

bool operator==(const Text & a, const char * utf8);
bool operator==(const Text & a, const Text & b);
bool operator!=(const Text & a, const Text & b);

bool operator<(const Text & a, const Text & b);

inline String & operator<<(String & lhs, const Text & rhs) { lhs << rhs.utf8(); return lhs; }

}

#endif
