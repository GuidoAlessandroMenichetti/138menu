#include "138_entry.h"

entry138 :: entry138()
{
	this->name = NULL;
	this->path = NULL;
	this->icon = NULL;
	this->type = entry138::UNKNOWN;
};

void entry138 :: clear()
{
	if(this->name) free(this->name);
	if(this->path) free(this->path);
	if(this->icon) freeImage(this->icon);
};

int entry138 :: create(const char * file)
{
	if(this->load(file)==entry138::LOAD_ERROR)
	{
		this->clear();
		return entry138::LOAD_ERROR;
	};
	return entry138::LOAD_OK;
};

int entry138 :: load(const char * file)
{
	if(gbExplorer::fileExist(file)!=gbExplorer::FILE_OK) return entry138::LOAD_ERROR;

	ebootHeader ebootData;
	unsigned zipped_offset = 0;
	char * parse_file, * icon_file;
	int get_icon, file_type = entry138::getFileType(file, 0);

	if(file_type==entry138::INVALID) return entry138::LOAD_ERROR;
	if(file_type==entry138::IS_EBOOT) this->type=entry138::HOMEBREW;
	else this->type=entry138::INSTALLER_HB;

	memset(&ebootData, 0, sizeof(ebootHeader));
	get_icon = this->type == entry138::HOMEBREW? cfg.app_icon: cfg.zip_icon;
	if(this->type == entry138::INSTALLER_HB && cfg.load_mode==config138::MODE_FAST) //Fast load mode
	{
		parse_file = (char *)malloc(strlen(file)+11);
		strcpy(parse_file, file);
		strcpy(strrchr(parse_file, '/')+1, FAST_PARAM);
		if(gbExplorer::fileExist(parse_file)!=gbExplorer::FILE_OK) //No PARSE.SFO = no fun
		{
			free(parse_file);
			return entry138::LOAD_ERROR; 
		};
		icon_file = (char *)malloc(strlen(file)+11);
		strcpy(icon_file, file);
		strcpy(strrchr(icon_file, '/')+1, FAST_ICON);
		if(gbExplorer::fileExist(icon_file)!=gbExplorer::FILE_OK) get_icon = get_icon && 1;
	}
	else //Accurate mode and homebrew load
	{
		parse_file = icon_file = (char *)file;
		if(this->type == entry138::INSTALLER_HB) 
		{
			zipped_offset = gbZip::getEbootOffset(file); //Look for the eboot offset inside the ZIP file
			if(!zipped_offset) return entry138::LOAD_ERROR;
		};
		if(!entry138::getEbootHeader(file, &ebootData, zipped_offset)) return entry138::LOAD_ERROR;
		get_icon = get_icon && ebootData.icon0!=ebootData.icon1;
	};

	int ret = this->parseSfo(parse_file, ebootData.param+zipped_offset); //Load entry name
	if(get_icon && ret!=entry138::PARSE_ERROR) //Load entry icon
	{
		this->icon = loadImage(icon_file, ebootData.icon0+zipped_offset);
		if(this->icon)
		{
			if(this->icon->imageWidth > MAX_IMG_WIDTH) this->icon->imageWidth = MAX_IMG_WIDTH;
			if(this->icon->imageHeight > MAX_IMG_HEIGHT) this->icon->imageHeight = MAX_IMG_HEIGHT;
		};
	};

	if(parse_file!=(char *)file) free(parse_file);
	if(icon_file!=(char *)file) free(icon_file);

	if(ret==entry138::PARSE_ERROR) return entry138::LOAD_ERROR;

	this->path = (char *)malloc(strlen(file)+1);
	if(this->path) strcpy(this->path, file);
	else return entry138::LOAD_ERROR;

	//Various string fixes
	if(strlen(this->name)>entry138::NAME_LIMIT) strcpy(this->name+entry138::NAME_LIMIT-4, "..\0");
	for(unsigned i=0;i<strlen(this->name);i++)
		if(*(this->name+i)=='\n') *(this->name+i) = ' ';

	return entry138::LOAD_OK;
};

int entry138 :: parseSfo(const char * file, unsigned offset)
{
	FILE * a;
	a = fopen(file, "rb");
	if(!a) return entry138::PARSE_ERROR;
	fseek(a, offset, SEEK_SET);

	sfoHeader sfo;
	indexTable index;
	char tmp[PARSE_BUF];

	fread(&sfo, sizeof(sfoHeader), 1, a);
	if(sfo.sign!=PARAM_MAGIC)
	{
		fclose(a);
		return entry138::PARSE_ERROR;
	};

	unsigned seek = ftell(a), i;
	memset(&index, 0, sizeof(indexTable));

	for(i=0;i<sfo.entries;i++)
	{
		memset(tmp, 0, sizeof(tmp));
		fread(&index, sizeof(indexTable), 1, a);
		fseek(a, (sfo.entries-1-i) * sizeof(indexTable) + index.offset, 1);
		fread(tmp, sizeof(tmp), 1, a);

		if(!strcmp("TITLE", tmp))
		{
			int ret;
			this->name = (char *)malloc(index.len+1);
			if(this->name)
			{
				fseek(a, sfo.data_table + index.data_offset + offset, 0);
				fread(this->name, index.len, 1, a);
				*(this->name+index.len) = '\0';
				ret = entry138::PARSE_OK;
			}
			else ret = entry138::PARSE_ERROR;
			fclose(a);
			return ret;
		};

		seek += sizeof(indexTable);
		fseek(a, seek, 0);
	};
	fclose(a);
	return entry138::PARSE_OK;
};

int entry138 :: getFileType(const char * file, unsigned offset)
{
	FILE * a;
	unsigned inp = 0;
	a = fopen(file, "rb");
	if(!a) return entry138::INVALID;
	fread(&inp, sizeof(unsigned), 1, a);
	fclose(a);
	return inp==EBOOT_MAGIC? entry138::IS_EBOOT: inp==ZIP_MAGIC? entry138::IS_ZIP: entry138::INVALID;
};

int entry138 :: getEbootHeader(const char * file, ebootHeader * eb, unsigned offset)
{
	FILE * a;
	a = fopen(file, "rb");
	if(!a) return 0;
	fseek(a, offset, SEEK_SET);
	fread(eb, sizeof(ebootHeader), 1, a);
	fclose(a);
	return 1;
};

int entry138 :: getType()
{
	return this->type;
};

char * entry138 :: getName()
{
	return this->name;
};

char * entry138 :: getPath()
{
	return this->path;
};

Image * entry138 :: getIcon()
{
	return this->icon;
};
