/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012 Csaszar, Peter
 */

#ifndef DEBUG
#define DEBUG
#endif

#include <csjp_string_chunk.h>
#include <csjp_test.h>

class TestStringChunk
{
public:
	void constructs();
	void compare();
	void read();
	void findFirst();
	void findFirstOf();
	void findFirstNotOf();
	void findLast();
	void findLastOf();
	void findLastNotOf();
	void startsWith();
	void endsWith();
	void count();
	void chopFront();
	void chopBack();
	void cutAt();
	void trim();
	void trimFront();
	void trimBack();
	void split();
};

void TestStringChunk::constructs()
{
	char * null = NULL;

	{
		csjp::StringChunk str(null);

		VERIFY(str.length == 0);
		VERIFY(str.str == 0);
	}

#ifndef PERFMODE
	{
		EXC_VERIFY(csjp::StringChunk str(null, 1), csjp::InvalidArgument);
	}
#endif

	{
		csjp::StringChunk str("nocsak");

		VERIFY(str.length == 6);
		VERIFY(str == "nocsak");
	}

	csjp::StringChunk str("nocsak");

	{
		csjp::StringChunk copy(str, 2, 4);
//		DBG("Copy: '%' Length: %", copy, copy.length);

		VERIFY(copy.length == 2);
		VERIFY(copy == "cs");
	}

	{
		csjp::StringChunk copy(str, 3, 3);

		VERIFY(copy.length == 0);
		VERIFY(copy == "");
	}

	{
		csjp::StringChunk copy(str, 0, 6);

		VERIFY(copy.length == 6);
		VERIFY(copy == "nocsak");
	}

#ifndef PERFMODE
	EXC_VERIFY(csjp::StringChunk copy(str, 8, 4), csjp::InvalidArgument);
	EXC_VERIFY(csjp::StringChunk copy(str, 2, 7), csjp::InvalidArgument);
	EXC_VERIFY(csjp::StringChunk copy(str, 4, 3), csjp::InvalidArgument);
#endif

	{
		csjp::StringChunk copy(str, 2);

		VERIFY(copy.length == 4);
		VERIFY(copy == "csak");
	}

	{
		csjp::StringChunk copy(str, 6);

		VERIFY(copy.length == 0);
		VERIFY(copy == "");
	}

	{
		csjp::StringChunk copy(str);

		VERIFY(copy.length == 6);
		VERIFY(copy == "nocsak");
	}

	{
		csjp::String str("data");

		TESTSTEP("Construct StringChunk from String");
		csjp::StringChunk chunk(str);
		VERIFY(chunk.str == str.c_str());
		VERIFY(chunk.length == str.length);
		VERIFY(chunk == str.c_str());
	}

#ifndef PERFMODE
	EXC_VERIFY(csjp::StringChunk copy(str, 8), csjp::InvalidArgument);
#endif
}

