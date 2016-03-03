/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#ifndef CSJP_CLIENT_H
#define CSJP_CLIENT_H

#include <csjp_socket.h>

namespace csjp {

class Client : public Socket
{
public:
	explicit Client(const Client & orig) = delete;
	const Client & operator=(const Client &) = delete;

	Client(Client && temp) : Socket(move_cast(temp)) { (void)temp; }
	const Client & operator=(Client && temp)
	{
		Socket::operator=(move_cast(temp));
		return *this;
	}

	Client(const char *name, unsigned port);
	virtual ~Client() {}
};

}

#endif
