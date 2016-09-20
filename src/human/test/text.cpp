/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2016 Csaszar, Peter
 */

#include <string.h>

#include <csjp_text.h>
#include <csjp_test.h>

class TestText
{
public:
	void constructs();
	void compare();
	void assign();
	void prepend();
	void append();
	void insert();
	void erase();
	void write();
	void read();
	void findFirst();
	void findLast();
	void startsWith();
	void endsWith();
	void replace();
	void cutAt();
	void trim();
};

void TestText::constructs()
{
	{
		csjp::Text text;
		VERIFY(text.utf8() == "");
		VERIFY(text == "");
		VERIFY(text.length() == 0);
	}

	{
		csjp::Text text("default");
		VERIFY(text.utf8() == "default");
		VERIFY(text == "default");
		VERIFY(text.length() == 7);
	}

	{
		csjp::Text text("répa");
		VERIFY(text.utf8() == "répa");
		VERIFY(text == "répa");
		VERIFY(text.length() == 4);
	}

	{
		csjp::Text text("répa");
		csjp::Text copied(text);
		VERIFY(copied.utf8() == "répa");
		VERIFY(copied == "répa");
		VERIFY(copied == text);
		VERIFY(copied.length() == text.length());
		VERIFY(copied.length() == 4);
	}

	{
		char *nullStr = NULL;
		csjp::Text text(nullStr);
		VERIFY(text.utf8() == "");
		VERIFY(text == "");
		VERIFY(text.length() == 0);
	}
}

void TestText::compare()
{
	csjp::Text null(NULL);

	{
		csjp::Text a;

		VERIFY(a == null);
		VERIFY(a.compare(null) == 0);
	}

	{
		csjp::Text a("b");

		VERIFY(a.length() == 1);
		VERIFY(a.compare(null) == 1);
		VERIFY(a.compare("b") == 0);
		VERIFY(a.compare("a") == 1);
		VERIFY(a.compare("c") == -1);
	}

	{
		csjp::Text a("alma");

		VERIFY(a.length() == 4);
		VERIFY(a.compare(null) == 1);
		VERIFY(a.compare("alma") == 0);
		VERIFY(a.compare("alaa") == 1);
		VERIFY(a.compare("almaa") == -1);
		VERIFY(a.compare("alm") == 1);
	}

	{
		csjp::Text a("csirkecomb");

		VERIFY(a.compare(null) == 1);
		VERIFY(a.compare("csibe") == 1);
		VERIFY(a.compare("aranyalma") == 1);
		VERIFY(a.compare("habarcs") == -1);
		VERIFY(a.compare("csirke") == 1);
		VERIFY(a.compare("csirkemell") == -1);
	}
}

void TestText::assign()
{
	csjp::Text text;

	char longText[] = "Long enought text, such that cant fit in 16 bytes.";
	char* nullStr = NULL;
	csjp::UniChar c;

	NOEXC_VERIFY(text.assign(nullStr, 0));
	VERIFY(text == "");
	VERIFY(text.length() == 0);

	NOEXC_VERIFY(text.assign(longText));
	VERIFY(text == longText);
	VERIFY(text.length() == strlen(longText));

	csjp::Text text2;
	NOEXC_VERIFY(text2 = text);
	VERIFY(text2 == longText);
	VERIFY(text2 == text);
	VERIFY(text2.length() == text.length());

	text = "short";
	text2 = text;

	VERIFY(text2 == "short");
	VERIFY(text2.length() == 5);
	text2.uniCharAt(c, 4);
	VERIFY(c == 't');

	text = "";

	VERIFY(text.length() == 0);

	text = "longer";

	VERIFY(text == "longer");
	VERIFY(text.length() == 6);
	text.uniCharAt(c, 5);
	VERIFY(c == 'r');

	text.clear();

	VERIFY(text.length() == 0);
}

void TestText::prepend()
{
	csjp::Text text;

	NOEXC_VERIFY(text.prepend(""));
	VERIFY(text.length() == 0);
	VERIFY(text == "");

	NOEXC_VERIFY(text.prepend("testing"));
	VERIFY(text.length() == 7);
	VERIFY(text == "testing");

	csjp::Text app("!");

	NOEXC_VERIFY(text.prepend(app));
	VERIFY(text.length() == 8);
	VERIFY(text == "!testing");
}

