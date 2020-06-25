/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#ifndef CSJP_EPOLL_H
#define CSJP_EPOLL_H

#include <sys/epoll.h>

#include <csjp_carray.h>
#include <csjp_socket.h>

namespace csjp {

class EPoll : public Socket
{
public:
	class Event;
public:
	explicit EPoll(const EPoll & orig) = delete;
	const EPoll & operator=(const EPoll &) = delete;

	EPoll(EPoll && temp) : Socket(move_cast(temp)) { (void)temp; }
	const EPoll & operator=(EPoll && temp)
	{
		Socket::operator=(move_cast(temp));
		return *this;
	}

	EPoll(unsigned maxEvents);
	virtual ~EPoll() {}

	void add(Socket &);
	void dataIsPending(Socket & socket);
	void noMoreDataIsPending(Socket & socket);
	void remove(Socket &);

public:
	enum ControlMode {
		Listen    = 1,
		Read      = 2,
		Write     = 4,
		ListenReadWrite = 7,
		ListenRead = 3,
		ReadWrite = 6,
	};

	Array<EPoll::Event> wait(int timeout = 0);

private:
	CArray<struct epoll_event> events;

public:
	enum class EventCode {
		IncomingConnection = 0,
		DataIn,
		DataOut,
		ReadHangup,
		Hangup,
		Error
	};

	static inline const char * eventName(EventCode code)
	{
		switch(code){
			case EventCode::IncomingConnection:
				return "IncomingConnection";
			case EventCode::DataIn: return "DataIn";
			case EventCode::DataOut: return "DataOut";
			case EventCode::ReadHangup: return "ReadHangup";
			case EventCode::Hangup: return "Hangup";
			case EventCode::Error: return "Error";
			default: return "Unknown";
		}
	}

	struct Event {
		Event(Socket & socket, EventCode code) :
			socket(socket), code(code)
		{}
		const char * name()
		{
			return EPoll::eventName(code);
		}
		Socket & socket;
		EventCode code;
	};

};

inline String &	operator<<(String & lhs, const EPoll::Event & rhs)
		{ lhs += EPoll::eventName(rhs.code); return lhs; }

}

#endif
