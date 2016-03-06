/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#include <unistd.h>

#include <csjp_signal.h>
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
		DBG("% readBuffer content: [%]", receive(bytesAvailable));
	}

	void writeFromBuffer()
	{
		Server::writeFromBuffer();
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
		DBG("readBuffer content: [%]", receive(bytesAvailable));
	}

	void writeFromBuffer()
	{
		Client::writeFromBuffer();
	}
};

class TestClient
{
public:
	void create();
	void receiveMsg();
	void serverSendsToClosedClient();
	void clientSendsToClosedServer();
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
	client.writeFromBuffer();

	TESTSTEP("Server receives data");
	csjp::String msg2 = server.receive(msg.length);
	//DBG("Received msg: %", msg2);
	VERIFY(msg == msg2);

	TESTSTEP("Server and listener closes");
	server.close();
	listener.close();
}

void TestClient::serverSendsToClosedClient()
{
	csjp::String msg("Hi there!");

	TESTSTEP("Setting up client server connection");
	csjp::Listener listener(csjp::String("127.0.0.1"), 30303);
	SocketClient client("127.0.0.1", 30303);
	SocketServer server(listener);
	usleep(10 * 1000); // 0.01 sec
	
	TESTSTEP("Client closes");
	client.close();
	
	TESTSTEP("Register SIGPIPE handler");
	csjp::Signal termSignal(SIGPIPE, csjp::Signal::sigpipeHandler);

	TESTSTEP("Server write should fail with proper exception");
	server.send(msg);
	EXC_VERIFY(server.writeFromBuffer(), csjp::SocketClosedByPeer);
}

void TestClient::clientSendsToClosedServer()
{
	csjp::String msg("Hi there!");

	TESTSTEP("Setting up client server connection");
	csjp::Listener listener(csjp::String("127.0.0.1"), 30303);
	SocketClient client("127.0.0.1", 30303);
	SocketServer server(listener);
	usleep(10 * 1000); // 0.01 sec
	
	TESTSTEP("Server closes");
	server.close();
	
	TESTSTEP("Register SIGPIPE handler");
	csjp::Signal termSignal(SIGPIPE, csjp::Signal::sigpipeHandler);

	TESTSTEP("Client write should fail with proper exception");
	client.send(msg);
	EXC_VERIFY(client.writeFromBuffer(), csjp::SocketClosedByPeer);
}

TEST_INIT(Client)

	TEST_RUN(create);
	TEST_RUN(receiveMsg);
	TEST_RUN(serverSendsToClosedClient);
	TEST_RUN(clientSendsToClosedServer);

TEST_FINISH(Client)

