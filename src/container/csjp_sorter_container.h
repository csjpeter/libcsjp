/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011 Csaszar, Peter
 */

#ifndef SORTER_CONTAINER_H
#define SORTER_CONTAINER_H

#include <csjp_ownerbintree.h>

namespace csjp {

template <typename DataType> class SorterContainer;

template <typename DataType> class CustomBox;

/** A general operator< for cases when no specific is defined. */
template <typename Type> bool operator<(const Type &a, const Type &b) { return &a < &b; }

template <typename DataType>
class PrimaryBox
{
/**
 * Do not inherit from this class! Actually, it is best if you are not using this class at all.
 */
public:
	const PrimaryBox & operator=(const PrimaryBox & orig) = delete;
	const PrimaryBox & operator=(PrimaryBox && temp) = delete;

	explicit PrimaryBox(const PrimaryBox & orig) = delete;
	PrimaryBox(PrimaryBox && temp) = delete;

	explicit PrimaryBox() = delete;
	~PrimaryBox() { if(owner && this->data) delete this->data; }

	explicit PrimaryBox(DataType * d, OwnerBinTree< CustomBox<DataType> > * p = NULL) :
		owner(false),
		data(d),
		custom(p)
	{
	}
public:
	bool owner;
	DataType *data;
	BinTree< CustomBox<DataType> > *custom;
};

/**
 * Runtime:		constant				<br/>
 */
template <typename DataType> bool operator<(/*{{{*/
		const PrimaryBox<DataType> &a, const PrimaryBox<DataType> &b)
{
	return *a.data < *b.data;
}/*}}}*/

/**
 * Runtime:		constant				<br/>
 */
template <typename DataType> bool operator<(/*{{{*/
		const DataType &a, const PrimaryBox<DataType> &b)
{
	return a < *b.data;
}/*}}}*/

/**
 * Runtime:		constant				<br/>
 */
template <typename DataType> bool operator<(/*{{{*/
		const PrimaryBox<DataType> &a, const DataType &b)
{
	return *a.data < b;
}/*}}}*/

/**
 * Runtime:		constant				<br/>
 */
template <typename DataType, typename ComparableType> bool operator<(/*{{{*/
		const ComparableType &a, const PrimaryBox<DataType> &b)
{
	return a < *b.data;
}/*}}}*/

/**
 * Runtime:		constant				<br/>
 */
template <typename DataType, typename ComparableType> bool operator<(/*{{{*/
		const PrimaryBox<DataType> &a, const ComparableType &b)
{
	return *a.data < b;
}/*}}}*/

/**
 * Runtime:		constant				<br/>
 */
template <typename DataType> bool operator==(/*{{{*/
		const PrimaryBox<DataType> &a, const PrimaryBox<DataType> &b)
{
	return *a.data == *b.data;
}/*}}}*/

/**
 * Runtime:		constant				<br/>
 */
template <typename DataType> bool operator!=(/*{{{*/
		const PrimaryBox<DataType> &a, const PrimaryBox<DataType> &b)
{
	return *a.data != *b.data;
}/*}}}*/

template <typename DataType>
class CustomBox
{
/**
 * Do not inherit from this class! Actually, it is best if you are not using this class at all.
 */
public:
	const CustomBox & operator=(const CustomBox & orig) = delete;
	const CustomBox & operator=(CustomBox && temp) = delete;

	explicit CustomBox(const CustomBox & orig) = delete;
	CustomBox(CustomBox && temp) = delete;

	explicit CustomBox() = delete;
	~CustomBox() { }

