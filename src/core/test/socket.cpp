/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#include <csjp_socket.h>
#include <csjp_test.h>

class TestSocket
{
public:
	void create();
};

void TestSocket::create()
{
}

TEST_INIT(Socket)

	TEST_RUN(create);

TEST_FINISH(Socket)