void TestText::append()
{
	csjp::Text text;

	NOEXC_VERIFY(text.append(""));
	VERIFY(text.length() == 0);
	VERIFY(text == "");

	NOEXC_VERIFY(text.append("testing"));
	VERIFY(text.length() == 7);
	VERIFY(text == "testing");

	csjp::Text app("!");

	NOEXC_VERIFY(text.append(app));
	VERIFY(text.length() == 8);
	VERIFY(text == "testing!");

	NOEXC_VERIFY(text.append("\0", 1));
	VERIFY(text.length() == 9);
	VERIFY(text.isEqual("testing!\0", 9));
}

void TestText::insert()
{
	csjp::Text text;

	NOEXC_VERIFY(text.insert(0, ""));
	VERIFY(text.length() == 0);
	VERIFY(text == "");

	NOEXC_VERIFY(text.insert(0, "big-bang"));
	VERIFY(text.length() == 8);
	VERIFY(text == "big-bang");

	NOEXC_VERIFY(text.insert(3, "g"));
	VERIFY(text.length() == 9);
	VERIFY(text == "bigg-bang");
}

void TestText::erase()
{
	csjp::Text text;

	NOEXC_VERIFY(text.erase(0, 0));
	VERIFY(text.length() == 0);
	VERIFY(text == "");

	text = "Hello big-big world!";

	NOEXC_VERIFY(text.erase(6, 14));
	VERIFY(text.length() == 12);
	VERIFY(text == "Hello world!");

	NOEXC_VERIFY(text.erase(11, 12));
	VERIFY(text.length() == 11);
	VERIFY(text == "Hello world");

	NOEXC_VERIFY(text.erase(0, 6));
	VERIFY(text.length() == 5);
	VERIFY(text == "world");

	NOEXC_VERIFY(text.erase(0, 0));
	VERIFY(text.length() == 5);
	VERIFY(text == "world");

	NOEXC_VERIFY(text.erase(2, 2));
	VERIFY(text.length() == 5);
	VERIFY(text == "world");
}

void TestText::write()
{
	csjp::Text text("big\0bang", 8);
	char* nullPtr = NULL;

	NOEXC_VERIFY(text.write(0, csjp::Text("")));
	VERIFY(text.length() == 8);
	VERIFY(text.isEqual("big\0bang", 8));

	NOEXC_VERIFY(text.write(0, csjp::Text(nullPtr)));
	VERIFY(text.length() == 8);
	VERIFY(text.isEqual("big\0bang", 8));
/*
#ifndef PERFMODE
	EXC_VERIFY(text.write(0, csjp::Text(nullPtr), 3), csjp::InvalidArgument);
	VERIFY(text.length() == 8);
	VERIFY(text.isEqual("big\0bang", 8));
#endif
*/
	NOEXC_VERIFY(text.write(3, csjp::Text("?")));
	VERIFY(text.length() == 8);
//	DBG("CHECK: '%'", text);
	VERIFY(text == "big?bang");

#ifndef PERFMODE
	EXC_VERIFY(text.write(8, csjp::Text("?")), csjp::InvalidArgument);
	VERIFY(text.length() == 8);
	VERIFY(text == "big?bang");

	EXC_VERIFY(text.write(7, csjp::Text("??")), csjp::InvalidArgument);
	VERIFY(text.length() == 8);
	VERIFY(text == "big?bang");
#endif
}

void TestText::read()
{
	csjp::Text text;
	csjp::Text t;
	csjp::String str;

	VERIFY(text.length() == 0);
	VERIFY(text == "");
	NOEXC_VERIFY(text.read(t, 0, 0));
	VERIFY(t.length() == 0);

	text = ("Hello big-big world!!!");

	NOEXC_VERIFY(text.read(t, 0, 0));
	VERIFY(t.length() == 0);

	NOEXC_VERIFY(text.read(t, 0, 5));
	VERIFY(t.length() == 5);
	VERIFY(t.utf8() == "Hello");

	NOEXC_VERIFY(text.read(t, 14, 22));
	VERIFY(t.length() == 8);
	VERIFY(t == "world!!!");

	NOEXC_VERIFY(text.read(t, 0, 1));
	VERIFY(t.length() == 1);
	VERIFY(t == "H");

	NOEXC_VERIFY(text.read(t, 6, 13));
	VERIFY(t.length() == 7);
	VERIFY(t == "big-big");
}

