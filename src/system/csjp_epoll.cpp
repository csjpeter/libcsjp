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
	int nfds = epoll_wait(file, events.ptr, events.size, timeout);
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

Array<EPoll::ControlEvent> EPoll::controlDataIn(Socket & socket)
{
	Array<EPoll::ControlEvent> list;

	auto old = socket.totalBytesReceived;
	socket.readToBuffer();
	if(old == socket.totalBytesReceived){
		list.add(ControlEvent(socket, ControlEventCode::ClosedByPeer));
		DBG("EPoll control fd: %, event: %",
				list.last().socket.file, list.last().name());
	}
	if(socket.file == -1){ // closed by business logic in child class
		list.add(ControlEvent(socket, ControlEventCode::ClosedByHost));
		DBG("EPoll control fd: %, event: %",
				list.last().socket.file, list.last().name());
	}

	return list;
}

Array<EPoll::ControlEvent> EPoll::controlDataOut(Socket & socket)
{
	Array<EPoll::ControlEvent> list;

	try{
		if(!socket.writeFromBuffer())
			dataIsPending(socket);
	} catch(csjp::SocketClosedByPeer & e){
		list.add(ControlEvent(socket, ControlEventCode::ClosedByPeer));
		DBG("EPoll control fd: %, event: %",
				list.last().socket.file, list.last().name());
	}
	socket.readyToSend();
	if(socket.file == -1){ // closed by business logic in child class
		list.add(ControlEvent(socket, ControlEventCode::ClosedByHost));
		DBG("EPoll control fd: %, event: %",
				list.last().socket.file, list.last().name());
	}

	return list;
}

Array<EPoll::ControlEvent> EPoll::controlError(Socket & socket)
{
	Array<EPoll::ControlEvent> list;

	int errNo = 0;
	socklen_t len = sizeof(errNo);
	if(getsockopt(socket.file, SOL_SOCKET, SO_ERROR, &errNo, &len) < 0)
		throw SocketError(errno, "Failed to get socket error "
				"option after EPOLLERR event.");

	if(errNo != 0 && errNo != EPIPE && errNo != ECONNRESET)
		throw SocketError(errNo, "Detected after EPOLLERR event.");

	list.add(ControlEvent(socket, ControlEventCode::ClosedByPeer));
	DBG("EPoll control fd: %, event: %",
			list.last().socket.file, list.last().name());

	return list;
}

Array<EPoll::ControlEvent> EPoll::waitAndControl(int timeout)
{
	Array<EPoll::ControlEvent> list;
	for(auto event : wait(timeout)){
		DBG("EPoll fd: %, event: %", event.socket.file, event.name());
		try {
			if(event.socket.file == -1)
				continue;
			switch(event.code)
			{
			case csjp::EPoll::EventCode::IncomingConnection :
				event.socket.dataReceived();
				break;
			case csjp::EPoll::EventCode::DataIn :
				list.join(controlDataIn(event.socket));
				break;
			case csjp::EPoll::EventCode::DataOut :
				list.join(controlDataOut(event.socket));
				break;
			case csjp::EPoll::EventCode::ReadHangup :
			case csjp::EPoll::EventCode::Hangup :
				list.add(ControlEvent(event.socket,
					ControlEventCode::ClosedByPeer));
				DBG("EPoll control fd: %, event: %",
				list.last().socket.file, list.last().name());
				break;
			case csjp::EPoll::EventCode::Error :
				list.join(controlError(event.socket));
				break;
			default:
				throw LogicError("Unknown EPoll::Event");
				break;
			}
		} catch(Exception & e){
			list.add(ControlEvent(event.socket,
					  ControlEventCode::Exception,
					  move_cast(e)));
			DBG("EPoll control fd: %, event: %",
					list.last().socket.file,
					list.last().name());
		} catch(std::exception & e){
			list.add(ControlEvent(event.socket,
					  ControlEventCode::Exception,
					  Exception(e)));
			DBG("EPoll control fd: %, event: %",
					list.last().socket.file,
					list.last().name());
		}
	}
	return list;
}

}

