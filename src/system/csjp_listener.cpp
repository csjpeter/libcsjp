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


#include "csjp_listener.h"

namespace csjp {

Listener::Listener(const char * ip, unsigned port,
		unsigned incomingConnectionQueueLength ) :
	Socket()
{
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if(inet_pton(AF_INET, ip, &address.sin_addr.s_addr) != 1)
		throw SocketError("Failed to parse or convert '%' to binary "
				"ip number.", ip);

	file = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if(file < 0)
		throw SocketError(errno, "Can not open listener socket "
				"to bind to port %.", port);

	int ov = 1;
	if(setsockopt(file, SOL_SOCKET, SO_REUSEADDR, &ov, sizeof(ov)) == -1){
		close(false);
		throw SocketError("Failed to set socket option SO_REUSEADDR.");
	}

	if(bind(file, (struct sockaddr *)&address, sizeof(address)) < 0){
		int errNo = errno;
		close(false);
		throw SocketError(errNo,
				"Can not bind socket to port %.", port);
	}

	if(::listen(file, incomingConnectionQueueLength) < 0){
		int errNo = errno;
		close();
		throw SocketError(errNo, "Listening error on port %.", port);
	}
}

}

