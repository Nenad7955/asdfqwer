//#include "main.h"
#define LENGTH 1024*1024
#define CHAR_LENGTH 1024*1024/4
#include <gtest/gtest.h>
#include "CustomFileSystem.h"

CustomFileSystem customFileSystem;

int main(int argc, char** argv) {
	setlocale(LC_ALL, "Russian");

	ifstream fstr;
	fstr.open("v2.dat", ios::binary);
	std::vector<char> buf (CHAR_LENGTH*4);
	fstr.read(reinterpret_cast<char*>(buf.data()), CHAR_LENGTH*4* sizeof(char));

	customFileSystem = CustomFileSystem(buf);

	string expected_info = customFileSystem.findFileAndOut(
	customFileSystem.rootCatalog.rootRaws[1].firstBlockNumber, customFileSystem.rootCatalog.rootRaws[1].name);

	
	customFileSystem.doDefragmentation();
	
	string actual_info = customFileSystem.findFileAndOut(
	customFileSystem.rootCatalog.rootRaws[1].firstBlockNumber, customFileSystem.rootCatalog.rootRaws[1].name);
		
	if(expected_info != actual_info)
	    return 1;
	    
	customFileSystem.findFileAndWrite(
		customFileSystem.rootCatalog.rootRaws[1].firstBlockNumber, 
		customFileSystem.rootCatalog.rootRaws[1].name);
	std::ifstream in(customFileSystem.rootCatalog.rootRaws[1].name, std::ifstream::ate | std::ifstream::binary);     
	if (in.tellg() < 1)
	    return 1;

	customFileSystem.showSystemBlockData();
	customFileSystem.fat.showFATTable();
	customFileSystem.rootCatalog.showFiles();
	//customFileSystem.
	customFileSystem.showFragmentationCoefficient();
	//customFileSystem.doDefragmentation();
	//customFileSystem.fat.showFATTable();
	//customFileSystem.showFragmentationCoefficient();
	//customFileSystem.rootCatalog.showFiles();
	//customFileSystem.showFragmentationCoefficient();
	//customFileSystem.outFiles();
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
//	return 0;
}

TEST(Test, checkNothingBreakAfterDefragmentation) {
	string expected_info = customFileSystem.findFileAndOut(
		customFileSystem.rootCatalog.rootRaws[1].firstBlockNumber, customFileSystem.rootCatalog.rootRaws[1].name);
	customFileSystem.doDefragmentation();
	string actual_info = customFileSystem.findFileAndOut(
		customFileSystem.rootCatalog.rootRaws[1].firstBlockNumber, customFileSystem.rootCatalog.rootRaws[1].name);
	ASSERT_EQ(expected_info, actual_info);
}

TEST(Test, checkFileCreated) {
	customFileSystem.findFileAndWrite(
		customFileSystem.rootCatalog.rootRaws[1].firstBlockNumber, 
		customFileSystem.rootCatalog.rootRaws[1].name);

	std::ifstream in(customFileSystem.rootCatalog.rootRaws[1].name, std::ifstream::ate | std::ifstream::binary); 
	ASSERT_EQ(in.tellg() > 0, true);
}
