// File: file.c
// Author: iBug

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <malloc.h>

#include "data.h"
#include "file.h"

static const int32_t MAGIC = 0xAABBCCDD;
static const int32_t VERSION = 1;

Data* readFile(const char* filename){
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL)
		return NULL;
	FileDat file;
	fread(&file.header, sizeof(file.header), 1, fp);
	if (file.header.magic != MAGIC || file.header.version > VERSION){
		fclose(fp);
		return NULL;
	}
	fread(&file.content.header, sizeof(file.content.header), 1, fp);
	Data *data = createEmptyData();
	file.content.blocks = malloc(sizeof(*file.content.blocks) * file.content.header.count);
	for (int i = 0; i < file.content.header.count; i ++){
		fread(&file.content.blocks[i].length, sizeof(file.content.blocks[i].length), 1, fp);
		char *buffer = malloc(file.content.blocks[i].length);
		fread(buffer, file.content.blocks[i].length, 1, fp);
		Record *tmpRecord = (Record*)buffer, record;
		record.id = tmpRecord->id;
		record.date = tmpRecord->date;
		record.amount = tmpRecord->amount;
		// Danger!!
		record.detail = (const char*)&(tmpRecord->detail);
		insertRecord(data, &record);
		free(buffer);
	}
	fclose(fp);
	return data;
}

size_t writeFile(const char* filename, const Data* data){
	FILE *fp = fopen(filename, "wb");
	if (fp == NULL)
		return 0;
	size_t size = 0;
	FileDat file;
	file.header.magic = MAGIC;
	file.header.version = VERSION;
	size += fwrite(&file.header, sizeof(file.header), 1, fp) * sizeof(file.header);
	file.content.header.count = data->count;
	size += fwrite(&file.content.header, sizeof(file.content.header), 1, fp) * sizeof(file.content.header);
	for (int i = 0; i < file.content.header.count; i ++){
		FileBlock block;
		size_t clen = strlen(data->records[i]->detail);
		block.length = sizeof(*(data->records[i])) - sizeof(const char*) + sizeof(char)*((clen/4+1)*4);
		char *buffer = malloc(block.length);
		Record *record = (Record*)buffer;
		record->id = data->records[i]->id;
		record->date.year = data->records[i]->date.year;
		record->date.month = data->records[i]->date.month;
		record->date.day = data->records[i]->date.day;
		record->amount = data->records[i]->amount;
		// Fill 4-byte padding with zero
		for (int i = 1; i <= 4; i ++)
			buffer[block.length-i] = 0;
		// Special bug here. Don't use strcpy() or strncpy().
		memcpy((char*)&record->detail, data->records[i]->detail, 1+clen);
		size += fwrite(&block.length, sizeof(block.length), 1, fp) * sizeof(block.length);
		size += fwrite(buffer, block.length, 1, fp) * block.length;
		free(buffer);
	}
	fclose(fp);
	return size;
}

