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

	virtual void readableEvent()
	{
		csjp::Server::readableEvent();
	}
	virtual void writableEvent()
	{
		csjp::Server::writableEvent();
	}
};

class SocketClient : public csjp::Client
{
public:
	SocketClient(const char *name, unsigned port) :
		csjp::Client(name, port) {}
	virtual ~SocketClient() {}

	virtual void readableEvent()
	{
		csjp::Client::readableEvent();
	}
	virtual void writableEvent()
	{
		csjp::Client::writableEvent();
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

	csjp::Listener listener(csjp::String("127.0.0.1"), 30303);

	SocketClient client("127.0.0.1", 30303);
	//usleep(10 * 1000); // 0.01 sec

	SocketServer server(listener);
	server.write(msg);
	server.writableEvent();

	while(client.bytesAvailable < 9){
		client.readableEvent();
		usleep(10 * 1000); // 0.01 sec
	}
	csjp::String msg2 = client.read(client.bytesAvailable);
	LOG("Received msg: %", msg2);
	VERIFY(msg == msg2);

	server.close();
	listener.close();
	//usleep(10 * 1000); // 0.01 sec
}

TEST_INIT(Client)

	TEST_RUN(create);
	TEST_RUN(receiveMsg);

TEST_FINISH(Client)

