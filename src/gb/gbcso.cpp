#include "gbcso.h"

gbcso :: gbcso()
{
	stream = NULL;
	read_buffer = NULL;
	buffer = NULL;
};

gbcso :: ~gbcso()
{
	clear();
};

void gbcso :: clear()
{
	if(stream)
		fclose(stream);

	if(read_buffer)
		delete [] read_buffer;
	if(buffer)
		delete [] buffer;
		
	stream = NULL;
	read_buffer = NULL;
	buffer = NULL;
};

int gbcso :: open(const char * path)
{
	clear();
	stream = fopen(path, "rb"); //try to open

	if(!stream)
		return 0;

	fread(&info, sizeof(cso_header), 1, stream); //read header

	if(info.magic != CSO_MAGIC) //check magic
	{
		clear();
		return 0;
	};

	current_sector = 0;
	buffer_pointer = buffer;

	//allocate mem for indices and read buffer
	read_buffer = new unsigned char[info.block_size];
	buffer = new unsigned char[info.block_size];
	
	//zlib related
	dec.zalloc = NULL;
	dec.zfree = NULL;
	dec.opaque = NULL;
	
	if(!load())
	{
		clear();
		return 0;
	};

	return 1;
};

int gbcso :: decompress_sector(unsigned sector)
{
	//sector decription based on BOOSTER cso compressor sample
	unsigned get_size, is_plain, read_position, compare_size;
	int status;

	if(inflateInit2(&dec,-15) != Z_OK)
		return 0;

	unsigned current_index = get_block_offset(sector);
	is_plain = current_index & 0x80000000;
	current_index = current_index & 0x7fffffff;
	read_position = current_index << (info.align); //alignment

	if(!is_plain)
	{
		unsigned next_index = get_block_offset(sector + 1) & 0x7fffffff;
		get_size = (next_index - current_index) << (info.align);
	}
	else 
		get_size = info.block_size;

	fseek(stream, read_position, SEEK_SET); //seek to sector block
	dec.avail_in = fread(read_buffer, 1, get_size, stream); //read block
	
	if(is_plain)
	{
		memcpy(buffer, read_buffer, get_size); //copy plain data
		compare_size = get_size;
	}
	else
	{
		dec.next_out = buffer;
		dec.avail_out = info.block_size;
		dec.next_in = read_buffer;

		status = inflate(&dec, Z_FULL_FLUSH); //decompress data
		compare_size = info.block_size - dec.avail_out;
		if((status != Z_STREAM_END) || (compare_size != info.block_size))
			return 0;
	};

	return inflateEnd(&dec) == Z_OK;
};

unsigned gbcso :: get_block_offset(unsigned sector)
{
	//will return the file offset in the index list
	unsigned block = 0;

	fseek(stream, sizeof(cso_header) + 4 * sector, SEEK_SET); //go to index offset
	fread(&block, sizeof(unsigned), 1, stream); //read block offset
	
	return block;
};

//overrides gbiso::read()
int gbcso :: read(void * destination, unsigned size, unsigned count, void * p)
{
	memcpy(destination, buffer_pointer, size * count); //copy to buffer
	buffer_pointer += size * count;

	return 1;
};

//overrides gbiso::seek()
int gbcso :: seek(void * p, unsigned bytes, int flag)
{
	unsigned new_sector = current_sector;

	if(flag == SEEK_SET) //emulate a seek using the memory buffer
	{
		new_sector = bytes / SECTOR_SIZE;
		buffer_pointer = buffer + (bytes % SECTOR_SIZE);
	}
	else if(flag == SEEK_CUR)
	{
		new_sector = ((int)(buffer_pointer - buffer) + bytes > SECTOR_SIZE)? current_sector + 1: current_sector;
		
		buffer_pointer = ((int)(buffer_pointer - buffer) + bytes > SECTOR_SIZE)?
			(buffer_pointer - (int)buffer) - SECTOR_SIZE + bytes:
			buffer_pointer + bytes;
	};

	if(new_sector != current_sector) //sector is different than previous one
	{
		current_sector = new_sector;
		return !decompress_sector(new_sector); //decompress new sector
	};
	
	return 0;
};
