/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2020 Csaszar, Peter
 */

#undef DEBUG
#define DEBUG

#include <csjp_signal.h>
#include <csjp_server.h>
#include <csjp_client.h>
#include <csjp_epoll.h>
#include <csjp_owner_container.h>
#include <csjp_websocket.h>
#include <csjp_test.h>

class TestWebSocket
{
public:
	void pingpong();
	void textShort();
	void textMedium();
	void textLong();
	void close();
};

void TestWebSocket::pingpong()
{
	csjp::WebSocketFrame client;
	csjp::WebSocketFrame server;

	client = csjp::WebSocketFrame(csjp::WebSocketOpcode::Enum::Ping);
	server.parse(client.toString("csjp"));

	VERIFY(server.finalBit == true);
	VERIFY(server.opcode == csjp::WebSocketOpcode::Enum::Ping);
	VERIFY(server.payload == "");

	client.clear();

	server = csjp::WebSocketFrame(csjp::WebSocketOpcode::Enum::Pong);
	client.parse(server.toString());

	VERIFY(client.finalBit == true);
	VERIFY(client.opcode == csjp::WebSocketOpcode::Enum::Pong);
	VERIFY(client.payload == "");
}

void TestWebSocket::textShort()
{
	const char * alphabet = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,.-?:_;*$<>#&@{}[]\\|~§'\"+!%/=()";

	csjp::String payload;
	for (unsigned i = 0; i < 125-6; i++)
		payload << alphabet[i % 96];

	csjp::WebSocketFrame client;
	csjp::WebSocketFrame server;

	client = csjp::WebSocketFrame(csjp::WebSocketOpcode::Enum::Text, payload);
	server.parse(client.toString("csjp"));

	VERIFY(server.finalBit == true);
	VERIFY(server.opcode == csjp::WebSocketOpcode::Enum::Text);
	VERIFY(server.payload.length == payload.length);
	VERIFY(server.payload == payload);

	client.clear();

	server = csjp::WebSocketFrame(csjp::WebSocketOpcode::Enum::Text, payload);
	client.parse(server.toString());

	VERIFY(client.finalBit == true);
	VERIFY(client.opcode == csjp::WebSocketOpcode::Enum::Text);
	VERIFY(client.payload.length == payload.length);
	VERIFY(client.payload == payload);
}

void TestWebSocket::textMedium()
{
	const char * alphabet = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,.-?:_;*$<>#&@{}[]\\|~§'\"+!%/=()";
	csjp::String payload;
	
	for (unsigned i = 0; i < 65535-8; i++)
		payload << alphabet[i % 96];

	csjp::WebSocketFrame client;
	csjp::WebSocketFrame server;

	client = csjp::WebSocketFrame(csjp::WebSocketOpcode::Enum::Text, payload);
	server.parse(client.toString("csjp"));

	VERIFY(server.finalBit == true);
	VERIFY(server.opcode == csjp::WebSocketOpcode::Enum::Text);
	DBG("server.payload.length: % payload.length: %",
			server.payload.length, payload.length);
	VERIFY(server.payload.length == payload.length);
	VERIFY(server.payload == payload);

	client.clear();

	server = csjp::WebSocketFrame(csjp::WebSocketOpcode::Enum::Text, payload);
	client.parse(server.toString());

	VERIFY(client.finalBit == true);
	VERIFY(client.opcode == csjp::WebSocketOpcode::Enum::Text);
	VERIFY(client.payload.length == payload.length);
	VERIFY(client.payload == payload);
}

void TestWebSocket::textLong()
{
	const char * alphabet = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,.-?:_;*$<>#&@{}[]\\|~§'\"+!%/=()";

	csjp::String payload;
	for (unsigned i = 0; i < 65535+100; i++)
		payload << alphabet[i % 96];

	csjp::WebSocketFrame client;
	csjp::WebSocketFrame server;

	client = csjp::WebSocketFrame(csjp::WebSocketOpcode::Enum::Text, payload);
	server.parse(client.toString("csjp"));

	VERIFY(server.finalBit == true);
	VERIFY(server.opcode == csjp::WebSocketOpcode::Enum::Text);
	VERIFY(server.payload.length == payload.length);
	VERIFY(server.payload == payload);

	client.clear();

	server = csjp::WebSocketFrame(csjp::WebSocketOpcode::Enum::Text, payload);
	client.parse(server.toString());

	VERIFY(client.finalBit == true);
	VERIFY(client.opcode == csjp::WebSocketOpcode::Enum::Text);
	VERIFY(client.payload.length == payload.length);
	VERIFY(client.payload == payload);
}

void TestWebSocket::close()
{
	csjp::WebSocketFrame client;
	csjp::WebSocketFrame server;

	client = csjp::WebSocketFrame(csjp::WebSocketOpcode::Enum::Close);
	server.parse(client.toString("csjp"));

	VERIFY(server.finalBit == true);
	VERIFY(server.opcode == csjp::WebSocketOpcode::Enum::Close);
	VERIFY(server.payload == "");

	client.clear();

	server = csjp::WebSocketFrame(csjp::WebSocketOpcode::Enum::Close);
	client.parse(server.toString());

	VERIFY(client.finalBit == true);
	VERIFY(client.opcode == csjp::WebSocketOpcode::Enum::Close);
	VERIFY(client.payload == "");
}

TEST_INIT(WebSocket)

	TEST_RUN(pingpong);
	TEST_RUN(textShort);
	TEST_RUN(textMedium);
	TEST_RUN(textLong);
	TEST_RUN(close);

TEST_FINISH(WebSocket)

