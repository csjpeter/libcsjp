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


#include "csjp_client.h"

namespace csjp {

Client::Client(const char *name, unsigned port) :
	Socket()
{
	const struct hostent *server = gethostbyname(name);
	if(!server)
		throw SocketError(errno, "Could not resolve name: %.", name);

	address.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&address.sin_addr.s_addr,
			server->h_length);
	address.sin_port = htons(port);
	
	file = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if(file < 0)
		throw SocketError(errno,
				"Can not open socket to connect to %:%.",
				name, port);

	int ret = ::connect(file, (struct sockaddr *)&address, sizeof(address));
	if(ret < 0 && errno != EINPROGRESS){
		int errNo = errno;
		char addr[256];
		if(!inet_ntop(AF_INET, &address.sin_addr.s_addr,
				  addr, sizeof(addr)))
			addr[0] = 0;
		close(false);
		throw SocketError(errNo, "Can not connect to %:% (%:%).",
				name, port, addr, port);
	}
}

}

