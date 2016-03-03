/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#ifndef CSJP_EPOLL_H
#define CSJP_EPOLL_H

#include <sys/epoll.h>

#include <csjp_carray.h>
#include <csjp_socket.h>

namespace csjp {

class EPoll : public Socket
{
public:
	explicit EPoll(const EPoll & orig) = delete;
	const EPoll & operator=(const EPoll &) = delete;

	EPoll(EPoll && temp) : Socket(move_cast(temp)) { (void)temp; }
	const EPoll & operator=(EPoll && temp)
	{
		Socket::operator=(move_cast(temp));
		return *this;
	}

	EPoll(unsigned maxEvents);
	virtual ~EPoll() {}

	void add(Socket &);
	void dataIsPending(Socket &);
	void noMoreDataIsPending(Socket &);
	void remove(Socket &);
	void wait(int timeout = 0);

	virtual void dataReceived()
	{
		LOG("% readBuffer content: [%]",
				__PRETTY_FUNCTION__, receive(bytesAvailable));
	}

private:
	CArray<struct epoll_event> events;
};

}

#endif
