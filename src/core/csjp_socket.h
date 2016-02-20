/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#ifndef CSJP_SOCKET_H
#define CSJP_SOCKET_H

/**
 * Planned usage
 *  class File : public Socket
 *  class Client : public Socket
 *  class Listener : public Socket
 *  class Server : public Socket
 *  class EPoll
 *  {
 *	addSocket(Socket & socket);
 *	Socket & wait();
 *  }
 *
 * Problem: how to avoid casting of returned Socket object to any of File, CLient, Listener, Server?
 *
 */

namespace csjp {

class Socket
{
public:
	explicit Socket(const Socket & orig) = delete;
	const Socket & operator=(const Socket &) = delete;

	Socket(Socket && temp) : file(temp.file) { temp.file = -1; }
	const Socket & operator=(Socket && temp)
	{
		file = temp.file;
		temp.file = -1;
		return *this;
	}

	explicit Socket() : file(-1) {}
	virtual ~Socket() {}
protected:
	mutable int file;
};

}

#endif
