/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
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


#include "csjp_socket.h"

namespace csjp {

Socket::Socket() :
	file(-1),
	bytesAvailable(readBuffer.length),
	bytesToSend(writeBuffer.length)
{
	bzero((char *) &address, sizeof(address));
}

Socket::~Socket()
{
	if(file != -1)
		close();
}

void Socket::close(bool throws) const
{
	if(file < 0)
		return;

#if 0 // might cause TIME_WAIT on the other side, which is not always desired
	if(::shutdown(file, SHUT_RDWR) < 0){
		if(errno != ENOTCONN){
			SocketError e(errno, "Could not shutdown socket.");
			if(throws)
				throw (SocketError &&)e;

			e.note("Absorbing (not throwing) exception in "
					"socket close.");
			EXCEPTION(e);
		}
	}
#endif
	int err;
	TEMP_FAILURE_RETRY_RESULT(err, ::close(file));
	if(err){
		SocketError e(errno, "Could not close socket.");
		if(throws)
			throw (SocketError &&)e;

		e.note("Absorbing (not throwing) exception in socket close.");
		EXCEPTION(e);
	}

	file = -1;
}

bool Socket::isListening()
{
	if(file < 0)
		throw SocketClosedByPeer();

	int val;
	socklen_t len = sizeof(val);
	if(getsockopt(file, SOL_SOCKET, SO_ACCEPTCONN, &val, &len) == -1)
		throw SocketError(errno,
				"Failed to query if socket is listening.");
	if(val)
		return true;
	return false;
}

void Socket::readToBuffer()
{
	if(file < 0)
		throw SocketClosedByPeer();

	char buffer[4096];
	ssize_t readIn = 0;

	do {
		readBuffer.append(buffer, readIn);
		readIn = ::read(file, buffer, sizeof(buffer));
	} while(0 < readIn);
	
	if(readIn < 0 && errno != EAGAIN
			/*&& errno != EWOULDBLOCK*/ && errno != EINTR)
		throw SocketError(errno, "Error while reading from socket.");
}

void Socket::writeFromBuffer()
{
	if(file < 0)
		throw SocketClosedByPeer();

	long unsigned written = 0;
	int justWritten = 0;
	do {
		written += justWritten;
		justWritten = ::write(file, writeBuffer.c_str() + written,
				writeBuffer.length - written);
		if(0 <= justWritten || errno == EAGAIN
				/*|| errno == EWOULDBLOCK*/ || errno == EINTR)
			continue;
		if(errno == EPIPE){
			int errNo = errno;
			close(false);
			throw SocketClosedByPeer(errNo,	"Error after writting "
					"% bytes to socket.", written);
		}
		throw SocketError(errno, "Error after writting % bytes "
				"to socket.", written);
	} while(0 < justWritten);
	writeBuffer.chopFront(written);
}

String Socket::receive(size_t length)
{
	if(file < 0)
		throw SocketClosedByPeer();

	if(readBuffer.length < length){
		readToBuffer();
		if(readBuffer.length < length)
			throw SocketError("Can not yet read % byte long "
					"string from socket.", length);
	}

	String ret(readBuffer.c_str(), length);
	readBuffer.chopFront(length);

	return ret;
}

void Socket::send(const String & data)
{
	if(file < 0)
		throw SocketClosedByPeer();

	writeBuffer.append(data);
	//FIXME how to register itself in epoll for write event
	writeFromBuffer();
}


}

