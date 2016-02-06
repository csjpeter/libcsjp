/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011 Csaszar, Peter
 */

#ifndef SORTER_REFERENCE_CONTAINER_H
#define SORTER_REFERENCE_CONTAINER_H

#include <csjp_sorter_container.h>

namespace csjp {

template <typename DataType>
class SorterReferenceContainer : public SorterContainer<DataType>
{
#define SorterReferenceContainerInitializer : SorterContainer<DataType>()
public:
	explicit SorterReferenceContainer(const SorterReferenceContainer & orig) :
		SorterContainer<DataType>(orig) { }
	const SorterReferenceContainer & operator=(const SorterReferenceContainer &) = delete;

	SorterReferenceContainer(SorterReferenceContainer && temp) :
		SorterContainer<DataType>(move_cast(temp))
	{}

	const SorterReferenceContainer & operator=(SorterReferenceContainer && temp)
	{
		SorterContainer<DataType>::operator=(move_cast(temp));
		return *this;
	}

public:
	explicit SorterReferenceContainer() : SorterContainer<DataType>(){}
	virtual ~SorterReferenceContainer(){}

	/**
	 * Runtime:		O(log(n))				<br/>
	 */
	void add(DataType & t) /* {{{ */
	{
		Object<CustomBox<DataType> > customBox(
				new CustomBox<DataType>(*this, &t, this->lastOrder + 1));
		Object<OwnerBinTree< CustomBox<DataType> > > customNode(
				new OwnerBinTree< CustomBox<DataType> >(customBox.ptr));

		Object<PrimaryBox<DataType> > primaryBox(
				new PrimaryBox<DataType>(&t));
		Object<OwnerBinTree< PrimaryBox<DataType> > > primaryNode(
				new OwnerBinTree< PrimaryBox<DataType> >(primaryBox.ptr));

		primaryNode->data->custom = customNode.ptr;
		customNode->data->primary = primaryNode.ptr;

		primaryNode->insert(this->root);
		primaryNode.ptr = NULL;
		customNode->insert(this->custom);
		customNode.ptr = NULL;
	}/*}}}*/

};

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType> bool operator==(/*{{{*/
		const SorterReferenceContainer<DataType> &a, const SorterReferenceContainer<DataType> &b)
{
	return a.isEqual(b);
}/*}}}*/

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType> bool operator!=(/*{{{*/
		const SorterReferenceContainer<DataType> &a, const SorterReferenceContainer<DataType> &b)
{
	return !a.isEqual(b);
}/*}}}*/

}

#endif
