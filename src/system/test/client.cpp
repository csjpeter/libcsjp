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

	void close(){ Server::close(); }
};

class SocketClient : public csjp::Client
{
public:
	SocketClient(const char *name, unsigned port) :
		csjp::Client(name, port) {}
	virtual ~SocketClient() {}

	void close(){ Client::close(); }
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
	csjp::Listener listener("127.0.0.1", 30303);

	TESTSTEP("Connecting client");
	SocketClient client("127.0.0.1", 30303);

	TESTSTEP("Server accepts connection");
	SocketServer server(listener);

	TESTSTEP("Client sends data");
	VERIFY(client.send(msg));
	VERIFY(client.totalBytesSent == 9);

	TESTSTEP("Server receives data");
	csjp::String msg2 = server.receive(msg.length);
	VERIFY(server.totalBytesReceived == 9);
	//DBG("Received msg: %", msg2);
	VERIFY(msg == msg2);

	TESTSTEP("Destructors do the rest of cleanup");
}

void TestClient::serverSendsToClosedClient()
{
	csjp::String msg("Hi there!");

	TESTSTEP("Setting up client server connection");
	csjp::Listener listener("127.0.0.1", 30303);
	SocketClient client("127.0.0.1", 30303);
	SocketServer server(listener);
	usleep(10 * 1000); // 0.01 sec
	
	TESTSTEP("Client closes");
	client.close();
	
	TESTSTEP("Register SIGPIPE handler");
	csjp::Signal pipeSignal(SIGPIPE, csjp::Signal::sigpipeHandler);

	TESTSTEP("Server write should end up in closed socket");
	EXC_VERIFY(server.send(msg), csjp::SocketClosedByPeer);
}

void TestClient::clientSendsToClosedServer()
{
	csjp::String msg("Hi there!");

	TESTSTEP("Setting up client server connection");
	csjp::Listener listener("127.0.0.1", 30303);
	SocketClient client("127.0.0.1", 30303);
	SocketServer server(listener);
	usleep(10 * 1000); // 0.01 sec
	
	TESTSTEP("Server should close after sent all data");
	server.closeOnSent = true;
	server.send("");
	// Closed socket is expected to throw InvalidState
	EXC_VERIFY(server.send(""), csjp::InvalidState);
	
	TESTSTEP("Register SIGPIPE handler");
	csjp::Signal pipeSignal(SIGPIPE, csjp::Signal::sigpipeHandler);

	TESTSTEP("Client write should fail with proper exception");
	EXC_VERIFY(client.send(msg), csjp::SocketClosedByPeer);
}

TEST_INIT(Client)

	TEST_RUN(create);
	TEST_RUN(receiveMsg);
	TEST_RUN(serverSendsToClosedClient);
	TEST_RUN(clientSendsToClosedServer);

TEST_FINISH(Client)

