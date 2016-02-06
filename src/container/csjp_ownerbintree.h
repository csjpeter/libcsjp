/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2010 Csaszar, Peter
 */

#ifndef OWNERBTREE_H
#define OWNERBTREE_H

#include <csjp_bintree.h>

namespace csjp {

template <typename DataType>
class OwnerBinTree : public BinTree<DataType>
{
public:
	explicit OwnerBinTree(const BinTree<DataType> &) = delete;
	OwnerBinTree<DataType>& operator=(const OwnerBinTree<DataType> &) = delete;

	OwnerBinTree(BinTree<DataType> && temp) :
		BinTree<DataType>(move_cast(temp))
	{
	}

	OwnerBinTree<DataType>& operator=(OwnerBinTree<DataType> && temp)
	{
		BinTree<DataType>::operator=(move_cast(temp));
		return *this;
	}

	explicit OwnerBinTree() = delete;

public:
	explicit OwnerBinTree(DataType *& t) : BinTree<DataType>()
	{
		BinTree<DataType>::data = t;
		t = NULL;
	}

	virtual ~OwnerBinTree() { delete BinTree<DataType>::data; }
};

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType>
bool operator==(const OwnerBinTree<DataType> &a, const OwnerBinTree<DataType> &b)
{
	return a.isEqual(b);
}

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType>
bool operator!=(const OwnerBinTree<DataType> &a, const OwnerBinTree<DataType> &b)
{
	return !a.isEqual(b);
}

}

#endif
