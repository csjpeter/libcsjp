/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#include <unistd.h>

#include <csjp_test.h>
#include <csjp_signal.h>
#include <csjp_server.h>
#include <csjp_client.h>
#include <csjp_epoll.h>

class SocketServer;
csjp::Array<SocketServer> servers;

csjp::String serverReceived;
csjp::String clientReceived;

class SocketListener : public csjp::Listener
{
public:
	SocketListener(const char * ip, unsigned port,
			unsigned incomingConnectionQueueLength = 0) :
		csjp::Listener(ip, port, incomingConnectionQueueLength) {}
	virtual ~SocketListener() {}

	virtual void dataReceived()
	{
		servers.add((csjp::Listener&)*this);
	}
	virtual void readyToSend()
	{
		VERIFY(false);
	}
};

class SocketServer : public csjp::Server
{
public:
	SocketServer(const csjp::Listener & l) : csjp::Server(l) {}
	virtual ~SocketServer() {}

	virtual void dataReceived()
	{
		VERIFY(bytesAvailable == 12 || serverReceived.length);
		serverReceived = receive(bytesAvailable);
	}
	virtual void readyToSend()
	{
		DBG("");
	}
};

class SocketClient : public csjp::Client
{
public:
	SocketClient(const char *name, unsigned port) :
		csjp::Client(name, port){}
	virtual ~SocketClient() {}

	virtual void dataReceived()
	{
		VERIFY(bytesAvailable == 12);
		clientReceived = receive(12);
	}
	virtual void readyToSend()
	{
		DBG("");
	}
};

class TestEPoll
{
public:
	void create();
	void receiveMsg();
};

void TestEPoll::create()
{
	csjp::EPoll epoll(1);
}

void TestEPoll::receiveMsg()
{
	TESTSTEP("Register SIGPIPE handler");
	csjp::Signal termSignal(SIGPIPE, csjp::Signal::sigpipeHandler);

	csjp::String msg("Hi there!");

	TESTSTEP("Creating EPoll");
	csjp::EPoll epoll(5);

	TESTSTEP("Listening");
	SocketListener listener("127.0.0.1", 30303);
	epoll.add(listener);

	TESTSTEP("Client connecting");
	SocketClient client("127.0.0.1", 30303);
	epoll.add(client, true); // automated epoll write registration

	TESTSTEP("EPoll waits for incoming connection");
	epoll.wait(10); // 0.01 sec
	VERIFY(servers.length == 1);
	epoll.add(servers[0], false); // manual epoll write registration

	TESTSTEP("Client writes with automated epoll write registration");
	client.send(csjp::String("from client\n"));

	TESTSTEP("EPoll waits");
	epoll.wait(10); // 0.01 sec for write event
	epoll.wait(10); // 0.01 sec for read event
	VERIFY(serverReceived == "from client\n");

	TESTSTEP("Server writes with manual epoll write registration");
	servers[0].send(csjp::String("from server\n"));
	epoll.dataIsPending(servers[0]);

	TESTSTEP("EPoll waits");
	epoll.wait(10); // 0.01 sec for write event
	epoll.wait(10); // 0.01 sec for read event
	VERIFY(clientReceived == "from server\n");

	TESTSTEP("Client closes (kernel removes it from epoll)");
	client.close();

	TESTSTEP("Server gets closed and thus removed from epoll");
	epoll.wait(10); // 0.01 sec
	EXC_VERIFY(epoll.remove(servers[0]), csjp::SocketError);
}

TEST_INIT(EPoll)

	TEST_RUN(create);
	TEST_RUN(receiveMsg);

TEST_FINISH(EPoll)