	explicit CustomBox(
			SorterContainer<DataType> & cont,
			DataType * d,
			long long unsigned order,
			OwnerBinTree< PrimaryBox<DataType> > * p = NULL) :
		container(&cont),
		data(d),
		order(order),
		primary(p)
	{
	}

public:
	SorterContainer<DataType> *container;
	DataType *data;
	long long unsigned order;
	BinTree< PrimaryBox<DataType> > *primary;
};

/**
 * Runtime:		constant				<br/>
 */
template <typename DataType> bool operator<(/*{{{*/
		const CustomBox<DataType> &a, const CustomBox<DataType> &b)
{
	ENSURE(a.container != NULL, InvalidArgument);

	int comp = a.container->compare(*a.data, *b.data);

	if(comp == 0)
		return a.order < b.order;
	else
		return comp < 0;
}/*}}}*/

/**
 * Runtime:		constant				<br/>
 */
template <typename DataType> bool operator==(/*{{{*/
		const CustomBox<DataType> &a, const CustomBox<DataType> &b)
{
	return *a.data == *b.data;
}/*}}}*/

/**
 * Runtime:		constant				<br/>
 */
template <typename DataType> bool operator!=(/*{{{*/
		const CustomBox<DataType> &a, const CustomBox<DataType> &b)
{
	return *a.data != *b.data;
}/*}}}*/

template <typename DataType>
class SorterContainer
{
public:
	class iterator
	{
	public:
		iterator(const BinTree<CustomBox<DataType>> * ptr) : ptr(ptr) {}
		iterator operator++() { ptr = ptr->next(); return *this; }
		bool operator!=(const iterator & other) { return ptr != other.ptr; }
		const DataType& operator*() const { return *(ptr->data->data); }
		//DataType& operator*() { return *(ptr->data->data); }
	private:
		const BinTree<CustomBox<DataType>>* ptr;
	};

#define SorterContainerInitializer : \
		root(NULL), \
		custom(NULL)
public:
	explicit SorterContainer(const SorterContainer<DataType> & orig)
		SorterContainerInitializer { copy(orig); }
	const SorterContainer & operator=(const SorterContainer<DataType> & orig) = delete;

	SorterContainer(SorterContainer<DataType> && temp) :
		root(temp.root),
		custom(temp.custom)
	{
		temp.root = 0;
		temp.custom = 0;
	}

	const SorterContainer & operator=(SorterContainer<DataType> && temp)
	{
		clear();

		root = temp.root;
		custom = temp.custom;

		temp.root = 0;
		temp.custom = 0;

		return *this;
	}

public:
	explicit SorterContainer() SorterContainerInitializer { }

	/**
	 * Runtime:		linear, O(n)				<br/>
	 */
	virtual ~SorterContainer() { clear(); }

private:
	/**
	 * Runtime:		O(n) + O(n_src * log(n_src))	<br/>
	 */
	void copy(const SorterContainer<DataType> & orig) /*{{{*/
	{
		if(orig.root == NULL){
			this->clear();
			return;
		}

		Object<BinTree< PrimaryBox<DataType> > > newRoot(NULL);
		Object<BinTree< CustomBox<DataType> > > newCustom(NULL);

		Object<DataType> dataObj(NULL);

		BinTree< PrimaryBox<DataType> > *iter;
		for(iter = orig.root->first(); iter != NULL; iter = iter->next()){
			DataType * data = NULL;
			if(iter->data->owner){
				dataObj.ptr = new DataType(*(iter->data->data));
				data = dataObj.ptr;
			} else
				data = iter->data->data;

			Object<CustomBox<DataType> > customBox(
					new CustomBox<DataType>(*this, data,
							iter->data->custom->data->order));
			Object<OwnerBinTree< CustomBox<DataType> > > customNode(
					new OwnerBinTree< CustomBox<DataType> >(customBox.ptr));

			Object<PrimaryBox<DataType> > primaryBox(
					new PrimaryBox<DataType>(data));
			Object<OwnerBinTree< PrimaryBox<DataType> > > primaryNode(
					new OwnerBinTree< PrimaryBox<DataType> >(primaryBox.ptr));

			primaryNode->data->custom = customNode.ptr;
			customNode->data->primary = primaryNode.ptr;

			/* changeing ownership */
			primaryNode->data->owner = iter->data->owner;
			dataObj.ptr = NULL;

			primaryNode->insert(newRoot.ptr);
			primaryNode.ptr = NULL;
			customNode->insert(newCustom.ptr);
			customNode.ptr = NULL;
		}
		/* No exception should occur from here. */

		/* Everything ok, lets clear the old content. */
		if(root){
			custom->clear();
			delete custom;
			root->clear();
			delete root;
		}

		root = newRoot.release();
		custom = newCustom.release();
	}/*}}}*/

protected:
	BinTree< PrimaryBox<DataType> > *root;
	BinTree< CustomBox<DataType> > *custom;
	/* FIXME : before overflow, lets reassign order values from zero again. */
	static long long unsigned lastOrder;

public:
	const iterator begin() const { if(!custom) return 0; return iterator(custom->first()); }
	const iterator end() const { return iterator(0); }

