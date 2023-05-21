/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2016 Csaszar, Peter
 */

#include <stdlib.h>
#include <string.h>

#include <unicode/unistr.h>
#include <unicode/ustring.h>
#include <unicode/stringoptions.h>

#include <csjp_carray.h>
#include "csjp_text.h"

namespace csjp {

class TextPrivate
{
public:
	icu::UnicodeString val;
	size_t len;
	String utf8;
	bool dirtyUtf8;

	void exportUtf8(String & utf8);
};

Text::Text(const Text & orig) TextInitializer
{
	priv = new TextPrivate;
	priv->val = orig.priv->val;
	priv->len = orig.priv->len;
	priv->dirtyUtf8 = true;
}

const Text & Text::operator=(const Text & copy)
{
	*priv = *(copy.priv);
	return *this;
}

Text::Text() TextInitializer
{
	priv = new TextPrivate;
	priv->len = 0;
	priv->dirtyUtf8 = false;
}

Text::~Text()
{
	if(priv)
		delete priv;
}

Text::Text(Text && temp) :
	priv(temp.priv)
{
	temp.priv = 0;
}

const Text & Text::operator=(Text && temp)
{
	delete priv;
	priv = temp.priv;

	temp.priv = 0;

	return *this;
}

Text::Text(const char * utf8, size_t _length) :
	priv(new TextPrivate)
{
	priv->len = 0;
	priv->dirtyUtf8 = false;

	assign(utf8, _length);
}

Text::Text(const char * utf8) :
	priv(new TextPrivate)
{
	priv->len = 0;
	priv->dirtyUtf8 = false;

	assign(utf8);
}

Text::Text(const Str & utf8) :
	priv(new TextPrivate)
{
	priv->len = 0;
	priv->dirtyUtf8 = false;

	assign(utf8);
}

const Text & Text::operator=(const char * utf8)
{
	assign(utf8);
	return *this;
}

const Text & Text::operator=(const String & utf8)
{
	assign(utf8);
	return *this;
}

const Text & Text::operator+=(const char * utf8)
{
	if(!utf8){
		clear();
		return *this;
	}

	size_t len = strlen(utf8);

	append(utf8, len);
	return *this;
}

const Text & Text::operator+=(const String & utf8)
{
	append(utf8.c_str(), utf8.length);
	return *this;
}

const Text & Text::operator+=(const Text & text)
{
	append(text);
	return *this;
}

void Text::assign(const char * utf8, size_t _length)
{
	ENSURE(utf8 || !_length, InvalidArgument);

	priv->val = icu::UnicodeString::fromUTF8(
			icu_66::StringPiece(utf8, _length));

	if(priv->val.isBogus())
		throw ParseError("Could not assign bad utf8 sequence to Text object.");

	priv->len = priv->val.countChar32();
	priv->dirtyUtf8 = true;
}

void Text::assign(const Str & utf8)
{
	assign(utf8.c_str(), utf8.length);
}

void Text::uniCharAt(UniChar & c, size_t pos) const
{
	size_t unitPos = 0;
	unitPos = priv->val.moveIndex32(unitPos, pos);
	c = priv->val.char32At(unitPos);
}

void TextPrivate::exportUtf8(String & utf8)
{
#if DEBUG
	ENSURE(!val.isBogus(), InvariantFailure);
#endif
	String _str;
	int32_t _len;
	UErrorCode err = U_ZERO_ERROR;

	u_strToUTF8(NULL, 0, &_len, val.getBuffer(), val.length(), &err);

	if(U_FAILURE(err) && err != U_BUFFER_OVERFLOW_ERROR)
		throw ConversionError("Could not count the neccessary amount of bytes to store "
				"Text in utf8 format. Error from u_strToUTF8: %",
				u_errorName(err));

	size_t size = _len + 1;
	CArray<char> buffer(size);

	err = U_ZERO_ERROR;
	u_strToUTF8(buffer.ptr, _len, &_len, val.getBuffer(), val.length(), &err);

	if(U_FAILURE(err))
		throw ConversionError("Could not convert Text into utf8 format. "
				"Error from u_strToUTF8: %",
				u_errorName(err));

	buffer.ptr[_len] = 0;

	DBG("_len: %; str: [%]", _len, buffer.ptr);

	utf8.adopt(buffer.ptr, _len, size);
}

const String & Text::utf8() const
{
	if(priv->dirtyUtf8){
		priv->exportUtf8(priv->utf8);
		priv->dirtyUtf8 = false;
	}
	return priv->utf8;
}

size_t Text::length() const
{
	return priv->len;
}

int Text::compare(const Text& text) const
{
	return priv->val.compareCodePointOrder(text.priv->val);
}

int Text::compare(const char * utf8) const
{
	return compare(Text(utf8));
}

int Text::compare(const char * utf8, size_t _length) const
{
	return compare(Text(utf8, _length));
}

int Text::caseCompare(const Text& text) const
{
	return priv->val.caseCompare(text.priv->val, U_COMPARE_CODE_POINT_ORDER);
}

int Text::caseCompare(const char * utf8) const
{
	return caseCompare(Text(utf8));
}

int Text::caseCompare(const char * utf8, size_t _length) const
{
	return caseCompare(Text(utf8, _length));
}

bool Text::isEqual(const Text &text) const
{
	return !compare(text);
}

bool Text::isEqual(const char * utf8) const
{
	return !compare(Text(utf8));
}

bool Text::isEqual(const char * utf8, size_t _length) const
{
	return !compare(Text(utf8, _length));
}

void Text::clear()
{
	priv->val.remove();
	priv->len = 0;
	priv->dirtyUtf8 = true;
}

void Text::prepend(const Text &text)
{
	insert(0, text);
}

void Text::prepend(const char * utf8)
{
	Text text(utf8);

	insert(0, text);
}

void Text::prepend(const char * utf8, size_t _length)
{
	Text text(utf8, _length);

	insert(0, text);
}

void Text::append(const Text &text)
{
	priv->val.append(text.priv->val);

	priv->len += text.priv->len;
	priv->dirtyUtf8 = true;
}

void Text::append(const char * utf8)
{
	Text text(utf8);

	append(text);
}

void Text::append(const char * utf8, size_t _length)
{
	Text text(utf8, _length);

	append(text);
}

void Text::insert(size_t pos, const Text & text)
{
	ENSURE(pos <= priv->len, InvalidArgument);

	size_t unitPos = 0;
	unitPos = priv->val.moveIndex32(unitPos, pos);

	priv->val.insert(unitPos, text.priv->val);

	priv->len += text.priv->len;
	priv->dirtyUtf8 = true;
}

void Text::insert(size_t pos, const char * utf8)
{
	insert(pos, Text(utf8));
}

void Text::insert(size_t pos, const char * utf8, size_t _length)
{
	insert(pos, Text(utf8, _length));
}

void Text::erase(size_t from, size_t until)
{
	ENSURE(from <= until, InvalidArgument);
	ENSURE(until <= priv->len, InvalidArgument);

	if(from == until)
		return;

	size_t unitFrom = priv->val.moveIndex32(0, from);
	size_t unitUntil = priv->val.moveIndex32(unitFrom, until - from);

	priv->val.remove(unitFrom, unitUntil - unitFrom);

	priv->len -= until - from;
	priv->dirtyUtf8 = true;
}

/** The new text must fit in the existing place (in code points). */
void Text::write(size_t pos, const Text & text)
{
	ENSURE(pos + text.length() <= priv->len, InvalidArgument);

	size_t unitFrom = priv->val.moveIndex32(0, pos);
	size_t unitUntil = priv->val.moveIndex32(unitFrom, text.length());

	priv->val.replace(unitFrom, unitUntil - unitFrom, text.priv->val);
	priv->dirtyUtf8 = true;
}

void Text::read(Text &text, size_t from, size_t until) const
{
	ENSURE(from <= until, InvalidArgument);
	ENSURE(until <= priv->len, InvalidArgument);

	if(from == until){
		text.clear();
		return;
	}

	size_t unitFrom = priv->val.moveIndex32(0, from);
	size_t unitUntil = priv->val.moveIndex32(unitFrom, until - from);

	priv->val.extract(unitFrom, unitUntil - unitFrom, text.priv->val);

	text.priv->len = until - from;
	priv->dirtyUtf8 = true;
}

bool Text::findFirst(size_t & pos, const Text & what, size_t from, size_t until) const
{
	ENSURE(what.length(), InvalidArgument);
	ENSURE(from <= priv->len, InvalidArgument);
	ENSURE(from <= until, InvalidArgument);
	ENSURE(until <= priv->len, InvalidArgument);

	if(until - from < what.priv->len)
		return false;
	if(!priv->len)
		return false;

	size_t unitFrom = priv->val.moveIndex32(0, from);
	size_t unitUntil = priv->val.moveIndex32(unitFrom, until - from);

	int32_t p = priv->val.indexOf(what.priv->val, unitFrom, unitUntil - unitFrom);

	if(0 <= p){
		pos = p;
		return true;
	}

	return false;
}

bool Text::findFirst(size_t & pos, const Text & what, size_t from) const
{
	return findFirst(pos, what, from, priv->len);
}

bool Text::findFirst(size_t & pos, const char * what, size_t _length, size_t from, size_t until) const
{
	Text whatText(what, _length);

	return findFirst(pos, whatText, from, until);
}

bool Text::findFirst(size_t & pos, const char * what, size_t from, size_t until) const
{
	Text whatText(what);

	return findFirst(pos, whatText, from, until);
}

bool Text::findFirst(size_t & pos, const char * what, size_t from) const
{
	Text whatText(what);

	return findFirst(pos, whatText, from, priv->len);
}

bool Text::findFirst(size_t & pos, const UniChar & c, size_t from, size_t until) const
{
	ENSURE(from <= until, InvalidArgument);
	ENSURE(until <= priv->len, InvalidArgument);
	ENSURE(from <= priv->len, InvalidArgument);

	if(until - from < priv->len)
		return false;
	if(!priv->len)
		return false;

	size_t unitFrom = priv->val.moveIndex32(0, from);
	size_t unitUntil = priv->val.moveIndex32(unitFrom, until - from);
	UChar32 c32 = c.value();

	int32_t p = priv->val.indexOf(c32, unitFrom, unitUntil - unitFrom);

	if(0 <= p){
		pos = p;
		return true;
	}

	return false;
}

bool Text::findFirst(size_t & pos, const UniChar & c, size_t from) const
{
	return findFirst(pos, c, from, priv->len);
}

bool Text::findLast(size_t & pos, const Text & what, size_t from, size_t until) const
{
	ENSURE(what.length(), InvalidArgument);
	ENSURE(from <= priv->len, InvalidArgument);
	ENSURE(from <= until, InvalidArgument);
	ENSURE(until <= priv->len, InvalidArgument);

	if(until - from < what.priv->len)
		return false;
	if(!priv->len)
		return false;

	size_t unitFrom = priv->val.moveIndex32(0, from);
	size_t unitUntil = priv->val.moveIndex32(unitFrom, until - from);

	int32_t p = priv->val.lastIndexOf(what.priv->val, unitFrom, unitUntil - unitFrom);

	if(0 <= p){
		pos = p;
		return true;
	}

	return false;
}

bool Text::findLast(size_t & pos, const Text & what, size_t from) const
{
	return findLast(pos, what, from, priv->len);
}

bool Text::findLast(size_t & pos, const char * what, size_t _length, size_t from, size_t until) const
{
	Text whatText(what, _length);

	return findLast(pos, whatText, from, until);
}

bool Text::findLast(size_t & pos, const char * what, size_t from, size_t until) const
{
	Text whatText(what);

	return findLast(pos, whatText, from, until);
}

bool Text::findLast(size_t & pos, const char * what, size_t from) const
{
	Text whatText(what);

	return findLast(pos, whatText, from, priv->len);
}

bool Text::findLast(size_t & pos, const UniChar & c, size_t from, size_t until) const
{
	ENSURE(from <= until, InvalidArgument);
	ENSURE(until <= priv->len, InvalidArgument);
	ENSURE(from <= priv->len, InvalidArgument);

	if(until - from < priv->len)
		return false;
	if(!priv->len)
		return false;

	size_t unitFrom = priv->val.moveIndex32(0, from);
	size_t unitUntil = priv->val.moveIndex32(unitFrom, until - from);
	UChar32 c32 = c.value();

	int32_t p = priv->val.lastIndexOf(c32, unitFrom, unitUntil - unitFrom);

	if(0 <= p){
		pos = p;
		return true;
	}

	return false;
}

bool Text::findLast(size_t & pos, const UniChar & c, size_t from) const
{
	return findLast(pos, c, from, priv->len);
}

bool Text::startsWith(const Text & text) const
{
	return priv->val.startsWith(text.priv->val);
}

bool Text::startsWith(const char * utf8) const
{
	return startsWith(Text(utf8));
}

bool Text::startsWith(const char * utf8, size_t _length) const
{
	return startsWith(Text(utf8, _length));
}

bool Text::endsWith(const Text & text) const
{
	return priv->val.endsWith(text.priv->val);
}

bool Text::endsWith(const char * utf8) const
{
	return endsWith(Text(utf8));
}

bool Text::endsWith(const char * utf8, size_t _length) const
{
	return endsWith(Text(utf8, _length));
}

/** Replaces all occurences of 'what' to 'to'. */
void Text::replace(const Text & what, const Text & to, size_t from, size_t until)
{
	ENSURE(from < until, InvalidArgument);
	ENSURE(until <= priv->len, InvalidArgument);
	ENSURE(what.priv->len, InvalidArgument);

	if(until - from < what.priv->len){
		return;
	}

	size_t unitFrom = priv->val.moveIndex32(0, from);
	size_t unitUntil = priv->val.moveIndex32(unitFrom, until - from);

	priv->val.findAndReplace(unitFrom, unitUntil - unitFrom, what.priv->val, to.priv->val);

	priv->len = priv->val.countChar32();
	priv->dirtyUtf8 = true;
}

void Text::replace(const Text & what, const Text & to, size_t from)
{
	replace(what, to, from, priv->len);
}

void Text::replace(const char * utf8What, size_t whatLength, const char * utf8To, size_t toLength, size_t from)
{
	Text textWhat(utf8What, whatLength);
	Text textTo(utf8To, toLength);

	replace(textWhat, textTo, from, priv->len);
}

void Text::replace(const char * utf8What, size_t whatLength, const char * utf8To, size_t toLength, size_t from, size_t until)
{
	Text textWhat(utf8What, whatLength);
	Text textTo(utf8To, toLength);

	replace(textWhat, textTo, from, until);
}

void Text::replace(const char * utf8What, const char * utf8To, size_t from, size_t until)
{
	Text textWhat(utf8What);
	Text textTo(utf8To);

	replace(textWhat, textTo, from, until);
}

void Text::replace(const char * utf8What, const char * utf8To, size_t from)
{
	Text textWhat(utf8What);
	Text textTo(utf8To);

	replace(textWhat, textTo, from, priv->len);
}

void Text::cutAt(size_t pos)
{
	ENSURE(pos <= priv->len,  InvalidArgument);

	size_t unitPos = priv->val.moveIndex32(0, pos);

	priv->val.remove(unitPos);

	priv->len = unitPos;
	priv->dirtyUtf8 = true;
}

void Text::trim()
{
	priv->val.trim();

	priv->len = priv->val.countChar32();
	priv->dirtyUtf8 = true;
}

bool operator==(const Text & a, const char * utf8)
{
	return a.isEqual(Text(utf8));
}

bool operator==(const Text& a, const Text& b)
{
	return a.isEqual(b);
}

bool operator!=(const Text& a, const Text& b)
{
	return !a.isEqual(b);
}

bool operator<(const Text& a, const Text& b)
{
	return a.compare(b) < 0;
}

}
