/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012 Csaszar, Peter
 */

#ifndef CSJP_MUTEX_H
#define CSJP_MUTEX_H

#include <csjp_defines.h>

namespace csjp {
/**
 * Do not inherit from this class!
 */
class Mutex
{
public:
#define MutexInitializer : \
		mutex(0)
public:
	explicit Mutex(const Mutex & orig) = delete;
	const Mutex & operator=(const Mutex & orig) = delete;

	Mutex(Mutex && temp) = delete;
	const Mutex & operator=(Mutex && temp) = delete;

	explicit Mutex() MutexInitializer {}
	~Mutex() {}

private:
	void lock();
	void unlock();

	unsigned mutex;

public:
	class Lock
	{
	public:
		explicit Lock() = delete;

		explicit Lock(const Lock & orig) = delete;
		const Lock & operator=(const Lock & orig) = delete;

		Lock(Lock && temp) = delete;
		const Lock & operator=(Lock && temp) = delete;

		Lock(Mutex & _mutex) : mutex(_mutex) { mutex.lock(); }
		~Lock() { mutex.unlock(); }

	private:
		Mutex & mutex;
	};
};

}

#endif
