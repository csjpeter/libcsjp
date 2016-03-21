/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#undef DEBUG

#include <csjp_signal.h>
#include <csjp_server.h>
#include <csjp_client.h>
#include <csjp_epoll.h>
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
		DBG("request headers: %", request.headers.value);
		VERIFY(request.headers.value == "content-length: 4");
		VERIFY(request.headers.properties.size() == 1);
		VERIFY(request.headers["content-length"] == "4");
		DBG("request body: %", request.body);
		VERIFY(request.body = "body");

		DBG("received request:\n%", request);
		VERIFY(request == requestStr);

		csjp::HTTPResponse response(csjp::String("answer"));
		responseStr = response;
		send(response);
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
		VERIFY(response.getStatusLine() == "HTTP-1.0 200 OK");
		DBG("response status code: %", response.status());
		VERIFY(response.status() == "200");
		DBG("response reason phrase: %", response.reason());
		VERIFY(response.reason() == "OK");
		DBG("response version: %", response.version);
		VERIFY(response.version == "1.0");
		DBG("response headers: %", response.headers.value);
		VERIFY(response.headers.value == "content-length: 6");
		VERIFY(response.headers.properties.size() == 1);
		VERIFY(response.headers["content-length"] == "6");
		DBG("response body: %", response.body);
		VERIFY(response.body == "answer");

		DBG("received response:\n%", response);
		VERIFY(response == responseStr);
	}

	void sendTestRequest()
	{
		csjp::HTTPRequest request(
				csjp::String("POST"),
				csjp::String("/"),
				csjp::String("body"));
		requestStr = request;
		send(request);
		DBG("sent request:\n%", request);
	}

	csjp::HTTPResponse response;
};




class TestHTTP
{
public:
	void create();
	void requestResponseOverSocket();
};

void TestHTTP::create()
{
	HTTPListener listener(csjp::String("127.0.0.1"), 20202);
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
	csjp::Signal termSignal(SIGPIPE, csjp::Signal::sigpipeHandler);
	csjp::EPoll epoll(5);
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
		case csjp::EPoll::ControlEventCode::ClosedByPeer:
			break;
		case csjp::EPoll::ControlEventCode::ClosedByHost:
			break;
		case csjp::EPoll::ControlEventCode::Exception:
			EXCEPTION(event.exception);
			break;
		}
	}

	TESTSTEP("Server responses");
	for(int i=0; i<10 && businessServers.size(); i++)
	{
		for(auto event : epoll.waitAndControl(10)){ // 0.01 sec
			DBG("ControlEvent received: %", event.name());
			switch(event.code){
			case csjp::EPoll::ControlEventCode::ClosedByPeer:
				break;
			case csjp::EPoll::ControlEventCode::ClosedByHost:
				break;
			case csjp::EPoll::ControlEventCode::Exception:
				EXCEPTION(event.exception);
				break;
			}
		}
	}
	VERIFY(businessServers.size() == 1);

	TESTSTEP("Destructors do the rest of cleanup");
}

TEST_INIT(HTTP)

	TEST_RUN(create);
	TEST_RUN(requestResponseOverSocket);

TEST_FINISH(HTTP)