void TestStringChunk::compare()
{
	char *null = NULL;

	{
		csjp::StringChunk a(null);

		VERIFY(a.compare(null) == 0);
#ifndef PERFMODE
		EXC_VERIFY(a.compare(null, 1), csjp::InvalidArgument);
#endif
		VERIFY(a.compare("") == 0);
	}

	{
		csjp::StringChunk a("b");

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

	{
		csjp::StringChunk a("alma");

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

	{
		csjp::StringChunk a("csirkecomb");

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

void TestStringChunk::read()
{
	TESTSTEP("Default construction");
	csjp::StringChunk text;
	csjp::StringChunk str;

	VERIFY(str.length == 0);
	VERIFY(str.str == 0);
	VERIFY(str == "");
	NOEXC_VERIFY(str = text.read(0, 0));
	VERIFY(text.length == 0);
	VERIFY(text.str == 0);
	VERIFY(text == "");

	TESTSTEP("const char* assignment and move copy assignment");
	text = csjp::StringChunk("Hello big-big world!!!");

	TESTSTEP("empty read");
	NOEXC_VERIFY(str = text.read(0, 0));
	DBG("str: %", str);
	VERIFY(str.length == 0);
	VERIFY(str.str == 0);
	VERIFY(str == "");

	TESTSTEP("read from begining");
	NOEXC_VERIFY(str = text.read(0, 5));
	VERIFY(str.length == 5);
	VERIFY(str.str != 0);
	EXC_VERIFY(str[5], csjp::IndexOutOfRange);
	VERIFY(str == "Hello");

	TESTSTEP("ending read");
	NOEXC_VERIFY(str = text.read(14, 22));
	VERIFY(str.length == 8);
	VERIFY(str.str != 0);
	EXC_VERIFY(str[8], csjp::IndexOutOfRange);
	VERIFY(str == "world!!!");

	TESTSTEP("one char read");
	NOEXC_VERIFY(str = text.read(0, 1));
	VERIFY(str.length == 1);
	VERIFY(str.str != 0);
	EXC_VERIFY(str[1], csjp::IndexOutOfRange);
	VERIFY(str == "H");

	TESTSTEP("middle read");
	NOEXC_VERIFY(str = text.read(6, 13));
	VERIFY(str.length == 7);
	VERIFY(str.str != 0);
	EXC_VERIFY(str[7], csjp::IndexOutOfRange);
	VERIFY(str == "big-big");
}

void TestStringChunk::findFirst()
{
	size_t pos = 100;
	csjp::StringChunk str("Medve\0cukorka", 13);
#ifndef PERFMODE
	char* nullStr = NULL;
#endif

	VERIFY(str.length == 13);
#ifndef PERFMODE
	EXC_VERIFY(str.findFirst(pos, nullStr), csjp::InvalidArgument);
	VERIFY(pos == 100);
	EXC_VERIFY(str.findFirst(pos, ""), csjp::InvalidArgument);
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

void TestStringChunk::findFirstOf()
{
	size_t pos = 100;
	csjp::StringChunk str("Medve\0cukorka", 13);
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

void TestStringChunk::findFirstNotOf()
{
	size_t pos = 100;
	csjp::StringChunk str("Medve\0cukorka", 13);
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

void TestStringChunk::findLast()
{
	size_t pos = 100;
	csjp::StringChunk str("Medve\0cukorka", 13);
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

void TestStringChunk::findLastOf()
{
	size_t pos = 100;
	csjp::StringChunk str("Medve\0cukorka", 13);
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

void TestStringChunk::findLastNotOf()
{
	size_t pos = 100;
	csjp::StringChunk str("Medve\0cukorka", 13);
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

void TestStringChunk::startsWith()
{
	char* nullStr = NULL;

	{
		csjp::StringChunk str("Medve cukorka", 13);

		VERIFY(str.length == 13);
#ifndef PERFMODE
		EXC_VERIFY(str.startsWith(nullStr, 1), csjp::InvalidArgument);
#endif
		VERIFY(str.startsWith(nullStr, 0) == true);
		VERIFY(str.startsWith(nullStr) == true);
	}

	{
		csjp::StringChunk str("");
		VERIFY(str.length == 0);
		VERIFY(str.startsWith("cukor") == false);
	}

	{
		csjp::StringChunk str("Medve cukorka");
		VERIFY(str.length == 13);

		VERIFY(str.startsWith("Medve cukorkak") == false);

		VERIFY(str.startsWith("cukorka") == false);
		VERIFY(str.startsWith("Medvek") == false);
		VERIFY(str.startsWith("Medve") == true);
		VERIFY(str.startsWith("Medve cukorka") == true);
	}
}

void TestStringChunk::endsWith()
{
	char* nullStr = NULL;

	{
		csjp::StringChunk str("Medve cukorka", 13);

		VERIFY(str.length == 13);
#ifndef PERFMODE
		EXC_VERIFY(str.endsWith(nullStr, 1), csjp::InvalidArgument);
#endif
		VERIFY(str.endsWith(nullStr, 0) == true);
		VERIFY(str.endsWith(nullStr) == true);
	}

	{
		csjp::StringChunk str("");
		VERIFY(str.length == 0);
		VERIFY(str.endsWith("cukor") == false);
	}

	{
		csjp::StringChunk str("Medve cukorka");
		VERIFY(str.length == 13);

		VERIFY(str.endsWith("Medve cukorkak") == false);

		VERIFY(str.endsWith("Medve") == false);
		VERIFY(str.endsWith("torka") == false);
		VERIFY(str.endsWith("korka") == true);
		VERIFY(str.endsWith("Medve cukorka") == true);
	}
}

void TestStringChunk::count()
{
//	char* nullStr = NULL;
	csjp::StringChunk str("Medve cukorka", 13);

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

void TestStringChunk::chopFront()
{
	csjp::StringChunk str("Maci Laci");

	TESTSTEP("Try to chop more than the length of the string.");
#ifndef PERFMODE
	EXC_VERIFY(str.chopFront(10), csjp::InvalidArgument);
#endif

	TESTSTEP("Chop 0 bytes from front.");
	int i = 0;
	NOEXC_VERIFY(str.chopFront(i));
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

	csjp::StringChunk str2("Maci Laci");
	bool res;

	TESTSTEP("Chop null from string.");
	const char * n = 0;
	NOEXC_VERIFY(res = str2.chopFront(n));
	VERIFY(res);
	VERIFY(str2 == "Maci Laci");
	VERIFY(str2.length == 9);

	TESTSTEP("Chop \"\" from string.");
	NOEXC_VERIFY(res = str2.chopFront(""));
	VERIFY(res);
	VERIFY(str2 == "Maci Laci");
	VERIFY(str2.length == 9);

	TESTSTEP("Chop Laci from string.");
	NOEXC_VERIFY(res = str2.chopFront("Laci"));
	VERIFY(!res);
	VERIFY(str2 == "Maci Laci");
	VERIFY(str2.length == 9);

	TESTSTEP("Chop Maci from string.");
	NOEXC_VERIFY(res = str2.chopFront("Maci"));
	VERIFY(res);
	VERIFY(str2 == " Laci");
	VERIFY(str2.length == 5);
}

void TestStringChunk::chopBack()
{
	csjp::StringChunk str("Maci Laci");

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

void TestStringChunk::cutAt()
{
	csjp::StringChunk str("Maci Laci");

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

void TestStringChunk::trim()
{
	csjp::StringChunk str(" \tMaci Laci\t ");

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

void TestStringChunk::trimFront()
{
	csjp::StringChunk str(" \tMaci Laci\t ");

	NOEXC_VERIFY(str.trimFront(" \t"));
	//DBG("trimFront: str: '%' len: %", str, str.length);
	VERIFY(str == "Maci Laci\t ");

	NOEXC_VERIFY(str.trimFront("MaciL"));
	//DBG("trimFront: str: '%' len: %", str, str.length);
	VERIFY(str == " Laci\t ");

	NOEXC_VERIFY(str.trimFront(" "));
	VERIFY(str == "Laci\t ");
}

void TestStringChunk::trimBack()
{
	csjp::StringChunk str(" \tMaci Laci\t ");

	NOEXC_VERIFY(str.trimBack(" \t"));
	//DBG("trimBack: str: '%' len: %", str), str.length);
	VERIFY(str == " \tMaci Laci");

	NOEXC_VERIFY(str.trimBack("MaciL"));
	//DBG("trimBack: str: '%' len: %", str, str.length);
	VERIFY(str == " \tMaci ");

	NOEXC_VERIFY(str.trimBack(" "));
	VERIFY(str == " \tMaci");
}

void TestStringChunk::split()
{
	const char * s = " \tMaci Laci\t ";
	csjp::StringChunk str(s, strlen(s));
	csjp::Array<csjp::StringChunk> res;

	TESTSTEP("Null pointer as delimiters");
	NOEXC_VERIFY(res = str.split(0, false));
	VERIFY(res.length == 1);
	VERIFY(res[0].str == str.str);
	VERIFY(res[0].length == str.length);

	TESTSTEP("Empty string as delimiters");
	NOEXC_VERIFY(res = str.split("", false));
	VERIFY(res.length == 1);
	VERIFY(res[0].str == str.str);
	VERIFY(res[0].length == str.length);

	TESTSTEP("One character delimiter");
	NOEXC_VERIFY(res = str.split(" ", false));
	VERIFY(res.length == 4);
	VERIFY(res[0].length == 0);
	VERIFY(res[1].length == 5);
	VERIFY(res[2].length == 5);
	VERIFY(res[3].length == 0);

	TESTSTEP("Two character delimiter");
	NOEXC_VERIFY(res = str.split(" \t", false));
	VERIFY(res.length == 6);
	VERIFY(res[0].length == 0);
	VERIFY(res[1].length == 0);
	VERIFY(res[2].length == 4);
	VERIFY(res[3].length == 4);
	VERIFY(res[4].length == 0);
	VERIFY(res[5].length == 0);

	TESTSTEP("Split new empty string");
	csjp::StringChunk str2(0, 0);
	NOEXC_VERIFY(res = str2.split(" ", false));
	VERIFY(res.length == 1);
	VERIFY(res[0].str == str2.str);
	VERIFY(res[0].length == str2.length);

	TESTSTEP("Split zero long empty string");
	csjp::StringChunk str21("", 0);
	NOEXC_VERIFY(res = str21.split(" ", false));
	VERIFY(res.length == 1);
	VERIFY(res[0].str == str21.str);
	VERIFY(res[0].length == str21.length);

	TESTSTEP("Split 1 character long string");
	csjp::StringChunk str22("a", 1);
	NOEXC_VERIFY(res = str22.split(" ", false));
	VERIFY(res.length == 1);
	VERIFY(res[0].str == str22.str);
	VERIFY(res[0].length == str22.length);

	TESTSTEP("Split 1 character long string containing a delimiter");
	csjp::StringChunk str23(" ", 1);
	NOEXC_VERIFY(res = str23.split(" ", false));
	VERIFY(res.length == 2);
	VERIFY(res[0].length == 0);
	VERIFY(res[1].length == 0);


	TESTSTEP("Null pointer as delimiters, avoiding empty result strings");
	NOEXC_VERIFY(res = str.split(0));
	VERIFY(res.length == 1);
	VERIFY(res[0].str == str.str);
	VERIFY(res[0].length == str.length);

	TESTSTEP("Empty string as delimiters, avoiding empty result strings");
	NOEXC_VERIFY(res = str.split(""));
	VERIFY(res.length == 1);
	VERIFY(res[0].str == str.str);
	VERIFY(res[0].length == str.length);

	TESTSTEP("One character delimiter, avoiding empty result strings");
	NOEXC_VERIFY(res = str.split(" "));
	VERIFY(res.length == 2);
	VERIFY(res[0].length == 5);
	VERIFY(res[1].length == 5);

	TESTSTEP("Two character delimiter, avoiding empty result strings");
	NOEXC_VERIFY(res = str.split(" \t"));
	VERIFY(res.length == 2);
	VERIFY(res[0].length == 4);
	VERIFY(res[1].length == 4);

	TESTSTEP("Split new empty string, avoiding empty result strings");
	csjp::StringChunk str3(0, 0);
	NOEXC_VERIFY(res = str3.split(" "));
	VERIFY(res.length == 0);

	TESTSTEP("Split zero long empty string, avoiding empty result strings");
	csjp::StringChunk str31("", 0);
	NOEXC_VERIFY(res = str31.split(" "));
	VERIFY(res.length == 0);

	TESTSTEP("Split 1 character long string, avoiding empty result strings");
	csjp::StringChunk str32("a", 1);
	NOEXC_VERIFY(res = str32.split(" "));
	VERIFY(res.length == 1);
	VERIFY(res[0].str == str32.str);
	VERIFY(res[0].length == str32.length);

	TESTSTEP("Split 1 character long string containing a delimiter, avoiding empty result strings");
	csjp::StringChunk str33(" ", 1);
	NOEXC_VERIFY(res = str33.split(" "));
	VERIFY(res.length == 0);
}

TEST_INIT(StringChunk)

	TEST_RUN(constructs);
	TEST_RUN(compare);
	TEST_RUN(read);
	TEST_RUN(findFirst);
	TEST_RUN(findFirstOf);
	TEST_RUN(findFirstNotOf);
	TEST_RUN(findLast);
	TEST_RUN(findLastOf);
	TEST_RUN(findLastNotOf);
	TEST_RUN(startsWith);
	TEST_RUN(endsWith);
	TEST_RUN(count);
	TEST_RUN(chopFront);
	TEST_RUN(chopBack);
	TEST_RUN(cutAt);
	TEST_RUN(trim);
	TEST_RUN(trimFront);
	TEST_RUN(trimBack);
	TEST_RUN(split);

TEST_FINISH(StringChunk)
