/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2009 Csaszar, Peter
 */

#ifndef CSJP_UTILITY_H
#define CSJP_UTILITY_H

#include <stdio.h>

inline int getsid(int pid) { (void)pid; return 0; }

long double strtold(const char *nptr, char **endptr);

namespace csjp {

typedef unsigned int size_t;

void basicLogger(const char * msg, FILE * stdfile, const char * binaryName);

long double log10l(long double);

}

#endif
