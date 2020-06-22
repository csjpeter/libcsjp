/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2010 Csaszar, Peter
 */

#ifndef VALUE_BINTREE_H
#define VALUE_BINTREE_H

#include <csjp_string.h>

namespace csjp {

/** Idea:
 *
 * Each ValueBinTree represents a node and a subtree with its descendants.
 * This class is an order statistic tree and thus each node has a
 * field containing the size of the subtree represented by the node.
 * For some more information on the basic theories see
 *  - http://en.wikipedia.org/wiki/Order_statistic_tree
 *
 *   Balancing trick/algorithm is what can be found in for example Red-Black
 *   tree or AVL tree documents: rotating.
 *
 *  Error safety (commonly known as exception safety) :
 *
 *   - noerror/safe : There is absolutely no risk.
 *   - strong : The operation either succeeds, or has no effect at all.
 *   - weak : Operation might have unfinished changes, but no resource leak,
 *     nor failing invariant.
 *
 *   For any of the cases, all the used/called functions shall have same
 *   or better safety.
 */

template <typename DataType>
class ValueBinTree
{
public:
	explicit ValueBinTree(const ValueBinTree<DataType> &) = delete;
	ValueBinTree<DataType>& operator=(const ValueBinTree<DataType> &) = delete;

	ValueBinTree(const ValueBinTree<DataType> && temp) :
		parent(temp.parent),
		left(temp.left),
		right(temp.right),
		size(temp.size),
		data(temp.data)
	{
		temp.parent = 0;
		temp.left = 0;
		temp.right = 0;
		temp.size = 0;
		temp.data = 0;
	}

	ValueBinTree<DataType>& operator=(ValueBinTree<DataType> && temp)
	{
		parent = temp.parent;
		left = temp.left;
		right = temp.right;
		size = temp.size;
		data = temp.data;

		temp.parent = 0;
		temp.left = 0;
		temp.right = 0;
		temp.size = 0;
		temp.data = 0;

		return *this;
	}

public:
	ValueBinTree *parent;
	ValueBinTree *left;
	ValueBinTree *right;
	unsigned size; /* size of this subtree (>=1) */
	DataType data;

	/**
	 * Runtime:		constant	<br/>
	 */
	explicit ValueBinTree() : /*{{{*/
		parent(NULL),
		left(NULL),
		right(NULL),
		size(1),
		data(NULL)
	{
	}/*}}}*/

	/**
	 * Runtime:		constant	<br/>
	 */
	explicit ValueBinTree(DataType & t) : /*{{{*/
		parent(NULL),
		left(NULL),
		right(NULL),
		size(1),
		data(t)
	{
	}/*}}}*/

	/**
	 * Runtime:		constant				<br/>
	 */
	virtual ~ValueBinTree() { clear(); }

	/**
	 * Runtime:		O(log n)	<br/>
	 */
	const ValueBinTree<DataType>* first() const  /*{{{*/
	{
		const ValueBinTree<DataType> *node = this;
		while(node->left != NULL)
			node = node->left;
		return node;
	}
	ValueBinTree<DataType>* first()
	{
		return const_cast<ValueBinTree<DataType> *>(
				const_cast<const ValueBinTree<DataType> *>(this)->first());
	}/*}}}*/

	/**
	 * Runtime:		O(log n)	<br/>
	 */
	const ValueBinTree<DataType>* last() const   /*{{{*/
	{
		const ValueBinTree<DataType> *node = this;
		while(node->right != NULL)
			node = node->right;
		return node;
	}
	ValueBinTree<DataType>* last()
	{
		return const_cast<ValueBinTree<DataType> *>(
				const_cast<const ValueBinTree<DataType> *>(this)->last());
	}/*}}}*/

	/**
	 * Runtime (single random call):	O(log n)	<br/>
	 * Runtime (n call from first to last):	O(n)		<br/>
	 * Postcondition:			can be NULL	<br/>
	 */
	const ValueBinTree<DataType>* prev() const   /*{{{*/
	{
		if(left != NULL)
			return left->last();

		const ValueBinTree<DataType> *node = this;
		const ValueBinTree<DataType> *prev = parent;
		while(prev != NULL && node == prev->left){
			node = prev;
			prev = prev->parent;
		}
		return prev;
	}
	ValueBinTree<DataType>* prev()
	{
		return const_cast<ValueBinTree<DataType> *>(
				const_cast<const ValueBinTree<DataType> *>(this)->prev());
	}/*}}}*/

