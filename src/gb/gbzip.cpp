#include "gbzip.h"

gbzip :: gbzip()
{
	initial_values();
};

gbzip :: ~gbzip()
{
	clear();
};

void gbzip :: initial_values()
{
	current_file = NULL;
	stream = NULL;
};

void gbzip :: clear()
{
	if(current_file)
		delete [] current_file;

	if(stream)
		fclose(stream);

	initial_values();
};

inline bool gbzip :: open_file(const char * path, const char * mode)
{
	return (stream = fopen(path, mode)) != NULL;
};

int gbzip :: extract(const char * file, const char * destination, const char * rename)
{
	this->destination = destination;
	this->source = file;
	this->rename = rename;

	if(!load(file)) //start loading zip file
	{
		clear(); //failed
		return 0;
	};

	//success
	return 1;
};

void gbzip :: fix_path(char * path)
{
	static const char invalid_chars[] = ":,?¿¡!*\'";
	
	for(unsigned i = 0; i < strlen(path); i++)
	{
		for(unsigned j = 0; j < sizeof(invalid_chars); j++)
		{
			if(path[i] == invalid_chars[j])
				path[i] = '_';
		};
	};
};

int gbzip :: extract_file(char * name, unsigned offset, unsigned size)
{
	unsigned restore = ftell(stream);

	fseek(stream, offset, SEEK_SET);
	t_zip_file_header file_info;
	fread(&file_info, sizeof(t_zip_file_header), 1, stream);

	unsigned absolute_offset = ftell(stream) + file_info.name_length + file_info.extra_length;

	char * new_destination = new char[strlen(destination) + strlen(name) + 1];

	//build new path
	strcpy(new_destination, destination);
	
	//validate name
	fix_path(name);
	
	if(!strcmp(name, "EBOOT.PBP"))
		strcat(new_destination, rename);
	else
		strcat(new_destination, name);

	int ret = 0;

	if(name[strlen(name) - 1] == '/') //it is a folder
		explorer138::create_folder(new_destination);
	else //it is a file
	{
		strcpy(current_file, strrchr(name, '/') + 1);
		if(!size)
			explorer138::create_dummy(new_destination);
		else
			explorer138::copy_file(source, new_destination, absolute_offset, size);
	};
	
	delete [] new_destination;

	fseek(stream, restore, SEEK_SET);

	return ret;
};

int gbzip :: start_reading_cds()
{
	//go to cds offset
	fseek(stream, end_of_cd.cd_offset, SEEK_SET);

	int current_cd = 0;
	while(current_cd < end_of_cd.cd_count)
	{
		//read central directory entry
		t_central_directory cd;
		fread(&cd, sizeof(cd), 1, stream);

		if(cd.compression) //file compressed? abort
			return 0;

		//read name
		char * name = new char[cd.name_length + 1];
		fread(name, cd.name_length, 1, stream);
		*(name + cd.name_length) = 0x0;

		//extract this file
		extract_file(name, cd.offset, cd.size_uncompressed);

		delete [] name;

		current_cd++;

		//skip central directory data
		fseek(stream, cd.extra_length + cd.comment_length, SEEK_CUR);
	};

	return 1;
};

int gbzip :: get_end_of_cd()
{
	unsigned magic;
	int retries = 0;

	//go to possible eocd offset
	fseek(stream, - sizeof(t_end_of_central_directory), SEEK_END);

	do
	{
		//read magic
		fread(&magic, sizeof(unsigned), 1, stream);

		if(magic == EOCD_MAGIC)
		{
			//go back 4 bytes (magic size)
			fseek(stream, - sizeof(unsigned), SEEK_CUR);

			//read eocd struct
			fread(&end_of_cd, sizeof(end_of_cd), 1, stream);

			return 1;
		};

		//go back 5 bytes (magic size + 1)
		fseek(stream, - sizeof(unsigned) - 1, SEEK_CUR);

		retries++;
	} while(magic != EOCD_MAGIC && retries < 0x40);

	//eocd not found
	return 0;
};

int gbzip :: load(const char * file)
{
	//try to open file
	if(open_file(file, "rb") == false)
		return 0; //couldnt open file

	//get end of central directory
	if(!get_end_of_cd())
		return 0; //failed

	current_file = new char[256];	
	
	//start reading entries from central directories
	if(!start_reading_cds())
		return 0; //failed

	//close file
	fclose(stream);

	//success
	return 1;
};

unsigned gbzip :: fast_search(const char * file, const char * search)
{
	if(open_file(file, "rb") == false)
		return 0; //couldnt open file

	//get end of central directory
	if(!get_end_of_cd())
		return 0; //failed

	bool found = false;
	unsigned result = 0;
	int current_cd = 0;

	//go to cds offset
	fseek(stream, end_of_cd.cd_offset, SEEK_SET);

	//start reading cd entries
	while(found == false && current_cd < end_of_cd.cd_count)
	{
		//read central directory entry
		t_central_directory cd;
		fread(&cd, sizeof(cd), 1, stream);

		//read name
		char * name = new char[cd.name_length + 1];
		fread(name, cd.name_length, 1, stream);
		*(name + cd.name_length) = 0x0;

		//define where starts the wanted file name
		char * compare_address = strrchr(name, '/');
		if(compare_address)
			compare_address++;
		else
			compare_address = name;

		if(!strcmp(search, compare_address))
		{
			//file found
			found = true;
			result = cd.offset + sizeof(t_zip_file_header) + cd.extra_length + cd.comment_length + cd.name_length;
		};

		delete [] name;
		current_cd++;

		//skip central directory data
		fseek(stream, cd.extra_length + cd.comment_length, SEEK_CUR);
	};

	fclose(stream);
	return result;
};
