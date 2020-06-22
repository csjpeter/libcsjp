/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2020 Csaszar, Peter
 */

#ifndef CSJP_JSON_PARSER_H
#define CSJP_JSON_PARSER_H

#include <csjp_owner_container.h>
#include <csjp_array.h>
#include <csjp_string.h>

namespace csjp {

class Json
{
/**
 * Do not inherit from this class!
 */
#define JsonInitializer
public:
	enum class Type
	{
		Null,
		Boolean,
		Number,
		String,
		Object,
		Array
	};

	explicit Json(const Json & orig) :
		type(Type::Null),
		string(orig.string),
		properties(orig.properties),
		array(orig.array)
	{}
	const Json & operator=(const Json & orig)
	{
		if(orig.type == Json::Type::Array) {
		//	type = orig.type;
		//	array = orig.array;
		//	properties.clear();
		//	string.clear();
		}
		else if(orig.type == Json::Type::Object) {
		//	type = orig.type;
		//	properties = orig.properties;
		//	array.clear();
		//	string.clear();
		}
		else {
			type = orig.type;
			string = orig.string;
			array.clear();
			properties.clear();
		}
		return *this;
	}

	Json(Json && temp) :
		type(temp.type),
		string(move_cast(temp.string)),
		properties(move_cast(temp.properties)),
		array(move_cast(temp.array))
	{}
	/* The target key will not be overwritten so just skip it. */
	const Json & operator=(Json && temp)
	{
		type = move_cast(temp.type);
		string = move_cast(temp.string);
		properties = move_cast(temp.properties);
		array = move_cast(temp.array);
		return *this;
	}
	const Json & operator=(String && temp)
	{
		string = move_cast(temp);
		type = Json::Type::String;
		return *this;
	}
	const Json & operator=(const Str & str)
	{
		string = str;
		type = Json::Type::String;
		return *this;
	}


public:
	explicit Json(Type type = Type::Null) : type(type) {}
	~Json() { }

public:
	bool isEqual(const Json& i) const{
		if(type != i.type) return false;
		if(string != i.string) return false;
		if(properties != i.properties) return false;
		if(array != i.array) return false;
		return true;
	}

	/* Comparison by key is necessary when Json is put into the
	 * properties OwnerContainer. */
	bool isLess(const Json & i) const { return key < i.key; }
	bool isLess(const Str & s) const { return key < s; }
	bool isMore(const Str & s) const { return s < key; }

public:
	/* For reading and creating array elements */

	const Json & operator[](size_t idx) const
	{
		if(idx < array.length)
			return array[idx];
		return empty;
	}
	const Json & operator[](int idx) const { return operator[]((size_t)idx); }
	const Json & operator[](unsigned idx) const { return operator[]((size_t)idx); }
	Json & operator[](size_t idx)
	{
		if(idx == array.length){
			Object<Json> child(new Json());
			array.add(child);
			type = Json::Type::Array;
		}
		if(idx < array.length)
			return const_cast<Json &>(array[idx]);
		throw IndexOutOfRange("Json array has less element than index '%'.", idx);
	}
	Json & operator[](int idx) { return operator[]((size_t)idx); }
	Json & operator[](unsigned idx) { return operator[]((size_t)idx); }

	/* For reading and creating key:value pairs */

	template <typename Type>
	bool has(const Type & obj)
	{
		if(type == Json::Type::Array) { return array.has(obj); }
		else if(type == Json::Type::Object) { return properties.has(obj); }
		else { return string.contains(obj); }
	}
	template <typename Type>
	const Json & operator[](const Type & obj) const
	{
		try {
			return properties.query(obj);
		} catch (Exception &e) {
			return empty;
		}
	}
	template <typename Type>
	Json & operator[](const Type & obj)
	{
		if(!properties.has(obj)){
			Object<Json> child(new Json());
			child->key = obj;
			properties.add(child);
			type = Json::Type::Object;
		}
		return const_cast<Json &>(properties.query(obj));
	}

	/* To be usable as basic datatypes without explicit conversion. */

