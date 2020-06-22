/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011 Csaszar, Peter
 */

#ifndef DEBUG
#define DEBUG
#endif

#include <stdlib.h>
#include <string.h>

#include <csjp_string.h>
#include <csjp_test.h>

class TestString
{
public:
	void constructs();
	void rangeForLoop();
	void compare();
	void findFirst();
	void findFirstOf();
	void findFirstNotOf();
	void findLast();
	void findLastOf();
	void findLastNotOf();
	void contains();
	void startsWith();
	void endsWith();
	void count();
	void capacity();
	void fill();
	void assign();
	void shiftForward();
	void shiftBackward();
	void prepend();
	void append();
	void print();
	void insert();
	void erase();
	void write();
	void read();
	void replace();
	void chopFront();
	void chopBack();
	void cutAt();
	void trim();
	void trimFront();
	void trimBack();
	void adopt();
	void swap();
	void appendOperator();
	void conversionOperator();
	void lowerUpper();
	void exceptionLastMessage();
	void base64();
};

void TestString::constructs()
{
	TESTSTEP("Default construction");

	csjp::String def;

	VERIFY(def.length == 0);
	VERIFY(def.capacity() == 0);
	VERIFY(def.c_str() == 0);

	TESTSTEP("Construction by null c string");

	char *nullStr = NULL;
	csjp::String nullTest(nullStr);

	VERIFY(0 == nullTest.capacity());
	VERIFY(nullTest.length == 0);
	VERIFY(nullTest.c_str() == 0);

	TESTSTEP("Construction by c string");

	csjp::String first("default");

	VERIFY(first == "default");

	TESTSTEP("Copy construction");

	csjp::String copied(first);

	VERIFY(copied == "default");
	VERIFY(copied == first);

	TESTSTEP("Construction by AStr string");

	csjp::Str str("str string");
	csjp::AStr & astr = str;
	csjp::String second(astr);

	VERIFY(second == "str string");
/*
	TESTSTEP("Construction by format string");

	csjp::String catftest("Number: % is % as boolean", 45, true);

	VERIFY(catftest == "Number: 45 is 0 as boolean");
*/
}

void TestString::rangeForLoop()
{
	TESTSTEP("Range based for loop");

	csjp::String str("default");
	VERIFY(str == "default");

	for(auto & c : str)
		c = 'a';

	VERIFY(str == "aaaaaaa");
}

void TestString::compare()
{
	char *null = NULL;

	TESTSTEP("null string");
	{
		csjp::String a(null);

		VERIFY(a.compare(null) == 0);
#ifndef PERFMODE
		EXC_VERIFY(a.compare(null, 1), csjp::InvalidArgument);
#endif
		VERIFY(a.compare("") == 0);
	}

	TESTSTEP("one character long string");
	{
		csjp::String a("b");

		VERIFY(a.length == 1);
		VERIFY(a.compare(null) == 1);
#ifndef PERFMODE
		EXC_VERIFY(a.compare(null, 1), csjp::InvalidArgument);
#endif
		//DBG("Compare: '%' ? '%' -> %", a, "", a.compare(""));
		VERIFY(a.compare("") == 1);
		VERIFY(a.compare("b") == 0);
		VERIFY(a.compare("a") == 1);
		VERIFY(a.compare("c") == -1);
	}

	TESTSTEP("multiple character long string: alma");
	{
		csjp::String a("alma");

		VERIFY(a.length == 4);
		VERIFY(a.compare(null) == 1);
#ifndef PERFMODE
		EXC_VERIFY(a.compare(null, 1), csjp::InvalidArgument);
#endif
		VERIFY(a.compare("") == 1);
		VERIFY(a.compare("alma") == 0);
		VERIFY(a.compare("alaa") == 1);
		VERIFY(a.compare("almaa") == -1);
		VERIFY(a.compare("alm") == 1);
	}

	TESTSTEP("multiple character long string: csirkecomb");
	{
		csjp::String a("csirkecomb");

		VERIFY(a.compare(null) == 1);
#ifndef PERFMODE
		EXC_VERIFY(a.compare(null, 1), csjp::InvalidArgument);
#endif
		VERIFY(a.compare("") == 1);
		VERIFY(a.compare("csibe") == 1);
		VERIFY(a.compare("aranyalma") == 1);
		VERIFY(a.compare("habarcs") == -1);
		VERIFY(a.compare("csirke") == 1);
		VERIFY(a.compare("csirkemell") == -1);
	}
}

void TestString::findFirst()
{
	size_t pos = 100;
	csjp::String str;
	str.assign("Medve\0cukorka", 13);
#ifndef PERFMODE
	char* nullStr = NULL;
#endif

	VERIFY(str.length == 13);
#ifndef PERFMODE
	EXC_VERIFY(str.findFirst(pos, nullStr), csjp::InvalidArgument);
	VERIFY(pos == 100);
	VERIFY(!str.findFirst(pos, ""));
	VERIFY(pos == 100);
	EXC_VERIFY(str.findFirst(pos, "ck", 50), csjp::InvalidArgument);
	VERIFY(pos == 100);
#endif
	VERIFY(str.findFirst(pos, "\0", 1, 0, str.length));
	VERIFY(pos == 5);
	VERIFY(str.findFirst(pos, "uk"));
	VERIFY(pos == 7);
	VERIFY(str.findFirst(pos, "e\0c", 3, 0, str.length));
	VERIFY(pos == 4);
	VERIFY(str.findFirst(pos, "\0c", 2, 0, str.length));
	VERIFY(pos == 5);
	pos = 100;
	VERIFY(str.findFirst(pos, "ec", 2) == false);
	VERIFY(pos == 100);
	VERIFY(str.findFirst(pos, "k", 9));
	VERIFY(pos == 11);
}

