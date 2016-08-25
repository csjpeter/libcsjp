/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2016 Csaszar, Peter
 */

#include <csjp_unichar.h>
#include <csjp_test.h>

class TestUniChar
{
public:
	void basics();
	void utils();

	void info(const csjp::UniChar & c);
};

void TestUniChar::basics()
{
	{
		csjp::UniChar c;
		VERIFY(c.value() == 0);
		VERIFY(c == 0);
	}

	{
		csjp::UniChar c('a');
		VERIFY(c.value() == 97);
		VERIFY(c == 'a');
		VERIFY(c != 'b');
	}

	{
		csjp::UniChar c("a");
		VERIFY(c.value() == 97);
		VERIFY(c.utf8() == "a");
	}

	{
		csjp::UniChar c("ű");
		VERIFY(c.value() == 369);
		VERIFY(c.utf8() == "ű");
		VERIFY(c.utf8() != "á");
	}

	{
		csjp::UniChar a('a');
		VERIFY(a.value() == 97);

		a = 'b';
		VERIFY(a.value() == 98);

		a = (int)99;
		VERIFY(a.value() == 99);
		VERIFY(a == 'c');

		a = (long int)99;
		VERIFY(a.value() == 99);
		VERIFY(a == 'c');

		a = (long long int)99;
		VERIFY(a.value() == 99);
		VERIFY(a == 'c');

		a = (long int)100;
		VERIFY(a.value() == 100);
		VERIFY(a == 'd');

		a = (long unsigned)101;
		VERIFY(a.value() == 101);
		VERIFY(a == 'e');

		a = (long long unsigned)101;
		VERIFY(a.value() == 101);
		VERIFY(a == 'e');

		csjp::UniChar b("ű");
		a = b;
		VERIFY(a.value() == 369);
		VERIFY(a.utf8() == "ű");
	}
}

void TestUniChar::info(const csjp::UniChar & c)
{
	csjp::UniChar up, low;
	csjp::String name, comment;

	up = c;
	up.toUpper();
	low = c;
	low.toLower();
	c.name(name);
	//c.isoComment(comment); FIXME deprecated
	DBG(	"Character: %; Upper: %; Lower: %; "
		"IDStarter: %; digitValue: %; numericValue: %; "
		"Name: %; Iso comment: %",
			c, up, low,
			c.isIdStarter() ? "yes" : "no", c.digitValue(), c.numericValue(),
			name, comment);
}

void TestUniChar::utils()
{
	info(csjp::UniChar("ű"));
	info(csjp::UniChar("$"));
	info(csjp::UniChar("@"));
	info(csjp::UniChar("#"));
	info(csjp::UniChar("€"));
	info(csjp::UniChar("Đ"));
}

TEST_INIT(UniChar)

	TEST_RUN(basics);
	TEST_RUN(utils);

TEST_FINISH(UniChar)
