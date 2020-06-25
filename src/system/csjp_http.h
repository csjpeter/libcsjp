/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#ifndef CSJP_HTTP_H
#define CSJP_HTTP_H

#include <csjp_json.h>
#include <csjp_server.h>
#include <csjp_client.h>

namespace csjp {

DECL_EXCEPTION(SocketError, HttpProtocolError);

class HTTPRequest
{
public:
	explicit HTTPRequest(const HTTPRequest & orig) = delete;
	const HTTPRequest & operator=(const HTTPRequest &) = delete;

	HTTPRequest(HTTPRequest && temp) :
		method(move_cast(temp.method)),
		uri(move_cast(temp.uri)),
		version(move_cast(temp.version)),
		headers(move_cast(temp.headers)),
		body(move_cast(temp.body)),
		requestLine(move_cast(requestLine))
	{}
	const HTTPRequest & operator=(HTTPRequest && temp)
	{
		method = move_cast(temp.method);
		uri = move_cast(temp.uri);
		version = move_cast(temp.version);
		headers = move_cast(temp.headers);
		body = move_cast(temp.body);
		requestLine = move_cast(temp.requestLine);
		return *this;
	}

	HTTPRequest() {}

	HTTPRequest(	const Str & method,
			const Str & uri = "/",
			const Str & body = "",
			const Str & version = "1.0");

	static HTTPRequest get(const Str & uri,
		const Str & version = "1.0");

	static HTTPRequest post(const Str & body,
		const Str & uri = "/",
		const Str & version = "1.0");

	String toString() const;

	unsigned parse(const Str & data);

	void clear();

	const String & getRequestLine(){ return requestLine; }

	String method;
	String uri;
	String version;
	Json headers;
	String body;
private:
	String requestLine;
};

class HTTPStatusCode /*{{{*/
{
public:
	enum class Enum;

	HTTPStatusCode(HTTPStatusCode::Enum code) : code(code) {}
	HTTPStatusCode(AStr & astr) : code(HTTPStatusCode::Enum::OK)
	{
		String str(astr);
		unsigned u;
		u <<= str;
		code = (HTTPStatusCode::Enum)u;
	}
	HTTPStatusCode() : code(HTTPStatusCode::Enum::OK) {}
	operator int () const { return (int)code; }

	Enum code;

	enum class Enum
	{
		/* Informal */
		Continue = 100,
		SwitchingProtocols = 101,
		/* Success */
		OK = 200,
		Created,
		Accepted,
		NonAuthoritativeInformation,
		NoContent,
		ResetContent,
		PartialContent,
		/* Redirection */
		MultipleChoices = 300,
		MovedPermanently,
		Found,
		SeeOther,
		NotModified,
		UseProxy,
		TemporaryRedirect = 307,
		/* Client Error */
		BadRequest = 400,
		Unauthorized,
		PaymentRequired,
		Forbidden,
		NotFound,
		MethodNotAllowed,
		NotAcceptable,
		ProxyAuthenticationRequired,
		RequestTimeout,
		Conflict,
		Gone,
		LengthRequired,
		PreconditionFailed,
		RequestEntityTooLarge,
		RequestURITooLarge,
		UnsupportedMediaType,
		RequestedRangeNotSatisfiable,
		ExpectationFailed,
		/* Server Error */
		InternalServerError = 500,
		NotImplemented,
		BadGateway,
		ServiceUnavailable,
		GatewayTimeout,
		HTTPVersionNotSupported
	};