void TestString::findFirstOf()
{
	size_t pos = 100;
	csjp::String str;
	str.assign("Medve\0cukorka", 13);
#ifndef PERFMODE
	char* nullStr = NULL;
#endif

	VERIFY(str.length == 13);
#ifndef PERFMODE
	EXC_VERIFY(str.findFirstOf(pos, nullStr), csjp::InvalidArgument);
	VERIFY(pos == 100);
	EXC_VERIFY(str.findFirstOf(pos, ""), csjp::InvalidArgument);
	VERIFY(pos == 100);
#endif
	VERIFY(str.findFirstOf(pos, "\0", 1, 0, str.length));
	VERIFY(pos == 5);
	pos = 100;
#ifndef PERFMODE
	EXC_VERIFY(str.findFirstOf(pos, "ck", 2, 50, str.length), csjp::InvalidArgument);
	VERIFY(pos == 100);
#endif
	VERIFY(str.findFirstOf(pos, "ck"));
	VERIFY(pos == 6);
	VERIFY(str.findFirstOf(pos, "ck", 2, 7, str.length));
	VERIFY(pos == 8);
}

void TestString::findFirstNotOf()
{
	size_t pos = 100;
	csjp::String str;
	str.assign("Medve\0cukorka", 13);
#ifndef PERFMODE
	char* nullStr = NULL;
#endif

	VERIFY(str.length == 13);
#ifndef PERFMODE
	EXC_VERIFY(str.findFirstNotOf(pos, nullStr), csjp::InvalidArgument);
	VERIFY(pos == 100);
	EXC_VERIFY(str.findFirstNotOf(pos, ""), csjp::InvalidArgument);
	VERIFY(pos == 100);
#endif
	VERIFY(str.findFirstNotOf(pos, "\0", 1, 0, str.length));
	VERIFY(pos == 0);
	pos = 100;
#ifndef PERFMODE
	EXC_VERIFY(str.findFirstNotOf(pos, "Medv", 50), csjp::InvalidArgument);
	VERIFY(pos == 100);
#endif
	VERIFY(str.findFirstNotOf(pos, "Medv"));
	VERIFY(pos == 5);
	VERIFY(str.findFirstNotOf(pos, "kor", 8));
	VERIFY(pos == 12);
}

void TestString::findLast()
{
	size_t pos = 100;
	csjp::String str;
	str.assign("Medve\0cukorka", 13);
#ifndef PERFMODE
	char* nullStr = NULL;
#endif

	VERIFY(str.length == 13);
#ifndef PERFMODE
	EXC_VERIFY(str.findLast(pos, nullStr), csjp::InvalidArgument);
	VERIFY(pos == 100);
	EXC_VERIFY(str.findLast(pos, ""), csjp::InvalidArgument);
	VERIFY(pos == 100);
	EXC_VERIFY(str.findLast(pos, "ck", 50), csjp::InvalidArgument);
	VERIFY(pos == 100);
#endif
	VERIFY(str.findLast(pos, "\0", 1, str.length));
	VERIFY(pos == 5);
	VERIFY(str.findLast(pos, "uk"));
	VERIFY(pos == 7);
	VERIFY(str.findLast(pos, "e\0c", 3, str.length));
	VERIFY(pos == 4);
	VERIFY(str.findLast(pos, "\0c", 2, str.length));
	VERIFY(pos == 5);
	pos = 100;
	NOEXC_VERIFY(str.findLast(pos, "ec", 2, str.length));
	VERIFY(pos == 100);
	VERIFY(str.findLast(pos, "k", 1, 12));
	VERIFY(pos == 11);
	VERIFY(str.findLast(pos, "k", 1, 10));
	VERIFY(pos == 8);
}

void TestString::findLastOf()
{
	size_t pos = 100;
	csjp::String str;
	str.assign("Medve\0cukorka", 13);
#ifndef PERFMODE
	char* nullStr = NULL;
#endif

	VERIFY(str.length == 13);
#ifndef PERFMODE
	EXC_VERIFY(str.findLastOf(pos, nullStr), csjp::InvalidArgument);
	VERIFY(pos == 100);
	EXC_VERIFY(str.findLastOf(pos, ""), csjp::InvalidArgument);
	VERIFY(pos == 100);
#endif
	VERIFY(str.findLastOf(pos, "\0", 1, str.length));
	VERIFY(pos == 5);
	pos = 100;
#ifndef PERFMODE
	EXC_VERIFY(str.findLastOf(pos, "ck", 2, 50), csjp::InvalidArgument);
	VERIFY(pos == 100);
#endif
	VERIFY(str.findLastOf(pos, "ck"));
	VERIFY(pos == 11);
	VERIFY(str.findLastOf(pos, "ck", 2, 10));
	VERIFY(pos == 8);
	VERIFY(str.findLastOf(pos, "ck", 2, 7));
	VERIFY(pos == 6);
}

