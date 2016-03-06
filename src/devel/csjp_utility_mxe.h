/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2009 Csaszar, Peter
 */

#ifndef CSJP_UTILITY_H
#define CSJP_UTILITY_H

#include <stdio.h>

#include <time.h>

struct tm;

/*
 * On win32 environment thread local variables are in use instead of
 * _r() function variants (afaik).
 */

void bzero(void * buf, long unsigned size);
const char * strerror_r(int errNo, char * buf, size_t buflen);
struct tm *localtime_r(const time_t *timep, struct tm *result);
int mkdir(const char *pathname, unsigned mode);

inline int getsid(int pid) { (void)pid; return 0; }
inline int getuid() { return 0; }
inline int geteuid() { return 0; }
inline int getgid() { return 0; }
inline int getegid() { return 0; }

#define EDQUOT (INT_MAX - 1)
#define ETXTBSY (INT_MAX - 2)

namespace csjp {

void basicLogger(const char * msg, FILE * stdfile, const char * binaryName);

typedef unsigned int size_t;

}

#endif
