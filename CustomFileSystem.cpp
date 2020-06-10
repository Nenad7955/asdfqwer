#include "CustomFileSystem.h"
#define LENGTH 1024*1024

unsigned int returnUnsignedInt(vector<char> &s, int begin) {
		int i;
		memcpy(&i, s.data()+begin, sizeof(i));
		return i;
}

void returnName(vector<char> &buf, int i, char *where) {
	char ar[12]; // = new char[12];

	ar[0] = buf[i];
	ar[1] = buf[i+1];
	ar[2] = buf[i+2];
	ar[3] = buf[i+3];
	ar[4] = buf[i+4];
	ar[5] = buf[i+5];
	ar[6] = buf[i+6];
	ar[7] = buf[i+7];
	ar[8] = buf[i+8];
	ar[9] = buf[i+9];
	ar[10] = buf[i+10];
	ar[11] = buf[i+11];
	for (int w = 0; w < 12; w++)
		where[w] = ar[w];
}

CustomFileSystem::CustomFileSystem(vector<char> buffer) {
	int i = 0;
	vector<unsigned int> superBlock;
	for (i = 0; i < 12; i = i + 4) {
		superBlock.push_back(returnUnsignedInt(buffer, i));
	}
	this->superBlock = SuperBlock(superBlock[0], superBlock[1], superBlock[2]);

	vector<unsigned int>table;
	for (i = 12 + 4; i < this->superBlock.fatSize + 12; i = i + 8) {
		table.push_back(returnUnsignedInt(buffer, i));
	}
	this->fat = FAT(table);

	vector<RootRaw> files;
	for (i = 4096 + 12; i < this->superBlock.fatSize + 12 + 256; i = i + 16) {
		char test[12] = {};
		returnName(buffer, i, test);
		files.push_back(RootRaw(test, returnUnsignedInt(buffer, i+12)));
	}
	this->rootCatalog = RootCatalog(files);

	vector<string> blocks;
	for (int i = this->superBlock.fatSize + 12 + 256; i < buffer.size() - 5000; i = i + 2048) {
		string test;
		for (int t = 0; t < 2048; t++)
			test += buffer[i+t];
		blocks.push_back(test);
	}

	this->blocks = blocks;

	//write all files
	/*
	for (int t = 0; t < this->rootCatalog.rootRaws.size(); t++) {
		if (this->rootCatalog.rootRaws[t].name != "")
			findFileAndWrite(this->rootCatalog.rootRaws[t].firstBlockNumber, this->rootCatalog.rootRaws[t].name);
	}
	*/

	//out files
	/*
		for (int t = 0; t < this->rootCatalog.rootRaws.size(); t++) {
		if (this->rootCatalog.rootRaws[t].name == "dir.c")
			findFileAndOut(this->rootCatalog.rootRaws[t].firstBlockNumber, this->rootCatalog.rootRaws[t].name);
	}
	*/
	
}

void CustomFileSystem::outFiles() {
	for (int t = 0; t < this->rootCatalog.rootRaws.size(); t++) {
		if (this->rootCatalog.rootRaws[t].name == "dir.c")
			findFileAndOut(this->rootCatalog.rootRaws[t].firstBlockNumber, this->rootCatalog.rootRaws[t].name);
	}
}

void CustomFileSystem::findFileAndWrite(int firstBlock, string name) {
	std::ofstream out(name, ios::binary);

	while (firstBlock != -1) {
			out << this->blocks[firstBlock];
			firstBlock = this->fat.fatTable[firstBlock];
		}
	out.close();
}

string CustomFileSystem::findFileAndOut(int firstBlock, string name) {
	string out = "";

	while (firstBlock != -1) {
			out += this->blocks[firstBlock];
			firstBlock = this->fat.fatTable[firstBlock];
		}
	//std::cout<<out<<std::endl;
	return out;
}

