/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2009 Csaszar, Peter
 */

#include <csjp_sorter_owner_container.h>
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
class CharDelegate : public csjp::BinTreeToCStrIface<csjp::CustomBox<Char> >
{
	public:
	void toCStr(char res[256], const csjp::CustomBox<Char> &box) const
	{
		snprintf(res, 256, "%d~%c at %p", box.data->data, box.data->data, &box);
	}
};

CharDelegate chrd;
#endif
#endif

class CharContainer : public csjp::SorterOwnerContainer<Char>
{
public:
	explicit CharContainer() :
		csjp::SorterOwnerContainer<Char>(),
		reverseOrdered(false)
	{
	}
	~CharContainer(){}
	virtual int compare(const Char &a, const Char &b) const
	{
		if(reverseOrdered)
			return (b < a) ? -1 : 1; /* Reverse lexical order */
		else
			return csjp::SorterContainer<Char>::compare(a, b);
	}

	explicit CharContainer(const CharContainer & src) :
		csjp::SorterOwnerContainer<Char>(src),
		reverseOrdered(src.reverseOrdered)
	{
	}

	CharContainer(CharContainer && temp) :
		csjp::SorterOwnerContainer<Char>(move_cast(temp)),
		reverseOrdered(temp.reverseOrdered)
	{
		temp.reverseOrdered = false;
	}

	CharContainer& operator=(const CharContainer & src)
	{
		CharContainer c(src);
		csjp::swap(c, *this);
		return *this;
	}

	CharContainer& operator=(CharContainer && temp)
	{
		csjp::SorterOwnerContainer<Char>::operator=(move_cast(temp));
		reverseOrdered = temp.reverseOrdered;
		temp.reverseOrdered = false;
		return *this;
	}

	bool reverseOrdered;
};

class TestSorterOwnerContainer
{
public:
	void empty();
	void swap();
	void singleNode();
	void manyOrderedNodes();
	void manyUnorderedNodes();
	void specialCase1();
	void specialCaseForDoubleUnbalancedness();
};

void TestSorterOwnerContainer::empty()
{
	CharContainer c1;
	CharContainer c2;

	VERIFY(c1.size() == 0);
	VERIFY(c1.empty() == true);
	IN_SAFEMODE(c1.validity());

	VERIFY(c1 == c2);

	c1 = c2;

	VERIFY(c1.size() == 0);
	VERIFY(c1.empty() == true);
	IN_SAFEMODE(c1.validity());

	VERIFY(c1 == c2);
}

void TestSorterOwnerContainer::swap()
{
	csjp::Object<Char> ch(NULL);

	CharContainer c1, c2;

	c1.add(new Char('s'));

	IN_SAFEMODE(c1.validity());
	IN_SAFEMODE(c2.validity());

	VERIFY(c1.size() == 1);
	VERIFY(c1.empty() == false);
	VERIFY(c1 != c2);
	VERIFY(c1.has(Char('s')));
	VERIFY(c1.index(Char('s')) == 0);
	VERIFY(c1.queryAt((unsigned)0) == Char('s'));

	csjp::swap(c2, c1);

	IN_SAFEMODE(c1.validity());
	IN_SAFEMODE(c2.validity());

	VERIFY(c1.size() == 0);
	VERIFY(c1.empty());
	VERIFY(c2.size() == 1);
	VERIFY(c2.empty() == false);
	VERIFY(c2 != c1);
	VERIFY(c2.has(Char('s')));
	VERIFY(c2.index(Char('s')) == 0);
	VERIFY(c2.queryAt((unsigned)0) == Char('s'));
}

void TestSorterOwnerContainer::singleNode()
{
	csjp::Object<Char> ch(NULL);

	CharContainer c1, c2;

	c1.add(new Char('s'));

	VERIFY(c1.size() == 1);
	VERIFY(c1.empty() == false);

	IN_SAFEMODE(c1.validity());

	VERIFY(c1 != c2);

	VERIFY(c1.has(Char('s')));

	VERIFY(c1.index(Char('s')) == 0);

	VERIFY(c1.queryAt((unsigned)0) == Char('s'));

	/* assignement */
	c2 = c1;

	VERIFY(c1 == c2);

	IN_SAFEMODE(c2.validity());

	/* remove */
	c2.removeAt(0);

	VERIFY(c2.size() == 0);
	VERIFY(c2.empty() == true);

	IN_SAFEMODE(c2.validity());

	/* copy constructing */
	CharContainer c3(c1);

	VERIFY(c1 == c3);

	IN_SAFEMODE(c3.validity());

	/* clear */
	c3.clear();

	VERIFY(c3.size() == 0);
	VERIFY(c3.empty() == true);

	IN_SAFEMODE(c3.validity());
}

