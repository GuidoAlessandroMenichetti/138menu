#include "gbsfo.h"

sfo :: sfo()
{
	init_values();
};

sfo :: sfo(const char * path)
{
	init_values();
	load(path);
};

sfo :: ~sfo()
{
	clear();
};

void sfo :: init_values()
{
	loaded = false;
	total_entries = 0;
	entries = NULL;
	stream = NULL;
};

void sfo :: clear()
{
	if(stream)
		fclose(stream);

	//free resources
	for(int i = 0; i < total_entries; i++)
	{
		//free entry data
		if(entries[i].key)
			delete [] entries[i].key;

		if(entries[i].data)
			delete [] entries[i].data;
	};

	if(entries)
		delete [] entries;

	init_values();
};

inline bool sfo :: open_file(const char * path, const char * mode)
{
	return (stream = fopen(path, mode)) != NULL;
};

inline int sfo :: get_file_type()
{
	//go to file start
	fseek(stream, 0, SEEK_SET);

	//read first 4 bytes
	unsigned magic;
	fread(&magic, sizeof(magic), 1, stream);

	//return type based on magic number
	return (magic == SFO_MAGIC)? TYPE_SFO: (magic == PBP_MAGIC)? TYPE_PBP: TYPE_UNKNOWN;
};

inline unsigned sfo :: get_sfo_offset()
{
	//get sfo offset inside a pbp file
	t_eboot_header header;

	//go to file start
	fseek(stream, 0, SEEK_SET);

	//read eboot header
	fread(&header, sizeof(header), 1, stream);

	//return offset
	return header.sfo;
};

char * sfo :: read_bytes(unsigned offset, unsigned short size)
{
	//save current position
	unsigned restore = ftell(stream);

	//allocate memory
	char * input = new char[size + 2];
	if(!input)
		return NULL;

	//go to offset
	fseek(stream, offset, SEEK_SET);

	//read bytes
	fread(input, size, 1, stream);

	//add a null char to avoid bugs
	*(input + size) = 0x0;

	//restore position
	fseek(stream, restore, SEEK_SET);

	return input;
};

inline int sfo :: get_entry(unsigned index, unsigned key_table_offset, unsigned data_table_offset)
{
	//read current index table entry
	t_index_table_entry index_table_entry;
	fread(&index_table_entry, sizeof(index_table_entry), 1, stream);
	
	//get key
	entries[index].key = read_bytes(key_table_offset + index_table_entry.offset, 32);

	//get data
	entries[index].data = read_bytes(data_table_offset + index_table_entry.data_offset, index_table_entry.len);

	//check if key and data were readen
	if(!entries[index].key || !entries[index].data)
		return 0;

	return 1;
};

inline int sfo :: read_index_table(unsigned start_offset)
{
	//go to sfo start
	fseek(stream, start_offset, SEEK_SET);

	//read sfo header
	t_sfo_header sfo_header;
	fread(&sfo_header, sizeof(sfo_header), 1, stream);

	//check correct magic
	if(sfo_header.sign != SFO_MAGIC)
		return 0;

	//allocate memory for entries
	entries = new t_entry[sfo_header.entries];
	if(!entries)
		return 0;

	total_entries = sfo_header.entries;

	//start reading entries
	unsigned key_table_absolute = start_offset + sfo_header.key_table;
	unsigned data_table_absolute = start_offset + sfo_header.data_table;

	for(unsigned i = 0; i < total_entries; i++)
	{
		//read entry
		if(!get_entry(i, key_table_absolute, data_table_absolute))
			return 0;
	};

	return 1;
};

int sfo :: load(const char * path, unsigned force_offset)
{
	//clear previous data
	//clear();

	//open file
	if(open_file(path, "rb") == false)
		return 0;

	unsigned sfo_offset = force_offset;
	int file_type = TYPE_SFO;

	//offset not forced?
	if(force_offset == NOT_FORCED)
	{
		//get file type
		file_type = get_file_type();

		//if file is a pbp, look for sfo offset
		if(file_type == TYPE_PBP)
			sfo_offset = get_sfo_offset();
	};

	//start reading entries
	if(file_type == TYPE_UNKNOWN || !read_index_table(sfo_offset))
	{
		clear();
		return 0;
	};

	loaded = true;
	fclose(stream);
	stream = NULL;
	
	return 1;
};

char * sfo :: get_data(const char * key)
{
	//search for the key
	char * ret = NULL;
	for(unsigned short i = 0; i < total_entries; i++)
	{
		if(!strcmp(key, entries[i].key))
		{
			ret = entries[i].data;
			break;
		};
	};

	return ret;
};

bool sfo :: is_loaded()
{
	return loaded;
};
