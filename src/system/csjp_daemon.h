/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#ifndef CSJP_DAEMON_H
#define CSJP_DAEMON_H

#include <csjp_file.h>

namespace csjp {

DECL_EXCEPTION(Exception, DaemonParent);

class Daemon {
	public:
		Daemon(const Daemon & orig) = delete;
		const Daemon & operator=(const Daemon & orig) = delete;

		//Daemon(Daemon && temp) /* ...impl... */ { /*...impl...*/ }
		//const Daemon & operator=(Daemon && temp) { /*...impl...*/ return *this; }

		explicit Daemon(bool exitParent = true);
		explicit Daemon(const Str & lockFileName, const Str & logDirName,
				bool exitParent = true);
		~Daemon();

	private:
		void daemonize(bool exitParent);

	private:
		File lockFile;
		String logDirName;
		pid_t pid;
};

}

#endif
