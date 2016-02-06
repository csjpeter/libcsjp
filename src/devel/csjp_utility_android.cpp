/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2009 Csaszar, Peter
 */

#include <stdlib.h>

#include <android/log.h>

#include "csjp_utility.h"

long double strtold(const char *nptr, char **endptr)
{
	return strtod(nptr, endptr);
}

namespace csjp {

void basicLogger(const char * msg, FILE * stdfile, const char * binaryName)
{
	(void)stdfile;
	__android_log_write(ANDROID_LOG_INFO, binaryName, msg);
}

long double log10l(long double d)
{
	return __builtin_log10l(d);
}

}
