/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2020 Csaszar, Peter
 */

#include <arpa/inet.h>

#include "csjp_websocket.h"

#undef DEBUG
#define DEBUG

// https://tools.ietf.org/html/rfc6455

/** TODO Idea for server protection:
 * - Do not accept longer than 1 megabytes frame,
 *   instead drop the connection and bann the ip.
 */

static unsigned g_frameLengthLimit = 2*1024*1024;

namespace csjp {

WebSocketFrame::WebSocketFrame() :
	frameLengthLimit(g_frameLengthLimit),
	finalBit(false),
	opcode(0)
{
}

WebSocketFrame::WebSocketFrame(
		WebSocketOpcode opcode) :
	frameLengthLimit(g_frameLengthLimit),
	finalBit(true),
	opcode(opcode)
{
}

WebSocketFrame::WebSocketFrame(
		WebSocketOpcode opcode,
		const AStr & payload,
		bool finalBit) :
	frameLengthLimit(g_frameLengthLimit),
	finalBit(finalBit),
	opcode(opcode),
	payload(payload)
{
}

/** If we are the client side, we should mask the payload
 * with the first 4 bytes of maskingKey. If maskingKey is empty,
 * so we are a server we should not mask it.
 */
String WebSocketFrame::toString(const char * _maskingKey, unsigned length) const
{
	Str maskingKey(_maskingKey, length);

	String frame;
	
	unsigned headerLength = 2;

	uint8_t h0 = 0;
	uint8_t h1 = 0;
	uint16_t shortLength = 0;
	uint64_t longLength = 0;

	if (finalBit)
		h0 |= 0x80;
	h0 |= (uint8_t)opcode;
	if (maskingKey.length){
		if (maskingKey.length != 4)
			throw WebSocketError("Masking key should be 4 bytes long.");
		h1 |= 0x80;
		headerLength += 4;
	}
	if (payload.length <= 125)
		h1 |= (uint8_t)(payload.length);
	else {
		headerLength += 2;
		if (payload.length <= 65535){
			h1 |= 126;
			shortLength = payload.length;
		} else {
			headerLength += 6;
			h1 |= 127;
			longLength = payload.length;
		}
	}

	// Allocating and preparing the data area
	// +4 will be good later for easier masking algorithm

	frame.setCapacity(headerLength + payload.length + 4);

	// Setting the data with ptr arithmetic for maximum efficiency

	char * ptr = (char *)frame.c_str();
	*ptr = (char)h0;
	ptr++;
	*ptr = (char)h1;
	ptr++;
	if ((h1 & 0x7f) == 126)
	{
		*(uint16_t*)ptr = htons(shortLength);
		ptr += 2;
	}
	else if ((h1 & 0x7f) == 127){
		// FIXME lets assume this host little endian (might fail on cloud hosts)
		*(uint32_t*)ptr = htonl(longLength & 0xffffffff);
		ptr += 4;
		*(uint32_t*)ptr = htonl(longLength >> 32);
		ptr += 4;
	}

	if (maskingKey.length)
	{
		// 4 bytes in single step
		* (uint32_t*) ptr = * (uint32_t*) maskingKey.c_str();
		ptr += 4;
	}

	// Add payload with memcpy for best efficiency
	memcpy(ptr, payload.c_str(), payload.length);

	if (maskingKey.length)
	{
		// FIXME take care of byte alignment
		uint32_t * iptr = (uint32_t*) ptr;
		uint32_t * endptr = (uint32_t*)(ptr + payload.length);
		uint32_t mask = * (uint32_t*) maskingKey.c_str();
		for (; iptr < endptr; iptr++)
			* iptr ^= mask;
	}

	frame.setLength(headerLength + payload.length);
	frame[headerLength + payload.length] = 0;

	DBG("frame.length to send: % header length : % payload length: %",
			frame.length, headerLength, payload.length);

	return frame;
}

/**
 * For now we only parse the full request.
 * If any bytes missing even from the body,
 * the parser will report 0 processed bytes.
 */
unsigned WebSocketFrame::parse(const Str & data)
{
	DBG("WebSocket parser available length: %", data.length);
	if (data.length < 512) {
		DBG("WebSocket parser available data: %", data);
	}

	String maskingKey;

	// two byte is the minimal frame length
	if (data.length < 2)
		return 0;

	uint8_t h0 = *(uint8_t*)(data.c_str());
	uint8_t h1 = *(uint8_t*)(data.c_str()+1);

	finalBit        = h0 & 0x80;

	opcode          = WebSocketOpcode(h0 & 0xf);

	bool maskedBit  = h1 & 0x80;
	uint64_t length = h1 & 0x7f;

	unsigned reservedBits = h0 & 0x70;
	if (reservedBits)
		throw WebSocketError("Reserved bits of WebSocket "
				"frame header should not be set.");

	unsigned payloadStartPos = 0;

	if (length < 126)
	{
		payloadStartPos = 2;

		if (maskedBit)
		{
			payloadStartPos = 6;
			maskingKey = data.read(2, 6);
		}
	}
	else if (length == 126)
	{
		payloadStartPos = 4;
		length = ntohs(*(uint16_t*)(data.c_str()+2));

		if (maskedBit)
		{
			payloadStartPos = 8;
			maskingKey = data.read(4, 8);
		}
	}
	else if (length == 127)
	{
		payloadStartPos = 10;
		uint32_t l1 = ntohl(*(uint32_t*)(data.c_str()+2));
		uint32_t l2 = ntohl(*(uint32_t*)(data.c_str()+6));
		// FIXME lets assume this host little endian (might fail on cloud hosts)
		length = l2;
		length <<= 32;
		length += l1;

		DBG("parsed l1: %, l2: %, length: % ", l1, l2, length);

		if (maskedBit)
		{
			payloadStartPos = 14;
			maskingKey = data.read(10, 14);
		}
	}

	if (frameLengthLimit < length + payloadStartPos)
		throw WebSocketLimitError("Too long frame received. "
				"Received length is %, the limit is %.",
				length, frameLengthLimit);

	if (data.length < length + payloadStartPos)
	{
		DBG("parsed length : %, header length: %, available length: %",
				length, payloadStartPos, data.length);
		if (data.length < 512) {
			DBG("Available data: %", data);
		}
		return 0;
	}

	payload = data.read(payloadStartPos, length + payloadStartPos);
	if (maskedBit)
	{
		for (unsigned i = 0, m = 0; i < payload.length; i++, m++)
			payload[i] ^= maskingKey[m % 4];
	}

	DBG("parsed length : % payload startPos : % payload.length : %",
			length, payloadStartPos, payload.length);

	return length + payloadStartPos;
}

void WebSocketFrame::clear()
{
	finalBit = false;
	opcode = 0;
	payload.clear();
}

}

