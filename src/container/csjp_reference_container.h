/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2009 Csaszar, Peter
 */

#ifndef REFERENCE_CONTAINER_H
#define REFERENCE_CONTAINER_H

#include <csjp_container.h>

namespace csjp {

template <typename DataType>
class ReferenceContainer : public Container<DataType>
{
#define ReferenceContainerInitializer : Container<DataType>()
public:
	explicit ReferenceContainer(const ReferenceContainer & orig)
		ReferenceContainerInitializer { copy(orig); }
	const ReferenceContainer & operator=(const ReferenceContainer &) = delete;

	ReferenceContainer(ReferenceContainer && temp) :
		Container<DataType>(move_cast(temp))
	{}

	const ReferenceContainer & operator=(ReferenceContainer && temp)
	{
		Container<DataType>::operator=(move_cast(temp));
		return *this;
	}

public:
	explicit ReferenceContainer() : Container<DataType>(){}
	virtual ~ReferenceContainer(){}

private:
	/**
	 * Runtime:		O(n) + O(n_src * log(n_src))	<br/>
	 */
	void copy(const ReferenceContainer<DataType> & orig) /*{{{*/
	{
		if(orig.root == NULL){
			Container<DataType>::clear();
			return;
		}

		Object<BinTree<DataType> > copy(NULL);

		BinTree<DataType> *iter;
		for(iter = orig.root->first(); iter != NULL; iter = iter->next()){
			BinTree<DataType> *node = new BinTree<DataType>();
			node->data = const_cast<DataType*>(iter->data);
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
	void add(DataType & t) /* {{{ */
	{
		BinTree<DataType> * node = new BinTree<DataType>();
		node->data = &t;
		node->insert(Container<DataType>::root);
	}/*}}}*/

};

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType> bool operator==(/*{{{*/
		const ReferenceContainer<DataType> &a, const ReferenceContainer<DataType> &b)
{
	return a.isEqual(b);
}/*}}}*/

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType> bool operator!=(/*{{{*/
		const ReferenceContainer<DataType> &a, const ReferenceContainer<DataType> &b)
{
	return !a.isEqual(b);
}/*}}}*/

}

#endif
