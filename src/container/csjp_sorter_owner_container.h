/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011 Csaszar, Peter
 */

#ifndef SORTER_OWNER_CONTAINER_H
#define SORTER_OWNER_CONTAINER_H

#include <csjp_sorter_container.h>
#include <csjp_ownerbintree.h>

namespace csjp {

template <typename DataType>
class SorterOwnerContainer : public SorterContainer<DataType>
{
#define SorterOwnerContainerInitializer : SorterContainer<DataType>()
public:
	explicit SorterOwnerContainer(const SorterOwnerContainer & orig) :
		SorterContainer<DataType>(orig) { }
	const SorterOwnerContainer & operator=(const SorterOwnerContainer &) = delete;

	SorterOwnerContainer(SorterOwnerContainer && temp) :
		SorterContainer<DataType>(move_cast(temp))
	{}

	const SorterOwnerContainer & operator=(SorterOwnerContainer && temp)
	{
		SorterContainer<DataType>::operator=(move_cast(temp));
		return *this;
	}

public:
	explicit SorterOwnerContainer() SorterOwnerContainerInitializer { }
	virtual ~SorterOwnerContainer() { }

public:
	/**
	 * Runtime:		O(log(n))				<br/>
	 */
	void add(DataType *& t) /* {{{ */
	{
		Object<CustomBox<DataType> > customBox(
				new CustomBox<DataType>(*this, t, this->lastOrder + 1));
		Object<OwnerBinTree< CustomBox<DataType> > > customNode(
				new OwnerBinTree< CustomBox<DataType> >(customBox.ptr));

		Object<PrimaryBox<DataType> > primaryBox(
				new PrimaryBox<DataType>(t));
		Object<OwnerBinTree< PrimaryBox<DataType> > > primaryNode(
				new OwnerBinTree< PrimaryBox<DataType> >(primaryBox.ptr));

		primaryNode->data->owner = true;
		t = NULL;

		primaryNode->data->custom = customNode.ptr;
		customNode->data->primary = primaryNode.ptr;

		primaryNode->insert(this->root);
		primaryNode.ptr = NULL;
		customNode->insert(this->custom);
		customNode.ptr = NULL;
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
		const SorterOwnerContainer<DataType> &a, const SorterOwnerContainer<DataType> &b)
{
	return a.isEqual(b);
}/*}}}*/

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType> bool operator!=(/*{{{*/
		const SorterOwnerContainer<DataType> &a, const SorterOwnerContainer<DataType> &b)
{
	return !a.isEqual(b);
}/*}}}*/

}

#endif
