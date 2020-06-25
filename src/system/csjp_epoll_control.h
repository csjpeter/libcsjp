/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016-2020 Csaszar, Peter
 */

#ifndef CSJP_EPOLL_CONTROL_H
#define CSJP_EPOLL_CONTROL_H

#include <sys/epoll.h>

#include <csjp_carray.h>
#include <csjp_socket.h>
#include <csjp_epoll.h>

namespace csjp {

class EPollControl : public EPoll
{
public:
	class ControlEvent;
public:
	explicit EPollControl(const EPollControl & orig) = delete;
	const EPollControl & operator=(const EPollControl &) = delete;

	EPollControl(EPollControl && temp) : EPoll(move_cast(temp)) { (void)temp; }
	const EPollControl & operator=(EPollControl && temp)
	{
		EPoll::operator=(move_cast(temp));
		return *this;
	}

	EPollControl(unsigned maxEvents);
	virtual ~EPollControl() {}

public:
	enum ControlMode {
		Listen    = 1,
		Read      = 2,
		Write     = 4,
		ListenReadWrite = 7,
		ListenRead = 3,
		ReadWrite = 6,
	};

	Array<EPollControl::ControlEvent> controlDataIn(Socket & socket);
	Array<EPollControl::ControlEvent> controlDataOut(Socket & socket);
	Array<EPollControl::ControlEvent> controlError(Socket & socket);
	Array<EPollControl::ControlEvent> waitAndControl(
			int timeout = 0,
			enum ControlMode mode = ControlMode::ListenReadWrite);

public:

	enum class ControlEventCode {
		ClosedByPeer = 0,
		ClosedByHost,
		Exception
	};

	static inline const char * controlEventName(ControlEventCode code)
	{
		switch(code){
		case ControlEventCode::ClosedByPeer: return "ClosedByPeer";
		case ControlEventCode::ClosedByHost: return "ClosedByHost";
		case ControlEventCode::Exception: return "Exception";
		default: return "Unknown";
		}
	}

	struct ControlEvent {
		ControlEvent(ControlEvent & event) :
			socket(event.socket),
			code(event.code),
			exception(move_cast(event.exception))
		{}
		ControlEvent(Socket & socket,
				ControlEventCode code,
				Exception && exception = Exception()) :
			socket(socket),
			code(code),
			exception(move_cast(exception))
		{}
		const char * name()
		{
			return EPollControl::controlEventName(code);
		}
		Socket & socket;
		ControlEventCode code;
		Exception exception;
	};
};

inline String &	operator<<(String & lhs, const EPollControl::ControlEvent & rhs)
		{ lhs += EPollControl::controlEventName(rhs.code); return lhs; }

}

#endif
