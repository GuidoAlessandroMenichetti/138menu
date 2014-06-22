#include "gbiso.h"

gbiso :: gbiso()
{
	stream = NULL;
};

gbiso :: ~gbiso()
{
	clear();
};

void gbiso :: clear()
{
	if(stream)
		fclose(stream);

	stream = NULL;
};

int gbiso :: open(const char * path)
{
	clear();

	stream = fopen(path, "rb");

	if(!stream)
		return 0;

	if(!load()) //try to load volume descriptor and path table
	{
		clear();
		return 0;
	};

	return 1;
};

int gbiso :: load()
{
	if(seek(stream, SECTOR_SIZE * VOLUME_DESCRIPTOR_START_SECTOR, SEEK_SET)) //try to seek to pvd offset
		return 0;

	if(!read(&pvd, sizeof(primary_volume_descriptor), 1, stream)) //read pvd
		return 0;

	if(strncmp(PVD_MAGIC, pvd.magic, 5)) //check magic pvd string
		return 0;

	if(seek(stream, pvd.path_table_location_LSB * SECTOR_SIZE, SEEK_SET)) //seek to path table
		return 0;

	if(!read(&main_path_entry, sizeof(path_entry), 1, stream)) //read first path entry
		return 0;

	return 1;
};

int gbiso :: find(const char * path, unsigned * store_offset, unsigned * store_size)
{
	if(!stream)
		return 0;

	return search_in_directory_entry(path, main_path_entry.location, store_offset, store_size); //start recursive search
};

int gbiso :: search_in_directory_entry(const char * path, unsigned dir_entry_sector, unsigned * store_offset, unsigned * store_size)
{
	directory_record dr;
	char * folder_end = get_name_end(path);

	if(seek(stream, dir_entry_sector * SECTOR_SIZE, SEEK_SET)) //go to dir record offset
		return 0;

	if(!read(&dr, sizeof(directory_record), 1, stream)) //try to read first record
		return 0;

	unsigned bytes_read = 0;

	while(dr.length)
	{
		char * file_name = new char[dr.length_file_id + 1]; //allocate mem for name
		read(file_name, dr.length_file_id, 1, stream); //read name
		*(file_name + dr.length_file_id) = 0x0;

		if(!strcmp(file_name + dr.length_file_id - 2, ";1")) //fix the extra ";1" added by some software
			*(file_name + dr.length_file_id - 2) = 0x0;

		//is this the file/subdir we are searching?
		int found = !strncmp(file_name, path, (int)(folder_end - path));

		delete [] file_name; //we dont need the entry name anymore

		if(found) //names match
		{
			if(dr.flags & FLAG_DIRECTORY) //start searching in new dir
				return search_in_directory_entry(folder_end + 1, dr.location.little, store_offset, store_size);
			else //file was found omg!
			{
				//store results
				if(store_offset)
					* store_offset = dr.location.little * SECTOR_SIZE;

				if(store_size)
					* store_size = dr.data_length.little;

				return 1;
			};
		};

		//skip padding/system use
		if(seek(stream, dr.length - (sizeof(directory_record) + dr.length_file_id), SEEK_CUR))
			return 0;

		//check that dir entry does not exit the current sector
		bytes_read += dr.length;
		if(bytes_read + sizeof(directory_record) + 8 > SECTOR_SIZE)
		{
			seek(stream, SECTOR_SIZE - bytes_read, SEEK_CUR);
			bytes_read = 0;
		};

		if(!read(&dr, sizeof(directory_record), 1, stream)) //try to read next record
			break;
	};

	return 0;
};

int gbiso :: extract(const char * file, const char * destination)
{
	unsigned sector, size;
	
	if(!find(file, &sector, &size)) //find file
		return 0;
		
	sector /= SECTOR_SIZE; //offset gets transformed into sector

	FILE * output = fopen(destination, "wb"); //open output file
	
	if(!output)
		return 0;

	bool abort = false;
		
	while(size && abort == false) 
	{
		unsigned char buffer[SECTOR_SIZE];
		unsigned copy_size = size > SECTOR_SIZE? SECTOR_SIZE: size; //calculate bytes count
		
		if(!seek(stream, sector * SECTOR_SIZE, SEEK_SET)) //seek to sector
		{
			if(read(buffer, copy_size, 1, stream)) //read bytes
				fwrite(buffer, copy_size, 1, output); //copy bytes
			else
				abort = true;
		}
		else
			abort = true;
	
		sector++;
		size -= copy_size;
	};
	
	fclose(output);
	
	return abort == false;
};

int gbiso :: read(void * destination, unsigned size, unsigned count, void * p)
{
	return fread(destination, size, count, (FILE *)p);
};

int gbiso :: seek(void * p, unsigned bytes, int flag)
{
	return fseek((FILE *)p, bytes, flag);
};

char * gbiso :: get_name_end(const char * str)
{
	while(* str != '/' && * str)
		str++;

	return (char *)str;
};
