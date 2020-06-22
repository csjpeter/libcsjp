/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2020 Csaszar, Peter
 */

#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>


#include "csjp_dup_socket.h"

namespace csjp {

DupSocket::DupSocket(const Socket & orig) : Socket()
{
	do {
		file = ::dup(orig.file);
		if (file == -1 && errno != EINTR)
			throw SocketError(errno,
					"Could not duplicate file descriptor.");
	} while (errno == EINTR);
}

}