	/**
	 * Runtime (single random call):	O(log n)	<br/>
	 * Runtime (n call from first to last):	O(n)		<br/>
	 * Postcondition:			can be NULL	<br/>
	 */
	const ValueBinTree<DataType>* next() const   /*{{{*/
	{
		if(right != NULL)
			return right->first();

		const ValueBinTree<DataType> *node = this;
		const ValueBinTree<DataType> *next = node->parent;
		while(next != NULL && node == next->right){
			node = next;
			next = next->parent;
		}
		return next;
	}
	ValueBinTree<DataType>* next()
	{
		return const_cast<ValueBinTree<DataType> *>(
				const_cast<const ValueBinTree<DataType> *>(this)->next());
	}/*}}}*/

	/**
	 * Rotate right: make 'a' (node) to be the right child of 'b' (left child)
	 *
	 * Runtime:		constant	<br/>
	 * Precondition:	has left child	<br/>
	 */
	void rotateRight(ValueBinTree<DataType> *&root)   /*{{{*/
	{
		//DBG("Rotate right on node %", toCStr());

		/* 'b' is left child of 'a' */
		ValueBinTree<DataType> *a = this;
		ValueBinTree<DataType> *b = a->left;

		ENSURE(b != NULL, LogicError);

		/* child trees of 'a' and 'b' */
		ValueBinTree<DataType> *ar = a->right;
		ValueBinTree<DataType> *bl = b->left;
		ValueBinTree<DataType> *br = b->right;

		/* the new parent of 'b' is the old parent of 'a' */
		ValueBinTree<DataType> *p = a->parent;

		/* rotation */

		if(a == root) /* if 'a' was the root, the new root will be 'b' */
			root = b;
		else if(a == p->left) /* was 'a' a left or right child */
			p->left = b;
		else if(a == p->right) /* was 'a' a left or right child */
			p->right = b;
		else
			throw LogicError(EXCLI);

		b->parent = p;
		/*b->left = bl;*/
		b->right = a;

		a->parent = b;
		a->left = br;
		/*a->right = ar;*/

		if(br != NULL)
			br->parent = a;

		/* maintain size changes */
		a->size--;
		if(bl != NULL)
			a->size -= bl->size;

		b->size++;
		if(ar != NULL)
			b->size += ar->size;
	}/*}}}*/

	/**
	 * Rotate left: make 'a' (node) to be the left child of 'b' (right child)
	 *
	 * Runtime:		constant	<br/>
	 * Precondition:	has right child	<br/>
	 */
	void rotateLeft(ValueBinTree<DataType> *&root)  /*{{{*/
	{
		//DBG("Rotate left on node %", toCStr());

		/* 'b' is right child of 'a' */
		ValueBinTree<DataType> *a = this;
		ValueBinTree<DataType> *b = a->right;

		ENSURE(b != NULL, LogicError);

		/* child trees of 'a' and 'b' */
		ValueBinTree<DataType> *al = a->left;
		ValueBinTree<DataType> *bl = b->left;
		ValueBinTree<DataType> *br = b->right;

		/* the new parent of 'b' is the old parent of 'a' */
		ValueBinTree<DataType> *p = a->parent;

		/* rotation */

		if(a == root) /* if 'a' was the root, the new root will be 'b' */
			root = b;
		else if(a == p->left) /* was 'a' a left or right child */
			p->left = b;
		else if(a == p->right) /* was 'a' a left or right child */
			p->right = b;
		else
			throw LogicError(EXCLI);

		b->parent = p;
		b->left = a;
		/*b->right = br;*/

		a->parent = b;
		/*a->left = al;*/
		a->right = bl;

		if(bl != NULL)
			bl->parent = a;

		/* maintain size changes */
		a->size--;
		if(br != NULL)
			a->size -= br->size;

		b->size++;
		if(al != NULL)
			b->size += al->size;
	}/*}}}*/

