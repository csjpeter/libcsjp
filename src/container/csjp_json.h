/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2015 Csaszar, Peter
 */

#ifndef CSJP_JSON_PARSER_H
#define CSJP_JSON_PARSER_H

#include <csjp_owner_container.h>
#include <csjp_ref_array.h>
#include <csjp_string_chunk.h>

namespace csjp {

class Json
{
/**
 * Do not inherit from this class!
 */
#define JsonInitializer
public:
	explicit Json(const Json & orig) :
		key(orig.key),
		value(orig.value),
		properties(orig.properties)
	{}
	const Json & operator=(const Json & orig) = delete;

	Json(Json && temp) :
		key(move_cast(temp.key)),
		value(move_cast(temp.value)),
		properties(move_cast(temp.properties))
	{}
	/* The target key will not be overwritten so just skip it. */
	const Json & operator=(Json && temp)
	{
		//key = move_cast(temp.key);
		value = move_cast(temp.value);
		properties = move_cast(temp.properties);
		return *this;
	}
	const Json & operator=(String && temp)
	{
		value = move_cast(temp);
		return *this;
	}
	const Json & operator=(const String & str)
	{
		value = str;
		return *this;
	}
	const Json & operator=(const char * str)
	{
		value = str;
		return *this;
	}


public:
	explicit Json() { }
	~Json() { }

public:
	explicit Json(const String & k) : key(k) {}
	explicit Json(const StringChunk & k) : key(k.str, k.length) {}
	explicit Json(const char * k) : key(k) {}

	bool isEqual(const Json& i) const{
		if(key != i.key) return false;
		if(properties != i.properties) return false;
		return true;
	}

	bool isLess(const Json & i) const { return key < i.key; }
	bool isLess(const String & s) const { return key < s; }
	bool isMore(const String & s) const { return s < key; }
	bool isLess(const StringChunk & s) const { return key < s; }
	bool isMore(const StringChunk & s) const { return s < key; }

public:
	template <typename Type>
	const Json & operator[](const Type & obj) const
	{
		try {
			return properties.query(obj);
		} catch (Exception &e) {
			e.note("Missing key in json object: '%'", obj);
			throw;
		}
	}

