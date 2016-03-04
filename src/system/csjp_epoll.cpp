/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
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


#include "csjp_epoll.h"

namespace csjp {
	
EPoll::EPoll(unsigned maxEvents) :
	Socket(),
	events(maxEvents)
{
	file = epoll_create1(0/*EPOLL_CLOEXEC*/);
	if(file == -1)
		throw SocketError(errno, "epoll_create1() failed");
}

void EPoll::add(Socket & socket)
{
	struct epoll_event ev;

	ev.events = EPOLLIN;
	ev.data.ptr = &socket;
	if(epoll_ctl(file, EPOLL_CTL_ADD, socket.file, &ev) == -1)
		throw SocketError(errno, "Failed to add a socket to epoll.");
	//DBG("Added fd: %", socket.file);
}

void EPoll::dataIsPending(Socket & socket)
{
	struct epoll_event ev;

	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.ptr = &socket;
	if(epoll_ctl(file, EPOLL_CTL_MOD, socket.file, &ev) == -1)
		throw SocketError(errno, "Failed to modify a socket in epoll.");
	//DBG("Modified for read and write events; fd: %", socket.file);
}

void EPoll::noMoreDataIsPending(Socket & socket)
{
	struct epoll_event ev;

	ev.events = EPOLLIN;
	ev.data.ptr = &socket;
	if(epoll_ctl(file, EPOLL_CTL_MOD, socket.file, &ev) == -1)
		throw SocketError(errno, "Failed to modify a socket in epoll.");
	//DBG("Modified for read only events; fd: %", socket.file);
}

void EPoll::remove(Socket & socket)
{
	struct epoll_event ev;

	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.ptr = &socket;
	if(epoll_ctl(file, EPOLL_CTL_DEL, socket.file, &ev) == -1)
		throw SocketError(errno,
				"Failed to remove a socket from epoll.");
}

void EPoll::wait(int timeout)
{
	int nfds = epoll_wait(file, events.ptr, events.size, timeout);
	if(nfds == -1)
		throw SocketError(errno, "epoll wait failure");

	for(int i = 0; i < nfds; ++i){
		Socket & socket = *((Socket*)(events.ptr[i].data.ptr));
		int e = events.ptr[i].events;

		//DBG("epoll events found: % for fd: %", e, socket.file);

		if((e & EPOLLERR) == EPOLLERR) DBG("EPOLLERR");
		if((e & EPOLLHUP) == EPOLLHUP){
			DBG("EPOLLHUP");
			remove(socket);
			continue;
		}
#if 0
		if((e & EPOLLRDHUP) == EPOLLRDHUP) DBG("EPOLLRDHUP");
		if((e & EPOLLPRI) == EPOLLPRI) DBG("EPOLLPRI");
		if((e & EPOLLET) == EPOLLET) DBG("EPOLLET");
		if((e & EPOLLONESHOT) == EPOLLONESHOT) DBG("EPOLLONESHOT");
#endif
		if((e & EPOLLIN) == EPOLLIN){
			if(!socket.isListening()){
				//DBG("EPoll reads socket fd: %", socket.file);
				auto old = socket.bytesAvailable;
				socket.readToBuffer();
				if(old == socket.bytesAvailable){ // peer closed
					socket.close();
					socket.closedByPeer();
					continue;
				}
			}
			socket.dataReceived();
		}
		if((e & EPOLLOUT) == EPOLLOUT){
			socket.writeFromBuffer();
			socket.readyToSend();
			if(socket.bytesToSend == 0)
				noMoreDataIsPending(socket);
		}
	}
}

}

