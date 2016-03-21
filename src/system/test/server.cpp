/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#include <unistd.h>

#include <csjp_server.h>
#include <csjp_test.h>

class TestServer
{
public:
	void create();
};

void TestServer::create()
{
	csjp::Listener listener("127.0.0.1", 20202);
	try{
		csjp::Server server(listener);
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
