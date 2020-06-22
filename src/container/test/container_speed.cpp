/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2013-2016 Csaszar, Peter
 */

#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>

#include <csjp_exception.h>
#include <csjp_pod_array.h>
#include <csjp_owner_container.h>
#include <csjp_sorter_owner_container.h>
#include <csjp_file.h>
#include <csjp_stopper.h>

#define ARRAY_SLOW_REMOVE
#define ARRAY_SLOW_LOOKUP
#define GENERAL_CONT

//#undef ARRAY_SLOW_REMOVE
//#undef ARRAY_SLOW_LOOKUP
//#undef GENERAL_CONT

// -r 3 -M 0 -m 10000 -i 2000 -v

struct DataStruct
{
	unsigned data;
	char cstr[1];
};

DataStruct newDataStruct(unsigned u)
{
	DataStruct d;
	d.data = u;
	memset(d.cstr, 'a', sizeof(d.cstr));
	return d;
}

class Data
{
public:
	Data() = delete;
	Data(unsigned u)
	{
		data = u;
		memset(cstr, 'a', sizeof(cstr));
		//str += "aaaaaaaaaaaaaaaa"; // 16 byte data
		/*str += // 8 * 64 char = 512 byte data
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			;*/
		heapData = malloc(8192);
	}
	Data(const Data & orig) : data(orig.data)
	{
		//str.assign(orig.str);
		memcpy(cstr, orig.cstr, sizeof(cstr));
		heapData = malloc(8192);
	}
	Data(Data && temp) : data(temp.data)
	{
		//str = move_cast(temp.str);
		memcpy(cstr, temp.cstr, sizeof(cstr));
		heapData = temp.heapData;
		temp.heapData = 0;
	}
	~Data()
	{
		free(heapData);
	}
	bool isEqual(const Data &d) const
	{
		return (data == d.data)
			//&& (str == d.str)
			&& (!strncmp(cstr, d.cstr, sizeof(cstr)))
			;
	}
	const Data & operator=(const Data &d)
	{
		data = d.data;
		memcpy(cstr, d.cstr, sizeof(cstr));
		//str.assign(d.str);
		heapData = malloc(8192);
		return *this;
	}
	const Data & operator=(Data && temp)
	{
		data = temp.data;
		memcpy(cstr, temp.cstr, sizeof(cstr));
		//str = move_cast(temp.str);
		heapData = temp.heapData;
		temp.heapData = 0;
		return *this;
	}
	bool isEqual(const unsigned int &c) const
	{
		return data == c;
	}
	unsigned data;
	char cstr[512];
	void * heapData;
	//csjp::String str;
};
/*
class DataDelegate : public csjp::BinTreeToCStrIface<Data>
{
	public:
	void toCStr(char res[256], const Data &data) const
	{
		snprintf(res, 256, "'%u' at %p", data.data, &data);
	}
};

DataDelegate dataDelegate;*/

bool operator<(const Data & a, const Data & b)
{
	return a.data < b.data;
}

bool operator<(const unsigned & a, const Data & b)
{
	return a < b.data;
}

bool operator<(const Data & a, const unsigned & b)
{
	return a.data < b;
}

bool operator==(const Data &a, const Data &b)
{
	return a.isEqual(b);
}
/*
bool operator==(const Data &a, const unsigned int &b)
{
	return a.isEqual(b);
}
*/
class DataContainer : public csjp::SorterOwnerContainer<Data>
{
public:
	bool reverseOrdered;
public:
	explicit DataContainer() :
		csjp::SorterOwnerContainer<Data>(),
		reverseOrdered(false)
	{
	}
	~DataContainer(){}

	explicit DataContainer(const DataContainer & src) = delete;
	DataContainer& operator=(const DataContainer & src) = delete;

	virtual int compare(const Data & a, const Data & b) const
	{
		if(reverseOrdered)
			return (b < a) ? -1 : 1; /* Reverse lexical order */
		else
			return csjp::SorterContainer<Data>::compare(a, b);
	}
};

inline bool cmpAsc(const Data & a, const Data & b){ return a < b; }
inline bool cmpDesc(const Data & a, const Data & b){ return b < a; }

class SpeedTest
{
private:
	unsigned repeats;

	csjp::File addElementsPlotFile;
	csjp::File reverseSortingPlotFile;
	csjp::File sameSortingPlotFile;
	csjp::File lookupByIndexPlotFile;
	csjp::File lookupByKeyPlotFile;
	csjp::File removeElementsPlotFile;