void TestString::findLastNotOf()
{
	size_t pos = 100;
	csjp::String str;
	str.assign("Medve\0cukorka", 13);
#ifndef PERFMODE
	char* nullStr = NULL;
#endif

	VERIFY(str.length == 13);
	VERIFY(str.findLastNotOf(pos, nullStr));
	VERIFY(pos == 12);
	pos = 100;
	VERIFY(str.findLastNotOf(pos, ""));
	VERIFY(pos == 12);
	pos = 100;
	VERIFY(str.findLastNotOf(pos, "\0", 1, 6));
	VERIFY(pos == 4);
	pos = 100;
#ifndef PERFMODE
	EXC_VERIFY(str.findLastNotOf(pos, "orka", 50), csjp::InvalidArgument);
	VERIFY(pos == 100);
#endif
	VERIFY(str.findLastNotOf(pos, "orka"));
	VERIFY(pos == 7);
}

void TestString::contains()
{
	csjp::String str("maki majom");

	VERIFY(!str.contains("im"));
	VERIFY(str.contains("maj"));
	VERIFY(!str.contains(""));

	str.clear();
	VERIFY(!str.contains("maj"));
	VERIFY(!str.contains(""));
}

void TestString::startsWith()
{
	char* nullStr = NULL;

	{
		csjp::String str("Medve cukorka");

		VERIFY(str.length == 13);
#ifndef PERFMODE
		EXC_VERIFY(str.startsWith(nullStr, 1), csjp::InvalidArgument);
#endif
		VERIFY(str.startsWith(nullStr, 0) == true);
		VERIFY(str.startsWith(nullStr) == true);
	}

	{
		csjp::String str("");
		VERIFY(str.length == 0);
		VERIFY(str.startsWith("cukor") == false);
	}

	{
		csjp::String str("Medve cukorka");
		VERIFY(str.length == 13);

		VERIFY(str.startsWith("Medve cukorkak") == false);

		VERIFY(str.startsWith("cukorka") == false);
		VERIFY(str.startsWith("Medvek") == false);
		VERIFY(str.startsWith("Medve") == true);
		VERIFY(str.startsWith("Medve cukorka") == true);
	}
}

void TestString::endsWith()
{
	char* nullStr = NULL;

	{
		csjp::String str("Medve cukorka");

		VERIFY(str.length == 13);
#ifndef PERFMODE
		EXC_VERIFY(str.endsWith(nullStr, 1), csjp::InvalidArgument);
#endif
		VERIFY(str.endsWith(nullStr, 0) == true);
		VERIFY(str.endsWith(nullStr) == true);
	}

	{
		csjp::String str("");
		VERIFY(str.length == 0);
		VERIFY(str.endsWith("cukor") == false);
	}

	{
		csjp::String str("Medve cukorka");
		VERIFY(str.length == 13);

		VERIFY(str.endsWith("Medve cukorkak") == false);

		VERIFY(str.endsWith("Medve") == false);
		VERIFY(str.endsWith("torka") == false);
		VERIFY(str.endsWith("korka") == true);
		VERIFY(str.endsWith("Medve cukorka") == true);
	}
}

void TestString::count()
{
//	char* nullStr = NULL;
	csjp::String str("Medve cukorka");

#ifndef PERFMODE
//	EXC_VERIFY(str.count(nullStr), csjp::InvalidArgument);
#endif

	VERIFY(str.length == 13);
#ifndef PERFMODE
	EXC_VERIFY(str.count("cukor", 50), csjp::InvalidArgument);
	EXC_VERIFY(str.count("cukor", 10, 20), csjp::InvalidArgument);
	EXC_VERIFY(str.count("gumicukorka", 7, 5), csjp::InvalidArgument);
#endif
	VERIFY(str.count("gumicukorka", 11, 7, str.length) == 0);
	VERIFY(str.count("cukor") == 1);
	VERIFY(str.count("k") == 2);
	VERIFY(str.count("k", 9) == 1);
	VERIFY(str.count("k", 9, 11) == 0);
}

void TestString::capacity()
{
	csjp::String str((size_t)0);

	VERIFY(str.capacity() == 0);

	NOEXC_VERIFY(str.setCapacity(12));

	VERIFY(str.capacity() == 12);

	NOEXC_VERIFY(str.extendCapacity(12));

	VERIFY(12 < str.capacity());
}

void TestString::fill()
{
	TESTSTEP("init");
	csjp::String str;

	TESTSTEP("Fill with 0 length");
	NOEXC_VERIFY(str.fill(' ', 0));
	VERIFY(str.length == 0);
	VERIFY(str == "");

	TESTSTEP("Fill with 1 length");
	NOEXC_VERIFY(str.fill(' ', 1));
	VERIFY(str.length == 1);
	VERIFY(str == " ");

	TESTSTEP("Fill with 2 length");
	NOEXC_VERIFY(str.fill('-', 2));
	VERIFY(str.length == 2);
	VERIFY(str == "--");

	TESTSTEP("Fille with 0 length");
	NOEXC_VERIFY(str.fill('_', 0));
	VERIFY(str.length == 0);
	VERIFY(str == "");
}

