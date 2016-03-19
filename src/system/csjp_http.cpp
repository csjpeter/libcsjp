/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#include "csjp_http.h"

/* TODO
 * - convert header keys to lowercase before storing them
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
	request.catf("% % HTTP/%\r\n",
			method.length ? method : "GET",
			uri.length ? uri : "/",
			version.length ? version : "1.0");
	for(auto & h : headers.properties){
		if(h.key == "content-length")
			continue;
		request.catf("%: %\r\n", h.key, h.value);
	}
	request.catf("content-length: %\r\n", body.length);
	request.catf("\r\n%", body);
	return request;
}

bool HTTPRequest::receive(Server & server)
{
	if(!method.length){
		size_t pos;
		if(!server.findFirst(pos, String("\r\n")))
			return false;
		String line = server.receive(pos+2);
		Array<StringChunk> result(3);
		if(!subStringByRegexp(line, result,
				"\\([^ ]*\\) \\([^ ]*\\) HTTP/\\(.*\\)\r\n"))
			throw HttpProtocolError(
					"Invalid HTTP request line: %", line);
		method <<= result[0];
		uri <<= result[1];
		version <<= result[2];
	}

	if(!headers.value.length){
		size_t pos;
		if(!server.findFirst(pos, String("\r\n\r\n")))
			return false;
		headers.value = server.receive(pos+4);
		headers.value.trim("\r\n");
		Array<StringChunk> array = split(headers.value, "\r\n");
		for(auto & str : array){
			if(!str.findFirst(pos, ":"))
				throw HttpProtocolError(
						"Invalid header line: %", str);
			StringChunk key(str.str, pos);
			key.trim(" \t");
			StringChunk value(str.str + pos + 1,
					str.length - pos - 1);
			value.trim(" \t");
			headers[key] = value;
		}
	}

	if(!body.length){
		size_t bodyLength = 0;
		bodyLength <<= headers["content-length"];
		if(server.bytesAvailable < bodyLength)
			return false;
		body = server.receive(bodyLength);
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
}

HTTPResponse::operator String () const
{
	String response;
	response.catf("HTTP-% % %\r\n",
			version.length ? version : "1.0",
			statusCode, reasonPhrase);//FIXME can these be empty?
	for(auto & h : headers.properties){
		if(h.key == "content-length")
			continue;
		response.catf("%: %\r\n", h.key, h.value);
	}
	response.catf("content-length: %\r\n", body.length);
	response.catf("\r\n%", body);
	return response;
}

bool HTTPResponse::receive(Client & client)
{
	if(!statusCode.length){
		size_t pos;
		if(!client.findFirst(pos, String("\r\n")))
			return false;
		String line = client.receive(pos+2);
		Array<StringChunk> result(3);
		if(!subStringByRegexp(line, result,
				"HTTP-\\([^ ]*\\) \\([^ ]*\\) \\(.*\\)\r\n"))
			throw HttpProtocolError(
					"Invalid HTTP request line: %", line);
		version <<= result[0];
		statusCode <<= result[1];
		reasonPhrase <<= result[2];
	}

	if(!headers.value.length){
		size_t pos;
		if(!client.findFirst(pos, String("\r\n\r\n")))
			return false;
		headers.value = client.receive(pos+4);
		headers.value.trim("\r\n");
		Array<StringChunk> array = split(headers.value, "\r\n");
		for(auto & str : array){
			if(!str.findFirst(pos, ":"))
				throw HttpProtocolError(
						"Invalid header line: %", str);
			StringChunk key(str.str, pos);
			key.trim(" \t");
			StringChunk value(str.str + pos + 1,
					str.length - pos - 1);
			value.trim(" \t");
			headers[key] = value;
		}
	}

	if(!body.length){
		size_t bodyLength = 0;
		bodyLength <<= headers["content-length"];
		if(client.bytesAvailable < bodyLength)
			return false;
		body = client.receive(bodyLength);
	}

	return true;
}

}

