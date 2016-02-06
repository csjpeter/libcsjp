/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2009-2012 Csaszar, Peter
 */

#ifndef CSJP_LOGGER_H
#define CSJP_LOGGER_H

#include <stdio.h>
#include <stdlib.h>

#include <csjp_defines.h>

namespace csjp {

void setLogDir(const char * dir = NULL) __attribute__ ((no_instrument_function));
void setBinaryName(const char * name = 0) __attribute__ ((no_instrument_function));
const char * getBinaryName() __attribute__ ((no_instrument_function));
const char * logFileName() __attribute__ ((no_instrument_function));
void setLogFileNameSpecializer(const char * str = 0) __attribute__ ((no_instrument_function));

/** Thread safe. */
void msgLogger(FILE * stdfile, const char * msg, size_t len = 0)
		__attribute__ ((no_instrument_function));

extern bool verboseMode;

}

#endif
