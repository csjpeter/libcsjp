/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#ifndef CSJP_SIGNAL_H
#define CSJP_SIGNAL_H

#include <signal.h>

#include <csjp_file.h>

namespace csjp {

typedef void (*SignalHandlerFunction)(int, siginfo_t *, void *);

class Signal {
	public:
		Signal(const Signal & orig) = delete;
		const Signal & operator=(const Signal & orig) = delete;

		//Signal(Signal && temp) /* ...impl... */ { /*...impl...*/ }
		//const Signal & operator=(Signal && temp) { /*...impl...*/ return *this; }

		explicit Signal(int signum, SignalHandlerFunction func);
		~Signal();

		static void sigtermHandler(int, siginfo_t *, void *);
		static void sigintHandler(int, siginfo_t *, void *);
		static void sigkillHandler(int, siginfo_t *, void *);
		static void sigpipeHandler(int, siginfo_t *, void *);

	public:
		static bool sigTermReceived;
		static bool sigIntReceived;
		static bool sigKillReceived;
		static bool sigPipeReceived;

	private:
		const int signum;
		SignalHandlerFunction func;
		struct sigaction action;
		struct sigaction oldAction;
};

}

#endif
