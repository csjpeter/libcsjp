/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#ifndef CSJP_OBJECT_H
#define CSJP_OBJECT_H

#include <csjp_exception.h>

namespace csjp {

/**
 * One should never create dynamic instance of such object.
 *
 * This is a kind of smart pointer that owns a class based object
 * pointed by the given pointer, and deletes it when this object
 * is destroyed.
 *
 * Uses c++ new and delete.
 */
template <typename Type>
class Object
{
#define ObjectInitializer : \
		ptr(NULL)
public:
	explicit Object(const Object &) = delete;
	const Object & operator=(const Object &) = delete;

	Object(Object && temp) :
		ptr(temp.ptr)
	{
		temp.ptr = 0;
	}

	const Object & operator=(Object && temp)
	{
		delete ptr;
		ptr = temp.ptr;
		temp.ptr = 0;
		return *this;
	}

	void * operator new(size_t size) = delete;
	void operator delete(void * ptr) throw() = delete;

	/**
	 * One can safely do:
	 *	Object<Type> O(new Type(...));
	 * It is safe because this constructor only fails if there is
	 * a stack overflow, which is undefendable.
	 */
	explicit Object(Type * t) :
		ptr(t)
	{
	}

	explicit Object() :
		ptr(NULL)
	{
		ptr = new Type();
	}

	~Object()
	{
		delete ptr;
	}

	inline Type * release()
	{
		Type * ret = ptr;
		ptr = NULL;
		return ret;
	}

	Type * operator->() const
	{
		return ptr;
	}

	Type & operator*() const
	{
		return *ptr;
	}

	Type *& operator&()
	{
		return ptr;
	}

	Type * ptr;
};

template<typename Type>
bool operator==(Object<Type> & a, Object<Type> & b)
{
	return *a.ptr == *b.ptr;
}

template<typename Type>
bool operator!=(Object<Type> & a, Object<Type> & b)
{
	return *a.ptr != *b.ptr;
}

template<typename Type>
bool operator<(Object<Type> & a, Object<Type> & b)
{
	return *a.ptr < *b.ptr;
}

}

#endif
