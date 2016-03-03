/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#ifndef CSJP_SOCKET_H
#define CSJP_SOCKET_H

#include <netinet/in.h>
#include <string>
#include <csjp_string_chunk.h>

/**
 * Usefull info:
 * http://hea-www.harvard.edu/~fine/Tech/addrinuse.html
 * http://www.softlab.ntua.gr/facilities/documentation/unix/unix-socket-faq/unix-socket-faq.html#toc4
 *
 * Planned usage
 *  class File : public Socket
 *  class Client : public Socket
 *  class Listener : public Socket
 *  class Server : public Socket
 *  class EPoll : public Socket
 *  {
 *	addSocket(Socket & socket);
 *	Socket & wait();
 *  }
 *
 * Problem: how to avoid casting of returned Socket object to any of File, CLient, Listener, Server?
 *
 */

namespace csjp {

class EPoll;
class Socket
{
public:
	explicit Socket(const Socket & orig) = delete;
	const Socket & operator=(const Socket &) = delete;

	Socket(Socket && temp) :
		file(temp.file),
		bytesAvailable(readBuffer.length),
		bytesToSend(writeBuffer.length)
	{
		temp.file = -1;
	}
	const Socket & operator=(Socket && temp)
	{
		file = temp.file;
		temp.file = -1;
		return *this;
	}

	void close(bool throws = true) const;

	String receive(size_t length);
	void send(const String & data);

protected:
	Socket();
	virtual ~Socket();

	virtual void dataReceived() = 0;
	virtual void readyToSend(){};

private:
	bool isListening();
	void readToBuffer();
	void writeFromBuffer();

protected:
	mutable int file;
	struct sockaddr_in address;

private:
	String writeBuffer;
	String readBuffer;
public:
	const size_t & bytesAvailable;
	const size_t & bytesToSend;

	friend EPoll;
};

DECL_EXCEPTION(Exception, SocketError);

DECL_EXCEPTION(SocketError, SocketClosedByPeer);
DECL_EXCEPTION(SocketError, SocketNoneConnecting);

}

#endif
