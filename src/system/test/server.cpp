/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#include <unistd.h>

#include <csjp_server.h>
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

class TestServer
{
public:
	void create();
};

void TestServer::create()
{
	csjp::Listener listener(csjp::String("127.0.0.1"), 20202);
	try{
		SocketServer server(listener);
		VERIFY(false);
	} catch(csjp::SocketNoneConnecting & e){
		//EXCEPTION(e);
		VERIFY(true);
	} catch(csjp::SocketError & e){
		EXCEPTION(e);
		VERIFY(false);
	}
}

TEST_INIT(Server)

	TEST_RUN(create);

TEST_FINISH(Server)
