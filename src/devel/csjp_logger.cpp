/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2009-2012 Csaszar, Peter
 */

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include <unistd.h>

#include <errno.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>

#ifdef FCLOG
#include <fclog.h>
#endif

#include "csjp_logger.h"

namespace csjp {

static void dateStamp(char res[16])
{
	time_t t = time(NULL);
	strftime(res, 16, "%Y-%m-%d-%a", localtime(&t));
}

static void timeStamp(char res[16])
{
	char timeStr[8] = {0};
	timeval unixTime;

	gettimeofday(&unixTime, NULL);
	time_t t = unixTime.tv_sec;
	int unixMillisecs = unixTime.tv_usec/1000;
	struct tm _tm;

	localtime_r(&t, &_tm);
	strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &_tm);
	snprintf(res, 16, "%s:%03d", timeStr, unixMillisecs);
}

static char g_LogDir[128] = {0};
static char g_LogFileName[512] = {0};
static char g_LogFileNameSpecializer[64] = {0};
static char g_BinaryName[64] = "unknown";
bool verboseMode = false;
bool haveLogDir = true;

void resetLogFileName()
{
	g_LogFileName[0] = 0;
}

void setLogDir(const char * dir)
{
	if(dir)
		strncpy(g_LogDir, dir, sizeof(g_LogDir));
	else
		strncpy(g_LogDir, "./", sizeof(g_LogDir));
	haveLogDir = true;
	resetLogFileName();
}

void setBinaryName(const char * name)
{
	if(!name)
		return;

	const char *baseName = strrchr(name, '/');
	baseName = baseName ? baseName + 1 : name;
	strncpy(g_BinaryName, baseName, sizeof(g_BinaryName));

	resetLogFileName();
}

const char * getBinaryName()
{
	return g_BinaryName;
}

const char * logFileName()
{
#ifdef FCLOG
	return FCLOG_FILE;
#else
	if(g_LogFileName[0] == 0 && haveLogDir){
		struct stat fileStat;
		if(0 <= stat(g_LogDir, &fileStat))
			if(S_ISDIR(fileStat.st_mode)){
				snprintf(g_LogFileName, sizeof(g_LogFileName),
						"%s/%s%s.log", g_LogDir,
						g_BinaryName, g_LogFileNameSpecializer);
				return g_LogFileName;
			}
		haveLogDir = false;
	}
	return g_LogFileName;
#endif
}

void setLogFileNameSpecializer(const char * str)
{
	if(str != NULL){
		g_LogFileNameSpecializer[0] = '.';
		strncpy(g_LogFileNameSpecializer + 1, str, sizeof(g_LogFileNameSpecializer) - 1);
	} else
		g_LogFileNameSpecializer[0] = 0;
	resetLogFileName();
}

static unsigned loggerMutex = 0; /* Initialized before main() */
void msgLogger(FILE * stdfile, const char * string, size_t length)
{
	if(!length)
		length = strlen(string);

	char header[64];
	{
		char time_stamp[16];
		timeStamp(time_stamp);

		char date_stamp[16];
		dateStamp(date_stamp);

		double c = (double)(clock())/(double)(CLOCKS_PER_SEC);

		pid_t tid = syscall(SYS_gettid);
		//syscall(SYS_tgkill, getpid(), tid, SIGHUP);

		snprintf(header, sizeof(header), "%14s %12s %7.3f %5d %5d",
				date_stamp, time_stamp, c, getpid(), tid);
	}

	const char * fmt = "%s %s ";

#if ! _ISOC99_SOURCE
#error We need to use C99 for calculating printed number of characters with vsnprintf().
#endif

	/* Now lets calculate the amount of memory needed by the full log message. */
	int size = 0;
	size += strlen(fmt);
	size += sizeof(header);
	size += length;

	/* Allocate memory */
	char * msg = (char*)malloc(size);
	if(!msg){
		if(stderr)
			fprintf(stderr, VT_RED VT_TA_BOLD "ERROR" VT_NORMAL
					"No enoguh memory for log message!");
		return;
	}

	/* Compose the log message */
	int len = snprintf(msg, size, fmt, header, string);
	if(len < 0){
		if(stderr)
			fprintf(stderr, VT_RED VT_TA_BOLD "ERROR" VT_NORMAL
					"Failed to snprintf the log message and its header!\n");
		free(msg);
		return;
	}

	msg[len++] = '\n';
	msg[len] = 0;

	/* We need to have logging thread safe.
	 * We should not use posix mutex since that might fail, and
	 * on posix failure we should report the issue, thus call
	 * this logger. This would be a circular dependency and a
	 * possible place to have infinite recursion.
	 *
	 * So we are going to use gcc special low level atomic operations.
	 * http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html#Atomic-Builtins
	 * type __sync_fetch_and_add (type *ptr, type value, ...)
	 * type __sync_fetch_and_sub (type *ptr, type value, ...)
	 */
#if not __GNUC__ >= 4
#error "We need gcc 4 or later for __sync_fetch_and_add() and __sync_fetch_and_sub()"
#endif
	unsigned loggerMutexLast = 1;
	while(loggerMutexLast){
		loggerMutexLast = __sync_fetch_and_add(&loggerMutex, 1);
		if(loggerMutexLast){
			__sync_fetch_and_sub(&loggerMutex, 1);
			usleep(100);
		}
	}

	if(haveLogDir && logFileName()[0]){
		FILE *log = fopen(logFileName(), "a");
		if(log){
			fputs(msg, log);
			fflush(log);
			TEMP_FAILURE_RETRY( fclose(log) );
		} else if(stderr) {
			fprintf(stderr, VT_RED VT_TA_BOLD "ERROR" VT_NORMAL
					" Could not open logfile: %s\n", logFileName());
			haveLogDir = false;
		}
	}

	basicLogger(msg, stdfile, g_BinaryName);

	__sync_fetch_and_sub(&loggerMutex, 1);

	free(msg);
}

}