	csjp::Stopper stopper;

	std::vector<Data> vector;
	std::map<unsigned, Data> map;
	std::unordered_map<unsigned, Data> umap;
	csjp::Array<Data> array;
	csjp::PodArray<DataStruct> podArray;
	csjp::OwnerContainer<Data> container;
	DataContainer sorterContainer;

public:
	SpeedTest() = delete;
	SpeedTest(unsigned repeats);

	void renames();
	void fillAllTheContainers(unsigned numOfTestItems);
	void clearAllTheContainers(unsigned numOfTestItems);

	void addRemoveTest(unsigned numOfTestItems);
	void reverseSorting(unsigned numOfTestItems);
	void sameOrderSorting(unsigned numOfTestItems);
	void lookupByIndex(unsigned numOfTestItems);
	void lookupByKey(unsigned numOfTestItems);
};

SpeedTest::SpeedTest(unsigned repeats) :
	repeats(repeats),
	addElementsPlotFile("container-speed-test-add-elements.data"),
	reverseSortingPlotFile("container-speed-test-reverse-sorting.data"),
	sameSortingPlotFile("container-speed-test-same-sorting.data"),
	lookupByIndexPlotFile("container-speed-test-lookup-by-index.data"),
	lookupByKeyPlotFile("container-speed-test-lookup-by-key.data"),
	removeElementsPlotFile("container-speed-test-remove-elements.data")
{
	if(addElementsPlotFile.exists())
		addElementsPlotFile.resize(0);
	if(reverseSortingPlotFile.exists())
		reverseSortingPlotFile.resize(0);
	if(sameSortingPlotFile.exists())
		sameSortingPlotFile.resize(0);
	if(lookupByIndexPlotFile.exists())
		lookupByIndexPlotFile.resize(0);
	if(lookupByKeyPlotFile.exists())
		lookupByKeyPlotFile.resize(0);
	if(removeElementsPlotFile.exists())
		removeElementsPlotFile.resize(0);

	addElementsPlotFile.appendf(
			"\"container size\" std::vector std::map std::unordered_map "
			"csjp::Array csjp::PodArray csjp::OwnerContainer csjp::SorterOwnerContainer\n");
	reverseSortingPlotFile.appendf(
			"\"container size\" std::vector std::map std::unordered_map "
			"csjp::Array csjp::PodArray csjp::OwnerContainer csjp::SorterOwnerContainer\n");
	sameSortingPlotFile.appendf(
			"\"container size\" std::vector std::map std::unordered_map "
			"csjp::Array csjp::PodArray csjp::OwnerContainer csjp::SorterOwnerContainer\n");
	lookupByIndexPlotFile.appendf(
			"\"container size\" std::vector std::map std::unordered_map "
			"csjp::Array csjp::PodArray csjp::OwnerContainer csjp::SorterOwnerContainer\n");
	lookupByKeyPlotFile.appendf(
			"\"container size\" std::vector std::map std::unordered_map "
			"csjp::Array csjp::PodArray csjp::OwnerContainer csjp::SorterOwnerContainer\n");
	removeElementsPlotFile.appendf(
			"\"container size\" std::vector std::map std::unordered_map "
			"csjp::Array csjp::PodArray csjp::OwnerContainer csjp::SorterOwnerContainer\n");
}

void SpeedTest::renames()
{
	csjp::String name;

	name.assign("container-speed-test-add-elements");
	name.catf("--repeated-%-times.data", repeats);
	addElementsPlotFile.rename(name);

	name.assign("container-speed-test-reverse-sorting");
	name.catf("--repeated-%-times.data", repeats);
	reverseSortingPlotFile.rename(name);

	name.assign("container-speed-test-same-sorting");
	name.catf("--repeated-%-times.data", repeats);
	sameSortingPlotFile.rename(name);

	name.assign("container-speed-test-lookup-by-index");
	name.catf("--repeated-%-times.data", repeats);
	lookupByIndexPlotFile.rename(name);

	name.assign("container-speed-test-lookup-by-key");
	name.catf("--repeated-%-times.data", repeats);
	lookupByKeyPlotFile.rename(name);

	name.assign("container-speed-test-remove-elements");
	name.catf("--repeated-%-times.data", repeats);
	removeElementsPlotFile.rename(name);
}

