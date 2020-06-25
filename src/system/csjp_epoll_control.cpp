/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016-2020 Csaszar, Peter
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

#include "csjp_epoll_control.h"

#define EPOLL_EDGE_TRIGGERED EPOLLET
//#define EPOLL_EDGE_TRIGGERED 0


namespace csjp {
	
EPollControl::EPollControl(unsigned maxEvents) :
	EPoll(maxEvents)
{
}

Array<EPollControl::ControlEvent> EPollControl::controlDataIn(Socket & socket)
{
	Array<EPollControl::ControlEvent> list;

	auto old = socket.totalBytesReceived;
	socket.readToBuffer();
	if(old == socket.totalBytesReceived){
		list.add(ControlEvent(socket, ControlEventCode::ClosedByPeer));
		DBG("EPollControl control fd: %, event: %",
				list.last().socket.file, list.last().name());
	}
	if(socket.file == -1){ // closed by business logic in child class
		list.add(ControlEvent(socket, ControlEventCode::ClosedByHost));
		DBG("EPollControl control fd: %, event: %",
				list.last().socket.file, list.last().name());
	}

	return list;
}

Array<EPollControl::ControlEvent> EPollControl::controlDataOut(Socket & socket)
{
	Array<EPollControl::ControlEvent> list;

	try{
		if(!socket.writeFromBuffer())
			dataIsPending(socket);
	} catch(csjp::SocketClosedByPeer & e){
		list.add(ControlEvent(socket, ControlEventCode::ClosedByPeer));
		DBG("EPollControl control fd: %, event: %",
				list.last().socket.file, list.last().name());
	}
	socket.readyToSend();
	if(socket.file == -1){ // closed by business logic in child class
		list.add(ControlEvent(socket, ControlEventCode::ClosedByHost));
		DBG("EPollControl control fd: %, event: %",
				list.last().socket.file, list.last().name());
	}

	return list;
}

Array<EPollControl::ControlEvent> EPollControl::controlError(Socket & socket)
{
	Array<EPollControl::ControlEvent> list;

	socket.checkForError();

	list.add(ControlEvent(socket, ControlEventCode::ClosedByPeer));
	DBG("EPollControl control fd: %, event: %",
			list.last().socket.file, list.last().name());

	return list;
}

Array<EPollControl::ControlEvent> EPollControl::waitAndControl(
		int timeout, enum ControlMode mode)
{
	Array<EPollControl::ControlEvent> list;
	for(auto event : wait(timeout)){
		DBG("EPollControl fd: %, event: %", event.socket.file, event.name());
		try {
			if(event.socket.file == -1)
				continue;
			switch(event.code)
			{
			case csjp::EPoll::EventCode::IncomingConnection :
				if ( ! (mode | ControlMode::Read) )
					break;
				event.socket.dataReceived();
				break;
			case csjp::EPoll::EventCode::DataIn :
				if ( ! (mode | ControlMode::Read) )
					break;
				list.join(controlDataIn(event.socket));
				break;
			case csjp::EPoll::EventCode::DataOut :
				if ( ! (mode | ControlMode::Write) )
					break;
				list.join(controlDataOut(event.socket));
				break;
			case csjp::EPoll::EventCode::ReadHangup :
			case csjp::EPoll::EventCode::Hangup :
				list.add(ControlEvent(event.socket,
					ControlEventCode::ClosedByPeer));
				DBG("EPollControl control fd: %, event: %",
				list.last().socket.file, list.last().name());
				break;
			case csjp::EPoll::EventCode::Error :
				list.join(controlError(event.socket));
				break;
			default:
				throw LogicError("Unknown EPollControl::Event");
				break;
			}
		} catch(Exception & e){
			list.add(ControlEvent(event.socket,
					  ControlEventCode::Exception,
					  move_cast(e)));
			DBG("EPollControl control fd: %, event: %",
					list.last().socket.file,
					list.last().name());
		} catch(std::exception & e){
			list.add(ControlEvent(event.socket,
					  ControlEventCode::Exception,
					  Exception(e)));
			DBG("EPollControl control fd: %, event: %",
					list.last().socket.file,
					list.last().name());
		}
	}
	return list;
}

}

