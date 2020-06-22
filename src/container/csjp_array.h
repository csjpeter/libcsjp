/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015-2016 Csaszar, Peter
 */

#ifndef CSJP_ARRAY_H
#define CSJP_ARRAY_H

#include <string.h>
#include <csjp_object.h>
#include <csjp_string.h>

namespace csjp {

/**
 * General array for c++ objects. Content can be constructed in place or
 * added by pointer. This array takes ownership of the objects added to it.
 *
 * Uses c++ new and delete operators for stored objects,
 * and uses libc alloc and free for storing the pointers of c++ objects.
 */

template <typename DataType>
class Array
{
public:
	class iterator
	{
	public:
		iterator(DataType ** ptr): ptr(ptr){}
		iterator operator++() { ++ptr; return *this; }
		bool operator!=(const iterator & other) { return ptr != other.ptr; }
		const DataType& operator*() const { return **ptr; }
		DataType& operator*() { return **ptr; }
	private:
		DataType ** ptr;
	};

#define ArrayInitializer : \
		cap(0), \
		len(0), \
		val(NULL), \
		capacity(cap), \
		length(len)
public:
	explicit Array(const Array<DataType> & orig)
		ArrayInitializer
	{
		setCapacity(orig.len);

		DataType ** iter = orig.val;
		DataType ** end = orig.val + orig.len;
		for(; iter < end; iter++)
			add(**iter);
		val[len] = 0;
	}
	const Array & operator=(const Array<DataType> & orig) = delete;

	Array(Array<DataType> && temp) :
		cap(temp.cap),
		len(temp.len),
		val(temp.val),
		capacity(cap),
		length(len)
	{
		temp.val = 0;
		temp.len = 0;
		temp.cap = 0;
	}
	const Array & operator=(Array<DataType> && temp)
	{
		clear();
		if(val)
			free(val);

		val = temp.val;
		len = temp.len;
		cap = temp.cap;

		temp.val = 0;
		temp.len = 0;
		temp.cap = 0;

		return *this;
	}

public:
	explicit Array() ArrayInitializer { }

	/**
	 * Runtime:		linear, O(n)	<br/>
	 */
	virtual ~Array()
	{
		clear();
		if(val)
			free(val);
	}

private:
	size_t cap; // capacity
	size_t len;
	DataType **val;
	/*
	 * Invariants:
	 * - if val != NULL and points to valid area :
	 *   - len <= cap
	 * - if val == NULL :
	 *   - len == 0
	 */
public:
	const size_t & capacity;
	const size_t & length;

public:
	iterator begin() const { return iterator(val); }
	iterator end() const { return iterator(val + len); }

	explicit Array(size_t s) ArrayInitializer
	{
		setCapacity(s);
	}

	void setCapacity(size_t _cap)
	{
		DataType **dst = (DataType **)realloc(val, sizeof(DataType*) * (_cap + 1));
		if(!dst)
			throw OutOfMemory("No enough memory for Array allocation with "
					"% number of elements.", _cap);

		cap = _cap;
		val = dst;
		if(_cap < len)
			len = _cap;
		val[len] = 0;
	}

	void extendCapacity(size_t _cap)
	{
		ENSURE(len <= _cap,  InvalidArgument);

		if(_cap < 32)
			_cap = 64;
		else if(_cap < 256)
			_cap = 512;
		else
			_cap *= 2;

		setCapacity(_cap);
	}

	const DataType& operator[](size_t i) const
	{
		ENSURE(i < len, IndexOutOfRange);
		return *(val[i]);
	}
	DataType& operator[](size_t i)
	{
		ENSURE(i < len, IndexOutOfRange);
		return *(val[i]);
	}
/*	const DataType & operator[](const DataType & dt) const
	{
		return val[index<DataType>(dt)];
	}
	inline DataType & operator[](const DataType & dt)
	{
		return val[index<DataType>(dt)];
	}*/
	/*template <typename TypeQuery>
	const DataType & operator[](const TypeQuery & tq) const
	{
		return val[index<TypeQuery>(tq)];
	}*/

	/**
	 * Runtime:		O(1) or O(n+1)				<br/>
	 */
	void add(Object<DataType> & t)
	{
		if(cap <= len)
			extendCapacity(len + 1);

		val[len] = t.release();
		len++;
		val[len] = 0;
	}

	void add(DataType * && dt)
	{
		if(cap <= len)
			extendCapacity(len + 1);

		val[len] = dt;
		len++;
		val[len] = 0;
		dt = 0;
	}

	void add(DataType && dt)
	{
		if(cap <= len)
			extendCapacity(len + 1);

		val[len] = new DataType(dt);
		len++;
		val[len] = 0;
	}
#if 0
	void push(Object<DataType> & t)
	{
		add(t);
	}

	void prepend(Object<DataType> & t)
	{
		if(cap <= len)
			extendCapacity(len + 1);

		shiftForward(0, len, 1);
		val[0] = t.release();
		len++;
	}
#endif

	template<typename... Args>
	void add(Args & ... args)
	{
		if(cap <= len)
			extendCapacity(len + 1);

		val[len] = new DataType(args...);
		len++;
		val[len] = 0;
	}

	template<typename... Args>
	void add(const Args & ... args)
	{
		if(cap <= len)
			extendCapacity(len + 1);

		val[len] = new DataType(args...);
		len++;
		val[len] = 0;
	}

	void join(Array<DataType> & array)
	{
		if(cap < len + array.length)
			extendCapacity(len + array.length);

		for(size_t i = 0; i < array.length; i++){
			val[len] = array.val[i];
			len++;
			val[len] = 0;
			array.val[i] = 0;
		}
		array.len = 0;
	}