void SpeedTest::fillAllTheContainers(unsigned numOfTestItems)
{
	DBG("Filling the containers.\n");
	array.setCapacity(numOfTestItems + 1);
	for(unsigned i=0; i < numOfTestItems; i++){
		vector.emplace_back(i);
		array.add(i);
		podArray.add(newDataStruct(i));
#ifdef GENERAL_CONT
		Data d(i);
		map.insert(std::pair<unsigned, Data>(d.data, d));
		umap.insert(std::pair<unsigned, Data>(d.data, d));
		container.add(new Data(i));
		sorterContainer.add(new Data(i));
#endif
	}
}

void SpeedTest::clearAllTheContainers(unsigned numOfTestItems)
{
	DBG("Cleaning the containers.\n");
	vector.clear();
	array.clear();
	podArray.clear();
#ifdef GENERAL_CONT
	for(unsigned i=0; i < numOfTestItems; i++){
		map.erase(map.begin());
		umap.erase(umap.begin());
		container.removeAt(0);
		sorterContainer.removeAt(0);
	}
#endif
}

void SpeedTest::addRemoveTest(unsigned numOfTestItems)
{
	double vectorAddTime = 0;
	double mapAddTime = 0;
	double umapAddTime = 0;
	double arrayAddTime = 0;
	double podArrayAddTime = 0;
	double containerAddTime = 0;
	double sorterContainerAddTime = 0;

	double vectorRemoveTime = 0;
	double mapRemoveTime = 0;
	double umapRemoveTime = 0;
	double arrayRemoveTime = 0;
	double podArrayRemoveTime = 0;
	double containerRemoveTime = 0;
	double sorterContainerRemoveTime = 0;

	DBG("Adding % number of allocated elements to and removing them from:\n", numOfTestItems);

	std::vector<Data> *vector[repeats];
	std::map<unsigned, Data> *map[repeats];
	std::unordered_map<unsigned, Data> *umap[repeats];
	csjp::Array<Data> *array[repeats];
	csjp::PodArray<DataStruct> *podArray[repeats];
	csjp::OwnerContainer<Data> *container[repeats];
	DataContainer *sorterContainer[repeats];

	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		vector[r] = new std::vector<Data>;
		for(unsigned i=0; i < numOfTestItems; i++)
			vector[r]->emplace_back(i);
	}
	vectorAddTime += stopper.elapsedSoFar();
	DBG("- added to std::vector<Data> in % time.\n", vectorAddTime);

	stopper.restart();
#ifdef ARRAY_SLOW_REMOVE
	for(unsigned r = 0; r < repeats; r++){
		for(auto &i : *vector[r]){
			vector[r]->erase(vector[r]->begin() + vector[r]->size() / 2);
			(void)i;
		}
		delete vector[r];
	}
#else
	for(unsigned r = 0; r < repeats; r++)
		delete vector[r];
#endif
	vectorRemoveTime += stopper.elapsedSoFar();
	DBG("- removed from std::vector<Data> in % time.\n", vectorRemoveTime);



#ifdef GENERAL_CONT
	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		map[r] = new std::map<unsigned, Data>;
		for(unsigned i=0; i < numOfTestItems; i++){
			Data d(i);
			map[r]->insert(std::pair<unsigned, Data>(d.data, d));
		}
	}
	mapAddTime += stopper.elapsedSoFar();
	DBG("- added to std::map<Data> in % time.\n", mapAddTime);

	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		while(map[r]->size())
			map[r]->erase(map[r]->begin());
		delete map[r];
	}
	mapRemoveTime += stopper.elapsedSoFar();
	DBG("- removed from std::map<Data> in % time.\n", mapRemoveTime);



	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		umap[r] = new std::unordered_map<unsigned, Data>;
		for(unsigned i=0; i < numOfTestItems; i++){
			Data d(i);
			umap[r]->insert(std::pair<unsigned, Data>(d.data, d));
		}
	}
	umapAddTime += stopper.elapsedSoFar();
	DBG("- added to std::unordered_map<Data> in % time.\n", umapAddTime);

	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		while(umap[r]->size())
			umap[r]->erase(umap[r]->begin());
		delete umap[r];
	}
	umapRemoveTime += stopper.elapsedSoFar();
	DBG("- removed from std::unordered_map<Data> in % time.\n", umapRemoveTime);
