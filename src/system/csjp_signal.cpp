/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#include <strings.h>

#include "csjp_signal.h"

namespace csjp {

bool Signal::sigTermReceived = false;
bool Signal::sigIntReceived = false;
bool Signal::sigKillReceived = false;
bool Signal::sigPipeReceived = false;

void Signal::sigtermHandler(int signum, siginfo_t *info, void *context)
{
	(void)signum;
	(void)info;
	(void)context;

	Signal::sigTermReceived = true;
	LOG("Terminate signal (SIGTERM) received.");
}

void Signal::sigintHandler(int signum, siginfo_t *info, void *context)
{
	(void)signum;
	(void)info;
	(void)context;

	Signal::sigIntReceived = true;
	LOG("Interrupt signal (SIGINT) received.");
}

void Signal::sigkillHandler(int signum, siginfo_t *info, void *context)
{
	(void)signum;
	(void)info;
	(void)context;

	Signal::sigKillReceived = true;
	LOG("Kill signal (SIGKILL) received.");
}

void Signal::sigpipeHandler(int signum, siginfo_t *info, void *context)
{
	(void)signum;
	(void)info;
	(void)context;

	Signal::sigPipeReceived = true;
	LOG("Pipe signal (SIGPIPE) received.");
}

Signal::Signal(int signum, SignalHandlerFunction func) :
	signum(signum),
	func(func)
{
        bzero((char *) &action, sizeof(action));
        bzero((char *) &oldAction, sizeof(oldAction));

        action.sa_flags = SA_RESTART | SA_SIGINFO;
        action.sa_sigaction = func;

        if(sigaction(signum, &action, &oldAction) < 0)
                throw SystemError("Could not set terminate signal handler. Error: %",
				csjp::ErrNo());
}

Signal::~Signal()
{
        if(sigaction(signum, &oldAction, NULL) < 0){
                SystemError e(errno, "Could not change SIGTERM handler to default.");
		e.note("Absorbing (not throwing) exception from Signal destructor.");
		EXCEPTION(e);
	}
}

}

