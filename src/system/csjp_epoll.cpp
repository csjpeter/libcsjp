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

	ev.events = EPOLLIN | EPOLLOUT;//FIXME what does EPOLLOUT for listener
	ev.data.ptr = &socket;
	if(epoll_ctl(file, EPOLL_CTL_ADD, socket.file, &ev) == -1)
		throw SocketError(errno, "Failed to add a socket to epoll.");
	LOG("Added fd: %", socket.file);
}

void EPoll::remove(Socket & socket)
{
	struct epoll_event ev;

	ev.events = EPOLLIN | EPOLLOUT;//FIXME what does EPOLLOUT for listener
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

		LOG("epoll events found: % for fd: %", e, socket.file);

		if((e & EPOLLERR) == EPOLLERR) LOG("EPOLLERR");
		if((e & EPOLLHUP) == EPOLLHUP){
			LOG("EPOLLHUP");
			remove(socket);
			continue;
		}

		if((e & EPOLLRDHUP) == EPOLLRDHUP) LOG("EPOLLRDHUP");
		if((e & EPOLLPRI) == EPOLLPRI) LOG("EPOLLPRI");
		if((e & EPOLLET) == EPOLLET) LOG("EPOLLET");
		if((e & EPOLLONESHOT) == EPOLLONESHOT) LOG("EPOLLONESHOT");

		if((e & EPOLLIN) == EPOLLIN) socket.readableEvent();
		if((e & EPOLLOUT) == EPOLLOUT) socket.writeableEvent();

		/*
		if((e & EPOLLRDHUP) == EPOLLRDHUP) LOG("EPOLLRDHUP");
		if((e & EPOLLPRI) == EPOLLPRI) LOG("EPOLLPRI");
		if((e & EPOLLERR) == EPOLLERR) LOG("EPOLLERR");
		if((e & EPOLLHUP) == EPOLLHUP) LOG("EPOLLHUP");
		if((e & EPOLLET) == EPOLLET) LOG("EPOLLET");
		if((e & EPOLLONESHOT) == EPOLLONESHOT) LOG("EPOLLONESHOT");
		*/
	}
}

}