	/**
	 * Runtime:		O(n) <br/>
	 *
	 * Note: <br/>
	 *  This method is expected to be redefined in child classes to support
	 *  custom orderings for special data structures.
	 */
	virtual int compare(const DataType &a, const DataType &b) const
	{
		(void)a;
		(void)b;

		return 0;
	}

	/**
	 * Runtime:		O(n) + 2 * O(n * log n)	<br/>
	 */
	void sort() /*{{{*/
	{
		if(root == NULL)
			return;

		Object<BinTree< CustomBox<DataType> > > newCustom(NULL);

		BinTree< PrimaryBox<DataType> > *iter;
		for(iter = root->first(); iter != NULL; iter = iter->next()){
			Object<CustomBox<DataType> > customBox(
					new CustomBox<DataType>(*this, iter->data->data,
							iter->data->custom->data->order));
			Object<OwnerBinTree< CustomBox<DataType> > > customNode(
					new OwnerBinTree< CustomBox<DataType> >(customBox.ptr));

			customNode->data->primary = iter;

			customNode->insert(newCustom.ptr);
			customNode.ptr = NULL;
		}
		/* From here, no exception should happen. */

		/* Update primary nodes to point to new custom ones. */
		BinTree< CustomBox<DataType> > *citer;
		for(citer = newCustom->first(); citer != NULL; citer = citer->next())
			root->query( *(citer->data->data) )->data->custom = citer;

		/* Everything ok, lets clear the old content. */
		if(custom){
			custom->clear();
			delete custom;
		}

		custom = newCustom.release();
	}/*}}}*/

	/**
	 * Runtime:		guess O(log(n))	<br/>
	 */
	void removeAt(unsigned i) /*{{{*/
	{
		ENSURE(root != NULL, IndexOutOfRange);

		BinTree< CustomBox<DataType> > *node = custom->queryAt(i);
		node->remove(custom);
		node->data->primary->remove(root);
		delete node->data->primary;
		delete node;
	}/*}}}*/

	/**
	 * Runtime:		guess O(log(n))				<br/>
	 */
	void remove(const DataType &t) /*{{{*/
	{
		ENSURE(root != NULL, ObjectNotFound);

		BinTree< PrimaryBox<DataType> > *node = root->template query<DataType>(t);
		node->data->custom->remove(custom);
		node->remove(root);
		delete node->data->custom;
		delete node;
	}/*}}}*/

	/**
	 * Runtime:		guess O(log(n))				<br/>
	 */
	template <class TypeRemove>
	void remove(const TypeRemove &tr) /*{{{*/
	{
		ENSURE(root != NULL, ObjectNotFound);

		BinTree< PrimaryBox<DataType> > *node = root->template query<TypeRemove>(tr);
		node->data->custom->remove(custom);
		node->remove(root);
		delete node->data->custom;
		delete node;
	}/*}}}*/

	/**
	 * Runtime:		linear, O(n)				<br/>
	 */
	void clear()/*{{{*/
	{
		if(!root)
			return;

		custom->clear();
		delete custom;
		custom = NULL;
		root->clear();
		delete root;
		root = NULL;
	}/*}}}*/

	/**
	 * Runtime:		guess O(log(n))	<br/>
	 */
	const DataType & last() const throw() /*{{{*/
	{
		ENSURE(root != NULL, IndexOutOfRange);

		return *(custom->last()->data->data);
	}
	/*DataType & last() throw()
	{
		ENSURE(root != NULL, IndexOutOfRange);

		return *(custom->last()->data->data);
	}*/
	/*}}}*/