	/**
	 * Maintain blancedness from given node to the top
	 *
	 * Runtime:		O(log(n))	<br/>
	 */
	void balancing(ValueBinTree<DataType> *&root)   /*{{{*/
	{
		//DBG("Balancing");
		//root->treeValidity();

		ValueBinTree<DataType> *iter;

		/* Lets check for violations */
		for(iter = this; iter != NULL; iter = iter->parent){
			//DBG("balance check on %", iter->toCStr());
			ValueBinTree<DataType> *left = iter->left;
			ValueBinTree<DataType> *right = iter->right;
			unsigned left_size = left ? left->size : 0;
			unsigned right_size = right ? right->size : 0;

			//DBG("ValueBinTree: %, Left_size: %, Right_size: %",
			//		iter->toCStr(), left_size, right_size);

			if(left_size * 2 + 1 < right_size){
				//DBG("balancing to left");
				/* We will rotate to left, so lets take care that the
				 * right child of the right is bigger than the
				 * left  child of the right.*/
				unsigned r_left_size = right->left ? right->left->size : 0;
				unsigned r_right_size = right->right ? right->right->size : 0;
				if(r_right_size < r_left_size){
					//DBG("balancing to left (right)");
					right->rotateRight(root);
					unsigned r2_left_size = right->left ? right->left->size : 0;
					unsigned r2_right_size = right->right ? right->right->size : 0;
					if(r2_left_size * 2 + 1 < r2_right_size){
						//DBG("balancing to left (right-left)");
						right->rotateLeft(root);
					}else if(r2_right_size * 2 + 1 < r2_left_size){
						//DBG("balancing to left (right-left-right)");
						right->rotateRight(root);
					}
				}
				iter->rotateLeft(root);
			}else if(right_size * 2 + 1 < left_size){
				//DBG("balancing to right");
				/* We will rotate to right, so lets take care that the
				 * left  child of the left is bigger than the
				 * right child of the left.*/
				unsigned l_left_size = left->left ? left->left->size : 0;
				unsigned l_right_size = left->right ? left->right->size : 0;
				if(l_left_size < l_right_size){
					//DBG("balancing to right (left)");
					left->rotateLeft(root);
					unsigned l2_left_size = left->left ? left->left->size : 0;
					unsigned l2_right_size = left->right ? left->right->size : 0;
					if(l2_left_size * 2 + 1 < l2_right_size){
						//DBG("balancing to left (right-left)");
						left->rotateLeft(root);
					}else if(l2_right_size * 2 + 1 < l2_left_size){
						//DBG("balancing to left (right-left-right)");
						left->rotateRight(root);
					}
				}
				iter->rotateRight(root);
			}
		}
		//root->treeValidity();
	}/*}}}*/

	/**
	 * Runtime:		O(log(n))	<br/>
	 */
	void insert(ValueBinTree<DataType>*& root)  /*{{{*/
	{
		ValueBinTree<DataType> *iter;

		/* trivial case */
		if(root == NULL){
			root = this;
			root->parent = NULL;
			return;
		}

		/* '<' relation is important so ordering is stable:
		 * Later inserted data with equal key will be inserted after the existing one. */

		/* o(1) < runtime < O(log n) */
		/* lets find the parent / where to insert */
		for(iter = root, parent = NULL; iter != NULL; ){
			parent = iter;
			parent->size++;
			if(data < (iter->data))
				iter = iter->left;
			else
				iter = iter->right;
		}

		/* insert the new node */
		ENSURE(parent != NULL, LogicError);

		if(data < parent->data)
			parent->left = this;
		else
			parent->right = this;

		//DBG("Right after insert and before balance of ", toCStr());
		//root->dump();
		balancing(root);
		//root->treeValidity();
	}/*}}}*/

