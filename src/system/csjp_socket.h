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
 * Basic architecture:
 *  class Socket
 *  class Listener : public Socket
 *  class Server : public Socket
 *  class Client : public Socket
 *  class EPoll : public Socket
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
		totalReceived(temp.totalReceived),
		totalSent(temp.totalSent),
		bytesAvailable(readBuffer.length),
		bytesToSend(writeBuffer.length),
		totalBytesReceived(totalReceived),
		totalBytesSent(totalSent)
	{
		temp.file = -1;
		bzero((char *) &address, sizeof(address));
		totalReceived = 0;
		totalSent = 0;
	}
	const Socket & operator=(Socket && temp)
	{
		file = temp.file;
		address = temp.address; // FIXME is this right?
		totalReceived = temp.totalReceived;
		totalSent = temp.totalSent;

		temp.file = -1;
		bzero((char *) &address, sizeof(address));
		temp.totalReceived = 0;
		temp.totalSent = 0;

		return *this;
	}

	String receive(size_t length);
	bool send(const String & data); //returns false on EAGAIN or EWOULDBLOCK

	template <typename TypeReceive>
	bool receive(TypeReceive & parser)
	{
		const StringChunk input(readBuffer);
		unsigned length = parser.parse(input);
		if(!length)
			return false;
		readBuffer.chopFront(length);
		return true;
	}

protected:
	Socket();
	virtual ~Socket();
	void close(bool throws = true) const;

private:
	bool readToBuffer(); // returns false on EAGAIN or EWOULDBLOCK
	bool writeFromBuffer(); // returns false on EAGAIN or EWOULDBLOCK

	virtual void dataReceived() {} // place for child's business logic
	virtual void readyToSend() {} // place for child's business logic

protected:
	mutable int file;
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
