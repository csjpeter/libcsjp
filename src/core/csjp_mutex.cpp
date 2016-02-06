/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012 Csaszar, Peter
 */

#include <sched.h>

#include "csjp_mutex.h"

namespace csjp {

void Mutex::lock()
{
	/* We are going to use gcc's special low level atomic operations.
	 * http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html#Atomic-Builtins
	 * type __sync_fetch_and_add (type *ptr, type value, ...)
	 * type __sync_fetch_and_sub (type *ptr, type value, ...)
	 */
#if not __GNUC__ >= 4
#error "We need gcc 4 or later for __sync_fetch_and_add() and __sync_fetch_and_sub()"
#endif
	unsigned mutexLast = 1;
	while(mutexLast){
		/* __sync_fetch_and_... functions returns the original value of the first param. */
		mutexLast = __sync_fetch_and_add(&mutex, 1);
		if(mutexLast){
			__sync_fetch_and_sub(&mutex, 1);
			sched_yield();
		}
	}
}

void Mutex::unlock()
{
	__sync_fetch_and_sub(&mutex, 1);
}

}
