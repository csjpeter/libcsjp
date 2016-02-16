/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2015 Csaszar, Peter
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
};

void TestJson::onlyValues()
{
	csjp::String data(
			"{ # comment\n"
			"gyümölcs : alma, # comment\n"
			" # comment\n"
			"zöldség : répa"
			"}");
	csjp::Json ot;

	TESTSTEP("Parse data into csjp::Json");
	NOEXC_VERIFY(ot.parse(data));
	VERIFY(ot.key == "");
	VERIFY(ot.properties.size() == 2);
	VERIFY(ot.properties.has("gyümölcs"));
	VERIFY(ot["gyümölcs"] == "alma");
	VERIFY(ot.properties.has("zöldség"));
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
	csjp::String data(
			"{ # comment\n"
			"\"gyümölcs\" : {}, # comment \n"
			"  # comment \n"
			"\"zöldség\" : {}"
			"}");
	csjp::Json ot;

	TESTSTEP("Parse data into csjp::Json");
	NOEXC_VERIFY(ot.parse(data));
	VERIFY(ot.key == "");
	VERIFY(ot.properties.size() == 2);
	VERIFY(ot.properties.has("gyümölcs"));
	VERIFY(ot["gyümölcs"].key == "gyümölcs");
	VERIFY(ot["gyümölcs"].properties.size() == 0);
	VERIFY(ot.properties.has("zöldség"));
	VERIFY(ot["zöldség"].key == "zöldség");
	VERIFY(ot["zöldség"].properties.size() == 0);

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
	csjp::String data(
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
	VERIFY(ot.key == "");
	VERIFY(ot.properties.size() == 4);
	VERIFY(ot["hami-papi"] == "fincsi");
	VERIFY(ot.properties.has("gyümölcs"));
	VERIFY(ot["gyümölcs"].key == "gyümölcs");
	VERIFY(ot["gyümölcs"].properties.size() == 1);
	VERIFY(ot["gyümölcs"]["alma"] == "arany");
	VERIFY(ot.properties.has("zöldség"));
	VERIFY(ot["zöldség"].key == "zöldség");
	VERIFY(ot["zöldség"].properties.size() == 0);
	VERIFY(ot.properties.has("hús"));
	VERIFY(ot["hús"].key == "hús");
	VERIFY(ot["hús"].properties.size() == 1);
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
	csjp::String data(
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

TEST_INIT(Json)

	TEST_RUN(onlyValues);
	TEST_RUN(onlyObjects);
	TEST_RUN(valuesInObjects);
	TEST_RUN(multipleSources);

TEST_FINISH(Json)
