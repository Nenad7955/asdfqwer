#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>


using namespace std;

class RootRaw {
public:
	string name;
	unsigned int firstBlockNumber;
	RootRaw();
	RootRaw(string name, unsigned int firstBlockNumber);
};

class SuperBlock {
public:
	unsigned int fileSystemBlockSize;
	unsigned int fatSize;
	unsigned int rootCatalogSize;
	SuperBlock();
	SuperBlock(unsigned int fileSystemBlockSize, 
					   unsigned int fatSize,
					   unsigned int rootCatalogSize);
};

class FAT {
public:
	vector<unsigned int> fatTable;
	FAT();
	FAT(vector<unsigned int> array);
	void showFATTable();
};

class RootCatalog {
public:
	vector<RootRaw> rootRaws;
	RootCatalog();
	RootCatalog(vector<RootRaw> raws);
	void showFiles();
};	

class CustomFileSystem {
public:
	SuperBlock superBlock;
	FAT fat;
	RootCatalog rootCatalog;
	vector<string> blocks;
	double fragmentationCoefficient;

	CustomFileSystem(){}
	CustomFileSystem(vector<char> buffer);
	void showSystemBlockData();
	void showFragmentationCoefficient();
	void findFileAndWrite(int firstBlock, string name);
	string findFileAndOut(int firstBlock, string name);
	void checkFragmentationCoefficient();
	void doDefragmentation();
	void outFiles();
};




