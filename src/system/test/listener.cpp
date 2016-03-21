/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#include <unistd.h>

#include <csjp_listener.h>
#include <csjp_test.h>

class TestListener
{
public:
	void create();
};

void TestListener::create()
{
	csjp::Listener listener("127.0.0.1", 10101);
	// and  just destruct ...
}

TEST_INIT(Listener)

	TEST_RUN(create);

TEST_FINISH(Listener)