#endif



	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		array[r] = new csjp::Array<Data>;
		array[r]->setCapacity(numOfTestItems + 1);
		for(unsigned i=0; i < numOfTestItems; i++)
			array[r]->add(i);
	}
	arrayAddTime += stopper.elapsedSoFar();
	DBG("- added to csjp::array<Data> in % time.\n", arrayAddTime);

	stopper.restart();
#ifdef ARRAY_SLOW_REMOVE
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++){
			array[r]->removeAt(array[r]->length / 2);
			(void)i;
		}
		delete array[r];
	}
#else
	for(unsigned r = 0; r < repeats; r++){
		array[r]->clear();
		delete array[r];
	}
#endif
	arrayRemoveTime += stopper.elapsedSoFar();
	DBG("- removed from csjp::array<Data> in % time.\n", arrayRemoveTime);



	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		podArray[r] = new csjp::PodArray<DataStruct>;
		//podArray[r]->setCapacity(numOfTestItems + 1);
		for(unsigned i=0; i < numOfTestItems; i++)
			podArray[r]->add(newDataStruct(i));
	}
	podArrayAddTime += stopper.elapsedSoFar();
	DBG("- added to csjp::PodArray<DataStruct> in % time.\n", podArrayAddTime);

	stopper.restart();
#ifdef ARRAY_SLOW_REMOVE
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++){
			podArray[r]->removeAt(podArray[r]->length / 2);
			(void)i;
		}
		delete podArray[r];
	}
#else
	for(unsigned r = 0; r < repeats; r++){
		podArray[r]->clear();
		delete podArray[r];
	}
#endif
	podArrayRemoveTime += stopper.elapsedSoFar();
	DBG("- removed from csjp::PodArray<DataStruct> in % time.\n", podArrayRemoveTime);



#ifdef GENERAL_CONT
	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		container[r] = new csjp::OwnerContainer<Data>;
		for(unsigned i=0; i < numOfTestItems; i++)
			container[r]->add(new Data(i));
	}
	containerAddTime += stopper.elapsedSoFar();
	DBG("- added to csjp::OwnerContainer<Data> in % time.\n", containerAddTime);

	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++){
			container[r]->removeAt(0);
		}
		delete container[r];
	}
	containerRemoveTime += stopper.elapsedSoFar();
	DBG("- removed from csjp::OwnerContainer<Data> in % time.\n", containerRemoveTime);



	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		sorterContainer[r] = new DataContainer();
		for(unsigned i=0; i < numOfTestItems; i++)
			sorterContainer[r]->add(new Data(i));
	}
	sorterContainerAddTime += stopper.elapsedSoFar();

	DBG("- added to csjp::SorterOwnerContainer<Data> in % time.\n", sorterContainerAddTime);
	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++)
			sorterContainer[r]->removeAt(0);
		delete sorterContainer[r];
	}
	sorterContainerRemoveTime += stopper.elapsedSoFar();
	DBG("- removed from csjp::SorterOwnerContainer<Data> in % time.\n", sorterContainerRemoveTime);
#endif

/*	vectorAddTime /= repeats;
	mapAddTime /= repeats;
	umapAddTime /= repeats;
	arrayAddTime /= repeats;
	containerAddTime /= repeats;
	sorterContainerAddTime /= repeats;

	vectorRemoveTime /= repeats;
	mapRemoveTime /= repeats;
	umapRemoveTime /= repeats;
	arrayRemoveTime /= repeats;
	containerRemoveTime /= repeats;
	sorterContainerRemoveTime /= repeats;
*/
	addElementsPlotFile.appendf("% % % % % % % %\n", numOfTestItems,
			vectorAddTime, mapAddTime, umapAddTime,
			arrayAddTime, podArrayAddTime, containerAddTime, sorterContainerAddTime);
	removeElementsPlotFile.appendf("% % % % % % % %\n", numOfTestItems,
			vectorRemoveTime, mapRemoveTime, umapRemoveTime,
			arrayRemoveTime, podArrayAddTime, containerRemoveTime, sorterContainerRemoveTime);
}

