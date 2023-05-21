/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2011 Csaszar, Peter
 */

#include <csjp_ownerbintree.h>

#include <csjp_test.h>

class Char
{
public:
	static int constructions;
	static int copyConstructions;
	static int destructions;

	static bool leakFree()
	{
		bool ret = (0 == constructions + copyConstructions - destructions);
		if(!ret){
			fprintf(stderr, "Current leak results:\n");
			fprintf(stderr, "---------------------\n");
			fprintf(stderr, "Constructions:      %d\n", constructions);
			fprintf(stderr, "Copy constructions: %d\n", copyConstructions);
			fprintf(stderr, "Destructions:       %d\n", destructions);
			fprintf(stderr, "Remaining:          %d\n",
					constructions + copyConstructions - destructions);
			fprintf(stderr, "---------------------\n");
			fflush(stderr);
		}
		return ret;
	}

	static void resetLeakCount()
	{
		constructions = 0;
		copyConstructions = 0;
		destructions = 0;
	}

public:
	Char()
	{
		constructions++;
		data = ' ';
	}

	Char& operator=(const Char & c)
	{
		data = c.data;
		return *this;
	}

	Char(unsigned char c)
	{
		constructions++;
		data = c;
	}

	Char(const Char &c)
	{
		copyConstructions++;
		data = c.data;
	}

	~Char()
	{
		destructions++;
	}

	unsigned char data;
};
int Char::constructions = 0;
int Char::copyConstructions = 0;
int Char::destructions = 0;

bool operator!=(const Char &a, const Char &b)
{
	return a.data != b.data;;
}

bool operator==(const Char &a, const Char &b)
{
	return a.data == b.data;;
}

bool operator<(const Char &a, const Char &b)
{
	return a.data < b.data;;
}

#ifndef PERFMODE
#ifdef DEBUG
class CharDelegate : public csjp::BinTreeToCStrIface<Char>
{
	public:
	void toCStr(char res[256], const Char &data) const
	{
		snprintf(res, 256, "%d~%c at %p", data.data, data.data, &data);
	}
};

CharDelegate chrd;
#endif
#endif

void add(csjp::BinTree<Char> *&co, Char _c){
	csjp::Object<Char> c(new Char(_c));
	csjp::BinTree<Char> *d = new csjp::OwnerBinTree<Char>(c.ptr);
	d->insert(co);
}

class TestBinTree
{
public:
	void singleBinTree();
	void doubleBinTree();
	void manyOrderedBinTrees();
	void manyUnorderedBinTrees();
	void specialCase1();
	void specialCaseForDoubleUnbalancedness();
};

void TestBinTree::singleBinTree()
{
	csjp::Object<Char> charS(new Char('s'));
	csjp::Object<Char> charT(new Char('t'));

	csjp::Object<csjp::BinTree<Char> > c1(new csjp::OwnerBinTree<Char>(charS.ptr));
	csjp::Object<csjp::BinTree<Char> > c2(new csjp::OwnerBinTree<Char>(charT.ptr));

	VERIFY(c1->size == 1);

	IN_SAFEMODE(c1->validity());

	VERIFY(c1 != c2);

	VERIFY(c1->has<Char>(Char('s')));

	VERIFY(c1->index() == 0);

	VERIFY(*(c1->query(Char('s'))->data) == Char('s'));

	VERIFY(*(c1->queryAt((unsigned)0)->data) == Char('s'));

	/* clear */
	c1->clear();
	c2->clear();
}

void TestBinTree::doubleBinTree()
{
	csjp::Object<Char> charS(new Char('s'));
	csjp::Object<Char> charT(new Char('t'));
	csjp::Object<Char> charY(new Char('y'));

	csjp::Object<csjp::BinTree<Char> > c1(new csjp::OwnerBinTree<Char>(charS.ptr));
	csjp::Object<csjp::BinTree<Char> > c2(new csjp::OwnerBinTree<Char>(charT.ptr));

	csjp::BinTree<Char> * d = new csjp::OwnerBinTree<Char>(charY.ptr);
	d->insert(c1.ptr);

	VERIFY(c1->size == 2);

	IN_SAFEMODE(c1->validity());

	VERIFY(c1 != c2);

	VERIFY(c1->has<Char>(Char('y')));
/*
	DBG("c1 tree:");
	c1->dump(&chrd);
	c1->dump();
*/
	VERIFY(c1->index() == 0);
	VERIFY(d->index() == 1);

	VERIFY(*(c1->query(Char('s'))->data) == Char('s'));

	VERIFY(*(c1->queryAt((unsigned)0)->data) == Char('s'));

	VERIFY(*(c1->query(Char('y'))->data) == Char('y'));

	VERIFY(*(c1->queryAt((unsigned)1)->data) == Char('y'));
}

