/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2020 Csaszar, Peter
 */

#ifndef DEBUG
#define DEBUG
#endif

#include <csjp_test.h>

#include <csjp_json.h>

class TestJson
{
public:
	void onlyValues();
	void onlyObjects();
	void valuesInObjects();
	void multipleSources();
	void parseItself();
	void array();
};

void TestJson::onlyValues()
{
	csjp::Str data(
			"{ # comment\n"
			"gyümölcs : alma, # comment\n"
			" # comment\n"
			"zöldség : répa"
			"}");
	csjp::Json ot;

	TESTSTEP("Parse data into csjp::Json");
	NOEXC_VERIFY(ot.parse(data));
	VERIFY(ot.size() == 2);
	//VERIFY(ot.has("gyümölcs"));
	VERIFY(ot["gyümölcs"] == csjp::Json::Type::String);
	VERIFY(ot["gyümölcs"] == "alma");
	//VERIFY(ot.has("zöldség"));
	VERIFY(ot["zöldség"] == csjp::Json::Type::String);
	VERIFY(ot["zöldség"] == "répa");

	csjp::String str;

	TESTSTEP("Export csjp::Json into csjp::String");
	NOEXC_VERIFY(str = ot.toString());

	csjp::Json ot2;

	TESTSTEP("Parse new csjp::String back into csjp::Json");
	LOG("%", str);
	NOEXC_VERIFY(ot2.parse(str));
	VERIFY(ot == ot2);
}

void TestJson::onlyObjects()
{
	csjp::Str data(
			"{ # comment\n"
			"\"gyümölcs\" : {}, # comment \n"
			"  # comment \n"
			"\"zöldség\" : {}"
			"}");
	csjp::Json ot;

	TESTSTEP("Parse data into csjp::Json");
	NOEXC_VERIFY(ot.parse(data));
	VERIFY(ot.size() == 2);
	//VERIFY(ot.has("gyümölcs"));
	VERIFY(ot["gyümölcs"] == csjp::Json::Type::Object);
	VERIFY(ot["gyümölcs"].size() == 0);
	//VERIFY(ot.has("zöldség"));
	VERIFY(ot["zöldség"] == csjp::Json::Type::Object);
	VERIFY(ot["zöldség"].size() == 0);

	csjp::String str;
	csjp::Json ot2;

	TESTSTEP("Export csjp::Json into csjp::String");
	NOEXC_VERIFY(str = ot.toString());

	TESTSTEP("Parse new string back into csjp::Json");
	LOG("%", str);
	NOEXC_VERIFY(ot2.parse(str));
	VERIFY(ot == ot2);
}

void TestJson::valuesInObjects()
{
	csjp::Str data(
			"{"
			"\"hami-papi\" : \"fincsi\","
			"\"gyümölcs\" : {"
				"\"alma\" : \"arany\""
			"},"
			"\"zöldség\" : {"
			"},"
			"\"hús\" : {"
				"\"kolbász\" : \"paraszt\""
			"}"
			"}");
	csjp::Json ot;

	TESTSTEP("Parse csjp::Json into csjp::String");
	NOEXC_VERIFY(ot.parse(data));
	VERIFY(ot.size() == 4);
	VERIFY(ot["hami-papi"] == "fincsi");
	//VERIFY(ot.has("gyümölcs"));
	VERIFY(ot["gyümölcs"] == csjp::Json::Type::Object);
	VERIFY(ot["gyümölcs"].size() == 1);
	VERIFY(ot["gyümölcs"]["alma"] == "arany");
	//VERIFY(ot.has("zöldség"));
	VERIFY(ot["zöldség"].size() == 0);
	//VERIFY(ot.has("hús"));
	VERIFY(ot["hús"].size() == 1);
	VERIFY(ot["hús"]["kolbász"] == "paraszt");

	csjp::String str;
	csjp::Json ot2;

	TESTSTEP("Export csjp::Json into csjp::String");
	NOEXC_VERIFY(str = ot.toString());

	TESTSTEP("Parse new csjp::String back into csjp::Json");
	LOG("%", str);
	NOEXC_VERIFY(ot2.parse(str));
	VERIFY(ot == ot2);
}

void TestJson::multipleSources()
{
	csjp::Str data(
			"{"
			"\"hami-papi\" : \"fincsi\","
			"\"gyümölcs\" : {"
				"\"alma\" : \"arany\""
			"},"
			"\"zöldség\" : {"
			"},"
			"\"hús\" : {"
				"\"kolbász\" : \"paraszt\""
			"}"
			"}");
	csjp::Json ot;

	NOEXC_VERIFY(ot.parse(data));

	csjp::String str;
	csjp::Json ot2;

	TESTSTEP("Export csjp::Json into csjp::String");
	NOEXC_VERIFY(str = ot.toString());
	LOG("%", str);

	TESTSTEP("Parse new csjp::String back into csjp::Json");
	NOEXC_VERIFY(ot2.parse(str));
	VERIFY(ot == ot2);

	TESTSTEP("Parse csjp::String back into the same csjp::Json second time.");
	NOEXC_VERIFY(ot2.parse(str));
	VERIFY(ot == ot2);
}

void TestJson::array()
{
	csjp::Str data(
			"[ # comment\n"
			"	gyümölcs, # comment\n"
			" # comment\n"
			"	zöldség,\n"
			"	[\n"
			"		gomba\n"
			"	],\n"
			"	{\n"
			"		kulcs : fűszer,\n"
			"		tömb : [\n"
			"			bonyolult\n"
			"		]\n"
			"	}\n"
			"]");
	csjp::Json ot;

	TESTSTEP("Parse data into csjp::Json");
	NOEXC_VERIFY(ot.parse(data));
	//VERIFY(ot.key == "");
	VERIFY(ot == csjp::Json::Type::Array);
	VERIFY(ot.size() == 4);
	VERIFY(ot[0] == "gyümölcs");
	VERIFY(ot[1] == "zöldség");
	VERIFY(ot[0] == csjp::Json::Type::String);
	VERIFY(ot[1] == csjp::Json::Type::String);

	VERIFY(ot[2] == csjp::Json::Type::Array);
	VERIFY(ot[2].size() == 1);
	VERIFY(ot[2][0] == csjp::Json::Type::String);
	VERIFY(ot[2][0] == "gomba");

	VERIFY(ot[3] == csjp::Json::Type::Object);
	VERIFY(ot[3].size() == 2);
	VERIFY(ot[3]["kulcs"] == csjp::Json::Type::String);
	VERIFY(ot[3]["kulcs"] == "fűszer");
	VERIFY(ot[3]["tömb"] == csjp::Json::Type::Array);
	VERIFY(ot[3]["tömb"].size() == 1);
	VERIFY(ot[3]["tömb"][0] == "bonyolult");

	csjp::String str;

	TESTSTEP("Export csjp::Json into csjp::String");
	NOEXC_VERIFY(str = ot.toString());

	csjp::Json ot2;

	TESTSTEP("Parse new csjp::String back into csjp::Json");
	NOEXC_VERIFY(ot2.parse(str));
	VERIFY(ot == ot2);
}

TEST_INIT(Json)

	TEST_RUN(onlyValues);
	TEST_RUN(onlyObjects);
	TEST_RUN(valuesInObjects);
	TEST_RUN(multipleSources);
	TEST_RUN(array);

TEST_FINISH(Json)
