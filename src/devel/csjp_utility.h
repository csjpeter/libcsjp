/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2009 Csaszar, Peter
 */

#ifndef CSJP_UTILITY_H
#define CSJP_UTILITY_H

#include <stdio.h>

namespace csjp {

#if defined(__amd64__) || defined(__x86_64__) || defined(__amd64) || defined(__x86_64)
typedef long unsigned int size_t;
#else
typedef unsigned int size_t;
#endif

void basicLogger(const char * msg, FILE * stdfile, const char * binaryName);

}

#endif