void TestBinTree::manyOrderedBinTrees()
{
	csjp::Object<Char> ch(NULL);

	csjp::Object<csjp::BinTree<Char> > c1(NULL);
	csjp::Object<csjp::BinTree<Char> > c2(NULL);
	csjp::BinTree<Char> * d = NULL;
	const csjp::BinTree<Char> *cd;

	unsigned c;
	for(c='a'; c <= 'z'; c++){
		ch.ptr = new Char(c);
		d = new csjp::OwnerBinTree<Char>(ch.ptr);
		d->insert(c1.ptr);
	}

	VERIFY(c1->size == 'z'-'a'+1);

	IN_SAFEMODE(c1->validity());

	for(c='a'; c <= 'z'; c++){
		/*DBG("c1.has(%)", c);*/
		VERIFY(c1->has(Char(c)));
	}

	for(c='a'; c <= 'z'; c++){
		cd = c1->query(Char(c));
		VERIFY(cd != NULL);
		VERIFY(cd->index() == c-'a');
	}

	for(c='a'; c <= 'z'; c++){
		cd = c1->queryAt(c-'a');
		VERIFY(cd != NULL);
		VERIFY(*cd->data == Char(c));
	}

	/* second half sized treenode */
	for(c='a'; c <= 'z'; c++){
		if(c%2){
//			DBG("Insert % - % ...", c, c);
			ch.ptr = new Char(c);
			d = new csjp::OwnerBinTree<Char>(ch.ptr);
			d->insert(c2.ptr);
		}
	}

	IN_SAFEMODE(c2->validity());

	VERIFY(*c1 != *c2);

	/* remove elemetns from c1 */

	IN_SAFEMODE(c1->validity());

	for(c='z'; c >= 'a'; c--){
		if(!(c%2)){
//			DBG("Remove % - % ...", c, c);
			d = c1->query(Char(c));
			VERIFY(d != NULL);
			d->remove(c1.ptr);
			delete d;
			IN_SAFEMODE(c1->validity());
		}
	}

	IN_SAFEMODE(c1->validity());

	VERIFY(c1->size == c2->size);
	VERIFY(*c1 == *c2);

	/* clear c1, c2 */
	c1->clear();
	c2->clear();
}

void TestBinTree::manyUnorderedBinTrees()
{
	csjp::Object<Char> ch(NULL);

	csjp::BinTree<Char> *c1, *c2, *d;
	const csjp::BinTree<Char> *cd;
	c1 = NULL;
	c2 = NULL;

	unsigned i, c, s, gen;
	s = 'z'-'a'+1;
	gen = 17;
	c = gen;
	for(i=0; i<s; i++){
		ch.ptr = new Char(c+'a');
		d = new csjp::OwnerBinTree<Char>(ch.ptr);
		d->insert(c1);

		c += gen;
		c %= s;
		IN_SAFEMODE(c1->validity());
	}

	VERIFY(c1->size == 'z'-'a'+1);

	IN_SAFEMODE(c1->validity());

	for(c='a'; c <= 'z'; c++){
		VERIFY(c1->has(Char(c)));
	}

	for(c='a'; c <= 'z'; c++){
		cd = c1->query(Char(c));
		VERIFY(cd != NULL);
		VERIFY(cd->index() == c-'a');
	}

	for(c='a'; c <= 'z'; c++){
		cd = c1->queryAt(c-'a');
		VERIFY(cd != NULL);
		VERIFY(*cd->data == Char(c));
	}

	/* second half sized treenode */
	for(c='a'; c <= 'z'; c++){
		if(c%2){
//			DBG("Insert % - % ...", c, c);
			ch.ptr = new Char(c);
			d = new csjp::OwnerBinTree<Char>(ch.ptr);
			d->insert(c2);
		}
	}

	IN_SAFEMODE(c2->validity());

	VERIFY(*c1 != *c2);

	/* remove elemetns from c1 */

	IN_SAFEMODE(c1->validity());

	for(c='z'; c >= 'a'; c--){
		if(!(c%2)){
//			DBG("Remove % - % ...", c, c);
			d = c1->query(Char(c));
			VERIFY(d != NULL);
			d->remove(c1);
			delete d;
			IN_SAFEMODE(c1->validity());
		}
	}

	IN_SAFEMODE(c1->validity());

	VERIFY(c1->size == c2->size);
	VERIFY(*c1 == *c2);

	/* clear c1, c2 */
	c1->clear();
	delete c1;
	c2->clear();
	delete c2;
}

