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

class Socket;
class SocketObserver
{
public:
	virtual void dataIsPending(Socket & socket) = 0;
};

class EPoll;
class Socket
{
public:
	explicit Socket(const Socket & orig) = delete;
	const Socket & operator=(const Socket &) = delete;

	Socket(Socket && temp) :
		file(temp.file),
		observer(temp.observer),
		totalReceived(temp.totalReceived),
		totalSent(temp.totalSent),
		bytesAvailable(readBuffer.length),
		bytesToSend(writeBuffer.length),
		totalBytesReceived(totalReceived),
		totalBytesSent(totalSent)
	{
		temp.file = -1;
		observer = 0;
		bzero((char *) &address, sizeof(address));
		totalReceived = 0;
		totalSent = 0;
	}
	const Socket & operator=(Socket && temp)
	{
		file = temp.file;
		observer = temp.observer;
		address = temp.address; // FIXME is this right?
		totalReceived = temp.totalReceived;
		totalSent = temp.totalSent;

		temp.file = -1;
		temp.observer = 0;
		bzero((char *) &address, sizeof(address));
		temp.totalReceived = 0;
		temp.totalSent = 0;

		return *this;
	}

	void close(bool throws = true) const;

	String receive(size_t length);
	void send(const String & data);

protected:
	Socket();
	virtual ~Socket();

	virtual void closedByPeer() {}
	virtual void dataReceived() = 0;
	virtual void readyToSend() {}

	void readToBuffer();
	void writeFromBuffer();

protected:
	mutable int file;
	SocketObserver * observer;
	struct sockaddr_in address;

private:
	String writeBuffer;
	String readBuffer;
	size_t totalReceived;
	size_t totalSent;

public:
	const size_t & bytesAvailable;
	const size_t & bytesToSend;
	const size_t & totalBytesReceived;
	const size_t & totalBytesSent;

	friend EPoll;
};

DECL_EXCEPTION(Exception, SocketError);

DECL_EXCEPTION(SocketError, SocketClosedByPeer);
DECL_EXCEPTION(SocketError, SocketNoneConnecting);

}

#endif
