#include <iostream>
#include <cassert>
#include "dat_handler.h"

int testRead(const char *filename)
{
	DatFileHandler fileHandler(filename);
	fileHandler.showInfo();
	double time, value;
	for (int i = 0; i < fileHandler.getNumberofRecords(); ++i) {
		fileHandler.seekRecord(i);
		fileHandler.readRawRecord(&time, &value);
		printf("%lf: %lf\n", time, value);
	}
	return 0;
}

int testWrite(const char *filename)
{
	DatFileHandler fileHandler(filename);
	fileHandler.showInfo();
	double time  = 10000000.000;
	double value = 123456.;
	for (int i = 0; i < 10; ++i) { /* Append 10 records */
		time  += 0.001;
		value += i;
		assert(fileHandler.pushBackRawRecord(1, time, &value));
	}
	return 0;
}

int main(int argc, const char *argv[])
{
	std::cout<<"Run write test...\n";
	testWrite("test.dat");
	std::cout<<"Write Finished\n";
	
	std::cout<<"Run raed test...\n";
	testRead("test.dat");
	std::cout<<"Run write test...\n";
	return 0;
}
