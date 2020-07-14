#include <cassert>
#include <cstring>
#include <iostream>
#include "dat_handler.h"

#define ASSERT_ON_BUILD(condition)\
	((void)sizeof(char[1 - 2*!!(condition)]))
/*=========== compile-time test ===========*/
#ifdef ENABLE_FOPS
void __________compile_time_test___________()
{
	ASSERT_ON_BUILD(DAT_HEADER_OFFSET > sizeof(dat_header_t));
}
#endif

const int DatFileHandler::DefaultItems = DAT_DEFAULT_ITEMS;

DatFileHandler::DatFileHandler(const char *filename)
{
	header = (dat_header_t*)calloc(1, sizeof(dat_header_t));
	fp = fopen(filename, "rb+");
	if (!fp) {
		fp = createDatFile(filename);
		assert(fp); /* Exception... */
	} else {
		loadHeader();
	}
	recordLength = DAT_RECORD_TIME_LEN +
		header->recordItems * DAT_RECORD_ITEM_LEN;
	seekCurrentRecord();
}

DatFileHandler::~DatFileHandler()
{
	syncHeader();
	if (header)
		free(header);
	fclose(fp);
}

FILE *DatFileHandler::createDatFile(const char *filename)
{
	FILE *fp = fopen(filename, "wb+");
	assert(fp); /* if (!fp) ... */
	/*
	 * Initialize and write header...
	 */
	header->recordItems = DatFileHandler::DefaultItems; /* Default cfg */
	fseek(fp, DAT_HEADER_OFFSET, SEEK_SET);
	return fp;
}

void DatFileHandler::loadHeader()
{
	size_t off = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(&header->nRecords, sizeof(int), 1, fp);
	fread(&header->recordItems, sizeof(int), 1, fp);
	fseek(fp, off, SEEK_SET);
}

void DatFileHandler::syncHeader()
{
	size_t off = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fwrite(header, sizeof(dat_header_t), 1, fp);
	fseek(fp, off, SEEK_SET);
}

void DatFileHandler::seekCurrentRecord()
{
	size_t off = DAT_HEADER_OFFSET + header->nRecords * recordLength;
	fseek(fp, off, SEEK_SET);
	isCurrentCsr = true;
}

bool DatFileHandler::seekRecord(int recordId)
{
	if (recordId >= header->nRecords)
		return false;
	size_t off = DAT_HEADER_OFFSET + recordId * recordLength;
	fseek(fp, off, SEEK_SET);
	isCurrentCsr = false;
	return true;
}

void DatFileHandler::showInfo() const
{
	std::cout<<"DAT FILE INFO:\n"<<"Number of Records: "<<header->nRecords \
		<<"\nEach Record have "<<header->recordItems<<" Items\n";
}

int DatFileHandler::getNumberofRecordItems() const
{
	return header->recordItems;
}

int DatFileHandler::getNumberofRecords() const
{
	return header->nRecords;
}

bool DatFileHandler::pushBackRawRecord(int nItems, double time, void *dat)
{
	if (!isCurrentCsr)
		seekCurrentRecord();
	if (fwrite(&time, sizeof(double), 1, fp) != 1)
		return false;
	double *dptr = (double*)dat;
	for (int i = 0; i < nItems; ++i) {
		if (fwrite(dptr++, sizeof(double), 1, fp) != 1)
			return false;
		/*
		 * May write preserved info
		 */
		fseek(fp, DAT_RECORD_PRESERVE, SEEK_CUR);
	}
	header->nRecords++;
	return true;
}

bool DatFileHandler::readRawRecord(double *time, void *buffer)
{
	size_t off = ftell(fp);
	if (off == DAT_HEADER_OFFSET + header->nRecords * recordLength)
		return false; /* Reached current cursor */
	if (fread(time, sizeof(double), 1, fp) != 1)
		return false;
	double *dptr = (double*)buffer;
	for (int i = 0; i < header->recordItems; ++i) {
		if (fread(dptr++, sizeof(double), 1, fp) != 1)
			return false;
		/*
		 * May read preserved info
		 */
		fseek(fp, DAT_RECORD_PRESERVE, SEEK_CUR);
	}
	return true;
}
