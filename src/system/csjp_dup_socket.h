/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2020 Csaszar, Peter
 */

#ifndef CSJP_DUP_SOCKET_H
#define CSJP_DUP_SOCKET_H

#include <csjp_socket.h>

namespace csjp {

class DupSocket : public Socket
{
public:
	explicit DupSocket(const DupSocket & orig) = delete;
	const DupSocket & operator=(const DupSocket &) = delete;

	DupSocket(DupSocket && temp) : Socket(move_cast(temp)) { (void)temp; }
	const DupSocket & operator=(DupSocket && temp)
	{
		Socket::operator=(move_cast(temp));
		return *this;
	}

	DupSocket(const Socket &);
	virtual ~DupSocket() {}
};

}

#endif
