/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2009 Csaszar, Peter
 */

#ifndef OWNER_CONTAINER_H
#define OWNER_CONTAINER_H

#include <csjp_container.h>
#include <csjp_ownerbintree.h>

namespace csjp {

template <typename DataType>
class OwnerContainer : public Container<DataType>
{
#define OwnerContainerInitializer : Container<DataType>()
public:
	explicit OwnerContainer(const OwnerContainer & orig)
		OwnerContainerInitializer { OwnerContainer::copy(orig); }
	const OwnerContainer & operator=(const OwnerContainer &) = delete;

	OwnerContainer(OwnerContainer && temp) :
		Container<DataType>(move_cast(temp))
	{}

	const OwnerContainer & operator=(OwnerContainer && temp)
	{
		Container<DataType>::operator=(move_cast(temp));
		return *this;
	}

public:
	explicit OwnerContainer() OwnerContainerInitializer { }
	virtual ~OwnerContainer() { }

private:
	/**
	 * Runtime:		O(n) + O(n_src * log(n_src))	<br/>
	 */
	void copy(const OwnerContainer<DataType> & orig) /*{{{*/
	{
		if(orig.root == NULL){
			Container<DataType>::clear();
			return;
		}

		Object<BinTree<DataType> > copy(NULL);

		BinTree<DataType> *iter;
		for(iter = orig.root->first(); iter != NULL; iter = iter->next()){
			Object<DataType> cp(new DataType(*(iter->data)));
			BinTree<DataType> * node = new OwnerBinTree<DataType>(cp.ptr);
			node->insert(copy.ptr);
		}

		/* Everything ok, lets clear the old content. */
		if(Container<DataType>::root){
			Container<DataType>::root->clear();
			delete Container<DataType>::root;
		}

		/* ... and set the new. */
		Container<DataType>::root = copy.release();
	}/*}}}*/

public:
	/**
	 * Runtime:		O(log(n))				<br/>
	 */
	void add(DataType *& t) /* {{{ */
	{
		OwnerBinTree<DataType> * node = new OwnerBinTree<DataType>(t);
		node->insert(Container<DataType>::root);
	}/*}}}*/

	void add(Object<DataType> & odt)
	{
		add(odt.ptr);
	}

	/** Not 100% transactional. On error, the object pointed will be destructed! */
	void add(DataType * && dt)
	{
		Object<DataType> o(dt);
		add(o);
	}

};

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType> bool operator==(/*{{{*/
		const OwnerContainer<DataType> &a, const OwnerContainer<DataType> &b)
{
	return a.isEqual(b);
}/*}}}*/

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType> bool operator!=(/*{{{*/
		const OwnerContainer<DataType> &a, const OwnerContainer<DataType> &b)
{
	return !a.isEqual(b);
} /* }}} */

}

#endif