void TestString::assign()
{
	csjp::String str;

	char longText[] = "Long enough text, such that cant fit in 16 bytes.";
	char* nullStr = NULL;

	NOEXC_VERIFY(str.assign(nullStr, 0));
	VERIFY(str == "");
	VERIFY(str.length == 0);

	NOEXC_VERIFY(str.assign(longText));
	VERIFY(str == longText);
	VERIFY(str.length == strlen(longText));

	csjp::String str2(str);

	NOEXC_VERIFY(str2.assign(str));
	VERIFY(str2 == longText);
	VERIFY(str2 == str);
	VERIFY(str2.length == str.length);

	str = "short";
	str2.assign(str);

	VERIFY(str2 == "short");
	VERIFY(str2.length == 5);
	VERIFY(str2[4] == 't');

	str = "";

	VERIFY(str.length == 0);
	VERIFY(str.c_str() != 0);
	VERIFY(str[0] == 0);

	str = "longer";

	VERIFY(str == "longer");
	VERIFY(str.length == 6);
	VERIFY(str[5] == 'r');

	str.clear();

	VERIFY(str.length == 0);
	VERIFY(str.capacity() == 0);
}

void TestString::shiftForward()
{
	csjp::String str;

	str = "Hello big-big world!!!";

	NOEXC_VERIFY(str.shiftForward(10, 22, 4));
	DBG("Shifted: %", str);
	VERIFY(str.length == 18);
	VERIFY(str.c_str() != 0);
	VERIFY(str[18] == 0);
	VERIFY(str == "Hello big world!!!");

	NOEXC_VERIFY(str.shiftForward(15, 17, 9));
	DBG("Shifted: %", str);
	VERIFY(str.length == 18);
	VERIFY(str.c_str() != 0);
	VERIFY(str[18] == 0);
	VERIFY(str == "Hello !!g world!!!");

	NOEXC_VERIFY(str.shiftForward(10, 18, 10));
	DBG("Shifted: %", str);
	VERIFY(str.length == 8);
	VERIFY(str.c_str() != 0);
	VERIFY(str[8] == 0);
	VERIFY(str == "world!!!");

	NOEXC_VERIFY(str.shiftForward(5, 8, 0));
	DBG("Shifted: %", str);
	VERIFY(str.length == 8);
	VERIFY(str.c_str() != 0);
	VERIFY(str[8] == 0);
	VERIFY(str == "world!!!");
}

void TestString::shiftBackward()
{
	csjp::String str;

	str = "Hello big-big world!!!";

	NOEXC_VERIFY(str.shiftBackward(0, 6, 4));
	DBG("Shifted: %", str);
	VERIFY(str.length == 22);
	VERIFY(str.c_str() != 0);
	VERIFY(str[22] == 0);
	VERIFY(str == "HellHello big world!!!");

	str = "Hello big-big world!!!";

	NOEXC_VERIFY(str.shiftBackward(13, 14, 8));
	DBG("Shifted: %", str);
	VERIFY(str.length == 22);
	VERIFY(str.c_str() != 0);
	VERIFY(str[22] == 0);
	VERIFY(str == "Hello big-big world!! ");

	str = "Hello big-big world!!!";

	NOEXC_VERIFY(str.shiftBackward(5, 22, 14));
	DBG("Shifted: %", str);
	VERIFY(str.length == 36);
	VERIFY(str.c_str() != 0);
	VERIFY(str[36] == 0);
	VERIFY(str == "Hello big-big world big-big world!!!");

	str = "Hello world!!";

	NOEXC_VERIFY(str.shiftBackward(12, 13, 1));
	DBG("Shifted: %", str);
	VERIFY(str.length == 14);
	VERIFY(str.c_str() != 0);
	VERIFY(str[14] == 0);
	VERIFY(str == "Hello world!!!");
}

void TestString::prepend()
{
	csjp::String str;

	NOEXC_VERIFY(str.prepend(""));
	VERIFY(str.length == 0);
	VERIFY(str.c_str() != 0);
	VERIFY(str[0] == 0);
	VERIFY(str == "");

	NOEXC_VERIFY(str.prepend("testing"));
	VERIFY(str.length == 7);
	VERIFY(str.c_str() != 0);
	VERIFY(str[7] == 0);
	VERIFY(str == "testing");

	csjp::String app("!");

	NOEXC_VERIFY(str.prepend(app));
	VERIFY(str.length == 8);
	VERIFY(str.c_str() != 0);
	VERIFY(str[8] == 0);
	VERIFY(str == "!testing");

	NOEXC_VERIFY(str.prepend('\0'));
	VERIFY(str.length == 9);
	VERIFY(str.c_str() != 0);
	VERIFY(str[0] == 0);
	VERIFY(str[9] == 0);
	VERIFY(str.isEqual("\0!testing", 9));
}

