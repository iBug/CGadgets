// File: file.h
// Author: iBug

#ifndef _FILE_H
#define _FILE_H

#include <stddef.h>
#include "data.h"

typedef struct _FileBlockHeader{
	size_t count;
} FileBlockHeader;

typedef struct _FileBlock{
	size_t length;
	Record *record;
} FileBlock;

typedef struct _FileContent{
	FileBlockHeader header;
	FileBlock *blocks;
} FileContent;

typedef struct _FileHeader{
	unsigned long magic;
	unsigned long version;
} FileHeader;

typedef struct _FileDat{
	FileHeader header;
	FileContent content;
} FileDat;

Data* readFile(const char*);
size_t writeFile(const char*, const Data*);

#endif // _FILE_H
