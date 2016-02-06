/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011 Csaszar, Peter
 */

#ifndef CSJP_STOPPER_H
#define CSJP_STOPPER_H

#include <sys/time.h>
#include <stdlib.h>

namespace csjp {

class Stopper{
public:
	Stopper() :
		started(0),
		stopped(0),
		elapsed(0)
	{
		restart();
	}

	~Stopper()
	{
	}

	double sysTime()
	{
		double secs;
		timeval unixTime;

		gettimeofday(&unixTime, NULL);

		secs = unixTime.tv_usec/1000;
		secs /= 1000;
		secs += unixTime.tv_sec;

		return secs;
	}

	void restart()
	{
		stopped = 0;
		elapsed = 0;
		started = sysTime();
	}

	double stop()
	{
		stopped = sysTime();
		elapsed += stopped - started;
		return elapsed;
	}

	void cont()
	{
		started = sysTime();
	}

	double elapsedSoFar()
	{
		double e = elapsed;
		if(stopped < started)
			e += sysTime() - started;
		return e;
	}

public:
	double started;
	double stopped;
	double elapsed;
};

};

#endif
