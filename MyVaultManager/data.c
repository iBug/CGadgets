// File: data.c
// Author: iBug

#include <stddef.h>
#include <string.h>
#include <malloc.h>

#include "data.h"

Record* createRecord(Date date, currency_t amount, const char* detail){
	size_t len = strlen(detail);
	char* buffer = malloc(sizeof(char) * (len+1));
	if (buffer == NULL)
		return NULL;
	strcpy(buffer, detail);
	Record *record = malloc(sizeof(Record));
	record->id = ID_INVALID;
	record->date.day = date.day;
	record->date.month = date.month;
	record->date.year = date.year; 
	record->amount = amount;
	record->detail = buffer;
	return record;
}

size_t destroyRecord(Record* record){
	if (!record) return 0; 
	size_t size = sizeof(*record) + strlen(record->detail) + 1;
	free((void*)record->detail);
	free((void*)record);
	return size;
}

id_t updateRecord(Record* dest, const Record* src){
	if (!dest || !src) return 0; 
	dest->date.day = src->date.day;
	dest->date.month = src->date.month;
	dest->date.year = src->date.year;
	dest->amount = src->amount;
	char *buffer = malloc(sizeof(char) * (strlen(src->detail) + 1));
	strcpy(buffer, src->detail);
	free((void*)dest->detail);
	dest->detail = buffer;
	return dest->id;
}

Data* createEmptyData(void){
	Data *data = malloc(sizeof(Data));
	data->count = 0;
	data->records = NULL;
	return data;
}

Data* createData(size_t count, Record** records){
	Data* data = createEmptyData();
	for (size_t i = 0; i < count; i ++){
		insertRecord(data, records[i]);
	}
	return data;
}

id_t getNextId(const Data* data){
	if (!data) return ID_INVALID;
	id_t next = -1;
	for (id_t i = 0; i < data->count; i ++){
		if (data->records[i]->id > next)
			next = data->records[i]->id;
	}
	return ++next;
}

id_t insertRecord(Data* data, const Record* record){
	if (!data || !record) return ID_INVALID;
	Record* newRecord = createRecord(record->date, record->amount, record->detail);
	id_t nextId = getNextId(data);
	newRecord->id = record->id;
	if (newRecord->id > nextId) newRecord->id = nextId;
	if (newRecord->id < 0) newRecord->id = 0;
	if (data->count > 0){
		for (id_t i = newRecord->id; i < data->count-1; i ++){
			data->records[i]->id = data->records[i+1]->id;
		}
		data->records[data->count - 1]->id = nextId;
	}
	data->records = realloc(data->records, sizeof(Record*) * ++(data->count));
	for (id_t i = data->count - 1; i > newRecord->id; i --){
		data->records[i] = data->records[i-1];
	}
	data->records[newRecord->id] = newRecord;
	return newRecord->id;
}

Record* queryRecord(Data* data, id_t id){
	if (!data) return NULL;
	for (size_t i = 0; i < data->count; i ++){
		if (data->records[i]->id == id)
			return data->records[i];
	}
	return NULL;
}

size_t deleteRecord(Data* data, id_t id){
	if (!data) return 0;
	id_t found = ID_INVALID;
	for (id_t i = 0; i < data->count; i ++){
		if (data->records[i]->id == id){
			found = i;
			break;
		}
	}
	if (found == ID_INVALID) return 0;
	Record *record = data->records[found];
	for (id_t i = found; i < data->count-1; i ++){
		data->records[i] = data->records[i+1];
	}
	--(data->count);
	for (id_t i = data->count-1; i > found; i --){
		data->records[i]->id = data->records[i-1]->id;
	}
	data->records[found]->id = record->id;
	return destroyRecord(record);
}

size_t destroyData(Data* data){
	if (!data) return 0;
	size_t size = 0;
	for (id_t i = data->count-1; i >= 0; i --){
		size += deleteRecord(data, data->records[i]->id);
	}
	size += sizeof(*data);
	free(data);
	return size;
}

