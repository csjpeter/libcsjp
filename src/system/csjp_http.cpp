/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#include "csjp_http.h"

#undef DEBUG
//#define DEBUG

/* TODO
 * - HTTP 1.1 continue, keep-alive, chunked transfer support
 */

namespace csjp {

HTTPRequest::HTTPRequest(
		const Str & method,
		const Str & uri,
		const Str & body,
		const Str & version) :
	method(method),
	uri(uri),
	version(version),
	body(body)
{
	if(method.length)
		requestLine.cat(method);
	else
		requestLine.cat("GET");

	if(uri.length)
		requestLine.cat(" ", uri);
	else
		requestLine.cat(" /");

	if(version.length)
		requestLine.cat(" HTTP/", version);
	else
		requestLine.cat(" HTTP/1.0");
}

HTTPRequest HTTPRequest::get(
		const Str & uri,
		const Str & version)
{
	(void)uri;
	(void)version;
	return HTTPRequest("GET", uri, "", version);
}

HTTPRequest HTTPRequest::post(
		const Str & body,
		const Str & uri,
		const Str & version)
{
	return HTTPRequest(Str("POST"), uri, body, version);
}

String HTTPRequest::toString() const
{
	String request;
	request.catf("%\r\n", requestLine);
	for(auto & h : headers.properties){
		if(h.key == "content-length")
			continue;
		String k(h.key);
		k.lower();
		request.catf("%: %\r\n", k, h.value());
	}
	request.catf("content-length: %\r\n", body.length);
	request.catf("\r\n%", body);
	return request;
}

/**
 * For now we only parse the full request.
 * If any bytes missing even from the body,
 * the parser will report 0 processed bytes.
 */
unsigned HTTPRequest::parse(const Str & data)
{
	DBG("HTTPRequest parser available length: %", data.length);
	if (data.length < 512) {
		DBG("HTTPRequest parser available data: %", data);
	}

	if(!requestLine.length){
		size_t pos;
		if(!data.findFirst(pos, "\r\n"))
			return 0;
		requestLine <<= data.read(0, pos);
		auto result = requestLine.regexpMatches("\\([^ ]*\\) \\([^ ]*\\) HTTP/\\(.*\\)$");
		if(result.size() != 3)
			throw HttpProtocolError("Invalid HTTP request line: %", requestLine);
		method <<= result[0];
		uri <<= result[1];
		version <<= result[2];
	}

	if(!headers.value().length){
		size_t pos;
		if(!data.findFirst(pos, "\r\n\r\n", requestLine.length+2))
			return 0;
		headers <<= data.read(requestLine.length+2, pos);
		Array<Str> array = headers.value().split("\r\n");
		String key;
		for(auto & str : array){
			if(str.findFirst(pos, ":")){
				key <<= str.read(0, pos);
				pos++;
				key.trim(" \t");
				key.lower();
			} else {
				if(!key.length)
					throw HttpProtocolError(
						"Invalid header line: %", str);
				pos = 0;
			}
			Str value(str.c_str() + pos, str.length - pos);
			if(!value.startsWith(" ") && !value.startsWith("\t"))
				throw HttpProtocolError(
						"Invalid multiline header "
						"line: %", str);
			value.trim(" \t");
			headers[key] << value;
		}
	}

	if(!body.length){
		size_t readIn = requestLine.length+2 + headers.value().length+4;
		size_t bodyLength = 0;
		bodyLength <<= headers["content-length"];
		if(data.length < readIn + bodyLength)
			return 0;
		body <<= data.read(readIn, readIn + bodyLength);
		return readIn + bodyLength;
	}

	return 0;
}

void HTTPRequest::clear()
{
	method.clear();
	uri.clear();
	version.clear();
	headers.clear();
	body.clear();
	requestLine.clear();
}





HTTPResponse::HTTPResponse(
		HTTPStatusCode code,
		const Str & body,
		const Str & version
		) :
	version(version),
	body(body),
	statusCode(code)
{
	reasonPhrase <<= code.phrase();
	statusLine.catf("HTTP/% % %",
			version.length ? version.c_str() : "1.0",
			statusCode, reasonPhrase);
}

HTTPResponse::HTTPResponse(
		const Str & body,
		const Str & version) :
	version(version),
	body(body)
{
	statusCode = HTTPStatusCode::Enum::OK;
	reasonPhrase <<= statusCode.phrase();
	statusLine.catf("HTTP/% % %",
			version.length ? version.c_str() : "1.0",
			statusCode, reasonPhrase);
}

String HTTPResponse::toString() const
{
	String response;
	response.catf("%\r\n", statusLine);
	for(auto & h : headers.properties){
		if(h.key == "content-length")
			continue;
		String k(h.key);
		k.lower();
		response.catf("%: %\r\n", k, h.value());
	}
	response.catf("content-length: %\r\n", body.length);
	response.catf("\r\n%", body);
	return response;
}

unsigned HTTPResponse::parse(const Str & data)
{
	if(!statusLine.length){
		size_t pos;
		if(!data.findFirst(pos, "\r\n"))
			return 0;
		statusLine <<= data.read(0, pos);
		auto result = statusLine.regexpMatches("HTTP/\\([^ ]*\\) \\([^ ]*\\) \\(.*\\)$");
		if(result.size() != 3)
			throw HttpProtocolError("Invalid HTTP status line: %", statusLine);
		version <<= result[0];
		statusCode = HTTPStatusCode(result[1]);
		reasonPhrase <<= result[2];
	}

	if(!headers.value().length){
		size_t pos;
		if(!data.findFirst(pos, "\r\n\r\n", statusLine.length+2))
			return 0;
		headers <<= data.read(statusLine.length+2, pos);
		Array<Str> array = headers.value().split("\r\n");
		String key;
		for(auto & str : array){
			if(str.findFirst(pos, ":")){
				key <<= str.read(0, pos);
				key.trim(" \t");
				key.lower();
				pos++;
			} else {
				if(!key.length)
					throw HttpProtocolError("Invalid header line: %", str);
				pos = 0;
			}
			Str value(str.c_str() + pos, str.length - pos);
			if(!value.startsWith(" ") && !value.startsWith("\t"))
				throw HttpProtocolError("Invalid multiline header line: %", str);
			value.trim(" \t");
			headers[key] << value;
		}
	}

	if(!body.length){
		size_t readIn = statusLine.length+2 + headers.value().length+4;
		size_t bodyLength = 0;
		bodyLength <<= headers["content-length"];
		if(data.length < readIn + bodyLength)
			return 0;
		body <<= data.read(readIn, readIn + bodyLength);
		return readIn + bodyLength;
	}

	return 0;
}

void HTTPResponse::clear()
{
	version.clear();
	headers.clear();
	body.clear();
	//statusCode.clear();
	reasonPhrase.clear();
	statusLine.clear();
}

}