	operator const csjp::String & ()const { return string; }
	operator const csjp::Str ()	const { return csjp::Str(string); }
	operator char ()		const { char i;			i <<= string; return i; }
	operator unsigned char ()	const { unsigned char i;	i <<= string; return i; }
	operator int ()			const { int i;			i <<= string; return i; }
	operator long int ()		const { long int i;		i <<= string; return i; }
	operator long long int ()	const { long long int i;	i <<= string; return i; }
	operator unsigned ()		const { unsigned i;		i <<= string; return i; }
	operator long unsigned ()	const { long long unsigned i;	i <<= string; return i; }
	operator long long unsigned ()	const { long long unsigned i;	i <<= string; return i; }
	operator float ()		const { float i;		i <<= string; return i; }
	operator double ()		const { double i;		i <<= string; return i; }
	operator long double ()		const { long double i;		i <<= string; return i; }
	operator const UInt ()		const { UInt i;			i <<= string; return i; }
	operator const Double ()	const { Double i;		i <<= string; return i; }
	operator const Char ()		const { Char i;			i <<= string; return i; }
	operator const YNBool ()	const { YNBool i;		i <<= string; return i; }

private:
	static Json fromString(const Str & data);
	csjp::String toString(int depth, bool withKey = true) const;

public:
	void parse(const Str & data) { *this = Json::fromString(data); }
	csjp::String toString() const { return toString(0, false); }
	void clear() { type = Type::Null; string.clear(); properties.clear(); array.clear(); }
	size_t size() { if(type == Json::Type::Array) return array.length;
			if(type == Json::Type::Object) return properties.size();
			return string.length;
	}

	const String & value() const { return string; }

	void setValue(const csjp::Str & v)		{ string = v; }
	void setValue(const unsigned v)			{ string.cutAt(0); string << v; }
	void setValue(const long unsigned v)		{ string.cutAt(0); string << v; }
	void setValue(const long long unsigned v)	{ string.cutAt(0); string << v; }
	void setValue(const int v)			{ string.cutAt(0); string << v; }
	void setValue(const long int v)			{ string.cutAt(0); string << v; }
	void setValue(const long long int v)		{ string.cutAt(0); string << v; }
	void setValue(const float v)			{ string.cutAt(0); string << v; }
	void setValue(const double v)			{ string.cutAt(0); string << v; }
	void setValue(const long double v)		{ string.cutAt(0); string << v; }
	void setValue(const UInt v)			{ string.cutAt(0); string << v.val; }
	void setValue(const Double v)			{ string.cutAt(0); string << v.val; }

	void setValue(const Json::Type v)		{ type = v;
			if(type == Json::Type::Array) { properties.clear(); string.clear(); }
			else if(type == Json::Type::Object) { array.clear(); string.clear(); }
			else { properties.clear(); array.clear(); }
	}

	void appendValue(const csjp::Str & v)		{ string << v; }
	void appendValue(const char * v)		{ string << v; }
	void appendValue(const char v)			{ string << v; }

	bool isEqual(const Json::Type b) const	{ return type == b; }

	template<typename Arg>
	void cat(const Arg & arg) { string << arg; }
	template<typename Arg, typename... Args>
	void cat(const Arg & arg, const Args & ... args) { string << arg; cat(args...); }