void TestString::append()
{
	csjp::String str;

	NOEXC_VERIFY(str.append(""));
	VERIFY(str.length == 0);
	VERIFY(str.c_str() == 0);
	VERIFY(str == "");

	NOEXC_VERIFY(str.append("testing"));
	VERIFY(str.length == 7);
	VERIFY(str.c_str() != 0);
	VERIFY(str[7] == 0);
	VERIFY(str == "testing");

	csjp::String app("!");

	NOEXC_VERIFY(str.append(app));
	VERIFY(str.length == 8);
	VERIFY(str.c_str() != 0);
	VERIFY(str[8] == 0);
	VERIFY(str == "testing!");

	NOEXC_VERIFY(str.append('\0'));
	VERIFY(str.length == 9);
	VERIFY(str.c_str() != 0);
	VERIFY(str[8] == 0);
	VERIFY(str[9] == 0);
	VERIFY(str.isEqual("testing!\0", 9));

	TESTSTEP("long double to string");
	long double d;

	str.chop();
	d = -.987654321098765432109876543210;
	NOEXC_VERIFY(str.append(d, 15));
	//printf("got: '%s'\n", str.c_str());
	VERIFY(str == "-0.987654321098765");

	str.chop();
	d = 9876543210.9876543210;
	NOEXC_VERIFY(str.append(d, 5));
	//printf("got: '%s'\n", str.c_str());
	VERIFY(str == "9876543210.98765");

	str.chop();
	d = 9876543210;
	NOEXC_VERIFY(str.append(d, 5));
	//printf("got: '%s'\n", str.c_str());
	VERIFY(str == "9876543210.00000");

	TESTSTEP("long long unsigned to string");
	long long unsigned u;

	str.chop();
	u = 987654321;
	NOEXC_VERIFY(str.append(u));
	printf("got: '%s'\n", str.c_str());
	VERIFY(str == "987654321");

	TESTSTEP("long long int to string");
	long long int i;

	str.chop();
	i = -987654321;
	NOEXC_VERIFY(str.append(i));
	printf("got: '%s'\n", str.c_str());
	VERIFY(str == "-987654321");
}

void TestString::print()
{
	csjp::String str;

	NOEXC_VERIFY(str.appendPrintf(NULL));
	VERIFY(str.length == 0);
	VERIFY(str.c_str() == 0);
	VERIFY(str == "");

	/* Compiler warning: zero-length gnu_printf format string [-Werror=format-zero-length] */
/*	NOEXC_VERIFY(str.appendPrintf(""));
	VERIFY(str.length == 0);
	VERIFY(str.c_str() != 0);
	VERIFY(str[0] == 0);*/

	NOEXC_VERIFY(str.appendPrintf("first"));
	VERIFY(str.length == 5);
	VERIFY(str.c_str() != 0);
	VERIFY(str[5] == 0);
	VERIFY(str == "first");

	const char * format = " %s";
	NOEXC_VERIFY(str.appendPrintf(format, "second"));

	VERIFY(str.length == 12);
	VERIFY(str.c_str() != 0);
	VERIFY(str[12] == 0);
	VERIFY(str == "first second");

	/* Compiler warning: zero-length gnu_printf format string [-Werror=format-zero-length] */
/*	NOEXC_VERIFY(str.appendPrintf(""));
	VERIFY(str.length == 12);
	VERIFY(str.c_str() != 0);
	VERIFY(str[12] == 0);
	VERIFY(str == "first second");*/
}

void TestString::insert()
{
	csjp::String str;

	NOEXC_VERIFY(str.insert(0, ""));
	VERIFY(str.length == 0);
	VERIFY(str.c_str() == 0);
	VERIFY(str == "");

	NOEXC_VERIFY(str.insert(0, "big-bang"));
	VERIFY(str.length == 8);
	VERIFY(str.c_str() != 0);
	VERIFY(str[8] == 0);
	VERIFY(str == "big-bang");

	NOEXC_VERIFY(str.insert(3, "g"));
	VERIFY(str.length == 9);
	VERIFY(str.c_str() != 0);
	VERIFY(str[9] == 0);
	VERIFY(str == "bigg-bang");
}

void TestString::erase()
{
	csjp::String str;

	NOEXC_VERIFY(str.erase(0, 0));
	VERIFY(str.length == 0);
	VERIFY(str.c_str() == 0);
	VERIFY(str == "");

	str = "Hello big-big world!";

	NOEXC_VERIFY(str.erase(6, 14));
	VERIFY(str.length == 12);
	VERIFY(str.c_str() != 0);
	VERIFY(str[12] == 0);
	VERIFY(str == "Hello world!");

	NOEXC_VERIFY(str.erase(11, 12));
	VERIFY(str.length == 11);
	VERIFY(str.c_str() != 0);
	VERIFY(str[11] == 0);
	VERIFY(str == "Hello world");

	NOEXC_VERIFY(str.erase(0, 6));
	VERIFY(str.length == 5);
	VERIFY(str.c_str() != 0);
	VERIFY(str[5] == 0);
	VERIFY(str == "world");

	NOEXC_VERIFY(str.erase(0, 0));
	VERIFY(str.length == 5);
	VERIFY(str.c_str() != 0);
	VERIFY(str[5] == 0);
	VERIFY(str == "world");

	NOEXC_VERIFY(str.erase(2, 2));
	VERIFY(str.length == 5);
	VERIFY(str.c_str() != 0);
	VERIFY(str[5] == 0);
	VERIFY(str == "world");
}

