/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#include "csjp_http.h"

/* TODO
 * - support multiline header values
 * - HTTP 1.1 continue, keep-alive, chunked transfer support
 */

namespace csjp {

HTTPRequest::HTTPRequest(
		const String & method,
		const String & uri,
		const String & body,
		const String & version) :
	method(method),
	uri(uri),
	version(version),
	body(body)
{
	requestLine.catf("% % HTTP/%",
			method.length ? method.c_str() : "GET",
			uri.length ? uri.c_str() : "/",
			version.length ? version.c_str() : "1.0");
}

HTTPRequest HTTPRequest::get(
		const String & uri,
		const String & version)
{
	(void)uri;
	(void)version;
	return HTTPRequest(String("GET"), uri, String(""), version);
}

HTTPRequest HTTPRequest::post(
		const String & body,
		const String & uri,
		const String & version)
{
	return HTTPRequest(String("POST"), uri, body, version);
}

HTTPRequest::operator String () const
{
	String request;
	request.catf("%\r\n", requestLine);
	for(auto & h : headers.properties){
		if(h.key == "content-length")
			continue;
		String k(h.key);
		k.lower();
		request.catf("%: %\r\n", k, h.value);
	}
	request.catf("content-length: %\r\n", body.length);
	request.catf("\r\n%", body);
	return request;
}

unsigned HTTPRequest::parse(const StringChunk & data)
{
	if(!method.length){
		size_t pos;
		if(!data.findFirst(pos, "\r\n"))
			return false;
		requestLine <<= data.read(0, pos);
		Array<StringChunk> result(3);
		if(!subStringByRegexp(requestLine, result,
				"\\([^ ]*\\) \\([^ ]*\\) HTTP/\\(.*\\)$"))
			throw HttpProtocolError(
					"Invalid HTTP request line: %",
					requestLine);
		method <<= result[0];
		uri <<= result[1];
		version <<= result[2];
	}

	if(!headers.value.length){
		size_t pos;
		if(!data.findFirst(pos, "\r\n\r\n", requestLine.length+2))
			return false;
		headers.value <<= data.read(requestLine.length+2, pos);
		Array<StringChunk> array = split(headers.value, "\r\n");
		for(auto & str : array){
			if(!str.findFirst(pos, ":"))
				throw HttpProtocolError(
						"Invalid header line: %", str);
			String key;
			key <<= str.read(0, pos);
			key.trim(" \t");
			key.lower();
			StringChunk value(str.str + pos + 1,
					str.length - pos - 1);
			value.trim(" \t");
			headers[key] = value;
		}
	}

	if(!body.length){
		size_t readIn = requestLine.length+2 + headers.value.length+4;
		size_t bodyLength = 0;
		bodyLength <<= headers["content-length"];
		if(data.length < readIn + bodyLength)
			return false;
		body <<= data.read(readIn, readIn + bodyLength);
	}

	return true;
}





HTTPResponse::HTTPResponse(
		HTTPStatusCode code,
		const String & body,
		const String & version
		) :
	version(version),
	body(body)
{
	statusCode <<= code;
	reasonPhrase <<= code.phrase();
	statusLine.catf("HTTP-% % %",
			version.length ? version.c_str() : "1.0",
			statusCode, reasonPhrase);
}

HTTPResponse::HTTPResponse(
		const String & body,
		const String & version) :
	version(version),
	body(body)
{
	HTTPStatusCode code = HTTPStatusCode::Enum::OK;
	statusCode <<= code;
	reasonPhrase <<= code.phrase();
	statusLine.catf("HTTP-% % %",
			version.length ? version.c_str() : "1.0",
			statusCode, reasonPhrase);
}

HTTPResponse::operator String () const
{
	String response;
	response.catf("%\r\n", statusLine);
	for(auto & h : headers.properties){
		if(h.key == "content-length")
			continue;
		String k(h.key);
		k.lower();
		response.catf("%: %\r\n", k, h.value);
	}
	response.catf("content-length: %\r\n", body.length);
	response.catf("\r\n%", body);
	return response;
}

unsigned HTTPResponse::parse(const StringChunk & data)
{
	if(!statusCode.length){
		size_t pos;
		if(!data.findFirst(pos, "\r\n"))
			return false;
		statusLine <<= data.read(0, pos);
		Array<StringChunk> result(3);
		if(!subStringByRegexp(statusLine, result,
				"HTTP-\\([^ ]*\\) \\([^ ]*\\) \\(.*\\)$"))
			throw HttpProtocolError(
					"Invalid HTTP status line: %",
					statusLine);
		version <<= result[0];
		statusCode <<= result[1];
		reasonPhrase <<= result[2];
	}

	if(!headers.value.length){
		size_t pos;
		if(!data.findFirst(pos, "\r\n\r\n", statusLine.length+2))
			return false;
		headers.value <<= data.read(statusLine.length+2, pos);
		Array<StringChunk> array = split(headers.value, "\r\n");
		for(auto & str : array){
			if(!str.findFirst(pos, ":"))
				throw HttpProtocolError(
						"Invalid header line: %", str);
			String key;
			key <<= str.read(0, pos);
			key.trim(" \t");
			StringChunk value(str.str + pos + 1,
					str.length - pos - 1);
			value.trim(" \t");
			key.lower();
			headers[key] = value;
		}
	}

	if(!body.length){
		size_t readIn = statusLine.length+2 + headers.value.length+4;
		size_t bodyLength = 0;
		bodyLength <<= headers["content-length"];
		if(data.length < readIn + bodyLength)
			return false;
		body <<= data.read(readIn, readIn + bodyLength);
	}

	return true;
}

}

