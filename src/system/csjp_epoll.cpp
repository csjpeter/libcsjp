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

#undef DEBUG

#include <string>

#include "csjp_epoll.h"

#define EPOLL_EDGE_TRIGGERED EPOLLET
//#define EPOLL_EDGE_TRIGGERED 0


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

	ev.events = EPOLL_EDGE_TRIGGERED | EPOLLIN | EPOLLOUT;
	ev.data.ptr = &socket;
	if(epoll_ctl(file, EPOLL_CTL_ADD, socket.file, &ev) == -1)
		throw SocketError(errno, "Failed to add a socket to epoll.");
	//DBG("Added fd: %", socket.file);
}

void EPoll::dataIsPending(Socket & socket)
{
       struct epoll_event ev;

       ev.events = EPOLL_EDGE_TRIGGERED | EPOLLIN | EPOLLOUT;
       ev.data.ptr = &socket;
       if(epoll_ctl(file, EPOLL_CTL_MOD, socket.file, &ev) == -1)
               throw SocketError(errno, "Failed to modify a socket in epoll.");
       //DBG("Modified for read and write events; fd: %", socket.file);
}

void EPoll::noMoreDataIsPending(Socket & socket)
{
       struct epoll_event ev;

       ev.events = EPOLL_EDGE_TRIGGERED | EPOLLIN;
       ev.data.ptr = &socket;
       if(epoll_ctl(file, EPOLL_CTL_MOD, socket.file, &ev) == -1)
               throw SocketError(errno, "Failed to modify a socket in epoll.");
       //DBG("Modified for read only events; fd: %", socket.file);
}

void EPoll::remove(Socket & socket)
{
	struct epoll_event ev;

	ev.events = EPOLL_EDGE_TRIGGERED | EPOLLIN | EPOLLOUT;
	ev.data.ptr = &socket;
	if(epoll_ctl(file, EPOLL_CTL_DEL, socket.file, &ev) == -1)
		throw SocketError(errno,
				"Failed to remove a socket from epoll.");
}

const char * epollEventMap(int event)
{
	switch(event)
	{
		case EPOLLERR : return "EPOLLERR"; break;
		case EPOLLHUP : return "EPOLLHUP"; break;
		case EPOLLIN : return "EPOLLIN"; break;
		case EPOLLOUT : return "EPOLLOUT"; break;
		case EPOLLRDHUP : return "EPOLLRDHUP"; break;
		case EPOLLPRI : return "EPOLLPRI"; break;
		case EPOLLET : return "EPOLLET"; break;
		case EPOLLONESHOT : return "EPOLLONESHOT"; break;
		default: return "UNKNOWN"; break;
	}
}

bool isSocketListening(int file)
{
	if(file < 0)
		throw InvalidState("Socket is closed.");

	int val;
	socklen_t len = sizeof(val);
	if(getsockopt(file, SOL_SOCKET, SO_ACCEPTCONN, &val, &len) == -1)
		throw SocketError(errno,
				"Failed to query if socket is listening.");
	if(val)
		return true;
	return false;
}

Array<EPoll::Event> EPoll::wait(int timeout)
{
	int nfds = 0;
	TEMP_FAILURE_RETRY_RESULT(nfds, epoll_wait(file, events.ptr, events.size, timeout));
	if(nfds == -1)
		throw SocketError(errno, "epoll wait failure");

	Array<EPoll::Event> list;
	for(int i = 0; i < nfds; ++i){
		Socket & socket = *((Socket*)(events.ptr[i].data.ptr));
		int e = events.ptr[i].events;

		//DBG("epoll event: %-% fd:%",epollEventMap(e),e,socket.file);

		if((e & EPOLLRDHUP) == EPOLLRDHUP){
			list.add(Event(socket, EventCode::ReadHangup));
			continue;
		}

		if((e & EPOLLHUP) == EPOLLHUP){
			list.add(Event(socket, EventCode::Hangup));
			continue;
		}

		if((e & EPOLLERR) == EPOLLERR){
			list.add(Event(socket, EventCode::Error));
			continue;
		}

		bool listening = isSocketListening(socket.file);
		if(listening && ((e & EPOLLIN) == EPOLLIN))
			list.add(Event(socket, EventCode::IncomingConnection));
		if(listening)
			continue;

		if((e & EPOLLIN) == EPOLLIN)
			list.add(Event(socket, EventCode::DataIn));

		if((e & EPOLLOUT) == EPOLLOUT)
			list.add(Event(socket, EventCode::DataOut));
	}

	return list;
}

}

