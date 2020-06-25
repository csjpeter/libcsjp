/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2009 Csaszar, Peter
 */

#ifndef CONTAINER_H
#define CONTAINER_H

#include <csjp_bintree.h>

namespace csjp {

template <typename DataType>
class Container
{
public:
	class iterator
	{
	public:
		iterator(BinTree<DataType> * ptr) : ptr(ptr) {}
		iterator operator++() const { ptr = ptr->next(); return *this; }
		bool operator!=(const iterator & other) const { return ptr != other.ptr; }
		const DataType& operator*() const { return *(ptr->data); }
		DataType& operator*() { return *(ptr->data); }
	private:
		mutable BinTree<DataType>* ptr;
	};

#define ContainerInitializer : \
	root(0)
public:
	explicit Container(const Container & orig) ContainerInitializer { }
	const Container & operator=(const Container &) = delete;

	Container(Container && temp) :
		root(temp.root)
	{
		temp.root = 0;
	}

	const Container & operator=(Container && temp)
	{
		clear();

		root = temp.root;
		temp.root = 0;

		return *this;
	}

public:
	explicit Container() ContainerInitializer { }
	/**
	 * Runtime:		linear, O(n)				<br/>
	 */
	virtual ~Container() { clear(); }

protected:
	BinTree<DataType> *root;

public:
	const iterator begin() const {
		if(!root) return 0; return iterator(root->first()); }
	const iterator end() const { return iterator(0); }
	iterator begin() { if(!root) return 0; return iterator(root->first()); }
	iterator end() { return iterator(0); }

/*	bool compare(const DataType &a, const DataType &b) const
	{
		return a < b;
	}*/

	/**
	 * Runtime:		guess O(log(n))	<br/>
	 */
	void removeAt(unsigned i) /*{{{*/
	{
		ENSURE(root != NULL, IndexOutOfRange);

		BinTree<DataType> *node = root->queryAt(i);
		node->remove(root);
		ENSURE(root != node, InvariantFailure);
		delete node;
	}/*}}}*/

	/**
	 * Runtime:		guess O(log(n))				<br/>
	 */
	void remove(const DataType &t) /*{{{*/
	{
		ENSURE(root != NULL, IndexOutOfRange);

		BinTree<DataType> *node = root->query(t);
		node->remove(root);
		ENSURE(root != node, InvariantFailure);
		delete node;
	}/*}}}*/

	/**
	 * Runtime:		guess O(log(n))				<br/>
	 */
	template <class TypeRemove>
	void remove(const TypeRemove &tr) /*{{{*/
	{
		ENSURE(root != NULL, IndexOutOfRange);

		BinTree<DataType> *node = root->template query<TypeRemove>(tr);
		node->remove(root);
		delete node;
	}/*}}}*/

	/**
	 * Runtime:		linear, O(n)				<br/>
	 */
	void clear()/*{{{*/
	{
		if(!root)
			return;

		root->clear();
		delete root;
		root = NULL;
	}/*}}}*/

	/**
	 * Runtime:		guess O(log(n))	<br/>
	 */
	const DataType & last() const   /*{{{*/
	{
		ENSURE(root != NULL, IndexOutOfRange);

		return *(root->last()->data);
	}
	DataType & last()
	{
		ENSURE(root != NULL, IndexOutOfRange);

		return *(root->last()->data);
	}/*}}}*/

	/**
	 * Runtime:		O(log n)	<br/>
	 * Precondition:	i < size()	<br/>
	 */
	const DataType& operator[](unsigned i) const
	{
		ENSURE(root != NULL, IndexOutOfRange);
		return *(root->queryAt(i)->data);
	}
	inline DataType& operator[](unsigned i)
	{
		ENSURE(root != NULL, IndexOutOfRange);
		return *(root->queryAt(i)->data);
	}
	const DataType& queryAt(unsigned i) const /*{{{*/
	{
		ENSURE(root != NULL, IndexOutOfRange);
		return *(root->queryAt(i)->data);
	}/*}}}*/

	/**
	 * Runtime:		O(log(n))				<br/>
	 */
	const DataType& query(const DataType &t) const /*{{{*/
	{
		ENSURE(root != NULL, ObjectNotFound);

		return *(root->template query<DataType>(t)->data);
	}/*}}}*/

	/**
	 * Runtime:		O(log(n))				<br/>
	 */
	template <typename TypeQuery>
	const DataType& query(const TypeQuery &tq) const /*{{{*/
	{
		ENSURE(root != NULL, ObjectNotFound);

		return *(root->template query<TypeQuery>(tq)->data);
	}/*}}}*/

	/**
	 * Runtime:		O(log(n))				<br/>
	 */
	template <typename TypeQuery>
	DataType& query(const TypeQuery &tq) /*{{{*/
	{
		ENSURE(root != NULL, ObjectNotFound);

		return *(root->template query<TypeQuery>(tq)->data);
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

		BinTree<DataType> *iter = root;

		iter = root->template query<TypeIndex>(ti);

		return iter->index();
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
	bool isEqual(const Container<DataType> &c) const/*{{{*/
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
	 * Runtime:		linear, O(n)		<br/>
	 */
	void dump(const BinTreeToCStrIface<DataType> *toCStrDelegate = NULL) const /*{{{*/
	{
		if(root){
			root->dump(toCStrDelegate);
		}
	}/*}}}*/

	/* }}} */
#endif
	/* }}} */
#endif
	/* }}} */
};

}

#endif
