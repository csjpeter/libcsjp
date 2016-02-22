/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "csjp_daemon.h"

//for more tricks: http://www.enderunix.org/docs/eng/daemon.php

namespace csjp {

String defaultDaemonLockFileName()
{
	String lockFileName;
	lockFileName.catf("/var/run/%.pid", getBinaryName());
	return lockFileName;
}

String defaultDaemonLogDirName()
{
	String logDirName;
	logDirName.catf("/var/log/");
	return logDirName;
}

Daemon::Daemon(bool exitParent) :
	lockFile(defaultDaemonLockFileName()),
	logDirName(defaultDaemonLogDirName()),
	pid(0)
{
	daemonize(exitParent);
}

Daemon::Daemon(const String & lockFileName, const String & logDirName, bool exitParent) :
	lockFile(lockFileName),
	logDirName(logDirName),
	pid(0)
{
	daemonize(exitParent);
}

void Daemon::daemonize(bool exitParent)
{
	LOG("Daemonizing: uid: %, euid: %, gid: %, egid: %",
			getuid(), geteuid(), getgid(), getegid());

	pid = fork();
	if(pid < 0)
		throw SystemError("Fork to daemonize failed. Error: %", csjp::ErrNo());
	if(0 < pid){
		if(exitParent)
			exit(EXIT_SUCCESS);
		else
			throw DaemonParent();
	}
	pid = getpid();

	/* Close standard file descriptors, we dont wnat to trash into them */
	// FIXME error handling?
	close(STDIN_FILENO); stdin = NULL;
	close(STDOUT_FILENO); stdout = NULL;
	close(STDERR_FILENO); stderr = NULL;

	// reopen on 0,1,2 file descriptors to avoid third party code caused crash
	int stdIn = open("/dev/null", O_RDWR); (void)stdIn;
	int stdOut = open("/dev/null", O_RDWR); (void)stdOut;
	int stdErr = open("/dev/null", O_RDWR); (void)stdErr;

	umask(S_IRWXO | S_IRWXG); // Only owner shall have acces to daemon created files, etc

	setLogDir(logDirName); // We want to log into the daemon's own log file

	LOG("Daemon process forked"); // First note into own log file to give life sign

	String pidString;
	pidString << pid;
	lockFile.write(pidString);
	lockFile.rewind();
	//lockFile.lock();

	LOG("Pid file is ready"); // Administrator can use the pid file from now on

	if((chdir("/")) < 0)
		throw SystemError("Failed to make root the current directory.");

	pid_t sid = setsid();
	if(sid < 0)
		throw SystemError("Failed to make the forked daemon process the group leader.");

	LOG("Daemonization is ready");
}


Daemon::~Daemon()
{
	//lockFile.unlock();
	lockFile.unlink();
}

}