void TestString::write()
{
	csjp::String str;
	str.assign("big\0bang", 8);
	char* nullPtr = NULL;

	NOEXC_VERIFY(str.write(0, ""));
	VERIFY(str.length == 8);
	VERIFY(str.c_str() != 0);
	VERIFY(str[8] == 0);
	VERIFY(str.isEqual("big\0bang", 8));

	NOEXC_VERIFY(str.write(0, nullPtr));
	VERIFY(str.length == 8);
	VERIFY(str.c_str() != 0);
	VERIFY(str[8] == 0);
	VERIFY(str.isEqual("big\0bang", 8));

#ifndef PERFMODE
	EXC_VERIFY(str.write(0, nullPtr, 3), csjp::InvalidArgument);
	VERIFY(str.length == 8);
	VERIFY(str.c_str() != 0);
	VERIFY(str[8] == 0);
	VERIFY(str.isEqual("big\0bang", 8));
#endif

	NOEXC_VERIFY(str.write(3, "?"));
	VERIFY(str.length == 8);
	VERIFY(str.c_str() != 0);
	VERIFY(str[8] == 0);
//	DBG("CHECK: '%'", str);
	VERIFY(str == "big?bang");

#ifndef PERFMODE
	EXC_VERIFY(str.write(8, "?"), csjp::InvalidArgument);
	VERIFY(str.length == 8);
	VERIFY(str.c_str() != 0);
	VERIFY(str[8] == 0);
	VERIFY(str == "big?bang");

	EXC_VERIFY(str.write(7, "??"), csjp::InvalidArgument);
	VERIFY(str.length == 8);
	VERIFY(str.c_str() != 0);
	VERIFY(str[8] == 0);
	VERIFY(str == "big?bang");
#endif
}

void TestString::read()
{
	csjp::String text;
	csjp::String str;

	VERIFY(str.length == 0);
	VERIFY(str.c_str() == 0);
	VERIFY(str == "");
	NOEXC_VERIFY(str = text.read(0, 0));
	VERIFY(text.length == 0);
	VERIFY(text.c_str() == 0);
	VERIFY(text == "");

	text = ("Hello big-big world!!!");

	NOEXC_VERIFY(str = text.read(0, 0));
	DBG("str: %", str);
	VERIFY(str.length == 0);
	VERIFY(str.c_str() == 0);
	VERIFY(str == "");

	NOEXC_VERIFY(str = text.read(0, 5));
	VERIFY(str.length == 5);
	VERIFY(str.c_str() != 0);
	VERIFY(str[5] == 0);
	VERIFY(str == "Hello");

	NOEXC_VERIFY(str = text.read(14, 22));
	VERIFY(str.length == 8);
	VERIFY(str.c_str() != 0);
	VERIFY(str[8] == 0);
	VERIFY(str == "world!!!");

	NOEXC_VERIFY(str = text.read(0, 1));
	VERIFY(str.length == 1);
	VERIFY(str.c_str() != 0);
	VERIFY(str[1] == 0);
	VERIFY(str == "H");

	NOEXC_VERIFY(str = text.read(6, 13));
	VERIFY(str.length == 7);
	VERIFY(str.c_str() != 0);
	VERIFY(str[7] == 0);
	VERIFY(str == "big-big");
}

void TestString::replace()
{
#ifndef PERFMODE
	char* nullStr = NULL;
#endif

	csjp::String str;
	str.assign("Medve\0cukorka", 13);

	/* replace to same length */
	VERIFY(str.length == 13);
#ifndef PERFMODE
	EXC_VERIFY(str.replace(nullStr, "kukutyin"), csjp::InvalidArgument);
	EXC_VERIFY(str.replace("cukorka", "kukutyin", 50), csjp::InvalidArgument);
	EXC_VERIFY(str.replace("cukorka", "kukac", 10, 20), csjp::InvalidArgument);
#endif
	NOEXC_VERIFY(str.replace("\0", 1, " ", 1));
	VERIFY(str.length == 13);
	VERIFY(str == "Medve cukorka");

	NOEXC_VERIFY(str.replace("cukorka", "kukac", 5, 5));
	VERIFY(str.length == 13);
	VERIFY(str == "Medve cukorka");

	NOEXC_VERIFY(str.replace("cukor", nullStr, 5, 5));
	VERIFY(str.length == 13);
	VERIFY(str == "Medve cukorka");

	NOEXC_VERIFY(str.replace("cukor", "kukac"));
	VERIFY(str == "Medve kukacka");
	NOEXC_VERIFY(str.replace("k", "p", 7, str.length, 1));
	VERIFY(str == "Medve kupacka");
	NOEXC_VERIFY(str.replace("k", "?"));
	VERIFY(str == "Medve ?upac?a");

	/* replace to longer */
	str.assign("Medve kukacka", 13);
	NOEXC_VERIFY(str.replace("k", "pp", 7, str.length, 1));
	NOEXC_VERIFY(str == "Medve kuppacka");
	NOEXC_VERIFY(str.replace("k", "??"));
	NOEXC_VERIFY(str == "Medve ??uppac??a");
	NOEXC_VERIFY(str.replace("M", "??"));
	NOEXC_VERIFY(str == "??edve ??uppac??a");
	NOEXC_VERIFY(str.replace("a", "____"));
	NOEXC_VERIFY(str == "??edve ??upp____c??____");

	/* replace to shorter */
	str.assign("Medve kukacka");
	NOEXC_VERIFY(str.replace("ka", "_", 7, str.length, 1));
	NOEXC_VERIFY(str == "Medve ku_cka");

	str.assign("Medve kukacka");
	NOEXC_VERIFY(str.replace("ka", "_", 7));
	NOEXC_VERIFY(str == "Medve ku_c_");

	str.assign("Medve kukacka");
	NOEXC_VERIFY(str.replace("k", ""));
	NOEXC_VERIFY(str == "Medve uaca");
	NOEXC_VERIFY(str.replace("M", " M"));
	NOEXC_VERIFY(str == " Medve uaca");
	NOEXC_VERIFY(str.replace("a", ""));
	NOEXC_VERIFY(str == " Medve uc");
}

