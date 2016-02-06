/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2015 Csaszar, Peter
 */

#include <string.h>

#include <csjp_object.h>
#include <csjp_string_chunk.h>
//#include <csjp_sorter_reference_container.h>

#include "csjp_json.h"

namespace csjp {

/* Parse error, other than the expected string was found for
 * the rule with the given language element. */
#define PARSE_ERROR(expected, languageElement) \
		throw ParseError("Expecting '%' for language element '%' at " \
				"linepos %:% index % (%) in json code.", \
				expected, languageElement, lineNum, \
				pos + 1 - linePos, pos + 1, data[pos]);

/* Recursive descent parser */
class JsonParser
{
private:
	JsonParser & operator=(const JsonParser & src) {(void)src; return *this;} /* forbidden */
public:
	static Json parse(const String & data)
	{
		Json target;
		JsonParser parser(target, data);
		parser.parse();
		return target;
	}

private:
	explicit JsonParser(Json & target, const String & data);
	~JsonParser();

	void parse();

	inline bool isWhiteSpace(char c);
	inline bool isTerminal(char c);
	bool skipComment();
	void skipWhiteSpace();
	bool readKeyword(const char *keyword);
	/* Returns the index, next to the found/expected character. */
	bool readCharacter(char expected);
	bool readFreeString(String & token); /* string without quotation and ecaped characters */
	bool readString(String & token);
	bool readNumber(String & token);
	bool readValue(String & token);
	bool readPair(bool must = false);
	bool readMembers(bool must = false);
	bool readObject();

private:
	RefArray<Json> objectStack;
	const String & data; /* Json formatted text. */
	Json & target; /* Object want to receive findings. */
	size_t len; /* Length of data */
	size_t pos; /* Parser position */
	size_t lineNum; /* For human readable error messages. */
	size_t linePos; /* Position where current line starts, for human readable error messages. */
	String key;
};


bool JsonParser::isWhiteSpace(char c)
{
	switch(c){
		case ' ' :
		case '\n' :
		case '\t' :
			return true;
		default:
			return false;
	}
}

bool JsonParser::isTerminal(char c)
{
	if(isWhiteSpace(c))
		return true;

	switch(c){
		case '{' :
		case '}' :
		case '[' :
		case ']' :
		case '"' :
		case ',' :
		case ':' :
			return true;
		default:
			return false;
	}
}

bool JsonParser::skipComment()
{
	char c;

	if(!readCharacter('#'))
		return false;

	for(; true; pos++) {
		if(len <= pos)
			break;

		c = data[pos];

		if(c == '\t'){
			continue;
		}

		if(c == '\n'){
			pos++;
			lineNum++;
			linePos = pos;
			break;
		}

		/* Because of android compiler we need this ugly cast here. */
		if((unsigned char)(c) < 32)
			throw ParseError("Control character found at linepos %:% "
					"position % while reading "
					"comment in json code: %",
					lineNum, pos - linePos + 1, pos + 1, data.str);
	}

	return true;
}

/* Returns the index, next to the found/expected character. */
void JsonParser::skipWhiteSpace()
{
	do {
		for(; pos < len; pos++){
			if(!isWhiteSpace(data[pos]))
				break;
			if(data[pos] == '\n'){
				lineNum++;
				linePos = pos + 1;
			}
		}
	} while(skipComment());
}

/* Returns if word starting under pos is the expected, pos is incremented on success. */
bool JsonParser::readKeyword(const char * keyword)
{
	size_t keyword_len = strlen(keyword);
	if(len <= pos + keyword_len)
		return false;

	StringChunk comp(data.str + pos, keyword_len);
	if(comp.compare(keyword))
		return false;

	if(!isTerminal(data[pos + keyword_len]))
		return false;

	pos += keyword_len;
	return true;
}

/* Returns if character under pos is the expected, pos is incrememtned on success. */
bool JsonParser::readCharacter(char expected)
{
	if(len <= pos)
		return false;

	if(expected != data[pos])
		return false;

	pos++;
	if(expected == '\n'){
		lineNum++;
		linePos = pos + 1;
	}
	return true;
}

bool JsonParser::readFreeString(String & token)
{
	char c;
	String str;
	size_t origPos = pos;

	for(; true; pos++) {
		if(len <= pos)
			throw ParseError("End of input while lookig for free string "
					"from position % in json code.", origPos);

		c = data[pos];

		/* Because of android compiler we need this ugly cast here. */
		if((unsigned char)(c) < 32 && c != '\n' && c != '\t')
			throw ParseError("Control character found at linepos %:% "
					"position % while reading "
					"string in json code: %",
					lineNum, pos - linePos + 1, pos + 1, data.str);

		/* Special hack trick to be C++ namespace separator friendly. */
		if(c == ':' && pos + 1 < len && data[pos+1] == ':'){
			str << "::";
			pos++;
			continue;
		}

		if((!isWhiteSpace(c) && isTerminal(c)) || c == '#'){
			str.trim(" \t\n");
			if(!str.length){
				return false;
			}
			swap(token, str);
			return true;
		}

		if(c == '\n'){
			lineNum++;
			linePos = pos + 1;
		}

		str << c;
	}
}

bool JsonParser::readString(String & token)
{
	char c;
	size_t escapeSequence = 0;
	String str;
	size_t origPos = pos;

	if(!readCharacter('"'))
		return readFreeString(token);

	for(; true; pos++) {
		if(len <= pos)
			throw ParseError("End of input while lookig for string from position % "
					"in json code.", origPos);

		c = data[pos];

		/* Because of android compiler we need this ugly cast here. */
		if((unsigned char)(c) < 32)
			throw ParseError("Control character found at linepos %:% "
					"position % while reading "
					"string in json code: %",
					lineNum, pos - linePos + 1, pos + 1, data.str);
		if(escapeSequence){
			switch(c){
				case '"' :
					str << '"';
					break;
				case '\\' :
					str << '\\';
					break;
				case '/' :
					str << '/';
					break;
				case 'b' :
					str << '\b';
					break;
				case 'f' :
					str << '\f';
					break;
				case 'n' :
					str << '\n';
					break;
				case 'r' :
					str << '\r';
					break;
				case 't' :
					str << '\t';
					break;
				default :
					str << '\\';
					str << c;
					throw ParseError("Unsuported escape sequence started with "
							"'%' at position: % in json code: %",
							c, pos + 1, data.str);
					break;
			}
			escapeSequence--;
			continue;
		}
		if(c == '\\'){
			escapeSequence = 1;;
			continue;
		}
		if(c == '"'){
			pos++;
			swap(token, str);
			return true;
		}

		if(c == '\n'){
			lineNum++;
			linePos = pos + 1;
		}

		str << c;
	}
}

bool JsonParser::readNumber(String & token)
{
	char c;
	String str;
	size_t origPos = pos;

	for(; true; pos++) {
		if(len <= pos)
			throw ParseError("End of input while lookig for number from position "
					"% in json code: %", origPos, data.str);

		c = data[pos];

		switch(c){
			case '0' :
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
			case '8' :
			case '9' :
			case '.' :
			case '-' :
			case '+' :
			case 'e' :
			case 'E' :
				str << c;
				break;
			default :
				if(str.length && isTerminal(c)){
					swap(token, str);
					return true;
				}
				pos = origPos;
				return false;
		}
	}
}

bool JsonParser::readValue(String & token)
{
	if(readNumber(token))
		return true;
	if(readKeyword("true")){
		token = "true";
		return true;
	}
	if(readKeyword("false")){
		token = "false";
		return true;
	}
	if(readKeyword("null")){
		token = "null";
		return true;
	}
	if(readString(token))
		return true;
	if(readObject())
		return true;
	return false;
}

bool JsonParser::readPair(bool must)
{
	String value;

	if(!readString(key)){
		if(must) {
			PARSE_ERROR("language element string", "pair");
		} else
			return false;
	}
	skipWhiteSpace();
	if(!readCharacter(':'))
		PARSE_ERROR(":", "pair");
	skipWhiteSpace();
	if(!readValue(value))
		PARSE_ERROR("language element value", "pair");

	if(key.length){
		DBG("Inserting key '%', value '%'", key, value);
		objectStack.last()[key] = move_cast(value);
		key.clear();
	}

	return true;
}

bool JsonParser::readMembers(bool must)
{
	if(!readPair(must))
		return false;
	skipWhiteSpace();
	if(readCharacter(',')){
		skipWhiteSpace();
		return readMembers(true);
	}
	return true;
}

/* FIXME : process 'null' object here instead as property, by just begining and ending it. */
bool JsonParser::readObject()
{
	if(!readCharacter('{'))
		return false;

	DBG("New object '%'", key);
	ENSURE(key.length || objectStack.empty(), ParseError);
	if(objectStack.empty()){
		ENSURE(key.length == 0, ParseError);
		objectStack.add(target);
	} else {
		ENSURE(key.length, ParseError);
		Object<Json> obj(new Json(key));
		Json *j = obj.ptr;
		objectStack.last().properties.add(obj.ptr);
		objectStack.add(*j);
		key.clear();
	}

	skipWhiteSpace();
	if(readMembers())
		skipWhiteSpace();

	objectStack.removeAt(objectStack.size()-1);

	if(!readCharacter('}'))
		PARSE_ERROR("}", "object");

	return true;
}

JsonParser::JsonParser(Json & target, const String & data) :
	data(data),
	target(target),
	len(0),
	pos(0),
	lineNum(1),
	linePos(0)
{
	len = data.length;
}

JsonParser::~JsonParser()
{
}

void JsonParser::parse()
{
	DBG("Parsing:\n%", data);
	if(readObject())
		return;

	PARSE_ERROR("{", "object");
}

csjp::String Json::toString(int depth) const
{
	size_t i, s;
	bool coma = false;
	csjp::String data;

	if(depth){
#ifndef PERFMODE
		data << "\n";
		for(size_t i = 0, s = depth; i < s; i++) data << "\t";
#endif
		data << "\"" << key << "\" : ";
	}

	if(value.str != 0){
		data << "\"" << value << "\"";
		return data;
	}

	data << '{';
	
	for(i = 0, s = properties.size(); i < s; i++){
		if(coma){
			data << ", ";
			coma = false;
		}
		depth++;
		data << properties.queryAt(i).toString(depth); /* recurse */
		depth--;
		coma = true;
	}

#ifndef PERFMODE
	if(properties.size()){
		data << "\n";
		for(size_t i = 0, s = depth; i < s; i++) data << "\t";
	}
#endif
	data << "}";

	return data;
}

Json Json::fromString(const String & data)
{
	return JsonParser::parse(data);
}

}