	/**
	 * Runtime:		guess O(log(n))	<br/>
	 */
	ValueBinTree<DataType>* remove(ValueBinTree<DataType> *&root)  /*{{{*/
	{
		ValueBinTree<DataType> *repl;

		/* find a replacement */
//		DBG("Find replacement for % - %", data, data ? toCStr() : 0);
		if(left != NULL)
			repl = prev()->remove(root);
		else if(right != NULL)
			repl = next()->remove(root);
		else
			repl = NULL;

//		DBG("Insert replacement % - %", repl ? repl->data : 0, repl ? *((unsigned*)(repl->data)) : 0);
		/* insert replacement */
		if(parent != NULL){
			if(this == parent->left)
				parent->left = repl;
			else if(this == parent->right)
				parent->right = repl;
			else
				throw InvariantFailure("Node to be removed is neither "
						"left nor right child of its parent.");
		}
		if(left != NULL)
			left->parent = repl;
		if(right != NULL)
			right->parent = repl;

		if(repl != NULL){
			repl->parent = parent;
			repl->right = right;
			repl->left = left;
			repl->size = size;
		} else { /* one less size for all ancestors */
			ValueBinTree<DataType> *parentIter = parent;
			while(parentIter != NULL){
				parentIter->size -= 1;
				parentIter = parentIter->parent;
			}
		}

//		if(root == this)
//			DBG("Replacement is the new root % - %", repl ? repl->data : 0, repl ? *((unsigned*)(repl->data)) : 0);
		if(root == this)
			root = repl;

//		DBG("Unlink % - %", this->data, toCStr());
		/* unlink the removed node */
		parent = NULL;
		left = NULL;
		right = NULL;
		size = 1;

		/* Lets check for violations */
//		DBG("Balancing root % - %", root ? root->data : 0, root ? *((unsigned*)(root->data)) : 0);
		repl->balancing(root);

		//if(root)
		//	root->treeValidity();

		return this;
	}/*}}}*/

	/**
	 * Runtime:		linear, O(n)				<br/>
	 */
	void clear()   /*{{{*/
	{
		if(left != NULL){
			delete left;
			left = NULL;
		}
		if(right != NULL){
			delete right;
			right = NULL;
		}
		size = 1;
	}/*}}}*/

	/**
	 * Runtime:		O(log(n))				<br/>
	 */
	template <class TypeHas>
	bool has(const TypeHas &th) const  /*{{{*/
	{
		const ValueBinTree<DataType> *iter = this;

		while(iter != NULL){
			if(th < iter->data)
				iter = iter->left;
			else if(iter->data < th)
				iter = iter->right;
			else {
				return true;
			}
		}

		return false;
	}/*}}}*/

	/**
	 * Runtime:		O(log(n))			<br/>
	 */
	unsigned index() const   /*{{{*/
	{
		const ValueBinTree<DataType> *iter = this;

		unsigned i = iter->left ? iter->left->size : 0;

		while(iter->parent != NULL){
			if(iter->parent->right == iter){
				if(iter->parent->left)
					i += iter->parent->left->size;
				i ++;
			}
			iter = iter->parent;
		}

		return i;
	}/*}}}*/

	/**
	 * Runtime:		O(log n)	<br/>
	 * Precondition:	n < size()	<br/>
	 */
	ValueBinTree<DataType>* queryAt(unsigned n) /*{{{*/
	{
		ENSURE(n < size, IndexOutOfRange);

		ValueBinTree<DataType> *iter = this;
		unsigned i = n;

		for(;;){
			ENSURE(iter != NULL, LogicError);
			unsigned left_size = iter->left ? iter->left->size : 0;

			if(i == left_size)
				return iter;

			if(i < left_size){
				iter = iter->left;
			} else {
				iter = iter->right;
				i--;
				i -= left_size;
			}
		}

		/* should never reached */
		ENSURE(false, ObjectNotFound);
		return NULL;
	}/*}}}*/

	/**
	 * Runtime:		O(log(n))				<br/>
	 */
	template <typename TypeQuery>
	const ValueBinTree<DataType>* query(const TypeQuery &tq) const /*{{{*/
	{
		const ValueBinTree<DataType> *iter = this;

		while(iter != NULL){
			if(tq < iter->data)
				iter = iter->left;
			else if(iter->data < tq)
				iter = iter->right;
			else
				return iter;
		}

		throw ObjectNotFound(EXCLI);
	}
	template <typename TypeQuery>
	ValueBinTree<DataType>* query(const TypeQuery &tq)
	{
		return const_cast<ValueBinTree<DataType> *>(
				const_cast<const ValueBinTree<DataType> *>(this)->query(tq));
	}/*}}}*/

