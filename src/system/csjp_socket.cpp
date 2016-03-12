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

#undef DEBUG

#include <string>

#include "csjp_socket.h"

namespace csjp {

Socket::Socket() :
	file(-1),
	totalReceived(0),
	totalSent(0),
	bytesAvailable(readBuffer.length),
	bytesToSend(writeBuffer.length),
	totalBytesReceived(totalReceived),
	totalBytesSent(totalSent)
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

#if 1 // might cause TIME_WAIT on the other side, which is not always desired
	DBG("Shutdown socket fd:%", file);
	if(::shutdown(file, SHUT_RDWR) < 0){
		DBG("Shutdown socket fd:% errno: %-%",
				file, errnoName(errno), errno);
		if(errno != ENOTCONN && errno != ENOTSOCK){
			SocketError e(errno, "Could not shutdown socket.");
			if(throws)
				throw (SocketError &&)e;

			e.note("Absorbing (not throwing) exception in "
					"socket close.");
			EXCEPTION(e);
		}
	}
#endif

	DBG("Closing socket: %", file);

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

bool Socket::readToBuffer()
{
	if(file < 0)
		throw InvalidState("Socket is closed.");

	//char buffer[4096];
	char buffer[4096*1024];
	ssize_t readIn = 0;

	do{
		readIn = 0;
		do {
			readBuffer.append(buffer, readIn);
			if(0 < readIn){
				totalReceived += readIn;
				dataReceived();
				if(file == -1) // closed by child class
					return true;
			}
			readIn = ::read(file, buffer, sizeof(buffer));
		} while(0 < readIn);
	} while(readIn < 0 && errno == EINTR);

	if(readIn < 0 && ( errno != EAGAIN /*&& errno != EWOULDBLOCK*/ ) )
		throw SocketError(errno, "Error while reading from socket.");

	return 0 <= readIn; // false if EAGAIN or EWOULDBLOCK happened
}

String Socket::receive(size_t length)
{
	if(file < 0)
		throw InvalidState("Socket is closed.");

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

String Socket::receiveAll()
{
	if(file < 0)
		throw InvalidState("Socket is closed.");

	readToBuffer();
	return String(move_cast(readBuffer));
}

bool Socket::writeFromBuffer()
{
	if(file < 0)
		throw InvalidState("Socket is closed.");

	long unsigned written = 0;
	int justWritten = 0;
	do {
		justWritten = 0;
		do {
			written += justWritten;
			justWritten = ::write(file,
					writeBuffer.c_str() + written,
					writeBuffer.length - written);
		} while(0 < justWritten);
	} while(justWritten < 0 && errno == EINTR);
	int errNo = errno;

	totalSent += written;
	writeBuffer.chopFront(written);

	if(justWritten < 0 && (errNo == EPIPE || errNo == ECONNRESET))
		throw SocketClosedByPeer(errNo, "Error after writting % bytes "
				"to socket.", written);

	if(justWritten < 0 && ( errNo != EAGAIN /*&& errNo != EWOULDBLOCK*/))
		throw SocketError(errNo, "Error after writting % bytes "
				"to socket.", written);

	return 0 <= justWritten; // false if EAGAIN or EWOULDBLOCK happened
}

bool Socket::send(const String & data)
{
	if(file < 0)
		throw InvalidState("Socket is closed.");

	writeBuffer.append(data);

	return writeFromBuffer();
}


}