void TestText::findFirst()
{
	size_t pos = 100;
	csjp::Text text("Medve\0cukorka", 13);
#ifndef PERFMODE
	char* nullStr = NULL;
#endif

	VERIFY(text.length() == 13);
#ifndef PERFMODE
	EXC_VERIFY(text.findFirst(pos, nullStr), csjp::InvalidArgument);
	VERIFY(pos == 100);
	EXC_VERIFY(text.findFirst(pos, ""), csjp::InvalidArgument);
	VERIFY(pos == 100);
	EXC_VERIFY(text.findFirst(pos, "ck", 50), csjp::InvalidArgument);
	VERIFY(pos == 100);
#endif
	VERIFY(text.findFirst(pos, "\0", 1, 0, text.length()));
	VERIFY(pos == 5);
	VERIFY(text.findFirst(pos, "uk"));
	VERIFY(pos == 7);
	VERIFY(text.findFirst(pos, "e\0c", 3, 0, text.length()));
	VERIFY(pos == 4);
	VERIFY(text.findFirst(pos, "\0c", 2, 0, text.length()));
	VERIFY(pos == 5);
	pos = 100;
#ifndef PERFMODE
	NOEXC_VERIFY(text.findFirst(pos, "ec", 2));
	VERIFY(pos == 100);
#endif
	VERIFY(text.findFirst(pos, "k", 9));
	VERIFY(pos == 11);
}

void TestText::findLast()
{
	size_t pos = 100;
	csjp::Text text("Medve\0cukorka", 13);
#ifndef PERFMODE
	char* nullStr = NULL;
#endif

	VERIFY(text.length() == 13);
#ifndef PERFMODE
	EXC_VERIFY(text.findLast(pos, nullStr), csjp::InvalidArgument);
	VERIFY(pos == 100);
	EXC_VERIFY(text.findLast(pos, ""), csjp::InvalidArgument);
	VERIFY(pos == 100);
	EXC_VERIFY(text.findLast(pos, "ck", 0, 50), csjp::InvalidArgument);
	VERIFY(pos == 100);
	EXC_VERIFY(text.findLast(pos, "ck", 50), csjp::InvalidArgument);
	VERIFY(pos == 100);
#endif
	VERIFY(text.findLast(pos, "\0", 1, 0, text.length()));
	VERIFY(pos == 5);
	VERIFY(text.findLast(pos, "uk"));
	VERIFY(pos == 7);
	VERIFY(text.findLast(pos, "e\0c", 3, 0, text.length()));
	VERIFY(pos == 4);
	VERIFY(text.findLast(pos, "\0c", 2, 0, text.length()));
	VERIFY(pos == 5);
	pos = 100;
#ifndef PERFMODE
	NOEXC_VERIFY(text.findLast(pos, "ec", 2, 0, text.length()));
	VERIFY(pos == 100);
#endif
	VERIFY(text.findLast(pos, "k", 1, 0, 12));
	VERIFY(pos == 11);
	VERIFY(text.findLast(pos, "k", 1, 0, 10));
	VERIFY(pos == 8);
}

void TestText::startsWith()
{
	char* nullStr = NULL;
	csjp::Text text("Medve cukorka", 13);

	VERIFY(text.length() == 13);
#ifndef PERFMODE
	EXC_VERIFY(text.startsWith(nullStr, 1), csjp::InvalidArgument);
#endif
	VERIFY(text.startsWith(nullStr, 0) == true);

	text = "";
	VERIFY(text.length() == 0);
	VERIFY(text.startsWith("cukor", 5) == false);

	text = "Medve cukorka";
	VERIFY(text.length() == 13);

	VERIFY(text.startsWith("Medve cukorkak", 14) == false);

	VERIFY(text.startsWith("cukorka", 7) == false);
	VERIFY(text.startsWith("Medvek", 6) == false);
	VERIFY(text.startsWith("Medve", 5) == true);
	VERIFY(text.startsWith("Medve cukorka", 13) == true);
}

void TestText::endsWith()
{
	char* nullStr = NULL;
	csjp::Text text("Medve cukorka", 13);

	VERIFY(text.length() == 13);
#ifndef PERFMODE
	EXC_VERIFY(text.endsWith(nullStr, 1), csjp::InvalidArgument);
#endif
	VERIFY(text.endsWith(nullStr, 0) == true);

	text = "";
	VERIFY(text.length() == 0);
	VERIFY(text.endsWith("cukor", 5) == false);

	text = "Medve cukorka";
	VERIFY(text.length() == 13);

	VERIFY(text.endsWith("Medve cukorkak", 14) == false);

	VERIFY(text.endsWith("Medve", 5) == false);
	VERIFY(text.endsWith("torka", 5) == false);
	VERIFY(text.endsWith("korka", 5) == true);
	VERIFY(text.endsWith("Medve cukorka", 13) == true);
}