void TestSorterOwnerContainer::manyOrderedNodes()
{
	csjp::Object<Char> ch(NULL);

	CharContainer c1, c2;

	TESTSTEP("Fill a container with ordered letters.");
	unsigned c;
	for(c='a'; c <= 'z'; c++){
		c1.add(new Char(c));
	}

	VERIFY(c1.size() == 'z'-'a'+1);
	VERIFY(c1.empty() == false);

	IN_SAFEMODE(c1.validity());

	for(c='a'; c <= 'z'; c++){
		/*MSG("c1.has(%c)", c);*/
		VERIFY(c1.has(Char(c)));
	}

	for(c='a'; c <= 'z'; c++){
		VERIFY(c1.index(Char(c)) == c-'a');
	}

	for(c='a'; c <= 'z'; c++){
		VERIFY(c1.queryAt(c-'a') == Char(c));
	}

	TESTSTEP("C++11 foreach");
	Char prev('a'-1);
	unsigned i = 0;
	for(const auto& c : c1){
		//printf("Char: %c at %u\n", c.data, i);
		VERIFY(prev < c);
		i++;
	}
	//printf("i: %u\n", i);
	VERIFY(i == c1.size());

	TESTSTEP("Fill a second container with every second letter.");
	/* second half sized container */
	for(c='a'; c <= 'z'; c++){
		if(c%2)
			c2.add(new Char(c));
	}

	IN_SAFEMODE(c2.validity());

	VERIFY(c1 != c2);

	TESTSTEP("Copy constructing.");
	/* copy constructing */
	CharContainer c3(c1);

	IN_SAFEMODE(c3.validity());

	VERIFY(c3.size() == c1.size());
	VERIFY(c3 == c1);

	TESTSTEP("Copying.");
	/* assignement */
	c1 = c2;

	IN_SAFEMODE(c1.validity());

	VERIFY(c1.size() == c2.size());
	VERIFY(c1 == c2);

	TESTSTEP("Remove every second letter from copy constructed container.");
	/* remove */
	for(c='z'; c >= 'a'; c--){
		if(!(c%2))
			c3.removeAt(c-'a');
		IN_SAFEMODE(c3.validity());
	}

	IN_SAFEMODE(c3.validity());

	VERIFY(c3.size() == c2.size());
	VERIFY(c3 == c2);

	TESTSTEP("Clear the copy constructed container.");
	/* clear */
	c3.clear();

	VERIFY(c3.size() == 0);
	VERIFY(c3.empty() == true);

	IN_SAFEMODE(c3.validity());
}

/*
#define LIST(c) \
	DBG("Content of " #c ":"); \
	for(unsigned i=0; i<c.size(); i++) \
		DBG("%: %", i, c.queryAt(i).data);
*/
#define LIST(c) ;

void TestSorterOwnerContainer::manyUnorderedNodes()
{
	csjp::Object<Char> ch(NULL);

	CharContainer c1, c2;

	TESTSTEP("Fill c1 in pseudo random order.");
	unsigned i, c, s, gen;
	s = 'z'-'a'+1;
	gen = 17;
	c = gen;
	for(i=0; i<s; i++){
		c1.add(new Char(c+'a'));
		c += gen;
		c %= s;
		IN_SAFEMODE(c1.validity());
	}
	LIST(c1);

	VERIFY(c1.size() == 'z'-'a'+1);
	VERIFY(c1.empty() == false);

	IN_SAFEMODE(c1.validity());

	for(c='a'; c <= 'z'; c++){
		VERIFY(c1.has(Char(c)));
	}

	s = 'z'-'a'+1;
	gen = 17;
	c = gen;
	for(i=0; i<s; i++){
		VERIFY(c1.index(Char(c+'a')) == i);
		VERIFY(c1.queryAt(i) == Char(c+'a'));
		c += gen;
		c %= s;
		IN_SAFEMODE(c1.validity());
	}

	TESTSTEP("Custom (reverse) ordering c1.");
	c1.reverseOrdered = true;
	c1.sort();
	LIST(c1);
	for(c='a', i='z'; c <= 'z'; c++, i--){
		VERIFY(c1.queryAt(c-'a') == Char(i));
	}

	TESTSTEP("Custom (normal) ordering c1.");
	c1.reverseOrdered = false;
	c1.sort();
	LIST(c1);
	for(c='a'; c <= 'z'; c++){
		VERIFY(c1.queryAt(c-'a') == Char(c));
	}

	/* second half sized container */
	TESTSTEP("Fill c2 with even values.");
	for(c='a'; c <= 'z'; c++){
		if(c%2)
			c2.add(new Char(c));
	}
	LIST(c2);

	IN_SAFEMODE(c2.validity());

	VERIFY(c1 != c2);

	TESTSTEP("Fill c3 by copy cosntructing from c1.");
	CharContainer c3(c1);

	LIST(c3);

	IN_SAFEMODE(c3.validity());

	VERIFY(c3.size() == c1.size());
	VERIFY(c3 == c1);

	TESTSTEP("Assign c2 into c1.");
	c1 = c2;

	LIST(c1);

	IN_SAFEMODE(c1.validity());

	VERIFY(c1.size() == c2.size());
	VERIFY(c1 == c2);

	TESTSTEP("Remove even values from c3.");
	for(c='z'; c >= 'a'; c--){
		if(!(c%2))
			c3.removeAt(c-'a');
		IN_SAFEMODE(c3.validity());
	}

	IN_SAFEMODE(c3.validity());

	LIST(c3);

	VERIFY(c3.size() == c2.size());
	VERIFY(c3 == c2);

	TESTSTEP("Clear c3.");
	c3.clear();
	LIST(c3);

	VERIFY(c3.size() == 0);
	VERIFY(c3.empty() == true);

	IN_SAFEMODE(c3.validity());
}

