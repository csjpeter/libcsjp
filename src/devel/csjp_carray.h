/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011-2016 Csaszar, Peter
 */

#ifndef CSJP_CARRAY_H
#define CSJP_CARRAY_H

#include <stdlib.h>

#include <csjp_string.h>

namespace csjp {

/**
 * One should never create dynamic instance of such object.
 *
 * This class is meant to be used in local scope for dealing
 * with old low level C API calls.
 *
 * This class holds a C type array, a typed pointer for a
 * block of memory for a number of POD objects. All members
 * are public and free to mess them up.
 *
 * The main benefit of using this, that this class takes care
 * of allocating, reallocating and freeing memory on destruction.
 *
 * Uses libc alloc and free.
 */
template <typename Type>
class CArray
{
#define CArrayInitializer : \
		ptr(NULL), \
		len(0), \
		size(0)
public:
	explicit CArray(const CArray &) = delete;
	const CArray & operator=(const CArray &) = delete;

	CArray(CArray && temp) :
		ptr(temp.ptr),
		len(temp.len),
		size(temp.size)
	{
		temp.ptr = 0;
		temp.len = 0;
		temp.size = 0;
	}

	const CArray & operator=(CArray && temp)
	{
		if(ptr)
			free(ptr);
		ptr = temp.ptr;
		temp.ptr = 0;
		len = temp.len;
		temp.len = 0;
		size = temp.size;
		temp.size = 0;
		return *this;
	}

	void * operator new(size_t size) = delete;
	void operator delete(void * ptr) throw() = delete;

	explicit CArray(size_t count = 0) CArrayInitializer
	{
		if(!count)
			return;

		size_t realSize = count * sizeof(Type);
		if((ptr = (Type*)realloc(NULL, realSize)) == NULL)
			throw OutOfMemory("No enough memory for allocation with size of % bytes.",
					realSize);
		size = count;
	}

	void resize(size_t count)
	{
		size_t realSize = count * sizeof(Type);
		Type *newCArray;
		if((newCArray = (Type*)realloc(ptr, realSize)) == NULL)
			throw OutOfMemory(
					"No enough memory for reallocation with size of % bytes.",
					realSize);
		ptr = newCArray;
		size = count;
	}

	~CArray()
	{
		if(ptr){
			free(ptr);
			ptr = NULL;
		}
	}

	Type * ptr;
	size_t len;
	size_t size;
};

}

#endif
