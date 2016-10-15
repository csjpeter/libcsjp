/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011 Csaszar, Peter
 */

#include <cxxabi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#ifdef DESKTOP
#include <execinfo.h>
#endif

#include "csjp_exception.h"

namespace csjp {

struct NotesNode
{
	void free()
	{
		if(prev){
			prev->free();
			::free(prev);
		}
		if(msg)
			::free(msg);
	}

	char * msg;
	size_t size;
	NotesNode * prev;
};

PrimeException::iterator PrimeException::iterator::operator++()
{
	if(node)
		node = node->prev;
	return *this;
}

const char * PrimeException::iterator::operator*() const
{
	if(node)
		return node->msg;
	return NULL;
}

/* Unique id by an atomic counter. */
static unsigned nextId()
{
	static unsigned id = 0;
	return __sync_fetch_and_add(&id, 1);
}

const char * errnoName(int errNo)
{
	switch(errNo)
	{
		case EBADF : return "EBADF";
		case EINVAL : return "EINVAL";
		case ENOTCONN : return "ENOTCONN";
		case ENOTSOCK : return "ENOTSOCK";
		case EACCES : return "EACCES";
		case EAFNOSUPPORT : return "EAFNOSUPPORT";
		case EMFILE : return "EMFILE";
		case ENFILE : return "ENFILE";
		case ENOBUFS : return "ENOBUFS";
		case ENOMEM : return "ENOMEM";
		case EPROTONOSUPPORT : return "EPROTONOSUPPORT";
		case EFAULT : return "EFAULT";
		case EFBIG : return "EFBIG";
		case EINTR : return "EINTR";
		case EISDIR : return "EISDIR";
		case ELOOP : return "ELOOP";
		case ENAMETOOLONG : return "ENAMETOOLONG";
		case ENODEV : return "ENODEV";
		case ENOENT : return "ENOENT";
		case ENOSPC : return "ENOSPC";
		case ENOTDIR : return "ENOTDIR";
		case ENXIO : return "ENXIO";
		case EOVERFLOW : return "EOVERFLOW";
		//case EWOULDBLOCK : return "EWOULDBLOCK";
		case EAGAIN : return "EAGAIN";
		case EIO : return "EIO";
		case EDESTADDRREQ : return "EDESTADDRREQ";
		case EPIPE : return "EPIPE";
		case ECONNABORTED : return "ECONNABORTED";
		case EOPNOTSUPP : return "EOPNOTSUPP";
		case EPROTO : return "EPROTO";
		case EPERM : return "EPERM";
		case EADDRINUSE : return "EADDRINUSE";
		case EEXIST : return "EEXIST";
		case EADDRNOTAVAIL : return "EADDRNOTAVAIL";
		case EROFS : return "EROFS";
		case EALREADY : return "EALREADY";
		case ECONNREFUSED : return "ECONNREFUSED";
		case EINPROGRESS : return "EINPROGRESS";
		case EISCONN : return "EISCONN";
		case ENETUNREACH : return "ENETUNREACH";
		case ETIMEDOUT : return "ETIMEDOUT";
		case EDQUOT : return "EDQUOT";
		case ETXTBSY : return "ETXTBSY";
		case ECONNRESET : return "ECONNRESET";
		default: return "-";
	}
}

PrimeException::PrimeException(const PrimeException & orig) :
	std::exception(),
	name(orig.name),
	id(orig.id),
	noMem(orig.noMem),
	lastNode(orig.lastNode),
	whatMessage(0)
{
	/* FIXME: hack because I hate deep copies. */
	((PrimeException&)orig).id = 0;
	((PrimeException&)orig).noMem = false;
	((PrimeException&)orig).name = 0;
	((PrimeException&)orig).lastNode = 0;
}

PrimeException::PrimeException(PrimeException && temp) :
	std::exception(),
	name(temp.name),
	id(temp.id),
	noMem(temp.noMem),
	lastNode(temp.lastNode),
	whatMessage(0)
{
	temp.name = 0;
	temp.id = 0;
	temp.noMem = false;
	temp.lastNode = 0;
}

PrimeException::PrimeException() :
	std::exception(),
	name("PrimeException"),
	id(nextId()),
	noMem(false),
	lastNode(NULL),
	whatMessage(0)
{
	pid_t pid = getpid();
	notePrintf("Process pid is %d, session pid is %d.", pid, getsid(pid));
	notePrintf("Owner uid: %d, euid: %d, gid: %d, egid: %d",
			getuid(), geteuid(), getgid(), getegid());
	noteBacktrace();
}

PrimeException::PrimeException(const std::exception & e) :
	std::exception(),
	name("PrimeException"),
	id(nextId()),
	noMem(false),
	lastNode(NULL),
	whatMessage(0)
{
	pid_t pid = getpid();
	notePrintf("Process pid is %d, session pid is %d.", pid, getsid(pid));
	notePrintf("Owner uid: %d, euid: %d, gid: %d, egid: %d",
			getuid(), geteuid(), getgid(), getegid());
	notePrintf("Causing exception was received as std:exception. What: %s", e.what());
}

PrimeException::PrimeException(int err) :
	std::exception(),
	name("PrimeException"),
	id(nextId()),
	noMem(false),
	lastNode(NULL),
	whatMessage(0)
{
	pid_t pid = getpid();
	notePrintf("Process pid is %d, session pid is %d.", pid, getsid(pid));
	notePrintf("Owner uid: %d, euid: %d, gid: %d, egid: %d",
			getuid(), geteuid(), getgid(), getegid());
	noteBacktrace();

	char errorMsg[512] = {0};
	const char * msg = NULL;
#ifdef _GNU_SOURCE
	msg = strerror_r(err, errorMsg, sizeof(errorMsg));
#else
	errno = 0;
	strerror_r(err, errorMsg, sizeof(errorMsg));
	int _errNo = errno;
	if(_errNo)
		notePrintf("Error in strerror_r: number %d : %s", _errNo, strerror(_errNo));
	msg = errorMsg;
#endif
	notePrintf("errno %d (%s) : %s", err, errnoName(err), msg);
}

PrimeException::~PrimeException() throw()
{
	if(lastNode){
		lastNode->free();
		free(lastNode);
	}
	if(whatMessage)
		free(whatMessage);
}

void PrimeException::noteBacktrace()
{
#ifdef DESKTOP
	unsigned i;
	size_t size;
	void *buffer[100];
	char **strings;

	size = backtrace(buffer, sizeof(buffer)/sizeof(void*));

	strings = backtrace_symbols(buffer, size);
	if (strings == NULL){
		int errNo = errno;
		notePrintf("Failed to get backtrace_symbols. errNo: %d", errNo);
		return;
	}

	for(i = size - 1; i < size; i--){
		char * openPos = strchr(strings[i], '(');
		char * offsetPos = strchr(strings[i], '+');
		char * closePos = strchr(strings[i], ')');
		if(!openPos || !closePos || !offsetPos
				|| openPos >= offsetPos || offsetPos >= closePos){
			notePrintf("%s", strings[i]);
			continue;
		}
		*openPos = 0; openPos++;
		*offsetPos = 0; offsetPos++;
		*closePos = 0; closePos++;

		const char * mangledName = strndup(openPos, offsetPos - openPos - 1);
		int status;
		// https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html
		char * realName = abi::__cxa_demangle(mangledName, 0, 0, &status);
		if(!realName || status)
			notePrintf("%s(%s %s)%s", strings[i], openPos, offsetPos, closePos);
		else
			notePrintf("%s(%s %s)%s", strings[i], realName, offsetPos, closePos);
		free((void*)mangledName);
		free(realName);
	}
	notePrintf("Backtrace:");

	free(strings);
#endif
}

void PrimeException::notePrintf(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	vnotePrintf(format, args);
	va_end(args);
}

void PrimeException::vnotePrintf(const char * format, va_list args)
{
	va_list args2;
	va_copy(args2, args);
	int size = vsnprintf(NULL, 0, format, args2);
	size += 1; /* vsnprintf returned value does not contain the terminating zero.*/

	char * msg = reinterpret_cast<char *>(malloc(size));
	if(!msg){
		noMem = true;
		return;
	}

	vsnprintf(msg, size, format, args);

	NotesNode * node = reinterpret_cast<NotesNode *>(malloc(sizeof(struct NotesNode)));
	if(!node){
		free(msg);
		noMem = true;
		return;
	}

	node->msg = msg;
	node->size = size;
	node->prev = lastNode;
	lastNode = node;
}

void PrimeException::note(const char * msg_)
{
	//abort();
	int size = strlen(msg_) + 1;
	char * msg = reinterpret_cast<char *>(malloc(size));
	if(!msg){
		noMem = true;
		return;
	}
	NotesNode * node = reinterpret_cast<NotesNode *>(malloc(sizeof(struct NotesNode)));
	if(!node){
		free(msg);
		noMem = true;
		return;
	}

	memcpy(msg, msg_, size);
	node->msg = msg;
	node->size = size;
	node->prev = lastNode;
	lastNode = node;
}

const char * PrimeException::what() const throw()
{
	if(!lastNode || !lastNode->msg){
		if(noMem)
			return "There was no enough memory for the 'what' message.";
		return "No explanation given.";
	}

	size_t size = 0;
	for(NotesNode * iter = lastNode; iter; iter = iter->prev)
		size += iter->size;
	if(whatMessage)
		free(whatMessage);
	whatMessage = reinterpret_cast<char *>(malloc(size));
	if(!whatMessage){
		if(lastNode && lastNode->msg)
			return lastNode->msg;
	}

	size = 0;
	char * msg = whatMessage;
	for(NotesNode * iter = lastNode; iter; iter = iter->prev){
		memcpy(msg + size, iter->msg, iter->size);
		size += iter->size;
		msg[size - 1] = '\n';
	}
	msg[size - 1] = 0;
	return msg;
}

const char * PrimeException::lastMessage()
{
	return lastNode ? lastNode->msg : "";
}

static void print_backtrace()
{
#ifdef DESKTOP
	unsigned i;
	size_t size;
	void *buffer[100];
	char **strings;

	size = backtrace(buffer, sizeof(buffer)/sizeof(void*));

	strings = backtrace_symbols(buffer, size);
	if (strings == NULL){
		int errNo = errno;
		fprintf(stderr, "Failed to get backtrace_symbols. errNo: %d", errNo);
		return;
	}

	fprintf(stderr, "Backtrace:\n");
	for(i = 0; i < size; i++){
		char * openPos = strchr(strings[i], '(');
		char * offsetPos = strchr(strings[i], '+');
		char * closePos = strchr(strings[i], ')');
		if(!openPos || !closePos || !offsetPos
				|| openPos >= offsetPos || offsetPos >= closePos){
			fprintf(stderr, "%s %p\n", strings[i], buffer[i]);
			continue;
		}
		*openPos = 0; openPos++;
		*offsetPos = 0; offsetPos++;
		*closePos = 0; closePos++;

		const char * mangledName = strndup(openPos, offsetPos - openPos - 1);
		int status;
		char * realName = abi::__cxa_demangle(mangledName, 0, 0, &status);
		if(!realName || status)
			fprintf(stderr, "%s(%s %s)%s %p\n",
					strings[i], openPos, offsetPos, closePos, buffer[i]);
		else
			fprintf(stderr, "%s(%s %s)%s %p\n",
					strings[i], realName, offsetPos, closePos, buffer[i]);
		free((void*)mangledName);
		free(realName);
	}

	free(strings);
#endif
}

char g_ExitCodeOnSegmentationViolation = 1;

static void segmentation_fault_signal_handler(int signalNumber)
{
	fprintf(stderr, "Error: signal %d:\n", signalNumber);
	print_backtrace();
	exit(g_ExitCodeOnSegmentationViolation);
}

void set_segmentation_fault_signal_handler()
{
	// FIXME use sigaction instead
	signal(SIGSEGV, segmentation_fault_signal_handler);
}

}