void SpeedTest::reverseSorting(unsigned numOfTestItems)
{
	double vectorTime = 0;
	double mapTime = 0;
	double umapTime = 0;
	double arrayTime = 0;
	double podArrayTime = 0;
	double containerTime = 0;
	double sorterContainerTime = 0;

	DBG("Reverse sorting:\n");

	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		std::sort(vector.begin(), vector.end(), cmpDesc);
		std::sort(vector.begin(), vector.end(), cmpAsc);
	}
	vectorTime += stopper.elapsedSoFar();
	DBG("- std::vector<Data> in % time.\n", vectorTime);

	mapTime = -0.0;
	umapTime = -0.0;
	arrayTime = -0.0;
	podArrayTime = -0.0;
	containerTime = -0.0;

#ifdef GENERAL_CONT
	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		sorterContainer.reverseOrdered = true;
		sorterContainer.sort();
		sorterContainer.reverseOrdered = false;
		sorterContainer.sort();
	}
	sorterContainerTime += stopper.elapsedSoFar();
	DBG("- csjp::SorterOwnerContainer<Data> in % time.\n", sorterContainerTime);
#endif

/*	vectorTime /= repeats;
	mapTime /= repeats;
	umapTime /= repeats;
	arrayTime /= repeats;
	podArrayTime /= repeats;
	containerTime /= repeats;
	sorterContainerTime /= repeats;*/

	reverseSortingPlotFile.appendf("% % % % % % % %\n", numOfTestItems,
			vectorTime, mapTime, umapTime,
			arrayTime, podArrayTime, containerTime, sorterContainerTime);
}

void SpeedTest::sameOrderSorting(unsigned numOfTestItems)
{
	double vectorTime = 0;
	double mapTime = 0;
	double umapTime = 0;
	double arrayTime = 0;
	double podArrayTime = 0;
	double containerTime = 0;
	double sorterContainerTime = 0;

	DBG("Resorting in same order:\n");

	std::sort(vector.begin(), vector.end(), cmpAsc);
	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		std::sort(vector.begin(), vector.end(), cmpAsc);
		std::sort(vector.begin(), vector.end(), cmpAsc);
	}
	vectorTime += stopper.elapsedSoFar();
	DBG("- std::vector<Data> in % time.\n", vectorTime);

	mapTime = -0.0;
	umapTime = -0.0;
	arrayTime = -0.0;
	podArrayTime = -0.0;
	containerTime = -0.0;

#ifdef GENERAL_CONT
	sorterContainer.reverseOrdered = false;
	sorterContainer.sort();
	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		sorterContainer.sort();
		sorterContainer.sort();
	}
	sorterContainerTime += stopper.elapsedSoFar();
	DBG("- csjp::SorterOwnerContainer<Data> in % time.\n", sorterContainerTime);
#endif

/*	vectorTime /= repeats;
	mapTime /= repeats;
	umapTime /= repeats;
	arrayTime /= repeats;
	podArrayTime /= repeats;
	containerTime /= repeats;
	sorterContainerTime /= repeats;*/

	sameSortingPlotFile.appendf("% % % % % % % %\n", numOfTestItems,
			vectorTime, mapTime, umapTime,
			arrayTime, podArrayTime, containerTime, sorterContainerTime);
}

void SpeedTest::lookupByIndex(unsigned numOfTestItems)
{
	double vectorTime = 0;
	double mapTime = 0;
	double umapTime = 0;
	double arrayTime = 0;
	double podArrayTime = 0;
	double containerTime = 0;
	double sorterContainerTime = 0;

	DBG("Looking up % elements by indexes in:\n", numOfTestItems);

	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++)
			volatile Data u(vector.at(i));
	}
	vectorTime += stopper.elapsedSoFar();
	DBG("- std::vector<Data> in % time.\n", vectorTime);

	mapTime = -0.0;
	umapTime = -0.0;

	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++)
			volatile Data u(array[i]);
	}
	arrayTime += stopper.elapsedSoFar();
	DBG("- csjp::Array<Data> in % time.\n", arrayTime);

	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++){
			volatile DataStruct u = podArray[i];
			(void)(u);
		}
	}
	podArrayTime += stopper.elapsedSoFar();
	DBG("- csjp::PodArray<DataStruct> in % time.\n", podArrayTime);

#ifdef GENERAL_CONT
	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++)
			volatile Data u(container.queryAt(i));
	}
	containerTime += stopper.elapsedSoFar();
	DBG("- csjp::OwnerContainer<Data> in % time.\n", containerTime);

	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++)
			volatile Data u(sorterContainer.queryAt(i));
	}
	sorterContainerTime += stopper.elapsedSoFar();
	DBG("- csjp::SorterOwnerContainer<Data> in % time.\n", sorterContainerTime);
