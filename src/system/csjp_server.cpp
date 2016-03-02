/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
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


#include "csjp_server.h"

namespace csjp {

Server::Server(const Listener & listener) : Socket()
{
	socklen_t clilen = sizeof(address);
	file = ::accept4(listener.file, 
			(struct sockaddr *) &address, &clilen, SOCK_NONBLOCK);
	if(file < 0){
		if(errno == EAGAIN || errno == EWOULDBLOCK)
			throw SocketNoneConnecting(errno, "There is no "
					"connection attempt to this socket.");
		throw SocketError(errno,
				"Socket failed to accept new connection.");
	}
}

}