	/**
	 * Runtime:		O(n)					<br/>
	 */
	bool isEqual(const ValueBinTree<DataType> &tree) const   /*{{{*/
	{
		if(size != tree.size)
			return false;

		bool ret = true;

		const ValueBinTree<DataType> *iter;
		const ValueBinTree<DataType> *iter2;
		for(iter = first(), iter2 = tree.first();
				iter != NULL;
				iter = iter->next(), iter2 = iter2->next()){
			if((iter->data) != (iter2->data)){
				ret = false;
				break;
			}
		}

		return ret;
	}/*}}}*/

public:
#ifndef PERFMODE
	/* Debug helpers {{{ */

	/**
	 * Check the validity of invariant on the given node
	 *
	 * Runtime:		linear, O(n)	<br/>
	 */
	void validity() const /*{{{*/
	{
		unsigned left_size = left ? left->size : 0;
		unsigned right_size = right ? right->size : 0;
		//DBG("left_size: %, right_size: %, data: %", left_size, right_size, toCStr());

		/* node size right */
		ENSURE(left_size + right_size + 1 == size, InvariantFailure);

		/* node is balanced */
		//ENSURE(left_size * 2 + 1 >= right_size, InvariantFailure);
		//ENSURE(right_size * 2 + 1 >= left_size, InvariantFailure);

		/* node is localy ordered (with against it's children */
		if(left)
			ENSURE(left->data < data, InvariantFailure);
		if(right)
			ENSURE(data < right->data, InvariantFailure);

		/* parent and child are pointing to eachother */
		if(parent)
			ENSURE(parent->left == this || parent->right == this, InvariantFailure);
		if(left)
			ENSURE(left->parent == this, InvariantFailure);
		if(right)
			ENSURE(right->parent == this, InvariantFailure);

		/* node is found by it's data's ordering via has() check */
		ENSURE(has(data), InvariantFailure);

		(void) left_size;
		(void) right_size;
	}/*}}}*/

	void treeValidity() const /*{{{*/
	{
		try{
			validity();
			if(left != NULL)
				left->treeValidity();
			if(right != NULL)
				right->treeValidity();
		} catch (InvariantFailure &e){
			//DBG("Dumping...");
			this->dump();
			throw;
		}
	}/*}}}*/

#ifdef DEBUG
	/**
	 * Runtime:		constant	<br/>
	 */
	const char * toCStr() const /*{{{*/
	{
		static char res[256];
		snprintf(res, 256, "%u", (unsigned)data);
		return res;
	}/* }}} */
	/**
	 * Runtime:		constant	<br/>
	 */
	void toCStr(char res[256]) const /*{{{*/
	{
		snprintf(res, 256, "%u", (unsigned)data);
	}/* }}} */

	/**
	 * Runtime:		linear, O(n)		<br/>
	 */
	void dump() const /*{{{*/
	{
		static char indent[64] = {0};
		static unsigned i = 0;
		//static char *direction = (char*)"Root";
		char res[256];

		if(i == sizeof(indent))
			FATAL("You either have more than 2^% GBytes memory, "
					"or the tree is not balanced.", sizeof(indent) - 30);

		toCStr(res);
		DBG("size: % value: % %", size, res, indent);

		if(left != NULL){
			indent[i] = '-'; i++; indent[i] = 0;
			//direction = (char*)"Left";
			left->dump();
			i--; indent[i] = 0;
		}

		if(right != NULL){
			indent[i] = '+'; i++; indent[i] = 0;
			//direction = (char*)"Right";
			right->dump();
			i--; indent[i] = 0;
		}

		//direction = (char*)"Root";
	}/*}}}*/
#endif
	/* }}} */
#endif
};

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType> bool operator==(/*{{{*/
		const ValueBinTree<DataType> &a, const ValueBinTree<DataType> &b)
{
	return a.isEqual(b);
}/*}}}*/

/**
 * Runtime:		O(n)					<br/>
 */
template <typename DataType> bool operator!=(/*{{{*/
		const ValueBinTree<DataType> &a, const ValueBinTree<DataType> &b)
{
	return !a.isEqual(b);
}/*}}}*/

}

#endif
