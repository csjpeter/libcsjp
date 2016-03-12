/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#ifndef CSJP_LISTENER_H
#define CSJP_LISTENER_H

#include <csjp_socket.h>

namespace csjp {

class Server;
class Listener : public Socket
{
public:
	explicit Listener(const Listener & orig) = delete;
	const Listener & operator=(const Listener &) = delete;

	Listener(Listener && temp) : Socket(move_cast(temp)) { (void)temp; }
	const Listener & operator=(Listener && temp)
	{
		Socket::operator=(move_cast(temp));
		return *this;
	}

	Listener(const char * ip, unsigned port,
			unsigned incomingConnectionQueueLength = 0);
	virtual ~Listener() {}

	friend Server;
};

}

#endif
