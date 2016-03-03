/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#ifndef CSJP_SERVER_H
#define CSJP_SERVER_H

#include <csjp_listener.h>

namespace csjp {

class Server : public Socket
{
public:
	explicit Server(const Server & orig) = delete;
	const Server & operator=(const Server &) = delete;

	Server(Server && temp) : Socket(move_cast(temp)) { (void)temp; }
	const Server & operator=(Server && temp)
	{
		Socket::operator=(move_cast(temp));
		return *this;
	}

	Server(const Listener &);
	virtual ~Server() {}
};

}

#endif
