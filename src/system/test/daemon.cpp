/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#ifndef DEBUG
#define DEBUG
#endif

#include <csjp_test.h>
#include <csjp_daemon.h>

#include <signal.h>
#include <unistd.h>

class TestDaemon
{
public:
	void daemonize();
};

void TestDaemon::daemonize()
{
	csjp::File testDir(TESTDIR);
	if(!testDir.exists() || !testDir.isDir())
		FATAL("This test requires the directory " TESTDIR " to exist and to be writable.");

	csjp::String pidFileName(TESTDIR "/test_daemon.pid");
	csjp::String logDirName(TESTDIR);

	try{
		TESTSTEP("Construct daemon");
		csjp::Daemon daemon(pidFileName, logDirName, false);

		TESTSTEP("Running daemon loop until term signal is received");
		while(true){
			DBG("Test daemon is running");
			usleep(100 * 1000); // wait 0.1 sec
		}
	} catch (csjp::DaemonParent & e){

		TESTSTEP("Open pidfile to get the daemon's pid");
		usleep(250 * 1000); // wait 0.25 sec, gives time to the daemon to write into pidfile
		csjp::File pidFile(pidFileName);
		auto pidStr = pidFile.readAll();
		pid_t pid; pid <<= pidStr;
                DBG("We have the daemon child with pid %.", pid);

		TESTSTEP("Sending SIGTERM to the daemon");
		if(kill(pid, SIGTERM))
			throw csjp::SystemError(errno, "Failed to kill the test daemon");
	}
}


TEST_INIT(Daemon)

	TEST_RUN(daemonize);

TEST_FINISH(Daemon)
