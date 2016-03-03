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

class SocketListener : public csjp::Listener
{
public:
	SocketListener(const char * ip, unsigned port,
			unsigned incomingConnectionQueueLength = 0) :
		csjp::Listener(ip, port, incomingConnectionQueueLength) {}
	virtual ~SocketListener() {}

	virtual void dataReceived()
	{
		//LOG("%", __PRETTY_FUNCTION__);
		servers.add((csjp::Listener&)*this);
	}
	virtual void readyToSend()
	{
		//LOG("%", __PRETTY_FUNCTION__);
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
		LOG("%", __PRETTY_FUNCTION__);
		VERIFY(bytesAvailable == 12);
		VERIFY(receive(12) == "from client\n");
		//LOG("Server receives: [%]", receive(bytesAvailable));
	}
	virtual void readyToSend()
	{
		LOG("%", __PRETTY_FUNCTION__);
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
		LOG("%", __PRETTY_FUNCTION__);
		//LOG("bytes avail: %", bytesAvailable);
		VERIFY(bytesAvailable == 12);
		VERIFY(receive(12) == "from server\n");
	}
	virtual void readyToSend()
	{
		LOG("%", __PRETTY_FUNCTION__);
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
	epoll.add(client);

	TESTSTEP("EPoll waits for incoming connection");
	epoll.wait(10); // 0.01 sec
	VERIFY(servers.length == 1);
	epoll.add(servers[0]);

	TESTSTEP("Client writes");
	client.send(csjp::String("from client\n"));
	epoll.dataIsPending(client); // FIXME how to automatize this ?

	TESTSTEP("EPoll waits");
	epoll.wait(10); // 0.01 sec

	TESTSTEP("Server writes");
	servers[0].send(csjp::String("from server\n"));
	epoll.dataIsPending(servers[0]); // FIXME how to automatize this ?

	TESTSTEP("EPoll waits");
	epoll.wait(10); // 0.01 sec

	TESTSTEP("Client closes (kernel removes it from epoll)");
	client.close();

	// FIXME how to handle this ???
	//TESTSTEP("Server receives read msg with no data ->client closed");
	//epoll.wait(10); // 0.01 sec

	TESTSTEP("Server write should fail with proper exception");
	EXC_VERIFY(servers[0].send(csjp::String("from server\n")), csjp::SocketClosedByPeer);

	TESTSTEP("Remove server from epoll");
	servers.removeAt(0);
}

TEST_INIT(EPoll)

	TEST_RUN(create);
	TEST_RUN(receiveMsg);

TEST_FINISH(EPoll)