#endif

/*	vectorTime /= repeats;
	mapTime /= repeats;
	umapTime /= repeats;
	arrayTime /= repeats;
	podArrayTime /= repeats;
	containerTime /= repeats;
	sorterContainerTime /= repeats;*/

	lookupByIndexPlotFile.appendf("% % % % % % % %\n", numOfTestItems,
			vectorTime, mapTime, umapTime,
			arrayTime, podArrayTime, containerTime, sorterContainerTime);
}

void SpeedTest::lookupByKey(unsigned numOfTestItems)
{
	double vectorTime = 0;
	double mapTime = 0;
	double umapTime = 0;
	double arrayTime = 0;
	double podArrayTime = 0;
	double containerTime = 0;
	double sorterContainerTime = 0;

	DBG("Looking up % elements by key in:\n", numOfTestItems);

	stopper.restart();
#ifdef ARRAY_SLOW_LOOKUP
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++){
			unsigned j = 0;
			for(; vector[j].data != i; j++)
				;
			volatile Data u(vector[j]);
		}
	}
	vectorTime += stopper.elapsedSoFar();
#else
	vectorTime = 1.0;
#endif
	DBG("- std::vector<Data> in % time.\n", vectorTime);
#ifdef GENERAL_CONT
	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++)
			volatile Data u(map.at(i));
	}
	mapTime += stopper.elapsedSoFar();
	DBG("- std::map<Data> in % time.\n", mapTime);

	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++)
			volatile Data u(umap.at(i));
	}
	umapTime += stopper.elapsedSoFar();
	DBG("- std::unordered_map<Data> in % time.\n", umapTime);
#endif

	stopper.restart();
#ifdef ARRAY_SLOW_LOOKUP
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++){
			unsigned j = 0;
			for(; array[j].data != i; j++)
				;
			volatile Data u(array[j]);
		}
		//for(unsigned i=0; i < numOfTestItems; i++)
		//	volatile Data u(array.query(i));
	}
	arrayTime += stopper.elapsedSoFar();
#else
	arrayTime = 1.0;
#endif
	DBG("- csjp::array<Data> in % time.\n", arrayTime);

	stopper.restart();
#ifdef ARRAY_SLOW_LOOKUP
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++){
			unsigned j = 0;
			for(; podArray[j].data != i; j++)
				;
			volatile DataStruct u = podArray[j];
			(void)(u);
		}
		//for(unsigned i=0; i < numOfTestItems; i++)
		//	volatile Data u(podArray.query(i));
	}
	podArrayTime += stopper.elapsedSoFar();
#else
	podArrayTime = 1.0;
#endif
	DBG("- csjp::podArray<Data> in % time.\n", podArrayTime);

#ifdef GENERAL_CONT
	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++)
			volatile Data u(container.query(i));
	}
	containerTime += stopper.elapsedSoFar();
	DBG("- csjp::OwnerContainer<Data> in % time.\n", containerTime);

	stopper.restart();
	for(unsigned r = 0; r < repeats; r++){
		for(unsigned i=0; i < numOfTestItems; i++)
			volatile Data u(sorterContainer.query(i));
	}
	sorterContainerTime += stopper.elapsedSoFar();
	DBG("- csjp::SorterOwnerContainer<Data> in % time.\n", sorterContainerTime);
#endif

/*	vectorTime /= repeats;
	mapTime /= repeats;
	umapTime /= repeats;
	arrayTime /= repeats;
	podArrayTime /= repeats;
	containerTime /= repeats;
	sorterContainerTime /= repeats;*/

	lookupByKeyPlotFile.appendf("% % % % % % % %\n", numOfTestItems,
			vectorTime, mapTime, umapTime,
			arrayTime, podArrayTime, containerTime, sorterContainerTime);
}