void TestText::replace()
{
	csjp::Text text("Medve\0cukorka", 13);
#ifndef PERFMODE
	char* nullStr = NULL;
#endif

	/* replace to same length */
	VERIFY(text.length() == 13);
#ifndef PERFMODE
	EXC_VERIFY(text.replace(nullStr, "kukutyin"), csjp::InvalidArgument);
	EXC_VERIFY(text.replace("cukorka", "kukutyin", 50), csjp::InvalidArgument);
	EXC_VERIFY(text.replace("cukorka", "kukac", 10, 20), csjp::InvalidArgument);
	EXC_VERIFY(text.replace("cukorka", "kukac", 5, 5), csjp::InvalidArgument);
	EXC_VERIFY(text.replace("cukor", nullStr, 5, 5), csjp::InvalidArgument);
#endif
	NOEXC_VERIFY(text.replace("\0", 1, " ", 1));
	VERIFY(text.length() == 13);
	VERIFY(text == "Medve cukorka");
	NOEXC_VERIFY(text.replace("cukor", "kukac"));
	VERIFY(text == "Medve kukacka");
	text = "Medve kupacka";
	NOEXC_VERIFY(text.replace("k", "?"));
	VERIFY(text == "Medve ?upac?a");

	/* replace to longer */
	text.assign("Medve kukacka", 13);
	text = "Medve kuppacka";
	NOEXC_VERIFY(text.replace("k", "??"));
	NOEXC_VERIFY(text == "Medve ??uppac??a");
	NOEXC_VERIFY(text.replace("M", "??"));
	NOEXC_VERIFY(text == "??edve ??uppac??a");
	NOEXC_VERIFY(text.replace("a", "____"));
	NOEXC_VERIFY(text == "??edve ??upp____c??____");

	/* replace to shorter */
	text.assign("Medve kukacka");
	text = "Medve ku_cka";

	text.assign("Medve kukacka");
	NOEXC_VERIFY(text.replace("ka", "_", 7));
	NOEXC_VERIFY(text == "Medve ku_c_");

	text.assign("Medve kukacka");
	NOEXC_VERIFY(text.replace("k", ""));
	NOEXC_VERIFY(text == "Medve uaca");
	NOEXC_VERIFY(text.replace("M", " M"));
	NOEXC_VERIFY(text == " Medve uaca");
	NOEXC_VERIFY(text.replace("a", ""));
	NOEXC_VERIFY(text == " Medve uc");
}

void TestText::cutAt()
{
	csjp::Text text;
//	csjp::String str;

	text.assign("Maci Laci");
	VERIFY(text.length() == 9);

#ifndef PERFMODE
	EXC_VERIFY(text.cutAt(10), csjp::InvalidArgument);
#endif

	NOEXC_VERIFY(text.cutAt(9));
	VERIFY(text == "Maci Laci");
	VERIFY(text.length() == 9);

	NOEXC_VERIFY(text.cutAt(4));
	VERIFY(text == "Maci");
	VERIFY(text.length() == 4);

	NOEXC_VERIFY(text.cutAt(0));
	VERIFY(text == "");
	VERIFY(text.length() == 0);
}

void TestText::trim()
{
	csjp::Text text;

	text.assign(" \tMaci Laci\t ");
	VERIFY(text.length() == 13);

	NOEXC_VERIFY(text.trim());
	VERIFY(text == "Maci Laci");
}

TEST_INIT(Text)

	TEST_RUN(constructs);
	TEST_RUN(compare);
	TEST_RUN(assign);
	TEST_RUN(prepend);
	TEST_RUN(append);
	TEST_RUN(insert);
	TEST_RUN(erase);
	TEST_RUN(write);
	TEST_RUN(read);
	TEST_RUN(findFirst);
	TEST_RUN(findLast);
	TEST_RUN(startsWith);
	TEST_RUN(endsWith);
	TEST_RUN(replace);
	TEST_RUN(cutAt);
	TEST_RUN(trim);

TEST_FINISH(Text)
