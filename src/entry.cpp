#include "entry.h"

entry138 :: entry138()
{
	path = name = NULL;
	icon = NULL;
};

entry138 :: ~entry138()
{
	clear();
};

void entry138 :: clear()
{
	if(path)
		delete [] path;
	if(name)
		delete [] name;
	if(icon)
		freeImage(icon);
	
	path = name = NULL;
	icon = NULL;
};

int entry138 :: create(const char * path)
{
	if(load(path) == LOAD_ERROR) //try to load all the data for this file
	{
		clear(); //clean resources
		return LOAD_ERROR;
	};
	
	//loaded successfully
	return LOAD_OK;
};

int entry138 :: load(const char * path)
{
	//get file type and check it is a valid file
	type = get_file_type(path);
	
	if(type_is_valid(type) == false) //file type is not valid, abort
		return LOAD_ERROR;
	
#ifndef CEF

	if(type != PBP && type != ZIP) //only pbp and zip files are allowed in HBL version
		return LOAD_ERROR;

#endif
	
	char * sfo_file_path = (char *)path; //define the sfo file path for this type of entry
	char * icon0_file_path = (char *)path; //define the icon0 file path for this entry
	unsigned sfo_offset = 0; //offset of sfo in file
	unsigned icon0_offset = 0; //offset of icon0 in file
	
	bool search_icon = (type == ZIP && cfg.zip_icon) || (type != ZIP && cfg.app_icon);
	
	switch(type)
	{
		case PBP:
		{
			eboot_header offsets = get_eboot_header(sfo_file_path); //get offsets from pbp header
		
			//store sfo and icon0 offset
			sfo_offset = offsets.param;
			icon0_offset = offsets.icon0;
			
			break;
		}
		case ZIP:
		{
			if(cfg.load_mode == MODE_ACCURATE) //accurate mode load
			{
				gbzip zip;
					
				//try to find an eboot file inside the zip
				unsigned eboot_offset = zip.fast_search(path, "EBOOT.PBP");
				if(!eboot_offset)
					return LOAD_ERROR; //not a valid installer, abort
				
				//find offsets inside zipped eboot
				eboot_header offsets = get_eboot_header(path, eboot_offset);
				sfo_offset = eboot_offset + offsets.param; 
				icon0_offset = eboot_offset + offsets.icon0;
				
				if(!sfo_offset)
					return LOAD_ERROR; //sfo not found, abort
					
			}
			else if(cfg.load_mode == MODE_FAST) //fast mode load
			{
				//set external files path
				
				//savedata/.../param.sfo file
				sfo_file_path = new char[strlen(path) + 8];
				strcpy(sfo_file_path, path);
				strcpy(strrchr(sfo_file_path, '/'), "/PARAM.SFO");
				
				//savedata/.../icon0.png file
				icon0_file_path = new char[strlen(path) + 8];
				strcpy(icon0_file_path, path);
				strcpy(strrchr(icon0_file_path, '/'), "/ICON0.PNG");
			}
			else //undefined load mode
				return LOAD_ERROR; 
				
			break;
		}
		case ISO:
		{
			gbiso iso;
		
			if(!iso.open(path)) //try to open iso file and load main structures
				return LOAD_ERROR;
			
			if(!iso.find("PSP_GAME/PARAM.SFO", &sfo_offset)) //locate param.sfo file inside iso
				return LOAD_ERROR;
			
			extra = iso.find("PSP_GAME/SYSDIR/EBOOT.OLD")? PATCHED: NOT_PATCHED; //search for EBOOT.OLD file (for patched isos)
			
			if(search_icon == true)
				iso.find("PSP_GAME/ICON0.PNG", &icon0_offset); //locate icon file inside iso
		
			break;
		}
		case CSO:
		{
			gbcso cso;
			
			if(!cso.open(path)) //try to open cso file
				return LOAD_ERROR;
		
			if(!cso.extract("PSP_GAME/PARAM.SFO", TEMPORAL_PARAM)) //try to extract param file
				return LOAD_ERROR;
			
			if(search_icon == true)
				cso.extract("PSP_GAME/ICON0.PNG", TEMPORAL_ICON); //try to extract icon
			
			sfo_file_path = (char *)TEMPORAL_PARAM;
			icon0_file_path = (char *)TEMPORAL_ICON;
		
			break;
		}
	};

	if(search_icon == true) //load icon?
	{
		//try to load icon
		icon = loadImage(icon0_file_path, icon0_offset);
		if(icon) //image was loaded
		{
			//limit image size
			if(icon->imageWidth >= MAX_ICON_WIDTH) //limit width
				icon->imageWidth = MAX_ICON_WIDTH;
				
			if(icon->imageHeight >= MAX_ICON_HEIGHT) //limit height
				icon->imageHeight = MAX_ICON_HEIGHT;
		};
	};
	
	//delete icon0 path array if allocated
	if((icon0_file_path != (char *)path) && (icon0_file_path != (char *)TEMPORAL_ICON)) 
		delete [] icon0_file_path;
	
	//create sfo parser object
	sfo file_info; 
	
	//try to parse sfo file
	int res = file_info.load(sfo_file_path, sfo_offset);
	
	//delete sfo file path array if allocated
	if((sfo_file_path != (char *)path) && (sfo_file_path != (char *)TEMPORAL_PARAM)) 
		delete [] sfo_file_path;
	
	if(!res) //couldnt parse file	
		return LOAD_ERROR; 
	
	//find important entries in sfo file
	char * sfo_title = file_info.get_data("TITLE");
	char * sfo_category = file_info.get_data("CATEGORY");
	
	if(!sfo_title || !sfo_category) //couldn't get important data from sfo, abort
		return LOAD_ERROR;
	
	//define the app type for pbps
	if(type == PBP)
	{
		extra = *(unsigned short *)sfo_category; //get category u16 from string
		
		if(extra != PSN && extra != PS1 && extra != APP) //valid types
			return 0;
			
#ifndef CEF
		
		if(extra != APP) //hbl only runs homebrew
			return LOAD_ERROR;
			
#endif	

	};
	
	//store file path
	this->path = new char[strlen(path) + 1];
	strcpy(this->path, path);
	
	char * use_name = sfo_title;
	
	if(type == ISO || type == CSO)
	{
		char * sfo_disc_id = file_info.get_data("DISC_ID");
		
		if(sfo_disc_id) //patch for japanese games names
		{
			if(sfo_disc_id[2] == 'J' || sfo_disc_id[2] == 'K' || sfo_disc_id[2] == 'A' || sfo_disc_id[2] == 'H')
				use_name = sfo_disc_id;
		};
	};
	
	//store title
	name = new char[strlen(use_name) + 1]; 
	strcpy(name, use_name);
	
	if(use_name == sfo_title) //fix title for non-japanese games
		fix_name_string(name);
	
	//success
	return LOAD_OK;
};

