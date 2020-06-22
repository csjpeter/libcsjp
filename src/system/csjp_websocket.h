/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2020 Csaszar, Peter
 */

#pragma once

#include <csjp_json.h>
#include <csjp_server.h>
#include <csjp_client.h>

namespace csjp {

DECL_EXCEPTION(SocketError, WebSocketError);
DECL_EXCEPTION(WebSocketError, WebSocketLimitError);

class WebSocketOpcode
{
public:
	enum class Enum;

	WebSocketOpcode(WebSocketOpcode::Enum code) : code(code) {}
	WebSocketOpcode(unsigned code)
	{
		switch (code)
		{
		case 0x0 : this->code = WebSocketOpcode::Enum::Continue; break;
		case 0x1 : this->code = WebSocketOpcode::Enum::Text; break;
		case 0x2 : this->code = WebSocketOpcode::Enum::Binary; break;
		case 0x8 : this->code = WebSocketOpcode::Enum::Close; break;
		case 0x9 : this->code = WebSocketOpcode::Enum::Ping; break;
		case 0xa : this->code = WebSocketOpcode::Enum::Pong; break;
		default :
			throw WebSocketError("Invalid opcode % received", code);
		}
	}
	operator int () const { return (int)code; }

	Enum code;

	enum class Enum
	{
		Continue = 0x0,
		Text = 0x1,
		Binary = 0x2,
		Close = 0x8,
		Ping = 0x9,
		Pong = 0xa
	};

	inline const char * text() const
	{
		switch(code)
		{
		case Enum::Continue : return "Continuation Frame";
		case Enum::Text : return "Text Frame";
		case Enum::Binary : return "Binary Frame";
		case Enum::Close : return "Connection Close Frame";
		case Enum::Ping : return "Ping Frame";
		case Enum::Pong : return "Pong";
		default: break;
		}
		throw InvalidState(
				"WebSocket Opcode % is unknown and could not "
				"be converted to text.", (int)code);
	}
};


class WebSocketFrame
{
public:
	explicit WebSocketFrame(const WebSocketFrame & orig) = delete;
	const WebSocketFrame & operator=(const WebSocketFrame &) = delete;

	WebSocketFrame(WebSocketFrame && temp) :
		finalBit(temp.finalBit),
		opcode(temp.opcode),
		payload(move_cast(temp.payload))
	{}
	const WebSocketFrame & operator=(WebSocketFrame && temp)
	{
		finalBit = temp.finalBit;
		opcode = temp.opcode;
		payload = move_cast(temp.payload);
		return *this;
	}

	WebSocketFrame();

	WebSocketFrame(WebSocketOpcode opcode);

	WebSocketFrame(WebSocketOpcode opcode,
			const AStr & payload,
			bool finalBit = true);

	String toString(const char * maskingKey, unsigned length) const;
	String toString(const char * maskingKey = "") const
		{ return toString(maskingKey, strlen(maskingKey)); }
	String toString(const AStr & maskingKey) const
		{ return toString(maskingKey.c_str(), maskingKey.length); }

	unsigned parse(const Str & data);

	void clear();

	unsigned frameLengthLimit;

	bool finalBit;          // 1 bit Indicates the last fragment of a message
	WebSocketOpcode opcode; // 4 bit
	String payload;
};


inline bool operator==(const csjp::WebSocketOpcode a,
		const csjp::WebSocketOpcode::Enum b) { return a.code == b; }
inline bool operator==(const csjp::WebSocketOpcode::Enum a,
		const csjp::WebSocketOpcode b) { return a == b.code; }

inline String &	operator<<(csjp::String & lhs, const csjp::WebSocketFrame & rhs)
		{ lhs += rhs.toString(); return lhs; }

inline String &	operator<<=(csjp::String & lhs, const csjp::WebSocketFrame & rhs)
		{ lhs = rhs.toString(); return lhs; }
inline String &	operator<<=(csjp::String & lhs, const csjp::WebSocketOpcode & rhs)
		{ lhs <<= (int)(rhs.code); return lhs; }


}

