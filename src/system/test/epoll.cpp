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

	virtual void readableEvent()
	{
		LOG("%", __PRETTY_FUNCTION__);
		servers.add((csjp::Listener&)*this);
	}
	virtual void writeableEvent()
	{
		LOG("%", __PRETTY_FUNCTION__);
	}
};

class SocketServer : public csjp::Server
{
public:
	SocketServer(const csjp::Listener & l) : csjp::Server(l) {}
	virtual ~SocketServer() {}

	virtual void readableEvent()
	{
		LOG("%", __PRETTY_FUNCTION__);
		csjp::Server::readableEvent();
		VERIFY(bytesAvailable == 12);
		VERIFY(read(12) == "from client\n");
	}
	virtual void writeableEvent()
	{
		LOG("%", __PRETTY_FUNCTION__);
		write(csjp::String("from server\n"));
		csjp::Server::writeableEvent();
	}
};

// readyToReceive and readyToSend
// actionCanBeTaken read or write

class SocketClient : public csjp::Client
{
public:
	SocketClient(const char *name, unsigned port) :
		csjp::Client(name, port) {}
	virtual ~SocketClient() {}

	virtual void readableEvent()
	{
		LOG("%", __PRETTY_FUNCTION__);
		csjp::Client::readableEvent();
		//LOG("bytes avail: %", bytesAvailable);
		VERIFY(bytesAvailable == 24);
		VERIFY(read(12) == "from server\n");
	}
	virtual void writeableEvent()
	{
		LOG("%", __PRETTY_FUNCTION__);
		write(csjp::String("from client\n"));
		csjp::Client::writeableEvent();
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
	csjp::Signal termSignal(SIGPIPE, csjp::Signal::sigpipeHandler);

	csjp::String msg("Hi there!");

	csjp::EPoll epoll(5);

	SocketListener listener("127.0.0.1", 30303);
	epoll.add(listener);
	epoll.wait(10); // 0.01 sec
	LOG("1st wait done");

	SocketClient client("127.0.0.1", 30303);
	epoll.wait(10); // 0.01 sec
	LOG("2nd wait done");
	epoll.add(servers[0]);
	epoll.add(client);
	epoll.wait(10); // 0.01 sec
	LOG("3rd wait done");

	epoll.wait(10); // 0.01 sec
	LOG("4th wait done");

	client.close();

	EXC_VERIFY(epoll.wait(10), csjp::SocketClosedByPeer); // 0.01 sec
	servers.removeAt(0);
	LOG("5th wait done");
}

TEST_INIT(EPoll)

	TEST_RUN(create);
	TEST_RUN(receiveMsg);

TEST_FINISH(EPoll)