void CustomFileSystem::checkFragmentationCoefficient() {
	double fragmElements = 0;
	double defragmElements = 0;

	for (int file = 0; file < this->rootCatalog.rootRaws.size(); file++) {
		cout<<this->rootCatalog.rootRaws[file].name<<endl;
		if(this->rootCatalog.rootRaws[file].name != "") {
			int firstBlock = this->rootCatalog.rootRaws[file].firstBlockNumber;
			while(firstBlock != -1) {
				if (firstBlock > this->fat.fatTable[firstBlock])
					fragmElements++;
				else defragmElements++;
				cout<<firstBlock<<endl;
				firstBlock = this->fat.fatTable[firstBlock];
			}
		}
	}
	this->fragmentationCoefficient = fragmElements/(fragmElements+defragmElements);
}

void CustomFileSystem::doDefragmentation() {
	vector<unsigned int> newFat(512);
	vector<string> newBlocks(512);
	int block = 1;
	int firstBlock;
	int sklj;
	
	for (int i = 0; i < this->rootCatalog.rootRaws.size(); i++) {
		if (this->rootCatalog.rootRaws[i].name != "") {
			firstBlock = this->rootCatalog.rootRaws[i].firstBlockNumber;
			this->rootCatalog.rootRaws[i].firstBlockNumber = block;
			newBlocks[block] = this->blocks[firstBlock];
			if (this->fat.fatTable[firstBlock] != -1) {
				newFat[block] = block+1;
				block++;

				sklj = firstBlock;
				sklj = this->fat.fatTable[sklj];
				
				while (sklj != -1){
					newBlocks[block] = this->blocks[sklj];
					newFat[block] = block+1;
					block++;
					sklj = this->fat.fatTable[sklj];
				}
				newFat[block] = -1;
				block++;
			}
			else {
				newFat[block] = -1;
				block++;
			}
		}
	}
	this->blocks = newBlocks;
	this->fat.fatTable = newFat;
	
}

void CustomFileSystem::showSystemBlockData() {
	std::cout<< "File system block size: " << this->superBlock.fileSystemBlockSize<<std::endl;
	std::cout<< "FAT size: " << this->superBlock.fatSize<<std::endl;
	std::cout<< "Root catalog size: " << this->superBlock.rootCatalogSize<<std::endl;
}

void CustomFileSystem::showFragmentationCoefficient() {
	this->checkFragmentationCoefficient();
	std::cout<< "Fragmentation coefficient: " << this->fragmentationCoefficient<<std::endl;
	}

void FAT::showFATTable() {
	for (int i=0; i<fatTable.size(); i++) {
		std::cout<<"Block number: " << i << "  Data: " << fatTable[i] << std::endl;
	}
}

void RootCatalog::showFiles() {
	for (int i = 0; i < this->rootRaws.size(); i++) {
		std::cout<<"File name: " << this->rootRaws[i].name << "  First block number: " << this->rootRaws[i].firstBlockNumber << std::endl;
	}
}

SuperBlock::SuperBlock(unsigned int fileSystemBlockSize, 
					   unsigned int fatSize,
					   unsigned int rootCatalogSize) {
	this->fileSystemBlockSize = fileSystemBlockSize;
	this->fatSize = fatSize;
	this->rootCatalogSize = rootCatalogSize;
}

SuperBlock::SuperBlock() {
	this->fatSize = NULL;
	this->fileSystemBlockSize = NULL;
	this->rootCatalogSize = NULL;
}

FAT::FAT(vector<unsigned int> table) {
	this->fatTable = table;	
}

FAT::FAT() {
	this->fatTable = vector<unsigned int>(0);
}

RootCatalog::RootCatalog() {
	this->rootRaws = vector<RootRaw>(0);
}

RootCatalog::RootCatalog(vector<RootRaw> raws) {
	this->rootRaws = raws;
}

RootRaw::RootRaw() {
	this->name = "";
	this->firstBlockNumber = NULL;
}

RootRaw::RootRaw(string name, unsigned int firstBlockNumber) {
	this->name = name;
	this->firstBlockNumber = firstBlockNumber;
}