	inline const char * phrase() const
	{
		switch(code)
		{
		case Enum::Continue : return "Continue";
		case Enum::SwitchingProtocols : return "Switching Protocols";
		case Enum::OK : return "OK";
		case Enum::Created : return "Created";
		case Enum::Accepted : return "Accepted";
		case Enum::NonAuthoritativeInformation :
				return "Non-Authoritative Information";
		case Enum::NoContent : return "No Content";
		case Enum::ResetContent : return "Reset Content";
		case Enum::PartialContent : return "Partial Content";
		case Enum::MultipleChoices : return "Multiple Choices";
		case Enum::MovedPermanently : return "Moved Permanently";
		case Enum::Found : return "Found";
		case Enum::SeeOther : return "See Other";
		case Enum::NotModified : return "Not Modified";
		case Enum::UseProxy : return "Use Proxy";
		case Enum::TemporaryRedirect : return "Temporary Redirect";
		case Enum::BadRequest : return "Bad Request";
		case Enum::Unauthorized : return "Unauthorized";
		case Enum::PaymentRequired : return "Payment Required";
		case Enum::Forbidden : return "Forbidden";
		case Enum::NotFound : return "Not Found";
		case Enum::MethodNotAllowed : return "Method Not Allowed";
		case Enum::NotAcceptable : return "Not Acceptable";
		case Enum::ProxyAuthenticationRequired :
				return "Proxy Authentication Required";
		case Enum::RequestTimeout : return "Request Time-out";
		case Enum::Conflict : return "Conflict";
		case Enum::Gone : return "Gone";
		case Enum::LengthRequired : return "Length Required";
		case Enum::PreconditionFailed : return "Precondition Failed";
		case Enum::RequestEntityTooLarge :
				return "Request Entity Too Large";
		case Enum::RequestURITooLarge :
				return "Request-URI Too Large";
		case Enum::UnsupportedMediaType :
				return "Unsupported Media Type";
		case Enum::RequestedRangeNotSatisfiable :
				return "Requested range not satisfiable";
		case Enum::ExpectationFailed : return "Expectation Failed";
		case Enum::InternalServerError :
				return "Internal Server Error";
		case Enum::NotImplemented : return "Not Implemented";
		case Enum::BadGateway : return "Bad Gateway";
		case Enum::ServiceUnavailable : return "Service Unavailable";
		case Enum::GatewayTimeout : return "Gateway Time-out";
		case Enum::HTTPVersionNotSupported :
				return "HTTP Version not supported";
		default: break;
		}
		throw InvalidState(
				"HTTP Status code % is unknown and could not "
				"be converted to reason phrase.", (int)code);
	}
};/*}}}*/

class HTTPResponse
{
public:
	explicit HTTPResponse(const HTTPResponse & orig) = delete;
	const HTTPResponse & operator=(const HTTPResponse &) = delete;

	HTTPResponse(HTTPResponse && temp) :
		version(move_cast(temp.version)),
		headers(move_cast(temp.headers)),
		body(move_cast(temp.body)),
		statusCode(move_cast(temp.statusCode)),
		reasonPhrase(move_cast(temp.reasonPhrase)),
		statusLine(move_cast(temp.statusLine))
	{}
	const HTTPResponse & operator=(HTTPResponse && temp)
	{
		version = move_cast(temp.version);
		headers = move_cast(temp.headers);
		body = move_cast(temp.body);
		statusCode = move_cast(temp.statusCode);
		reasonPhrase = move_cast(temp.reasonPhrase);
		statusLine = move_cast(temp.statusLine);
		return *this;
	}

	HTTPResponse() {}

	HTTPResponse(HTTPStatusCode statusCode,
			const Str & body = "",
			const Str & version = "1.0");

	HTTPResponse(const Str & body,
			const Str & version = "1.0");

	String toString() const;

	unsigned parse(const Str & data);

	void clear();

	HTTPStatusCode status(){ return statusCode; }
	const String & reason(){ return reasonPhrase; }
	const String & getStatusLine(){ return statusLine; }

	String version;
	Json headers;
	String body;

private:
	HTTPStatusCode statusCode;
	String reasonPhrase;
	String statusLine;
};

inline bool operator==(const HTTPStatusCode & lhs, const HTTPStatusCode::Enum & rhs)
		{ return lhs.code == rhs; }

inline String &	operator<<(csjp::String & lhs, const csjp::HTTPResponse & rhs)
		{ lhs += rhs.toString(); return lhs; }
inline String &	operator<<(csjp::String & lhs, const csjp::HTTPRequest & rhs)
		{ lhs += rhs.toString(); return lhs; }
inline String &	operator<<(csjp::String & lhs, const csjp::HTTPStatusCode & rhs)
		{ lhs << (int)(rhs.code); return lhs; }

inline String &	operator<<=(csjp::String & lhs, const csjp::HTTPResponse & rhs)
		{ lhs = rhs.toString(); return lhs; }
inline String &	operator<<=(csjp::String & lhs, const csjp::HTTPRequest & rhs)
		{ lhs = rhs.toString(); return lhs; }
inline String &	operator<<=(csjp::String & lhs, const csjp::HTTPStatusCode & rhs)
		{ lhs <<= (int)(rhs.code); return lhs; }


}

#endif
