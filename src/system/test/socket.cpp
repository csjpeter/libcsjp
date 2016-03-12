/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#include <csjp_socket.h>
#include <csjp_test.h>

class SocketChild : public csjp::Socket
{
public:
	SocketChild() : Socket() {}
	virtual ~SocketChild() {}
};

class TestSocket
{
public:
	void create();
};

void TestSocket::create()
{
	SocketChild socket;
	// and  just destruct ...
}

TEST_INIT(Socket)

	TEST_RUN(create);

TEST_FINISH(Socket)