	/**
	 * Runtime:		O(log n)	<br/>
	 * Precondition:	i < size()	<br/>
	 */
	const DataType& operator[](unsigned i) const
	{
		ENSURE(root != NULL, IndexOutOfRange);
		return *(custom->queryAt(i)->data->data);
	}
	inline DataType& operator[](unsigned i)
	{
		ENSURE(root != NULL, IndexOutOfRange);
		return *(custom->queryAt(i)->data->data);
	}
	const DataType& queryAt(unsigned i) const /*{{{*/
	{
		ENSURE(root != NULL, IndexOutOfRange);
		return *(custom->queryAt(i)->data->data);
	}/*}}}*/

	/**
	 * Runtime:		O(log(n))				<br/>
	 */
	const DataType& query(const DataType &t) const /*{{{*/
	{
		ENSURE(root != NULL, ObjectNotFound);

		return *(root->template query<DataType>(t)->data->data);
	}/*}}}*/

	/**
	 * Runtime:		O(log(n))				<br/>
	 */
	template <typename TypeQuery>
	const DataType& query(const TypeQuery &tq) const /*{{{*/
	{
		ENSURE(root != NULL, ObjectNotFound);

		return *(root->template query<TypeQuery>(tq)->data->data);
	}/*}}}*/

	/**
	 * Runtime:		constant	<br/>
	 */
	bool empty() const/*{{{*/
	{
		return root == NULL;
	}/*}}}*/

	/**
	 * Runtime:		O(log(n))			<br/>
	 */
	unsigned index(const DataType &t) const /*{{{*/
	{
		return index<DataType>(t);
	}/*}}}*/

	/**
	 * Runtime:		O(log(n))			<br/>
	 */
	template <class TypeIndex>
	unsigned index(const TypeIndex &ti) const /*{{{*/
	{
		ENSURE(root != NULL, ObjectNotFound);

		BinTree< PrimaryBox<DataType> > *iter = root->template query<TypeIndex>(ti);

		return iter->data->custom->index();
	}/*}}}*/

	/**
	 * Runtime:		constant	<br/>
	 */
	unsigned size() const/*{{{*/
	{
		return root ? root->size : 0;
	}/*}}}*/

	/**
	 * Runtime:		O(log(n))			<br/>
	 */
	bool has(const DataType &t) const/*{{{*/
	{
		return has<DataType>(t);
	}/*}}}*/

	/**
	 * Runtime:		O(log(n))				<br/>
	 */
	template <class TypeHas>
	bool has(const TypeHas &th) const/*{{{*/
	{
		return root->has(th);
	}/*}}}*/

	/**
	 * Runtime:		O(n)					<br/>
	 */
	bool isEqual(const SorterContainer<DataType> &c) const/*{{{*/
	{
		if(root == NULL){
			if(c.root == NULL)
				return true;
			else
				return false;
		} else if(c.root == NULL)
			return false;

		return root->isEqual(*(c.root));

	}/*}}}*/

#ifndef PERFMODE
	/* Debug helpers {{{ */

	/**
	 * Check the validity of invariant on the given node
	 *
	 * Runtime:		linear, O(n)	<br/>
	 */
	void validity()/*{{{*/
	{
		if(root)
			root->validity();
	}/* }}} */
#ifdef DEBUG
	/**
	 * Runtime:		constant	<br/>
	 */
	const char *printOwnerBinTree(const OwnerBinTree< PrimaryBox<DataType> > *node) const /*{{{*/
	{
		static char address[128];
		snprintf(address, 128, "%p", node->data->data);
		return address;
	}/* }}} */

	/**
	 * Runtime:		linear, O(n)		<br/>
	 */
	void dump() const /*{{{*/
	{
		if(root)
			root->dump();
	}/*}}}*/

	/**
	 * Runtime:		linear, O(n)		<br/>
	 */
	void dump(const BinTreeToCStrIface<CustomBox<DataType> > &toCStrDelegate) const /*{{{*/
	{
		if(custom)
			custom->dump(&toCStrDelegate);
	}/*}}}*/

	/* }}} */
#endif
	/* }}} */
#endif
};

template <typename DataType>
long long unsigned SorterContainer<DataType>::lastOrder = 0;

}

#endif
