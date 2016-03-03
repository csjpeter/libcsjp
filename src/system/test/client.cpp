/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#include <unistd.h>

#include <csjp_server.h>
#include <csjp_client.h>
#include <csjp_test.h>

class SocketServer : public csjp::Server
{
public:
	SocketServer(csjp::Listener & l) : csjp::Server(l) {}
	virtual ~SocketServer() {}

	virtual void dataReceived()
	{
		LOG("% readBuffer content: [%]",
				__PRETTY_FUNCTION__, receive(bytesAvailable));
	}
};

class SocketClient : public csjp::Client
{
public:
	SocketClient(const char *name, unsigned port) :
		csjp::Client(name, port) {}
	virtual ~SocketClient() {}

	virtual void dataReceived()
	{
		LOG("% readBuffer content: [%]",
				__PRETTY_FUNCTION__, receive(bytesAvailable));
	}
};

class TestClient
{
public:
	void create();
	void receiveMsg();
};

void TestClient::create()
{
	SocketClient client("127.0.0.1", 40404);
	// should not throw because connection should be in progress on linux
}

void TestClient::receiveMsg()
{
	csjp::String msg("Hi there!");

	TESTSTEP("Listening");
	csjp::Listener listener(csjp::String("127.0.0.1"), 30303);

	TESTSTEP("Connecting client");
	SocketClient client("127.0.0.1", 30303);

	TESTSTEP("Server accepts connection");
	SocketServer server(listener);

	TESTSTEP("Client sends data");
	client.send(msg);

	TESTSTEP("Server receives data");
	csjp::String msg2 = server.receive(msg.length);
	//LOG("Received msg: %", msg2);
	VERIFY(msg == msg2);

	// FIXME, what if client closes, and if server writes
	TESTSTEP("Server and listener closes");
	server.close();
	listener.close();
}

TEST_INIT(Client)

	TEST_RUN(create);
	TEST_RUN(receiveMsg);

TEST_FINISH(Client)

