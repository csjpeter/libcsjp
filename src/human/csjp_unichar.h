/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2016 Csaszar, Peter
 */

#ifndef CSJP_UNICHAR_H
#define CSJP_UNICHAR_H

#include <csjp_string.h>

namespace csjp {

class UniCharPrivate;
class UniChar
{
/**
 * Do not inherit from this class!
 */
#define UniCharInitializer : \
	priv(0)
public:
	explicit UniChar(const UniChar & orig);
	const UniChar & operator=(const UniChar & copy);

	UniChar(UniChar && temp);
	const UniChar & operator=(UniChar && temp);

public:
	explicit UniChar();
	~UniChar();

private:
	UniCharPrivate *priv;
public:
	explicit UniChar(char ascii);
	explicit UniChar(const char * utf8);

	const UniChar & operator=(char);
	const UniChar & operator=(int);
	const UniChar & operator=(long int);
	const UniChar & operator=(long long int);
	const UniChar & operator=(unsigned);
	const UniChar & operator=(long unsigned);
	const UniChar & operator=(long long unsigned);

	long int value() const;

	const String & utf8() const;

	bool isEqual(const char & b) const;
	bool isEqual(const UniChar & b) const;

	void toLower();
	void toUpper();

	int digitValue() const;
	double numericValue() const;

	void name(String & str) const;
	//void isoComment(String & str) const;

	bool isIdStarter() const;
	bool isAlpha() const;
	bool isAlphaNumeric() const;
	bool isDigit() const;
	bool isHexaDigit() const;
	bool isTitleCase() const;
	bool isUpper() const;
	bool isLower() const;
	bool isPunctuation() const;
	bool isGraphic() const;
	bool isBlank() const; /* word separator */
	bool isDefined() const;
	bool isSpace() const;
	bool isControl() const;
	bool isWhiteSpace() const;
};

bool operator==(const UniChar & a, char b);
bool operator==(const UniChar & a, const UniChar & b);
bool operator!=(const UniChar & a, char b);
bool operator!=(const UniChar & a, const UniChar & b);

/*
bool operator<(const UniChar & a, const UniChar & b);
*/

inline String & operator<<(String & lhs, const UniChar & rhs) { lhs << rhs.utf8(); return lhs; }

}

#endif
