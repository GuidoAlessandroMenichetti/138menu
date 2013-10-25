#ifndef GBISO_HEADER
#define GBISO_HEADER

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
    char type;
    char id[5];
    char version;
} volumeDescriptor;

typedef struct
{
    unsigned pathTableSize;
    unsigned pathTableSizeLE;
    unsigned pathTableOffset;
    unsigned pathTableOptionalOffset;
} primaryVolumeDescriptor;

typedef struct
{
    char length;
    char extended;
    unsigned location;
    short parent;
} pathTable; //8

typedef struct
{
    char length;
	char extended;
    unsigned location;
	unsigned locationLE;
	unsigned size;
	unsigned sizeLE;
	char trash[7];
	char flags;
	char other_size;
	char intervale;
	unsigned vol_seq;
    char nameLen;
} dirRecord;

#define PRIMARY_VD 1

class gbIso
{
	public:

	enum GBISO_RETURNS {ISO_ERROR, ISO_OK};
	enum GBISO_CONSTANTS {ISO_VD_MAGIC = 0x30444301, SECTOR_SIZE = 0x800};

	gbIso();
	int open(const char * path);
	unsigned findFile(const char * name);
	unsigned identifyEntry(const char * entry);
	void close();

	private:
	FILE * file;

};

#endif
