// File: interface.h
// Author: iBug 

#ifndef _INTERFACE_H
#define _INTERFACE_H

#include "data.h"

typedef signed int option_t;

extern const char * const _Filename;

option_t menuMain(Data**);

#define OPTION_INVALID (-1)

Data* menuNewData(void);
int menuNewRecord(Data*);
Record* menuUpdateRecord(Data*);
int menuDisplayData(const Data*);
int menuQueryRecord(Data*);
int menuDeleteRecord(Data*);
int menuSaveData(Data*);

Record* readRecord(void);
int displayRecord(const Record*);
int displayData(const Data*);

#endif // _INTERFACE_H