void TestString::chopFront()
{
	csjp::String str("Maci Laci");

	TESTSTEP("Try to chop more than the length of the string.");
#ifndef PERFMODE
	EXC_VERIFY(str.chopFront(10), csjp::InvalidArgument);
#endif

	TESTSTEP("Chop 0 bytes from front.");
	NOEXC_VERIFY(str.chopFront(0));
	VERIFY(str == "Maci Laci");
	VERIFY(str.length == 9);

	TESTSTEP("Chop 4 bytes from front.");
	NOEXC_VERIFY(str.chopFront(4));
	VERIFY(str == " Laci");
	VERIFY(str.length == 5);

	TESTSTEP("Chop to empty from front.");
	NOEXC_VERIFY(str.chopFront(5));
	VERIFY(str == "");
	VERIFY(str.length == 0);
}

void TestString::chopBack()
{
	csjp::String str("Maci Laci");

	TESTSTEP("Try to chop more than the length of the string.");
#ifndef PERFMODE
	EXC_VERIFY(str.chopBack(10), csjp::InvalidArgument);
#endif

	TESTSTEP("Chop 0 bytes from back.");
	NOEXC_VERIFY(str.chopBack(0));
	VERIFY(str == "Maci Laci");
	VERIFY(str.length == 9);

	TESTSTEP("Chop 4 bytes from back.");
	NOEXC_VERIFY(str.chopBack(4));
	VERIFY(str == "Maci ");
	VERIFY(str.length == 5);

	TESTSTEP("Chop to empty from back.");
	NOEXC_VERIFY(str.chopBack(5));
	VERIFY(str == "");
	VERIFY(str.length == 0);
}

void TestString::cutAt()
{
	csjp::String str("Maci Laci");

#ifndef PERFMODE
	TESTSTEP("Cut at position larger than length.");
	EXC_VERIFY(str.cutAt(10), csjp::InvalidArgument);
#endif

	TESTSTEP("Cut at end.");
	NOEXC_VERIFY(str.cutAt(9));
	VERIFY(str == "Maci Laci");
	VERIFY(str.length == 9);

	TESTSTEP("Cut at inside.");
	NOEXC_VERIFY(str.cutAt(4));
	VERIFY(str == "Maci");
	VERIFY(str.length == 4);

	TESTSTEP("Cut at begin.");
	NOEXC_VERIFY(str.cutAt(0));
	VERIFY(str == "");
	VERIFY(str.length == 0);
}

void TestString::trim()
{
	csjp::String str(" \tMaci Laci\t ");

	NOEXC_VERIFY(str.trim(" \t"));
	//DBG("trim: str: '%' len: %", str, str.length);
	VERIFY(str == "Maci Laci");

	NOEXC_VERIFY(str.trim("MaciL"));
	//DBG("trim: str: '%' len: %", str, str.length);
	VERIFY(str.length == 1);
	VERIFY(str == " ");

	NOEXC_VERIFY(str.trim(" "));
	VERIFY(str.length == 0);
	VERIFY(str == "");
}

void TestString::trimFront()
{
	csjp::String str(" \tMaci Laci\t ");

	NOEXC_VERIFY(str.trimFront(" \t"));
	//DBG("trimFront: str: '%' len: %", str, str.length);
	VERIFY(str == "Maci Laci\t ");

	NOEXC_VERIFY(str.trimFront("MaciL"));
	//DBG("trimFront: str: '%' len: %", str, str.length);
	VERIFY(str == " Laci\t ");

	NOEXC_VERIFY(str.trimFront(" "));
	VERIFY(str == "Laci\t ");
}

void TestString::trimBack()
{
	csjp::String str(" \tMaci Laci\t ");

	NOEXC_VERIFY(str.trimBack(" \t"));
	//DBG("trimBack: str: '%' len: %", str, str.length);
	VERIFY(str == " \tMaci Laci");

	NOEXC_VERIFY(str.trimBack("MaciL"));
	//DBG("trimBack: str: '%' len: %", str, str.length);
	VERIFY(str == " \tMaci ");

	NOEXC_VERIFY(str.trimBack(" "));
	VERIFY(str == " \tMaci");
}

void TestString::adopt()
{
	char* nullStr = NULL;
	csjp::String str;
	char* test = NULL;

	test = (char*)malloc(20);
	strcpy(test, "Test string");

	NOEXC_VERIFY(str.adopt(test));
	VERIFY(test == NULL);
	VERIFY(str.length == 11);
	VERIFY(str == "Test string");

	test = (char*)malloc(8);
	memcpy(test, "Just fit", 8);

	NOEXC_VERIFY(str.adopt(test, 8, 8));
	VERIFY(test == NULL);
	VERIFY(str.length == 8);
	VERIFY(str == "Just fit");

	NOEXC_VERIFY(str.adopt(nullStr));
	VERIFY(str.length == 0);
	VERIFY(str == "");
}

