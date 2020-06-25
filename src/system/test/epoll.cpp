/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#include <unistd.h>

#include <csjp_test.h>
#include <csjp_signal.h>
#include <csjp_server.h>
#include <csjp_client.h>
#include <csjp_epoll_control.h>
#include <csjp_owner_container.h>

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
		servers.add(*this);
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
		//DBG("%", bytesAvailable);
		VERIFY(bytesAvailable == 12 || serverReceived.length);
		serverReceived = receive(bytesAvailable);
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

	void close(){ Client::close(); }
};



class SocketFloodServer : public csjp::Server
{
public:
	SocketFloodServer(const csjp::Listener/*FIXME circular dependency*/ & l) :
		csjp::Server(l) {}
	virtual ~SocketFloodServer() {}

	virtual void dataReceived()
	{
		if(4096 < bytesAvailable){ // flood protection
			TESTSTEP("Too many data received: %", bytesAvailable);
			close();
		}
	}
};

bool operator<(const SocketFloodServer & lhs, const SocketFloodServer & rhs)
{
	return &lhs < &rhs;
}

bool operator<(const csjp::Socket & lhs, const SocketFloodServer & rhs)
{
	return &lhs < &rhs;
}

bool operator<(const SocketFloodServer & lhs, const csjp::Socket & rhs)
{
	return &lhs < &rhs;
}

csjp::OwnerContainer<SocketFloodServer> floodServers;

class SocketFloodListener : public csjp::Listener
{
public:
	SocketFloodListener(const char * ip, unsigned port,
			unsigned incomingConnectionQueueLength = 0) :
		csjp::Listener(ip, port, incomingConnectionQueueLength) {}
	virtual ~SocketFloodListener() {}

	virtual void dataReceived()
	{
		floodServers.add(new SocketFloodServer(*this));
	}
	virtual void readyToSend()
	{
		VERIFY(false);
	}
};

class SocketFloodClient : public csjp::Client
{
public:
	SocketFloodClient(const char *name, unsigned port) :
		csjp::Client(name, port){}
	virtual ~SocketFloodClient() {}

	virtual void dataReceived()
	{
		clientReceived = receive(bytesAvailable);
	}

	void close(){ Client::close(); }
};

class TestEPoll
{
public:
	void create();
	void receiveMsg();
	void flood();
};

void TestEPoll::create()
{
	csjp::EPollControl epoll(1);
}

void TestEPoll::receiveMsg()
{
	TESTSTEP("Register SIGPIPE handler");
	csjp::Signal pipeSignal(SIGPIPE, csjp::Signal::sigpipeHandler);

	TESTSTEP("Creating EPoll");
	csjp::EPollControl epoll(5);

	TESTSTEP("Listening");
	SocketListener listener("127.0.0.1", 30303);
	epoll.add(listener);

	TESTSTEP("Client connecting");
	SocketClient client("127.0.0.1", 30303);
	epoll.add(client);

	TESTSTEP("EPoll waits for incoming connection");
	epoll.waitAndControl(10); // 0.01 sec
	VERIFY(servers.length == 1);
	epoll.add(servers[0]);

	TESTSTEP("Client writes");
	VERIFY(client.send("from client\n"));

	TESTSTEP("EPoll waits and server receives");
	epoll.waitAndControl(10); // 0.01 sec
	VERIFY(serverReceived == "from client\n");

	TESTSTEP("Server writes");
	VERIFY(servers[0].send("from server\n"));

	TESTSTEP("EPoll waits");
	epoll.waitAndControl(10); // 0.01 sec
	VERIFY(clientReceived == "from server\n");

	TESTSTEP("Client closes (kernel will remove it from epoll on close)");
	client.close();

	TESTSTEP("Server experiences closed by peer state");
	for(auto event : epoll.waitAndControl(10)){ // 0.01 sec
		if(event.code == csjp::EPollControl::ControlEventCode::ClosedByPeer)
			servers.removeAt(0);
	}
	VERIFY(!servers.length);

	TESTSTEP("Destructors do the rest of cleanup");
}

void TestEPoll::flood()
{
	TESTSTEP("Register SIGPIPE handler");
	csjp::Signal pipeSignal(SIGPIPE, csjp::Signal::sigpipeHandler);

	TESTSTEP("Creating EPoll, listening, connecting with client");
	csjp::EPollControl epoll(5);
	SocketFloodListener listener("127.0.0.1", 30303);
	epoll.add(listener);
	SocketFloodClient client("127.0.0.1", 30303);
	epoll.add(client);
	epoll.waitAndControl(10); // 0.01 sec
	VERIFY(floodServers.size() == 1);
	for(SocketFloodServer & server : floodServers)
		epoll.add(server);

	TESTSTEP("Client sends lot of data, server closes the connection");
	csjp::String s;
	for(int i = 0; i < 1000000; i++)
		s.cat("%,", i);
	client.send(s);
	for(auto event : epoll.waitAndControl(10)){ // 0.01 sec
		DBG("ControlEvent received: %", event.name());
		switch(event.code){
		case csjp::EPollControl::ControlEventCode::ClosedByHost:
			NOEXC_VERIFY(floodServers.query(event.socket));
			VERIFY(&floodServers[0] == &event.socket);
			floodServers.remove(event.socket);
			break;
		default:
			VERIFY(false);
			break;
		}
	}
	VERIFY(floodServers.size() == 0);

	TESTSTEP("Client receives ClosedByPeer event, thus client closes");
	for(auto event : epoll.waitAndControl(10)){ // 0.01 sec
		DBG("ControlEvent received: %", event.name());
		switch(event.code){
		case csjp::EPollControl::ControlEventCode::ClosedByPeer:
			client.close();
			break;
		case csjp::EPollControl::ControlEventCode::Exception:
			//VERIFY(false);
			EXCEPTION(event.exception);
			break;
		default:
			VERIFY(false);
		}
	}

	TESTSTEP("Both client and server closed, no event should occur");
	for(auto event : epoll.waitAndControl(10)) // 0.01 sec
		VERIFY(false);

	TESTSTEP("Destructors do the rest of cleanup");
}

TEST_INIT(EPoll)

	TEST_RUN(create);
	TEST_RUN(receiveMsg);
	TEST_RUN(flood);

TEST_FINISH(EPoll)

