// File: data.h
// Author: iBug

#ifndef _DATA_H
#define _DATA_H

#include <stddef.h>

typedef short year_t;
typedef unsigned char month_t, day_t;
typedef struct _Date{
	year_t year;
	month_t month;
	day_t day;
} Date;

#ifdef __unix__
#define id_t ID
#endif
typedef signed long id_t;
typedef signed long currency_t;

typedef struct _Record{
	id_t id;
	Date date;
	currency_t amount; // In cents
	const char *detail;
} Record;

typedef struct _Data{
	size_t count;
	Record **records;
} Data;

#define ID_INVALID (-1)

Record* createRecord(Date, currency_t, const char*);
id_t updateRecord(Record*, const Record*);
size_t destroyRecord(Record*);

Data* createEmptyData(void);
Data* createData(size_t, Record**);
id_t getNextId(const Data*);
id_t insertRecord(Data*, const Record*);
Record* queryRecord(Data*, id_t id);
size_t deleteRecord(Data*, id_t id);
size_t destroyData(Data*);

#endif // _DATA_H
