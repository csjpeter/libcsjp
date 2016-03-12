/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015-2016 Csaszar, Peter
 */

#include <csjp_array.h>
#include <csjp_object.h>
#include <csjp_test.h>

class TestArray
{
public:
	void empty();
	void singleNode();
	void manyOrderedNodes();
};

void TestArray::empty()
{
	csjp::Array<char> c1;
	csjp::Array<char> c2;

	VERIFY(c1.size() == 0);
	VERIFY(c1.empty() == true);

	VERIFY(c1 == c2);

	VERIFY(c1.size() == 0);
	VERIFY(c1.empty() == true);

	VERIFY(c1 == c2);
}

void TestArray::singleNode()
{
	csjp::Object<char> ch(new char('s'));

	csjp::Array<char> c1(1), c2;

	TESTSTEP("Add one element");
	c1.add(ch);

	VERIFY(c1.size() == 1);
	VERIFY(c1.empty() == false);

	VERIFY(c1 != c2);

	VERIFY(c1.has(char('s')));

	VERIFY(c1.index(char('s')) == 0);

	VERIFY(c1[0] == char('s'));

	TESTSTEP("Copy constructing");
	csjp::Array<char> c3(c1);

	VERIFY(c1 == c3);

	TESTSTEP("Join");
	c1.join(c3);
	VERIFY(c3.length == 0)
	VERIFY(c1.length == 2)
	VERIFY(c1.has(char('s')));
	VERIFY(c1.index(char('s')) == 0);
	VERIFY(c1[0] == char('s'));
	VERIFY(c1[1] == char('s'));

	TESTSTEP("Clear");
	/* clear */
	c1.clear();

	VERIFY(c1.size() == 0);
	VERIFY(c1.empty() == true);

	c2.clear();

	VERIFY(c2.size() == 0);
	VERIFY(c2.empty() == true);

	c3.clear();

	VERIFY(c3.size() == 0);
	VERIFY(c3.empty() == true);
}

void TestArray::manyOrderedNodes()
{
	csjp::Object<char> ch(0);

	csjp::Array<char> c1(26), c2(26);

	TESTSTEP("Fill a container with ordered letters.");
	size_t c;
	for(c='a'; c <= 'z'; c++)
		c1.add(c);

	VERIFY(c1.size() == 'z'-'a'+1);
	VERIFY(c1.empty() == false);

	for(c='a'; c <= 'z'; c++){
		/*MSG("c1.has(%c)", c);*/
		VERIFY(c1.has(char(c)));
	}

	for(c='a'; c <= 'z'; c++){
		VERIFY(c1.index(char(c)) == c-'a');
	}

	for(c='a'; c <= 'z'; c++){
		VERIFY(c1[c-'a'] == char(c));
	}

	TESTSTEP("C++11 foreach");
	char prev('a'-1);
	unsigned i = 0;
	for(auto& c : c1){
		//printf("char: %c at %u\n", c, i);
		VERIFY(prev < c);
		i++;
	}
	//printf("i: %u\n", i);
	VERIFY(i == c1.length);

	TESTSTEP("Fill a second container with every second letter.");
	/* second half sized container */
	for(c='a'; c <= 'z'; c++){
		if(c%2)
			c2.add(char(c));
	}

	VERIFY(c1 != c2);

	TESTSTEP("Copy constructing.");
	/* copy constructing */
	csjp::Array<char> c3(c1);

	VERIFY(c3.size() == c1.size());
	VERIFY(c3 == c1);

	TESTSTEP("Copying.");
//	c3.dump(&chrd);

	TESTSTEP("Remove every second letter from copy constructed container.");
	/* remove */
	for(c='z'; c >= 'a'; c--){
		if(!(c%2)){
			c3.removeAt(c-'a');
		}
	}

	VERIFY(c3.size() == c2.size());
	VERIFY(c3 == c2);

	TESTSTEP("Clear the copy constructed container.");
	/* clear */
	c3.clear();

	VERIFY(c3.size() == 0);
	VERIFY(c3.empty() == true);
}


TEST_INIT(Array)

	TEST_RUN(empty);

	TEST_RUN(singleNode);

	TEST_RUN(manyOrderedNodes);

TEST_FINISH(Array)
