/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2016 Csaszar, Peter
 */

#include <stdlib.h>
#include <string.h>

#include <unicode/unistr.h>
#include <unicode/ustring.h>
#include <unicode/uchar.h>

#include <csjp_carray.h>
#include "csjp_unichar.h"

namespace csjp {

class UniCharPrivate
{
public:
	UChar32 val;
	String utf8;
	bool dirtyUtf8;

	void exportUtf8(String & _utf8) const;
};

/* http://www.unicoderg/reports/tr22/tr22-2.1.html
 *  Quote: The illegal code points are: out-of-range values (less than zero or greater than 10FFFF),
 *         unpaired surrogate values (D800 to DF00), and non-character values (of the form xxFFFF or
 *         xxFFFE).
 * */
static inline bool unicodePointValidity(UChar32 cp)
{
	if(cp < UCHAR_MIN_VALUE)
		return false;
	if(UCHAR_MAX_VALUE < cp)
		return false;
	if((cp & 0xfffe) == 0xfffe)
		return false;

	/* NOTE : These codes should be valid, but only if encountered in pairs. */
	if(0xd800 <= cp && cp <= 0xdf00)
		return false;

	/* We might want to use : int8_t u_charType(UChar32 c); */
	return u_charType(cp) != U_UNASSIGNED;

	return true;
}

const UniChar & UniChar::operator=(const UniChar & copy)
{
	*priv = *(copy.priv);
	return *this;
}

UniChar::UniChar(const UniChar & orig) UniCharInitializer
{
	priv = new UniCharPrivate();

	priv->val = orig.priv->val;
	priv->dirtyUtf8 = true;
}

UniChar::UniChar() UniCharInitializer
{
	priv = new UniCharPrivate();

	priv->val = 0;
	priv->dirtyUtf8 = true;
}

UniChar::~UniChar()
{
	delete priv;
}

UniChar::UniChar(UniChar && temp) :
	priv(temp.priv)
{
	temp.priv = 0;
}

const UniChar & UniChar::operator=(UniChar && temp)
{
	delete priv;
	priv = temp.priv;

	temp.priv = 0;

	return *this;
}

UniChar::UniChar(char ascii) :
	priv(NULL)
{
	priv = new UniCharPrivate();
	priv->val = ascii;
	priv->dirtyUtf8 = true;
}

UniChar::UniChar(const char * utf8) :
	priv(NULL)
{
	UChar32 cp = 0;

	/* If it happen to find a zero, terminating character, non of the if branches
	 * will apply, because each requires some non-null value. */

	/* http://en.wikipedia.ptrrg/wiki/UTF-8#Description */
		if(0x00 == (utf8[0] & 0x80)){ /* 7 bit -> 1 byte to use (ascii code) */
			cp = utf8[0];
	} else	if(0xc0 == (utf8[0] & 0xe0)){ /* 11 bit -> 2 byte to use */
			if(!(0x80 == (utf8[1] & 0xc0)))
				throw ConversionError("Invalid second byte in utf8 sequence: "
						"%", utf8[1]);
			cp = utf8[0] & 0x1f; cp <<= 6;
			cp |= utf8[1] & 0x3f;
	} else	if(0xe0 == (utf8[0] & 0xf0)) { /* 16 bit -> 3 byte to use */
			if(!(0x80 == (utf8[1] & 0xc0)))
				throw ConversionError("Invalid second byte in utf8 sequence: "
						"%", utf8[1]);
			if(!(0x80 == (utf8[2] & 0xc0)))
				throw ConversionError("Invalid third byte in utf8 sequence: "
						"%", utf8[2]);
			cp = utf8[0] & 0x0f; cp <<= 6;
			cp |= utf8[1] & 0x3f; cp <<= 6;
			cp |= utf8[2] & 0x3f;
	} else	if(0xf0 == (utf8[0] & 0xf8)) { /* 21 bit -> 4 byte to use */
			if(!(0x80 == (utf8[1] & 0xc0)))
				throw ConversionError("Invalid second byte in utf8 sequence: "
						"%", utf8[1]);
			if(!(0x80 == (utf8[2] & 0xc0)))
				throw ConversionError("Invalid third byte in utf8 sequence: "
						"%", utf8[2]);
			if(!(0x80 == (utf8[3] & 0xc0)))
				throw ConversionError("Invalid fourth byte in utf8 sequence: "
						"%", utf8[3]);
			cp = utf8[0] & 0x07; cp <<= 6;
			cp |= utf8[1] & 0x3f; cp <<= 6;
			cp |= utf8[2] & 0x3f; cp <<= 6;
			cp |= utf8[3] & 0x3f;
	} else
		throw ConversionError("Invalid first byte in utf8 sequence: %", utf8[0]);

	if(!unicodePointValidity(cp))
		throw InvalidArgument("The value '%' (converted from utf8 sequence "
				"'%%%%'), is not a valid unicode point.",
				(long unsigned int)cp, utf8[0], utf8[1], utf8[2], utf8[3]);

	priv = new UniCharPrivate();
	priv->val = cp;
	priv->dirtyUtf8 = true;
}

const UniChar & UniChar::operator=(char c)
{
	if(!unicodePointValidity(c))
		throw InvalidArgument("The value '%' is not a valid unicode point.",
				(long unsigned int)c);

	priv->val = c;

	priv->dirtyUtf8 = true;

	return *this;
}

const UniChar & UniChar::operator=(int i)
{
	if(!unicodePointValidity(i))
		throw InvalidArgument("The value '%' is not a valid unicode point.",
				(long unsigned int)i);

	priv->val = i;

	priv->dirtyUtf8 = true;

	return *this;
}

const UniChar & UniChar::operator=(long int i)
{
	if(!unicodePointValidity(i))
		throw InvalidArgument("The value '%' is not a valid unicode point.", i);

	priv->val = i;

	priv->dirtyUtf8 = true;

	return *this;
}

const UniChar & UniChar::operator=(long long int i)
{
	if(!unicodePointValidity(i))
		throw InvalidArgument("The value '%' is not a valid unicode point.", i);

	priv->val = i;

	priv->dirtyUtf8 = true;

	return *this;
}

const UniChar & UniChar::operator=(unsigned i)
{
	if(!unicodePointValidity(i))
		throw InvalidArgument("The value '%' is not a valid unicode point.", i);

	priv->val = i;

	priv->dirtyUtf8 = true;

	return *this;
}

const UniChar & UniChar::operator=(long unsigned i)
{
	if(!unicodePointValidity(i))
		throw InvalidArgument("The value '%' is not a valid unicode point.", i);

	priv->val = i;

	priv->dirtyUtf8 = true;

	return *this;
}

const UniChar & UniChar::operator=(long long unsigned i)
{
	if(!unicodePointValidity(i))
		throw InvalidArgument("The value '%' is not a valid unicode point.", i);

	priv->val = i;

	priv->dirtyUtf8 = true;

	return *this;
}

long int UniChar::value() const
{
	return priv->val;
}

void UniCharPrivate::exportUtf8(String & _utf8) const
{
	char utf8[5] = {0};
	UChar32 uc = val;
	/* http://en.wikipedia.ptrrg/wiki/UTF-8#Description */
		if(uc == (uc & 0x0000007f)) { /* 7 bit -> 1 byte to use (ascii code) */
			utf8[1] = 0;
			utf8[0] = uc & 0x0000007f;
	} else	if(uc == (uc & 0x000007ff)) { /* 11 bit -> 2 byte to use */
			utf8[2] = 0;
			utf8[1] = 0x80 | (uc & 0x0000003f); uc >>= 6;
			utf8[0] = 0xc0 | (uc & 0x0000001f);
	} else	if(uc == (uc & 0x0000ffff)) { /* 16 bit -> 3 byte to use */
			utf8[3] = 0;
			utf8[2] = 0x80 | (uc & 0x0000003f); uc >>= 6;
			utf8[1] = 0x80 | (uc & 0x0000003f); uc >>= 6;
			utf8[0] = 0xe0 | (uc & 0x0000000f);
	} else	if(uc == (uc & 0x001fffff)) { /* 21 bit -> 4 byte to use */
			utf8[4] = 0;
			utf8[3] = 0x80 | (uc & 0x0000003f); uc >>= 6;
			utf8[2] = 0x80 | (uc & 0x0000003f); uc >>= 6;
			utf8[1] = 0x80 | (uc & 0x0000003f); uc >>= 6;
			utf8[0] = 0xf0 | (uc & 0x00000007);
	} else	if(uc == (uc & 0x03ffffff)) { /* 26 bit -> 5 byte to use */
			throw ConversionError("There should not be need for fifth byte to "
					"represent unicode character in utf8.");
	} else	if(uc == (uc & 0x7fffffff)) { /* 31 bit -> 6 byte to use */
			throw ConversionError("There should not be need for sixth byte to "
					"represent unicode character in utf8.");
	} else
		throw ConversionError("Could not convert UniChar to Utf8.");
	_utf8.assign(utf8);
}

const String & UniChar::utf8() const
{
	if(priv->dirtyUtf8){
		priv->exportUtf8(priv->utf8);
		priv->dirtyUtf8 = false;
	}

	return priv->utf8;
}

bool UniChar::isEqual(const char & b) const
{
	return priv->val == b;
}

bool UniChar::isEqual(const UniChar & b) const
{
	return priv->val == b.priv->val;
}

void UniChar::toLower()
{
	priv->val = u_tolower(priv->val);

	priv->dirtyUtf8 = true;
}

void UniChar::toUpper()
{
	priv->val = u_toupper(priv->val);

	priv->dirtyUtf8 = true;
}

int UniChar::digitValue() const
{
	int32_t i = u_charDigitValue(priv->val);
	/*if(i == -1)
		throw ParseError("Unicode character (%) has no digital value.", utf8().c_str());*/

	return i;
}

double UniChar::numericValue() const
{
	double d = u_getNumericValue(priv->val);
	if(d == U_NO_NUMERIC_VALUE){
		d = -1;
	}

	return d;
}

void UniChar::name(String & str) const
{
	UErrorCode err = U_ZERO_ERROR;
	CArray<char> buffer;

	unsigned len = u_charName(priv->val, U_EXTENDED_CHAR_NAME, buffer.ptr, 0, &err);

	unsigned size = len + 1;
	buffer.resize(size);

	err = U_ZERO_ERROR;
	u_charName(priv->val, U_EXTENDED_CHAR_NAME, buffer.ptr, len, &err);

	if(U_FAILURE(err))
		throw ConversionError("Could not get name for unicode character: %. "
				"Error from u_charName: %",
				(long int)(priv->val), u_errorName(err));

	buffer.ptr[len] = 0;

	str.adopt(buffer.ptr, len, size);
}
/*
void UniChar::isoComment(String & str) const
{
	UErrorCode err = U_ZERO_ERROR;
	CArray<char> buffer;

	// FIXME deprecated, what to use instead?
	unsigned len = u_getISOComment(priv->val, buffer.ptr, 0, &err);

	unsigned size = len + 1;
	buffer.resize(size);

	err = U_ZERO_ERROR;
	u_getISOComment(priv->val, buffer.ptr, len, &err);

	if(U_FAILURE(err))
		throw ConversionError("Could not get iso comment for unicode character: %. "
				"Error from u_getISOComment: %",
				(long int)(priv->val), u_errorName(err));

	buffer.ptr[len] = 0;

	str.adopt(buffer.ptr, len, size);
}
*/
bool UniChar::isIdStarter() const
{
	return u_isIDStart(priv->val);
}

bool UniChar::isAlpha() const
{
	return u_isalpha(priv->val);
}

bool UniChar::isAlphaNumeric() const
{
	return u_isalnum(priv->val);
}

bool UniChar::isDigit() const
{
	return u_isdigit(priv->val);
}

bool UniChar::isHexaDigit() const
{
	return u_isxdigit(priv->val);
}

bool UniChar::isTitleCase() const
{
	return u_istitle(priv->val);
}

bool UniChar::isUpper() const
{
	return u_isupper(priv->val);
}

bool UniChar::isLower() const
{
	return u_islower(priv->val);
}

bool UniChar::isPunctuation() const
{
	return u_ispunct(priv->val);
}

bool UniChar::isGraphic() const
{
	return u_isgraph(priv->val);
}

bool UniChar::isBlank() const
{
	return u_isblank(priv->val);
}

bool UniChar::isDefined() const
{
	return u_isdefined(priv->val);
}

bool UniChar::isSpace() const
{
	return u_isspace(priv->val);
}

bool UniChar::isControl() const
{
	return u_iscntrl(priv->val);
}

bool UniChar::isWhiteSpace() const
{
	return u_isWhitespace(priv->val);
}

bool operator==(const UniChar & a, char b)
{
	return a.isEqual(b);
}

bool operator==(const UniChar & a, const UniChar & b)
{
	return a.isEqual(b);
}

bool operator!=(const UniChar & a, char b)
{
	return !a.isEqual(b);
}

bool operator!=(const UniChar & a, const UniChar & b)
{
	return !a.isEqual(b);
}
/*
bool operator<(const UniChar & a, const UniChar & b)
{
	return true;
}
*/
}
