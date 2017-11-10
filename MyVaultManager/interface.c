// File: interface.c
// Author: iBug 

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include "data.h"
#include "file.h"
#include "interface.h"
#include "sys.h"

option_t menuMain(Data** pdata){
	clearScreen();
	puts("******************");
	puts("*                *");
	puts("*  iBug MyVault  *");
	puts("*                *");
	puts("******************");
	puts("");
	if (*pdata == NULL)
		*pdata = readFile(_Filename);
	if (*pdata == NULL){
		puts("[Info] The default database isn't found. Creating an empty database.\n");
		*pdata = menuNewData();
		if (*pdata == NULL){
			puts("[ERROR] Failed to create default database");
			pause();
			exit(1);
		}
	}
	puts("Choose an action:");
	puts("   1. Display the whole database");
	puts("   2. Query a record");
	puts("   3. Create a new record");
	puts("   4. Change an existing record");
	puts("   5. Delete a record");
	puts("   6. Save to file");
	puts("");
	puts("   0. Quit");
	printf("\nEnter your choice: ");
	option_t option;
	int success = scanf("%d", &option);
	clearInputBuffer();
	if (!success){
		return OPTION_INVALID;
	}
	if (option < 0){
		return OPTION_INVALID;
	}
	return option;
}

Data* menuNewData(void){
	return createEmptyData();
}

Record* readRecord(void){
	static const unsigned char MonthDays[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	Date date;
	currency_t amount;
	double amountd;
	char detail[4096];
	printf("\nEnter date:\n");
	printf("    Enter year: ");
	while(!scanf("%hd", &date.year) || (date.year<0 || date.year>9999)){
		clearInputBuffer();
		printf("    Re-enter a corrent year: "); 
	}
	printf("    Enter month: ");
	while(!scanf("%hhu", &date.month) || (date.month>12 || date.month<1)){
		clearInputBuffer();
		printf("    Re-enter a corrent month: "); 
	}
	printf("    Enter day: ");
	while(!scanf("%hhu", &date.day) || (date.day > MonthDays[date.month] || date.day<1)){
		clearInputBuffer();
		printf("    Re-enter a corrent day: "); 
	}
	printf("Enter amount: ");
	while(!scanf("%lf", &amountd)) clearInputBuffer();
	amount = (currency_t)(amountd * 100.0);
	clearInputBuffer();
	printf("Enter detail: "); fgets(detail, sizeof(detail), stdin);
	return createRecord(date, amount, detail);
}

int menuNewRecord(Data* data){
	id_t id;
	printf("\nEnter the ID of new record [0-%lu]: ", data->count); 
	while(!scanf("%ld", &id) || (id<0 || id>data->count)){
		clearInputBuffer();
		printf("Re-enter a corrent ID: "); 
	}
	Record *record = readRecord();
	record->id = id;
	record->id = insertRecord(data, record);
	puts("\nThis is the new record created:");
	displayRecord(record);
	destroyRecord(record);
	puts("\nPress any key to continue.");
	pause();
	return 0;
}

Record* menuUpdateRecord(Data* data){
	id_t id;
	printf("\nEnter data ID: ");
	scanf("%ld", &id);
	Record *oldRecord = queryRecord(data, id);
	if (!oldRecord){
		puts("That record does not exist!");
		puts("\nPress any key to continue.");
		pause();
		return NULL;
	}
	puts("\nThis is the record to be changed:");
	displayRecord(oldRecord);
	puts("\nNow enter a new record:\n");
	Record *record = readRecord();
	updateRecord(oldRecord, record);
	destroyRecord(record); 
	puts("\nThis is the updated record:");
	displayRecord(oldRecord);
	puts("\nPress any key to continue.");
	pause();
	return oldRecord;
}

int menuDisplayData(const Data* data){
	clearScreen();
	puts("******  Data dump  ******");
	displayData(data);
	puts("\nPress any key to continue.");
	pause();
	return 0;
}

int menuQueryRecord(Data* data){
	id_t id;
	printf("\nEnter data ID to query: ");
	scanf("%ld", &id);
	Record *record = queryRecord(data, id);
	if (!record){
		puts("\nThat record does not exist!");
		puts("\nPress any key to continue.");
		pause();
		return 1;
	}
	puts("\nThis is the record:");
	displayRecord(record);
	puts("\nPress any key to continue.");
	pause();
	return 0;
}

int menuDeleteRecord(Data* data){
	id_t id;
	printf("\nEnter data ID to delete: ");
	scanf("%ld", &id);
	clearInputBuffer();
	Record *record = queryRecord(data, id);
	if (!record){
		puts("That record does not exist!");
		puts("\nPress any key to continue.");
		pause();
		return 1;
	}
	puts("This is the record deleted:");
	displayRecord(record);
	deleteRecord(data, record->id);
	puts("\nPress any key to continue.");
	pause();
	return 0;
}

int menuSaveData(Data* data){
	size_t size = writeFile(_Filename, data);
	if (size == 0){
		puts("Error saving data.");
		pause();
		return 1;
	}
	printf("%lu bytes successfully written to %s\n", size, _Filename);
	puts("\nPress any key to continue.");
	pause();
	return 0;
}

int displayRecord(const Record* record){
	printf("ID:     %10ld\n", record->id);
	printf("Date:   %4d/%2d/%2d\n", record->date.year, record->date.month, record->date.day);
	printf("Amount: %+7ld.%02ld\n", record->amount/100, record->amount%100); 
	printf("Detail: %s\n", record->detail);
	return 0;
}

int displayData(const Data* data){
	if (!data) return 0;
	printf("There are %lu records in the database.\n", data->count);
	for (int i = 0; i < data->count; i ++){
		puts("");
		displayRecord(data->records[i]);
	}
	return 0;
}