	void catf(const char * fmt) { appendValue(fmt); }
	template<typename Arg>
	void catf(const char * fmt, const Arg & arg);
	template<typename Arg, typename... Args>
	void catf(const char * fmt, const Arg & arg, const Args & ... args);

public:
	String key;
	Type type;
	String string;
	OwnerContainer<Json> properties;
	Array<Json> array;
private:
	static Json empty;
};

inline bool operator==(const Json& a, const Json& b) { return a.isEqual(b); }
inline bool operator!=(const Json& a, const Json& b) { return !a.isEqual(b); }
inline bool operator==(const Json & a, const Str & b) { return a.value().isEqual(b); }
inline bool operator!=(const Json & a, const Str & b) { return !a.value().isEqual(b); }
inline bool operator==(const Json & a, const char * b) { return a.value().isEqual(b); }
inline bool operator!=(const Json & a, const char * b) { return !a.value().isEqual(b); }

inline bool operator==(const Json& a, const Json::Type & b) { return a.isEqual(b); }
inline bool operator!=(const Json& a, const Json::Type & b) { return !a.isEqual(b); }

inline bool operator<(const Json& a, const Json& b) { return a.isLess(b); }
inline bool operator<(const Json& a, const String& b) { return a.isLess(b); }
inline bool operator<(const String& a, const Json& b) { return b.isMore(a); }
inline bool operator<(const Json& a, const Str& b) { return a.isLess(b); }
inline bool operator<(const Str& a, const Json& b) { return b.isMore(a); }
inline bool operator<(const Json& a, const char * b) { return a.isLess(String(b)); }
inline bool operator<(const char * a, const Json& b) { return b.isMore(String(a)); }

inline String &	operator<<(csjp::String & lhs, const csjp::Json & rhs)
		{ lhs += rhs.value(); return lhs; }

inline String &	operator<<=(csjp::String & lhs, const csjp::Json & rhs)
		{ lhs = rhs.value(); return lhs; }
inline int & operator<<=(int & lhs, const Json & rhs)
		{ lhs <<= rhs.value(); return lhs; }
inline long int & operator<<=(long int & lhs, const Json & rhs)
		{ lhs <<= rhs.value(); return lhs; }
inline long long int & operator<<=(long long int & lhs, const Json & rhs)
		{ lhs <<= rhs.value(); return lhs; }
inline char & operator<<=(char & lhs, const Json & rhs)
		{ lhs <<= rhs.value(); return lhs; }
inline unsigned & operator<<=(unsigned & lhs, const Json & rhs)
		{ lhs <<= rhs.value(); return lhs; }
inline long unsigned & operator<<=(long unsigned & lhs, const Json & rhs)
		{ lhs <<= rhs.value(); return lhs; }
inline long long unsigned & operator<<=(long long unsigned & lhs, const Json & rhs)
		{ lhs <<= rhs.value(); return lhs; }
inline float & operator<<=(float & lhs, const Json & rhs)
		{ lhs <<= rhs.value(); return lhs; }
inline double & operator<<=(double & lhs, const Json & rhs)
		{ lhs <<= rhs.value(); return lhs; }
inline long double & operator<<=(long double & lhs, const Json & rhs)
		{ lhs <<= rhs.value(); return lhs; }
inline UInt & operator<<=(UInt & lhs, const Json & rhs)
		{ lhs.val <<= rhs; return lhs; }
inline Double & operator<<=(Double & lhs, const Json & rhs)
		{ lhs.val <<= rhs; return lhs; }

inline Json & operator<<=(Json & lhs, const csjp::Str & rhs)	{ lhs.setValue(rhs); return lhs; }
inline Json & operator<<=(Json & lhs, const char rhs)		{ lhs.setValue(rhs); return lhs; }
inline Json & operator<<=(Json & lhs, const unsigned rhs)	{ lhs.setValue(rhs); return lhs; }
inline Json & operator<<=(Json & lhs, const long unsigned rhs)	{ lhs.setValue(rhs); return lhs; }
inline Json & operator<<=(Json & lhs, const long long unsigned rhs){lhs.setValue(rhs);return lhs;}
inline Json & operator<<=(Json & lhs, const int rhs)		{ lhs.setValue(rhs); return lhs; }
inline Json & operator<<=(Json & lhs, const long int rhs)	{ lhs.setValue(rhs); return lhs; }
inline Json & operator<<=(Json & lhs, const long long int rhs)	{ lhs.setValue(rhs); return lhs; }
inline Json & operator<<=(Json & lhs, const float rhs)		{ lhs.setValue(rhs); return lhs; }
inline Json & operator<<=(Json & lhs, const double rhs)		{ lhs.setValue(rhs); return lhs; }
inline Json & operator<<=(Json & lhs, const long double rhs)	{ lhs.setValue(rhs); return lhs; }
inline Json & operator<<=(Json & lhs, const UInt rhs)		{ lhs.setValue(rhs); return lhs; }
inline Json & operator<<=(Json & lhs, const Double rhs)		{ lhs.setValue(rhs); return lhs; }
inline Json & operator<<=(Json & lhs, const Json::Type rhs)	{ lhs.setValue(rhs); return lhs; }

inline Json & operator<<(Json & lhs, const csjp::Str & rhs)	{ lhs.appendValue(rhs); return lhs; }

template<typename Arg> void Json::catf(const char * fmt, const Arg & arg)
{
	while(*fmt != 0){
		if(*fmt == '%'){
			fmt++;
			if(*fmt != '%'){
				this->string << arg;
				appendValue(fmt);
				return;
			}
		}
		appendValue(*fmt);
		fmt++;
	}
	this->string << arg;
}
template<typename Arg, typename... Args>
void Json::catf(const char * fmt, const Arg & arg, const Args & ... args)
{
	while(*fmt != 0){
		if(*fmt == '%'){
			fmt++;
			if(*fmt != '%'){
				this->string << arg;
				catf(fmt, args...);
				return;
			}
		}
		appendValue(*fmt);
		fmt++;
	}
	cat(args...);
}

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
