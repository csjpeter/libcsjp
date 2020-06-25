/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#undef DEBUG
#define DEBUG

#include <csjp_signal.h>
#include <csjp_server.h>
#include <csjp_client.h>
#include <csjp_epoll_control.h>
#include <csjp_owner_container.h>
#include <csjp_http.h>
#include <csjp_test.h>

csjp::String requestStr, responseStr;

class HTTPServer : public csjp::Server
{
public:
	HTTPServer(const csjp::Listener & l) : csjp::Server(l) {}
	virtual ~HTTPServer() {}

	virtual void dataReceived()
	{
		if(!receive(request))
			return;

		DBG("request line: %", request.getRequestLine());
		VERIFY(request.getRequestLine() == "POST / HTTP/1.0");
		DBG("request method: %", request.method);
		VERIFY(request.method == "POST");
		DBG("request uri: %", request.uri);
		VERIFY(request.uri == "/");
		DBG("request version: %", request.version);
		VERIFY(request.version == "1.0");
		DBG("request headers: %", request.headers);
		VERIFY(request.headers == "content-length: 4");
		//DBG("request.headers.type: %", request.headers.type);
		//VERIFY(request.headers.type == Json::Type::Object);
		DBG("request.headers.size(): %", request.headers.size());
		VERIFY(request.headers.size() == 1);
		VERIFY(request.headers["content-length"] == "4");
		DBG("request body: %", request.body);
		VERIFY(request.body == "body");

		DBG("received request:\n%", request);
		VERIFY(request.toString() == requestStr);

		csjp::HTTPResponse response("answer");
		responseStr = response.toString();
		send(response.toString());
		DBG("sent response:\n%", response);
	}

	csjp::HTTPRequest request;
};

bool operator<(const HTTPServer & lhs, const HTTPServer & rhs)
{
	return &lhs < &rhs;
}

bool operator<(const csjp::Socket & lhs, const HTTPServer & rhs)
{
	return &lhs < &rhs;
}

bool operator<(const HTTPServer & lhs, const csjp::Socket & rhs)
{
	return &lhs < &rhs;
}

csjp::OwnerContainer<HTTPServer> businessServers;

class HTTPListener : public csjp::Listener
{
public:
	HTTPListener(const char * ip, unsigned port,
			unsigned incomingConnectionQueueLength = 0) :
		csjp::Listener(ip, port, incomingConnectionQueueLength) {}
	virtual ~HTTPListener() {}

	virtual void dataReceived()
	{
		businessServers.add(new HTTPServer(*this));
	}
	virtual void readyToSend()
	{
		VERIFY(false);
	}
};

class HTTPClient : public csjp::Client
{
public:
	HTTPClient(const char *name, unsigned port) :
		csjp::Client(name, port) {}
	virtual ~HTTPClient() {}

	virtual void dataReceived()
	{
		if(!receive(response))
			return;

		DBG("response status line: %", response.getStatusLine());
		VERIFY(response.getStatusLine() == "HTTP/1.0 200 OK");
		DBG("response status code: %", response.status());
		VERIFY(response.status() == csjp::HTTPStatusCode::Enum::OK);
		DBG("response reason phrase: %", response.reason());
		VERIFY(response.reason() == "OK");
		DBG("response version: %", response.version);
		VERIFY(response.version == "1.0");
		DBG("response headers: %", response.headers);
		VERIFY(response.headers == "content-length: 6");
		VERIFY(response.headers.size() == 1);
		VERIFY(response.headers["content-length"] == "6");
		DBG("response body: %", response.body);
		VERIFY(response.body == "answer");

		DBG("received response:\n%", response);
		VERIFY(response.toString() == responseStr);
	}

	void sendTestRequest()
	{
		csjp::HTTPRequest request(
				"POST",
				"/",
				"body");
		requestStr = request.toString();
		send(request.toString());
		DBG("sent request:\n%", request);
	}

	csjp::HTTPResponse response;
};




class TestHTTP
{
public:
	void create();
	void requestResponseOverSocket();
	void multiLineHeaders();
};

void TestHTTP::create()
{
	HTTPListener listener("127.0.0.1", 20202);
	try{
		HTTPServer server(listener);
		VERIFY(false);
	} catch(csjp::SocketNoneConnecting & e){
		VERIFY(true);
	} catch(csjp::SocketError & e){
		EXCEPTION(e);
		VERIFY(false);
	}
}

