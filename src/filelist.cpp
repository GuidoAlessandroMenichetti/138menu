#include "filelist.h"

filelist138 :: filelist138()
{

};

filelist138 :: ~filelist138()
{
	clear();
};

void filelist138 :: delete_entry(t_file * file)
{
	//delete file info
	if(file->path)
		delete [] file->path;
	if(file->name)
		delete [] file->name;
};

void filelist138 :: clear()
{
	//clear every file
	for(unsigned i = 0; i < files.size(); i++)
		delete_entry(files[i]);

	files.clear(); //clear vector
};

bool filelist138 :: sort_function(const t_file * file1, const t_file * file2)
{
	if(file1->type > file2->type) //FOLDER > FILE
		return true;
	else if(file1->type < file2->type)
		return false;
		
	return strcmpi(file1->name, file2->name) < 0; //compare names	
};

int filelist138 :: load(const char * path, const char * extension_filter)
{
	int count = 0;
	
	//try to open folder
	SceUID directory = sceIoDopen(path);
	
	if(directory >= 0)
	{
		SceIoDirent entry; 
		memset(&entry, 0, sizeof(SceIoDirent)); //clear directory entry
	
		char buffer[1024]; //buffer to create file path
	
		//start reading directory entries
		while(sceIoDread(directory, &entry) > 0)
		{
			//skip . and .. paths
			if(!strcmp(".", entry.d_name) || !strcmp("..", entry.d_name)) 
			{
				//clear directory entry and continue
				memset(&entry, 0, sizeof(SceIoDirent)); 
				continue;
			};
			
			//build full path
			strcpy(buffer, path);
			strcat(buffer, entry.d_name);
			
			char * name = strrchr(buffer, '/') + 1;
			
			//set entry type
			int type = FILES;
			if(explorer138::file_exists(buffer)) //is it a file?
			{
				//filter file
				if(extension_filter)
				{
					char * extension = strrchr(name, '.');
					if(extension)
					{
						extension++;
						if(strcmpi(extension_filter, extension)) //extension does not match
							continue; //skip file
					}
					else
						continue;
				};
			}
			else
			{
				if(extension_filter)
					continue;
					
				//must be a folder
				strcat(buffer, "/");
				type = FOLDER;
			};

			int size = explorer138::file_size(buffer);
			
			//add entry to vector
			add_file(buffer, name, type, size);
			
			count++;
		};
	
		//close folder
		sceIoDclose(directory);
		
		//sort files
		sort(files.begin(), files.end(), filelist138::sort_function);
	};
	
	return count;
};

int filelist138 :: delete_file(const char * path, int COMPARE_NAME)
{
	//search for file in the filelist
	int find = find_file(path, COMPARE_NAME);

	if(find >= 0)
	{
		//delete
		delete_entry(files[find]);
		files.erase(files.begin() + find);
		return 1;
	};
	
	return 0;
};

int filelist138 :: find_file(const char * path, int COMPARE_NAME)
{
	//search for file in the filelist
	for(int i = 0; i < (int)files.size(); i++)
	{
		if(COMPARE_NAME)
		{
			if(!strcmp(path, files[i]->name))
				return i;
		}
		else
		{
			if(!strcmp(path, files[i]->path)) //compare paths
				return i;
		};
		
	};
	
	return -1;
};

void filelist138 :: add_file(const char * path, const char * name, int type, int size)
{
	t_file * new_file = new t_file;
	
	//store path
	new_file->path = new char[strlen(path) + 1];
	strcpy(new_file->path, path);
	
	//store name
	new_file->name = new char[strlen(name) + 1];
	strcpy(new_file->name, name);
	
	//limit name
	if(strlen(new_file->name) > ENTRY_NAME_LIMIT)
		strcpy(new_file->name + ENTRY_NAME_LIMIT - 4, "..\0");
	
	//store type
	new_file->type = type;
	//store size
	new_file->size = size;
	
	//push to vector
	files.push_back(new_file);
};

t_file * filelist138 :: get(unsigned index)
{
	if(index > files.size())
		return NULL;
		
	return files[index];
};

int filelist138 :: get_count()
{
	return files.size();
};