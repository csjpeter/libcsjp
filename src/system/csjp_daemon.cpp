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

Daemon::Daemon(const Str & lockFileName, const Str & logDirName, bool exitParent) :
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

	/* Close standard file descriptors, we dont want to trash into them */
	// FIXME error handling?
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	// reopen on 0,1,2 file descriptors to avoid third party code caused crash
	stdin = fopen("/dev/null", "r+");
	stdout = fopen("/dev/null", "w+");
	stderr = fopen("/dev/null", "w+");

	umask(S_IRWXO | S_IRWXG); // Only owner shall have acces to daemon created files, etc

	setLogDir(logDirName.c_str()); // We want to log into the daemon's own log file

	LOG("Daemon process forked"); // First note into own log file to give life sign

	String pidString;
	pidString << pid;
	lockFile.write(pidString);
	lockFile.rewind();
	//lockFile.lock();

	LOG("Pid file is ready"); // Administrator can use the pid file from now on

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