void TestHTTP::requestResponseOverSocket()
{
	TESTSTEP("SigPipe handler, EPoll, listening, connecting with client");
	csjp::Signal pipeSignal(SIGPIPE, csjp::Signal::sigpipeHandler);
	csjp::EPollControl epoll(5);
	HTTPListener listener("127.0.0.1", 30303);
	epoll.add(listener);
	HTTPClient client("127.0.0.1", 30303);
	epoll.add(client);
	epoll.waitAndControl(10); // 0.01 sec
	VERIFY(businessServers.size() == 1);
	for(auto & server : businessServers)
		epoll.add(server);

	TESTSTEP("Client sends request");
	client.sendTestRequest();
	for(auto event : epoll.waitAndControl(10)){ // 0.01 sec
		DBG("ControlEvent received: %", event.name());
		switch(event.code){
		case csjp::EPollControl::ControlEventCode::ClosedByPeer:
			break;
		case csjp::EPollControl::ControlEventCode::ClosedByHost:
			break;
		case csjp::EPollControl::ControlEventCode::Exception:
			EXCEPTION(event.exception);
			throw;
			break;
		}
	}

	TESTSTEP("Server responses");
	for(int i=0; i<10 && businessServers.size(); i++)
	{
		for(auto event : epoll.waitAndControl(10)){ // 0.01 sec
			DBG("ControlEvent received: %", event.name());
			switch(event.code){
			case csjp::EPollControl::ControlEventCode::ClosedByPeer:
				break;
			case csjp::EPollControl::ControlEventCode::ClosedByHost:
				break;
			case csjp::EPollControl::ControlEventCode::Exception:
				EXCEPTION(event.exception);
				throw;
				break;
			}
		}
	}
	VERIFY(businessServers.size() == 1);

	TESTSTEP("Destructors do the rest of cleanup");
}

void TestHTTP::multiLineHeaders()
{
	TESTSTEP("Parse multiline header with \\n\\r character in it");
	{
		csjp::HTTPRequest request("POST");

		request.headers["multiline-test-header-key"] =
			"multiline-test-\r\n\theader-value";

		DBG("Whole request:\n%", request);

		csjp::HTTPRequest request2;
		request2.parse(request.toString());

		DBG("Parsed request header: %",
				request2.headers["multiline-test-header-key"]);
		VERIFY(request2.headers["multiline-test-header-key"] ==
				"multiline-test-header-value");
	}

	TESTSTEP("Parse multiline header with \\n characters in it");
	{
		csjp::HTTPRequest request("POST");

		request.headers["multiline-test-header-key"] =
			"multiline-test-\n header-value";

		DBG("Whole request:\n%", request);

		csjp::HTTPRequest request2;
		request2.parse(request.toString());

		DBG("Parsed request header: %",
				request2.headers["multiline-test-header-key"]);
		VERIFY(request2.headers["multiline-test-header-key"] ==
				"multiline-test-header-value");
	}

	TESTSTEP("Parse multiline header without "
			"starting whitespace character");
	{
		csjp::HTTPRequest request("POST");

		request.headers["multiline-test-header-key"] =
			"multiline-test-\nheader-value";

		DBG("Whole request:\n%", request);

		csjp::HTTPRequest request2;
		EXC_VERIFY(request2.parse(request.toString()),
				csjp::HttpProtocolError);
	}

	TESTSTEP("Parse multiline response header with \\n\\r character in it");
	{
		csjp::HTTPResponse response("POST");

		response.headers["multiline-test-header-key"] =
			"multiline-test-\n\r\theader-value";

		DBG("Whole response:\n%", response);

		csjp::HTTPResponse response2;
		response2.parse(response.toString());

		DBG("Parsed response header: %",
				response2.headers["multiline-test-header-key"]);
		VERIFY(response2.headers["multiline-test-header-key"] ==
				"multiline-test-header-value");
	}

	TESTSTEP("Parse multiline response header with \\n characters in it");
	{
		csjp::HTTPResponse response("POST");

		response.headers["multiline-test-header-key"] =
			"multiline-test-\n header-value";

		DBG("Whole response:\n%", response);

		csjp::HTTPResponse response2;
		response2.parse(response.toString());

		DBG("Parsed response header: %",
				response2.headers["multiline-test-header-key"]);
		VERIFY(response2.headers["multiline-test-header-key"] ==
				"multiline-test-header-value");
	}

	TESTSTEP("Parse multiline response header without "
			"starting whitespace character");
	{
		csjp::HTTPResponse response("POST");

		response.headers["multiline-test-header-key"] =
			"multiline-test-\nheader-value";

		DBG("Whole response:\n%", response);

		csjp::HTTPResponse response2;
		EXC_VERIFY(response2.parse(response.toString()),
				csjp::HttpProtocolError);
	}
}

TEST_INIT(HTTP)

	TEST_RUN(create);
	TEST_RUN(requestResponseOverSocket);
	TEST_RUN(multiLineHeaders);

TEST_FINISH(HTTP)

