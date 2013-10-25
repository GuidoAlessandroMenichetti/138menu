/* Thanks to BOOSTER for his CSO converter sample */

#ifndef GBCSO_HEADER
#define GBCSO_HEADER

#include "gbIso.h"
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <pspdebug.h>
#include <string.h>

typedef struct 
{
	unsigned magic;
	unsigned header_size;
	unsigned long long file_size;
	unsigned block_size;
	unsigned char version;
	unsigned char align;
	char reserved[2];
} cso_header;

class gbCso
{
	public:
	enum GBCSO_CONSTANTS { CSO_MAGIC = 0x4F534943 };
	enum GBCSO_RETURNS { GBCSO_OK, GBCSO_OPEN_ERROR, GBCSO_NOT_CSO, GBCSO_PVD_NOT_FOUND, GBCSO_ERROR };

	gbCso();
	int open(const char * path);
	int getPrimaryVolumeDescriptor();
	unsigned findFile(const char * file, unsigned * fileSize);
	unsigned char * getDataBlock(unsigned block);
	void extractFile(const char * name, unsigned block, unsigned size);
	void clear();
	
	private:
	unsigned identifyEntry(const char * name, unsigned block, unsigned * fileSize);
	
	FILE * file;
	unsigned * indices;
	unsigned char * data, * read_buffer;
	unsigned total_blocks, indices_len, current_index, current_index2;
	cso_header file_header, head;
	primaryVolumeDescriptor pvd;
	z_stream dec;
};

#endif