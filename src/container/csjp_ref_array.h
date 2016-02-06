/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015-2016 Csaszar, Peter
 */

#ifndef CSJP_REF_ARRAY_H
#define CSJP_REF_ARRAY_H

#include <string.h>
#include <csjp_object.h>
#include <csjp_string.h>

namespace csjp {

/**
 * Stores pointers as references to any type of data (C POD and C++ objects as well).
 * Does not care refernce counting, nor deleting on destruction or removal.
 */

template <typename DataType>
class RefArray
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

#define RefArrayInitializer : \
		cap(0), \
		len(0), \
		val(NULL), \
		capacity(cap), \
		length(len)
public:
	explicit RefArray(const RefArray<DataType> & orig)
		RefArrayInitializer
	{
		setCapacity(orig.len);

		DataType ** iter = orig.val;
		DataType ** end = orig.val + orig.len;
		for(; iter < end; iter++)
			add(**iter);
		val[len] = 0;
	}
	const RefArray & operator=(const RefArray<DataType> & orig) = delete;

	RefArray(RefArray<DataType> && temp) :
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
	const RefArray & operator=(RefArray<DataType> && temp)
	{
		if(val)
			free(val);

		val= temp.val;
		len = temp.len;
		cap = temp.cap;

		temp.val = 0;
		temp.len = 0;
		temp.cap = 0;

		return *this;
	}

public:
	explicit RefArray() RefArrayInitializer { }

	/**
	 * Runtime:		linear, O(n)	<br/>
	 */
	virtual ~RefArray()
	{
		if(val)
			free(val);
	}

private:
	size_t cap; // capacity
	size_t len;
	DataType ** val;
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

	explicit RefArray(size_t s) RefArrayInitializer
	{
		setCapacity(s);
	}

	void setCapacity(size_t _cap)
	{
		DataType **dst = (DataType **)realloc(val, sizeof(DataType*) * (_cap + 1));
		if(!dst)
			throw OutOfMemory("No enough memory for RefArray allocation with "
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

		// Preallocate 10% more bytes (or 100% more if 10% was less than 64 objects).
		size_t inc = _cap << 2;
		if(_cap < 512)
			inc = 64;
		_cap += inc;

		setCapacity(_cap);
	}

	const DataType& operator[](size_t i) const
	{
		ENSURE(i < len, IndexOutOfRange);
		return *(val[i]);
	}
	inline DataType& operator[](size_t i)
	{
		ENSURE(i < len, IndexOutOfRange);
		return *(val[i]);
	}

	/**
	 * Runtime:		O(1) or O(n+1)				<br/>
	 */
	void add(const DataType & t)
	{
		if(cap <= len)
			extendCapacity(len + 1);

		val[len] = const_cast<DataType*>(&t);
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
#endif
	/**
	 * Runtime:		guess O(log(n))		<br/>
	 */
	template <class TypeRemove>
	void remove(const TypeRemove & tr)
	{
		removeAt(index<TypeRemove>(tr));
	}

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
		len = 0;
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

	/**
	 * Runtime:		O(log(n))	<br/>
	 */
	template <typename TypeQuery>
	DataType & query(const TypeQuery &tq) const
	{
		return val[index<TypeQuery>(tq)];
	}
#endif
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
	bool isEqual(const RefArray<DataType> &c) const
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
template <typename DataType> bool operator==(const RefArray<DataType> &a, const RefArray<DataType> &b)
{
	return a.isEqual(b);
}

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType> bool operator!=(const RefArray<DataType> &a, const RefArray<DataType> &b)
{
	return !a.isEqual(b);
}

}

#endif
