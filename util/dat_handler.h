#ifndef _DAT_HANDLER_H_
#define _DAT_HANDLER_H_

#include <vector>

#define DAT_DEFAULT_ITEMS   1
#define DAT_HEADER_OFFSET   20
#define DAT_RECORD_TIME_LEN 8
#define DAT_RECORD_PRESERVE 4
#define DAT_RECORD_DATA_LEN 8
#define DAT_RECORD_ITEM_LEN (DAT_RECORD_PRESERVE + DAT_RECORD_DATA_LEN)

/*
 * Header info by default
 */
#ifdef _MSC_VER
#pragma pack(4)
#endif /* _MSC_VER */
typedef struct {
	int nRecords;      /* Number of Records */
	int recordItems;  /* Items in each record */
	/*
	 * Coding...
	 */
}
#ifdef __GNUC__
__attribute__ ((aligned (4)))
#endif /* __GNUC__ */
dat_header_t;

class DatFileHandler
{
public:
	static const int DefaultItems; 
	DatFileHandler(const char *filename);
	~DatFileHandler();
	void showInfo() const;
	int getNumberofRecords() const;
	int getNumberofRecordItems() const;
	bool seekRecord(int recordId);
	void seekCurrentRecord();
	bool pushBackRawRecord(int nItems, double time, void *dat);
	bool readRawRecord(double *time, void *buffer);
private:
	FILE *fp;
	dat_header_t *header;
	bool isCurrentCsr;
	int  recordLength;
	void syncHeader();
	void loadHeader();
	FILE *createDatFile(const char *filename);
	FILE *openExistedDatFile(const char *filename);
};

#endif /* _DAT_HANDLER_H_ */