void TestBinTree::specialCase1()
{
	csjp::BinTree<Char> *cont = NULL;

	add(cont, 'o'); // 14
	add(cont, 'h'); // 7
	add(cont, 'j'); // 9
	add(cont, 'g'); // 6
	add(cont, 'c'); // 2
	add(cont, 'l'); // 11
	add(cont, 'i'); // 8
	add(cont, 'm'); // 12
	add(cont, 'd'); // 3
	add(cont, 'k'); // 10
	add(cont, 'n'); // 13
	add(cont, 'e'); // 4
	add(cont, 'b'); // 1
	add(cont, 'a'); // 0
	add(cont, 'f'); // 5

	VERIFY(cont->size == 15);

	IN_SAFEMODE(cont->validity());

	csjp::BinTree<Char> *cont2 = NULL;

	add(cont2, 'o'); // 14
	add(cont2, 'h'); // 7
	add(cont2, 'j'); // 9
	add(cont2, 'g'); // 6
	add(cont2, 'c'); // 2
	add(cont2, 'l'); // 11
	add(cont2, 'i'); // 8
	add(cont2, 'm'); // 12
	add(cont2, 'd'); // 3
	add(cont2, 'k'); // 10
	add(cont2, 'n'); // 13
	add(cont2, 'e'); // 4
	add(cont2, 'b'); // 1
	add(cont2, 'a'); // 0
	add(cont2, 'f'); // 5

	VERIFY(cont2->size == 15);

	IN_SAFEMODE(cont2->validity());

//	DBG("cp:");
//	cp->dump(&chrd);

	unsigned i;
	for(i=0; i<cont2->size; i++){
		csjp::BinTree<Char> *d;
		const csjp::BinTree<Char> *cd;
		Char c;
		cd = cont2->queryAt(i);
		c = *cd->data;

//		DBG("remove % found at %", c, i);

		d = cont->query(c);
		d->remove(cont);
		delete d;

		if(cont){
/*			DBG("\ncont tree:");
			cont->dump(&chrd);
*/
			IN_SAFEMODE(cont->validity());
		}
	}

	VERIFY(cont == NULL);

	cont2->clear();
	delete cont2;
}

void TestBinTree::specialCaseForDoubleUnbalancedness()
{
	csjp::BinTree<Char> *cont = NULL;

	add(cont, 'g');
	add(cont, 'P');
	add(cont, 'T');
	add(cont, 'M');
	add(cont, 'E');
	add(cont, 'a');
	add(cont, 'R');
	add(cont, 'd');
	add(cont, 'G');
	add(cont, 'W');
	add(cont, 'f');
	add(cont, 'H');
	add(cont, 'A');
	add(cont, '5');
	add(cont, 'J');
	add(cont, 'C');
	add(cont, 'D');
	add(cont, '1');
	add(cont, '8');
	add(cont, '3');
	add(cont, 'Y');
	add(cont, 'B');
	add(cont, 'V');
	add(cont, 'N');
	add(cont, '6');
	add(cont, '4');
	add(cont, 'e');
	add(cont, 'Q');
	add(cont, '2');
	add(cont, 'X');
	add(cont, 'U');
	add(cont, 'F');
	add(cont, 'O');
	add(cont, 'K');
	add(cont, '7');
	add(cont, '0');
	add(cont, 'Z');
	add(cont, 'S');
	add(cont, 'c');
	add(cont, 'I');
	add(cont, 'L');
	add(cont, 'h');
	add(cont, 'b');
	add(cont, '9');
	add(cont, 'i');

	VERIFY(cont->size == 45);

	IN_SAFEMODE(cont->validity());

	cont->clear();
	delete cont;
}

TEST_INIT(BinTree)

	TEST_RUN(singleBinTree);

	VERIFY(Char::leakFree());
	Char::resetLeakCount();

	TEST_RUN(doubleBinTree);

	TEST_RUN(manyOrderedBinTrees);

	TEST_RUN(manyUnorderedBinTrees);

	TEST_RUN(specialCase1);

	TEST_RUN(specialCaseForDoubleUnbalancedness);

TEST_FINISH(BinTree)