int main(int argc, const char * args[])
{
	if(argc == 1 || ( 1 < argc && ( !strcmp(args[1], "--help") || !strcmp(args[1], "-h") ) ) ){
		printf(
		"Speed testing csjp::Container class against standard containers.\n"
		"\n"
		"Usage:\n"
		"	%s -h | --help\n"
		"		Shows this help.\n"
		"\n"
		"	%s _options_\n"
		"		Runs the tests (add and remove elements, sort in same and reverse\n"
		"		order, lookup by index and key) according to the given options.\n"
		"\n"
		"	Where _options_ can be any of the below in any order:\n"
		"\n"
		"	-l | --logdir\n"
		"\n"
		"	-v | --verbose\n"
		"\n"
		"	-m | --max-items n\n"
		"		Runs test with 'n' number of data.\n"
		"\n"
		"	-r | --repeats r\n"
		"		Repeats each test by 'r' times and calculates the average\n"
		"		of the results. By default 'r' = 1.\n"
		"\n"
		"	-i | --increment i\n"
		"		Testing with increasing number of elements in the containers\n"
		"		started with 0, increased by 'i' till 'n'.\n"
		"		By default 'i' = 'n' and zero element test is ommitted.\n"
		"\n"
		"	-M | --min-items n\n"
		"		If increment != n, runs tests with at lest 'n' number of data.\n"
		"\n"
		"	-x | --multiplicative-increment\n"
		"		If given, --min-items value is going to be multplied by --increment\n"
		"		value instead of addition till it reaches the value of --max-items.\n"
		"\n",
		args[0], args[0]);
		return 0;
	}

	unsigned minSize = 0;
	unsigned maxSize = 0;
	unsigned repeats = 1;
	unsigned increment = 0;
	bool multiplicativeIncrement = false;

	csjp::setBinaryName(args[0]);
	int argi = 1;
	while(argi < argc){
		if(2 <= argc - argi && (
				!strcmp(args[argi], "--logdir") ||
				!strcmp(args[argi], "-l"))){
			csjp::setLogDir(args[argi+1]);
			argi += 2;
			continue;
		}
		if(1 <= argc - argi && (
				!strcmp(args[argi], "--verbose") ||
				!strcmp(args[argi], "-v"))){
			csjp::verboseMode = true;
			argi++;
			continue;
		}
		if(2 <= argc - argi && (
				!strcmp(args[argi], "--max-items") ||
				!strcmp(args[argi], "-m"))){
			maxSize = atoi(args[argi+1]);
			argi += 2;
			continue;
		}
		if(2 <= argc - argi && (
				!strcmp(args[argi], "--repeats") ||
				!strcmp(args[argi], "-r"))){
			repeats = atoi(args[argi+1]);
			argi += 2;
			continue;
		}
		if(2 <= argc - argi && (
				!strcmp(args[argi], "--increment") ||
				!strcmp(args[argi], "-i"))){
			increment = atoi(args[argi+1]);
			argi += 2;
			continue;
		}
		if(2 <= argc - argi && (
				!strcmp(args[argi], "--min-items") ||
				!strcmp(args[argi], "-M"))){
			minSize = atoi(args[argi+1]);
			argi += 2;
			continue;
		}
		if(1 <= argc - argi && (
				!strcmp(args[argi], "--multiplicative-increment") ||
				!strcmp(args[argi], "-x"))){
			multiplicativeIncrement = true;
			argi += 1;
			continue;
		}
		fprintf(stderr, "Bad arguments given.\n");
		return 1;
	}

	if(!increment)
		increment = maxSize;
	if(multiplicativeIncrement && minSize == 0)
		minSize = 1;

	try{
		SpeedTest test(repeats);
		unsigned start = (increment == maxSize) ? maxSize : minSize;
		unsigned i = start;
		while(i <= maxSize){
			LOG("Testing containers with % number of elements.", i);
			test.addRemoveTest(i);

			test.fillAllTheContainers(i);
			test.reverseSorting(i);
			test.sameOrderSorting(i);
			test.lookupByIndex(i);
			test.lookupByKey(i);
			test.clearAllTheContainers(i);

			if(multiplicativeIncrement)
				i *= increment;
			else
				i += increment;
		}
		test.renames();
		LOG("All done");

		
/*		SpeedTest test(repeats);
		for(unsigned n = 3; n < 6; n++){
			unsigned step = pow(10, n);
			for(unsigned i = step*2; i <= 10*step; i += step*4){
				LOG("Testing containers with % number of elements.", i);
				test.addRemoveTest(i);

				test.reverseSorting(i);
				test.sameOrderSorting(i);
				test.lookupByIndex(i);
				test.lookupByKey(i);
			}
		}
		LOG("All done");*/
	} catch (csjp::Exception & e) {
		EXCEPTION(e);
		exit(-1);
	}
}