void TestString::swap()
{
	csjp::String adopter;
	size_t cap;
	csjp::String str;

	/* Testing adopting another string. */
	str = "test";
	VERIFY(str == "test");
	VERIFY(str.length == 4);
	cap = str.capacity();
	csjp::swap(adopter, str);

	VERIFY(adopter == "test");
	VERIFY(adopter.length == 4);
	VERIFY(adopter.capacity() == cap);
	VERIFY(str == "");
	VERIFY(str.length == 0);
	VERIFY(str.capacity() == 0);
	VERIFY(str.c_str() == 0);
}

void TestString::appendOperator()
{
	csjp::String str("empty");

	TESTSTEP("append char");
	str << ' ';
	VERIFY(str == "empty ");

	TESTSTEP("append char*");
	str << "string";
	VERIFY(str == "empty string");

	TESTSTEP("append int");
	str << (int)(-1);
	VERIFY(str == "empty string-1");

	TESTSTEP("append long int");
	str << (long int)(-1);
	VERIFY(str == "empty string-1-1");

	TESTSTEP("append long long int");
	str << (long long int)(-1);
	VERIFY(str == "empty string-1-1-1");

	TESTSTEP("append unsigned");
	str << (unsigned)(1);
	VERIFY(str == "empty string-1-1-11");

	TESTSTEP("append long unsigned");
	str << (long unsigned)(1);
	VERIFY(str == "empty string-1-1-111");

	TESTSTEP("append long long unsigned");
	str << (long long unsigned)(1);
	VERIFY(str == "empty string-1-1-1111");

	TESTSTEP("append char");
	str << (char)('a');
	VERIFY(str == "empty string-1-1-1111a");

	TESTSTEP("append unsigned char");
	str << (unsigned char)('a');
	VERIFY(str == "empty string-1-1-1111aa");

}

void TestString::conversionOperator()
{
	csjp::String str("0");
	int i = 0;

	TESTSTEP("convert \"1\" to int");
	str = "1";
	i <<= str;
	VERIFY(i == 1);

	TESTSTEP("convert \"-1\" to int");
	str = "-1";
	i <<= str;
	VERIFY(i == -1);

	TESTSTEP("convert \"0\" to int");
	str = "0";
	i <<= str;
	VERIFY(i == 0);
}

void TestString::lowerUpper()
{
	csjp::String str("aA Zz");

	TESTSTEP("To lower");
	csjp::String toLower(str);
	toLower.lower();
	VERIFY(toLower == "aa zz");

	TESTSTEP("To upper");
	csjp::String toUpper(str);
	toUpper.upper();
	VERIFY(toUpper == "AA ZZ");
}

void TestString::exceptionLastMessage()
{
	TESTSTEP("construct exception");
	csjp::Exception e("test message");

	TESTSTEP("Test last message's value");
	VERIFY(strcmp(e.lastMessage(), "test message") == 0);
}

void TestString::base64()
{
	{
		TESTSTEP("Convert string 'test' to base64 and back.");

		csjp::String test("test");
		csjp::String base64 = test.encodeBase64();
		VERIFY(base64 == "dGVzdA==");

		csjp::String str = base64.decodeBase64();
		VERIFY(str == test);
	}
	{
		TESTSTEP("Convert string 'test!' to base64 and back.");

		csjp::String test("test!");
		csjp::String base64 = test.encodeBase64();
		VERIFY(base64 == "dGVzdCE=");

		csjp::String str = base64.decodeBase64();
		VERIFY(str == test);
	}
	{
		TESTSTEP("Convert string 'árvíztűrőtükörfúrógép' to base64 and back.");

		csjp::String test("árvíztűrőtükörfúrógép");
		csjp::String base64 = test.encodeBase64();
		VERIFY(base64 == "w6FydsOtenTFsXLFkXTDvGvDtnJmw7pyw7Nnw6lw");

		csjp::String str = base64.decodeBase64();
		VERIFY(str == test);
	}
}

TEST_INIT(String)

	TEST_RUN(constructs);
	TEST_RUN(rangeForLoop);
	TEST_RUN(compare);
	TEST_RUN(findFirst);
	TEST_RUN(findFirstOf);
	TEST_RUN(findFirstNotOf);
	TEST_RUN(findLast);
	TEST_RUN(findLastOf);
	TEST_RUN(findLastNotOf);
	TEST_RUN(contains);
	TEST_RUN(startsWith);
	TEST_RUN(endsWith);
	TEST_RUN(count);
	TEST_RUN(capacity);
	TEST_RUN(fill);
	TEST_RUN(assign);
	TEST_RUN(shiftForward);
	TEST_RUN(shiftBackward);
	TEST_RUN(prepend);
	TEST_RUN(append);
	TEST_RUN(print);
	TEST_RUN(insert);
	TEST_RUN(erase);
	TEST_RUN(write);
	TEST_RUN(read);
	TEST_RUN(replace);
	TEST_RUN(chopFront);
	TEST_RUN(chopBack);
	TEST_RUN(cutAt);
	TEST_RUN(trim);
	TEST_RUN(trimFront);
	TEST_RUN(trimBack);
	TEST_RUN(adopt);
	TEST_RUN(swap);
	TEST_RUN(appendOperator);
	TEST_RUN(conversionOperator);
	TEST_RUN(lowerUpper);
	TEST_RUN(exceptionLastMessage);
	TEST_RUN(base64);

TEST_FINISH(String)
