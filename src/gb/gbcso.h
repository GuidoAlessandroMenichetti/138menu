#ifndef GBCSO_HEADER
#define GBCSO_HEADER

#include <zlib.h>
#include "gbiso.h"

#define CSO_MAGIC 0x4F534943

typedef struct
{
	unsigned magic;
	unsigned header_size;
	unsigned long long file_size;
	unsigned block_size;
	unsigned char version;
	unsigned char align;
	char reserved[2];
} __attribute__ ((__packed__)) cso_header;

class gbcso : public gbiso
{
	public:

	gbcso();
	~gbcso();

	int open(const char * path);

	private:

	void clear();
	int decompress_sector(unsigned sector);
	unsigned get_block_offset(unsigned sector);
	
	//overrides some gbiso methods
	int read(void * destination, unsigned size, unsigned count, void * p);
	int seek(void * p, unsigned bytes, int flag);

	unsigned current_sector;
	unsigned char * read_buffer;
	unsigned char * buffer;
	unsigned char * buffer_pointer;
	cso_header info;
	z_stream dec;
};

#endif
