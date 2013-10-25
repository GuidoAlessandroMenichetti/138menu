#include "gbCso.h"

void readUnsignedFromMem(unsigned * store, void * address)
{
	unsigned char * p = (unsigned char *)store;
	*(p) = *(unsigned char *)address;
	*(p+1) = *(unsigned char *)((unsigned)address+1);
	*(p+2) = *(unsigned char *)((unsigned)address+2);
	*(p+3) = *(unsigned char *)((unsigned)address+3);
};

gbCso :: gbCso()
{
	file = NULL;
	indices = NULL;
	data = NULL;
	read_buffer = NULL;
};

int gbCso :: open(const char * path)
{
	file = fopen(path, "rb");
	if(!file) return gbCso::GBCSO_OPEN_ERROR;
	fread(&head, sizeof(cso_header), 1, file);
	
	if(head.magic!=gbCso::CSO_MAGIC)
	{
		fclose(file);
		return gbCso::GBCSO_NOT_CSO;
	};
	
	total_blocks = head.file_size/head.block_size;
	indices_len = 4 * (total_blocks+1);
	current_index = 0;
	
	this->indices = (unsigned *)malloc(indices_len);
	this->data = (unsigned char *)malloc(head.block_size*2);
	this->read_buffer = (unsigned char *)malloc(head.block_size*2);
	dec.zalloc = NULL;
	dec.zfree = NULL;
	dec.opaque = NULL;
	
	fread(this->indices, 1, indices_len, file);
	
	return gbCso::GBCSO_OK;
};

void gbCso :: clear()
{
	if(file) fclose(file);
	if(indices) free(indices);
	if(data) free(data);
	if(read_buffer) free(read_buffer);
};

unsigned char * gbCso :: getDataBlock(unsigned block)
{
	unsigned get_size, is_plain, read_position, compare_size;
	int status;
	
	if(inflateInit2(&dec,-15) != Z_OK)
	{
		this->clear();
		return NULL;
	};
	
	current_index = this->indices[block];
	is_plain = current_index & 0x80000000;
	current_index = current_index & 0x7fffffff;
	read_position = current_index << (head.align);
	
	if(!is_plain)
	{
		current_index2 = this->indices[block+1] & 0x7fffffff;
		get_size = (current_index2-current_index) << (head.align);
	}
	else get_size = head.block_size;
	
	fseek(file, read_position, SEEK_SET);	
	dec.avail_in = fread(this->read_buffer, 1, get_size, file);
	if(is_plain)
	{
		memcpy(this->data, this->read_buffer, get_size);
		compare_size = get_size;
	}
	else
	{
		dec.next_out = this->data;
		dec.avail_out = head.block_size;
		dec.next_in = this->read_buffer;
		
		status = inflate(&dec, Z_FULL_FLUSH);
		compare_size = head.block_size - dec.avail_out;
		if((status != Z_STREAM_END) || (compare_size != head.block_size))
		{
			this->clear();
			return NULL;
		};
	};
	
	if(inflateEnd(&dec) != Z_OK)
	{
		this->clear();
		return NULL;
	};
	
	return this->data;
};

unsigned gbCso :: identifyEntry(const char * name, unsigned block, unsigned * fileSize)
{
	unsigned ret = 0;
	char * entry_name = NULL;
	unsigned char * ptr = this->data;
    dirRecord dr;
	
	this->getDataBlock(block);
	dr.length = *(char *)ptr;
    while(!ret && dr.length)
    {
        if(dr.length!=0x30)
        {
			readUnsignedFromMem(&dr.location, ptr+2);
			readUnsignedFromMem(&dr.size, ptr+10);
			dr.nameLen = *(char *)(ptr+32);

            entry_name = (char *)malloc(dr.nameLen+1);
			strncpy(entry_name, (char *)(ptr+33), dr.nameLen);
            *(entry_name+dr.nameLen)='\0';
			
            if(!strcmp(entry_name, name)) //File was found!
			{
                ret = dr.location;
				*fileSize = dr.size;
			};
            free(entry_name);
        };
		ptr+=dr.length;
        dr.length = *(char *)ptr;
    };
	return ret;
};

void gbCso :: extractFile(const char * name, unsigned block, unsigned size)
{
	FILE * b;
	b = fopen(name, "wb");
	unsigned total_write;
	while(size>0)
	{
		if(size>=0x800) total_write = 0x800;
		else total_write = size;
		
		this->getDataBlock(block);
		fwrite(this->data, total_write, 1, b);
		size-=total_write;
		block++;
	};
	fclose(b);
};

unsigned gbCso :: findFile(const char * name, unsigned * fileSize)
{
	pathTable pt;
	this->getDataBlock(this->pvd.pathTableOffset);
	unsigned char * ptr = this->data;
	unsigned ret = 0;
	char * dir_name = NULL;
	int end = 0;
	*fileSize = 0;

	while(((unsigned)(ptr-this->data) < pvd.pathTableSize) && !end)
	{
		pt.length = *(char *)(ptr);
		readUnsignedFromMem(&pt.location, ptr+2);
		ptr+=8;
		dir_name = (char *)malloc(pt.length+1);
		strncpy(dir_name, (char *)ptr, pt.length);
		*(dir_name+pt.length) = '\0';
		
		if(!strncmp(name, dir_name, (int)(strrchr(name, '/')-name)))
		{
			ret = this->identifyEntry(strrchr(name, '/')+1, pt.location, fileSize);
			end = 1;
		};
		ptr+=pt.length;
		if(pt.length % 2) ptr++;
		free(dir_name);
	};
	return ret;
};

int gbCso :: getPrimaryVolumeDescriptor()
{
	this->getDataBlock(0x10);
	this->pvd = *(primaryVolumeDescriptor *)(this->data+sizeof(volumeDescriptor)+0x7D);
	return 1;
};