inline bool entry138 :: type_is_valid(int type)
{
	//returns true if the file type is valid (compares magic)
	return type == PBP || type == ZIP || type == SFO || type == ISO || type == CSO;
};

int entry138 :: get_file_type(const char * path)
{
	//try to open file
	FILE * stream = fopen(path, "rb");
	
	if(!stream) //couldn't open file
		return 0;
	
	unsigned magic = 0;
	
	if(!fseek(stream, 0x8000, 0)) //try to seek to ISO pvd offset
		fread(&magic, sizeof(magic), 1, stream); //read magic

	if(magic != ISO) //it is not an iso
	{
		fseek(stream, 0, SEEK_SET); //go to file start again
		fread(&magic, sizeof(magic), 1, stream); //read magic
	};
	
	//close file
	fclose(stream);
	return magic;
};

eboot_header entry138 :: get_eboot_header(const char * path, unsigned offset)
{
	//open file
	FILE * stream = fopen(path, "rb");
	
	//go to offset
	fseek(stream, offset, SEEK_SET);
	
	//read eboot header
	eboot_header header;
	fread(&header, sizeof(eboot_header), 1, stream);
	
	//close file
	fclose(stream);
	
	return header;
};

void entry138 :: fix_name_string(char * str)
{
	//limit name lenght
	if(strlen(str) > ENTRY_NAME_LIMIT)
		strcpy(str + ENTRY_NAME_LIMIT /*- 4*/, "..\0");
	
	//replace invalid chars
	for(unsigned i = 0; i < strlen(str); i++)
	{
		if(str[i] == '\n' || str[i] == '\t')
			str[i] = ' ';
		
		if(i > 0)
		{
			if(str[i - 1] != ' ' && str[i] >= 'A' && str[i] <= 'Z')
				str[i] = str[i] + 32;
		};
	};
};

char * entry138 :: getPath() const
{
	return path;
};

char * entry138 :: getName() const
{
	return name;
};

int entry138 :: getType() const
{
	return type;
};

int entry138 :: getExtra() const
{
	return extra;
};

Image * entry138 :: getIcon()
{
	return icon;
};

int entry138 :: get_priority(const entry138 * entry)
{
	int type = entry->getType();
	int extra = entry->getExtra();

	if(type == ISO || type == CSO)
		return 5;
	if(extra == entry138::PSN)
		return 4;
	if(extra == entry138::PS1)
		return 3;
	if(extra == entry138::APP)
		return 2;
	
	return 1;	
};

bool entry138 :: entry_sort(const entry138 * a, const entry138 * b)
{
	int a_prior = entry138::get_priority(a), b_prior = entry138::get_priority(b);
	
	return a_prior > b_prior;
};