	void join(Array<DataType> && array)
	{
		join(array);
	}

#if 0
	/**
	 * Runtime:		O(n) <br/>
	 */
	virtual int compare(const DataType &a, const DataType &b) const
	{
		if(a < b)
			return -1;
		if(b < a)
			return 1;
		return 0;
	}

	/**
	 * Runtime:		O(n) + 2 * O(n * log n)	<br/>
	 */
	void sort()
	{
	}

	void moveToFrontAt(size_t pos)
	{
		ENSURE(pos <= len,  InvalidArgument);
		DataType * d = val[pos];
		shiftBackward(0, pos, 1);
		val[0] = d;
	}

	void moveToFront(DataType & t)
	{
		size_t pos = index(t);
		moveToFrontAt(pos);
	}

	void erase(size_t from, size_t until)
	{
		ENSURE(until <= len,  InvalidArgument);
		ENSURE(from <= until,  InvalidArgument);

		shiftForward(until, len, until - from);
	}
#endif
	void removeAt(size_t i)
	{
		ENSURE(i < len, InvalidArgument);

		DataType ** srcUntil = val + len;/* points to the first not to move */
		DataType ** srcPtr = val + i + 1;
		DataType ** dstPtr = val + i;
		delete val[i];
		for(; srcPtr < srcUntil; srcPtr++, dstPtr++)
			*dstPtr = *srcPtr;
		len--;
		val[len] = 0;
	}
#if 0
	/**
	 * Runtime:		guess O(log(n))		<br/>
	 */
	void remove(const DataType & t)
	{
		removeAt(index<DataType>(t));
	}

	/**
	 * Runtime:		guess O(log(n))		<br/>
	 */
	template <class TypeRemove>
	void remove(const TypeRemove & tr)
	{
		removeAt(index<TypeRemove>(tr));
	}
#endif
	/**
	 * Runtime:		linear, O(n)	<br/>
	 */
	Object<DataType> pop()
	{
		ENSURE(!val || 0 < len,  ObjectNotFound);
		len--;
		Object<DataType> o(val[len]);
		val[len] = 0;
		return o;
	}

	/**
	 * Runtime:		linear, O(n)	<br/>
	 */
	void clear()
	{
		for(DataType ** i = val; i < val + len; i++)
			delete *i;
		len = 0;
		if(val)
			val[len] = 0;
	}

	/**
	 * Runtime:		guess O(1)	<br/>
	 */
	const DataType & first() const throw()
	{
		ENSURE(len != 0,  ObjectNotFound);
		return **val;
	}
	DataType & first() throw()
	{
		ENSURE(len != 0,  ObjectNotFound);
		return **val;
	}

	/**
	 * Runtime:		guess O(log(n))	<br/>
	 */
	const DataType & last() const throw()
	{
		ENSURE(len != 0,  ObjectNotFound);
		return **(val + len - 1);
	}
	DataType & last() throw()
	{
		ENSURE(len != 0,  ObjectNotFound);
		return **(val + len - 1);
	}
#if 0
	/**
	 * Runtime:		O(log n)	<br/>
	 */
	DataType & queryAt(size_t i) const
	{
		ENSURE(i < len, IndexOutOfRange);
		return *val[i];
	}

	/**
	 * Runtime:		O(log(n))	<br/>
	 */
	DataType & query(const DataType &t) const
	{
		return query<DataType>(t);
	}
#endif
	/**
	 * Runtime:		O(log(n))	<br/>
	 */
	template <typename TypeQuery>
	const DataType & query(const TypeQuery &tq) const
	{
		return *val[index<TypeQuery>(tq)];
	}

	/**
	 * Runtime:		constant	<br/>
	 */
	bool empty() const
	{
		return len == 0;
	}
#if 0
	/**
	 * Runtime:		O(log(n))	<br/>
	 */
	size_t index(const DataType & t) const
	{
		return index<DataType>(t);
	}
#endif
	/**
	 * Runtime:		O(log(n))	<br/>
	 */
	template <class TypeIndex>
	size_t index(const TypeIndex & ti) const
	{
		DataType ** iter = val;
		DataType ** end = val + len;
		size_t u;
		for(u = 0; iter < end; iter++, u++)
			if(**iter == ti)
				return u;
		throw ObjectNotFound("Could not find index of object not in the array.");
	}

	/**
	 * Runtime:		constant	<br/>
	 */
	size_t size() const
	{
		return len;
	}

	/**
	 * Runtime:		O(n)	<br/>
	 */
	bool has(const DataType & t) const
	{
		return has<DataType>(t);
	}

	/**
	 * Runtime:		O(n)	<br/>
	 */
	template <class TypeHas>
	bool has(const TypeHas & th) const
	{
		DataType ** iter = val;
		DataType ** end = val + len;
		for(; iter < end; iter++)
			if(**iter == th)
				return true;
		return false;
	}

	/**
	 * Runtime:		constant	<br/>
	 */
	bool isEqual(const Array<DataType> &c) const
	{
		if(c.len != len)
			return false;

		DataType ** iterc = c.val;
		DataType ** iter = val;
		DataType ** end = val + len;
		for(; iter < end; iter++, iterc++)
			if(**iter != **iterc)
				return false;
		return true;
	}

};

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType> bool operator==(const Array<DataType> &a, const Array<DataType> &b)
{
	return a.isEqual(b);
}

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType> bool operator!=(const Array<DataType> &a, const Array<DataType> &b)
{
	return !a.isEqual(b);
}

}

#endif