	template <typename Type>
	Json & operator[](const Type & obj)
	{
		if(!properties.has(obj)){
			Object<Json> child(new Json(obj));
			properties.add(child);
		}
		return const_cast<Json &>(properties.query(obj));
	}

private:
	static Json fromString(const String & data);
	csjp::String toString(int depth) const;

public:
	void parse(const String & data) { *this = Json::fromString(data); }
	csjp::String toString() const { return toString(0); }

public:
	const String key;
	String value;
	OwnerContainer<Json> properties;
};

inline bool operator==(const Json& a, const Json& b) { return a.isEqual(b); }
inline bool operator!=(const Json& a, const Json& b) { return !a.isEqual(b); }
inline bool operator==(const Json & a, const String & b) { return a.value.isEqual(b); }
inline bool operator!=(const Json & a, const String & b) { return !a.value.isEqual(b); }
inline bool operator==(const Json & a, const char * b) { return a.value.isEqual(b); }
inline bool operator!=(const Json & a, const char * b) { return !a.value.isEqual(b); }

inline bool operator<(const Json& a, const Json& b) { return a.isLess(b); }
inline bool operator<(const Json& a, const String& b) { return a.isLess(b); }
inline bool operator<(const String& a, const Json& b) { return b.isMore(a); }
inline bool operator<(const Json& a, const StringChunk& b) { return a.isLess(b); }
inline bool operator<(const StringChunk& a, const Json& b) { return b.isMore(a); }
inline bool operator<(const Json& a, const char * b) { return a.isLess(String(b)); }
inline bool operator<(const char * a, const Json& b) { return b.isMore(String(a)); }

inline String &	operator<<=(csjp::String & lhs, const csjp::Json & rhs)
		{ lhs = rhs.value; return lhs; }
inline int & operator<<=(int & lhs, const Json & rhs)
		{ lhs <<= rhs.value; return lhs; }
inline long int & operator<<=(long int & lhs, const Json & rhs)
		{ lhs <<= rhs.value; return lhs; }
inline long long int & operator<<=(long long int & lhs, const Json & rhs)
		{ lhs <<= rhs.value; return lhs; }
inline unsigned & operator<<=(unsigned & lhs, const Json & rhs)
		{ lhs <<= rhs.value; return lhs; }
inline long unsigned & operator<<=(long unsigned & lhs, const Json & rhs)
		{ lhs <<= rhs.value; return lhs; }
inline long long unsigned & operator<<=(long long unsigned & lhs, const Json & rhs)
		{ lhs <<= rhs.value; return lhs; }
inline float & operator<<=(float & lhs, const Json & rhs)
		{ lhs <<= rhs.value; return lhs; }
inline double & operator<<=(double & lhs, const Json & rhs)
		{ lhs <<= rhs.value; return lhs; }
inline long double & operator<<=(long double & lhs, const Json & rhs)
		{ lhs <<= rhs.value; return lhs; }
inline UInt & operator<<=(UInt & lhs, const Json & rhs)
		{ lhs.val <<= rhs; return lhs; }
inline Double & operator<<=(Double & lhs, const Json & rhs)
		{ lhs.val <<= rhs; return lhs; }

inline Json & operator<<=(Json & lhs, const csjp::String & rhs)
		{ lhs.value = rhs; return lhs; }
inline Json & operator<<=(Json & lhs, const unsigned rhs)
		{ lhs.value.cutAt(0); lhs.value << rhs; return lhs; }
inline Json & operator<<=(Json & lhs, const long unsigned rhs)
		{ lhs.value.cutAt(0); lhs.value << rhs; return lhs; }
inline Json & operator<<=(Json & lhs, const long long unsigned rhs)
		{ lhs.value.cutAt(0); lhs.value << rhs; return lhs; }
inline Json & operator<<=(Json & lhs, const int rhs)
		{ lhs.value.cutAt(0); lhs.value << rhs; return lhs; }
inline Json & operator<<=(Json & lhs, const long int rhs)
		{ lhs.value.cutAt(0); lhs.value << rhs; return lhs; }
inline Json & operator<<=(Json & lhs, const long long int rhs)
		{ lhs.value.cutAt(0); lhs.value << rhs; return lhs; }
inline Json & operator<<=(Json & lhs, const float rhs)
		{ lhs.value.cutAt(0); lhs.value << rhs; return lhs; }
inline Json & operator<<=(Json & lhs, const double rhs)
		{ lhs.value.cutAt(0); lhs.value << rhs; return lhs; }
inline Json & operator<<=(Json & lhs, const long double rhs)
		{ lhs.value.cutAt(0); lhs.value << rhs; return lhs; }
inline Json & operator<<=(Json & lhs, const UInt rhs)
		{ lhs.value.cutAt(0); lhs.value <<= rhs.val; return lhs; }
inline Json & operator<<=(Json & lhs, const Double rhs)
		{ lhs.value.cutAt(0); lhs.value <<= rhs.val; return lhs; }


}

/* JSON language
 *
 * Whitespaces
 *	SPACE NEWLINE TAB
 *
 * Terminals
 *	{ } [ ] " , : Whitespaces
 *
 * Grammar
 *	object
 *		{}
 *		{ members }
 *	members
 *		pair
 *		pair , members
 *	pair
 *		string : value
 *	array
 *		[]
 *		[ elements ]
 *	elements
 *		value
 *		value , elements
 *	value
 *		string
 *		number
 *		true
 *		false
 *		null
 *		object
 *		array
 *
 * ******************
 * string
 *	""
 *	" chars "
 * chars
 *	char
 *	char chars
 * char
 *	any-Unicode-character-except-"-or-\-or-control-character
 *	\"
 *	\\
 *	\/
 *	\b
 *	\f
 *	\n
 *	\r
 *	\t
 *	\u four-hex-digits
 * number
 *	int
 *	int frac
 *	int exp
 *	int frac exp
 * int
 *	digit
 *	digit1-9 digits
 *	- digit
 *	- digit1-9 digits
 * frac
 *	. digits
 * exp
 *	e digits
 * digits
 *	digit
 *	digit digits
 * e
 *	e
 *	e+
 *	e-
 *	E
 *	E+
 *	E-
 */

#endif
