/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2009-2016 Csaszar, Peter
 */

#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <sys/time.h>
/*
#include <sched.h>
*/
#include <unistd.h>
#include <fcntl.h>
/*
#include <pthread.h>
*/
#include <errno.h>
/*#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
*/

#include "csjp_utility.h"

namespace csjp {

void basicLogger(const char * msg, FILE * stdfile, const char * binaryName)
{
	(void)binaryName;
	if(stdfile){
		fputs(msg, stdfile);
		fflush(stdfile);
	}
}

}

void bzero(void * buf, long unsigned size)
{
	memset(buf, 0, size);
}

const char * strerror_r(int errNo, char * buf, size_t buflen)
{
	strncpy(buf, strerror(errNo), buflen);
	buf[buflen-1] = 0;
	return buf;
}

struct tm *localtime_r(const time_t *timep, struct tm *result)
{
	  return localtime_s(result, timep) ? NULL : result;
}

int mkdir(const char *pathname, unsigned mode)
{
	(void)mode;
	return mkdir(pathname);
}