void TestSorterOwnerContainer::specialCase1()
{
	csjp::Object<Char> ch(NULL);

	CharContainer cont;

	cont.add(new Char('o')); // 14
	cont.add(new Char('h')); // 7
	cont.add(new Char('j')); // 9
	cont.add(new Char('g')); // 6
	cont.add(new Char('c')); // 2
	cont.add(new Char('l')); // 11
	cont.add(new Char('i')); // 8
	cont.add(new Char('m')); // 12
	cont.add(new Char('d')); // 3
	cont.add(new Char('k')); // 10
	cont.add(new Char('n')); // 13
	cont.add(new Char('e')); // 4
	cont.add(new Char('b')); // 1
	cont.add(new Char('a')); // 0
	cont.add(new Char('f')); // 5

	VERIFY(cont.size() == 15);
	VERIFY(cont.empty() == false);

	IN_SAFEMODE(cont.validity());

	/* clear */
	unsigned i;
	CharContainer cp(cont);
	for(i=0; i<cp.size(); i++){
		cont.remove(cp.queryAt(i));
	}

	VERIFY(cont.size() == 0);
	VERIFY(cont.empty() == true);

	IN_SAFEMODE(cont.validity());
}

void TestSorterOwnerContainer::specialCaseForDoubleUnbalancedness()
{
	csjp::Object<Char> ch(NULL);

	CharContainer cont;

	cont.add(new Char('g'));
	cont.add(new Char('P'));
	cont.add(new Char('T'));
	cont.add(new Char('M'));
	cont.add(new Char('E'));
	cont.add(new Char('a'));
	cont.add(new Char('R'));
	cont.add(new Char('d'));
	cont.add(new Char('G'));
	cont.add(new Char('W'));
	cont.add(new Char('f'));
	cont.add(new Char('H'));
	cont.add(new Char('A'));
	cont.add(new Char('5'));
	cont.add(new Char('J'));
	cont.add(new Char('C'));
	cont.add(new Char('D'));
	cont.add(new Char('1'));
	cont.add(new Char('8'));
	cont.add(new Char('3'));
	cont.add(new Char('Y'));
	cont.add(new Char('B'));
	cont.add(new Char('V'));
	cont.add(new Char('N'));
	cont.add(new Char('6'));
	cont.add(new Char('4'));
	cont.add(new Char('e'));
	cont.add(new Char('Q'));
	cont.add(new Char('2'));
	cont.add(new Char('X'));
	cont.add(new Char('U'));
	cont.add(new Char('F'));
	cont.add(new Char('O'));
	cont.add(new Char('K'));
	cont.add(new Char('7'));
	cont.add(new Char('0'));
	cont.add(new Char('Z'));
	cont.add(new Char('S'));
	cont.add(new Char('c'));
	cont.add(new Char('I'));
	cont.add(new Char('L'));
	cont.add(new Char('h'));
	cont.add(new Char('b'));
	cont.add(new Char('9'));
	cont.add(new Char('i'));

	VERIFY(cont.size() == 45);
	VERIFY(cont.empty() == false);

	IN_SAFEMODE(cont.validity());

	/* clear */
	unsigned i;
	CharContainer cp(cont);
	for(i=0; i<cp.size(); i++){
		cont.remove(cp.queryAt(i));
	}

	VERIFY(cont.size() == 0);
	VERIFY(cont.empty() == true);

	IN_SAFEMODE(cont.validity());
}

TEST_INIT(SorterOwnerContainer)

	TEST_RUN(empty);
	TEST_RUN(swap);
	TEST_RUN(singleNode);
	TEST_RUN(manyOrderedNodes);
	TEST_RUN(manyUnorderedNodes);
	TEST_RUN(specialCase1);
	TEST_RUN(specialCaseForDoubleUnbalancedness);

TEST_FINISH(SorterOwnerContainer)
