/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012 Csaszar, Peter
 */

#include <csjp_mutex.h>
#include <csjp_string.h>
#include <csjp_test.h>

class TestMutex
{
public:
	void lock();
};

void TestMutex::lock()
{
	csjp::Mutex m;
	csjp::Mutex::Lock lm(m);

	/* Dont remove below line until we have a specific log/macro test app. */
	LOG("Mutex is locked now in the test: ", __PRETTY_FUNCTION__);
	LOG("Testing LOG macros with a simple string.");
}

TEST_INIT(Mutex)

	TEST_RUN(lock);

TEST_FINISH(Mutex)
