/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2012-2016 Csaszar, Peter
 */

#ifndef CSJP_SKELETON_H
#define CSJP_SKELETON_H

namespace csjp {

class Skeleton {
/**
 * Do not inherit from this class!
 */
#define SkeletonInitializer
public:
	explicit Skeleton(const Skeleton & orig) = default;
	const Skeleton & operator=(const Skeleton & orig) = delete;

	Skeleton(Skeleton && temp) /* ...impl... */ { /*...impl...*/ }
	const Skeleton & operator=(Skeleton && temp) { /*...impl...*/ return *this; }

	explicit Skeleton() SkeletonInitializer { }
	~Skeleton() { }
};

}

#endif
