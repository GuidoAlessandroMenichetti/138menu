#include "138_entry.h"

entry138 :: entry138()
{
	this->name = NULL;
	this->path = NULL;
	this->icon = NULL;
	this->type = entry138::UNKNOWN;
	this->patched = 0;
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
#ifdef CEF
	gbExplorer::deleteFile(TEMP_PARAM);
	gbExplorer::deleteFile(TEMP_ICON);
#endif
	return entry138::LOAD_OK;
};

int entry138 :: load(const char * file)
{
	if(gbExplorer::fileExist(file)!=gbExplorer::FILE_OK) return entry138::LOAD_ERROR;

	ebootHeader ebootData;
	unsigned zipped_offset = 0, icon_offset = 0;
	char * parse_file = (char *)file, * icon_file = (char *)file;
	int get_icon, file_type = entry138::getFileType(file, 0);

	if(file_type==entry138::INVALID) return entry138::LOAD_ERROR;
	else if(file_type==entry138::IS_EBOOT) this->type=entry138::HOMEBREW;
	else if(file_type==entry138::IS_ISO) this->type=entry138::ISO;
	else if(file_type==entry138::IS_CSO) this->type=entry138::CSO;
	else this->type=entry138::INSTALLER_HB;

	memset(&ebootData, 0, sizeof(ebootHeader));
	get_icon = this->type == entry138::INSTALLER_HB? cfg.zip_icon: cfg.app_icon;

	if(this->type == entry138::INSTALLER_HB && cfg.load_mode==config138::MODE_FAST) //Fast load mode
	{
		parse_file = (char *)malloc(strlen(file)+11);
		if(!parse_file) return entry138::LOAD_ERROR;
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
		if(gbExplorer::fileExist(icon_file)!=gbExplorer::FILE_OK) get_icon = 0;
	}
	else if(this->type != entry138::ISO && this->type != entry138::CSO) //Accurate mode and homebrew load
	{
		if(this->type == entry138::INSTALLER_HB) 
		{
			zipped_offset = gbZip::getEbootOffset(file); //Look for the eboot offset inside the ZIP file
			if(!zipped_offset) return entry138::LOAD_ERROR;
		};
		if(!entry138::getEbootHeader(file, &ebootData, zipped_offset)) return entry138::LOAD_ERROR;
		get_icon = get_icon && ebootData.icon0!=ebootData.icon1;
		icon_offset = ebootData.icon0+zipped_offset;
	};

#ifdef CEF
	if(this->type == entry138::ISO)
	{
		gbIso iso;
		if(iso.open(file)==gbIso::ISO_ERROR) return entry138::LOAD_ERROR; 
		zipped_offset = iso.findFile("PSP_GAME/PARAM.SFO");
		icon_offset = iso.findFile("PSP_GAME/ICON0.PNG");
		this->patched = iso.findFile("SYSDIR/EBOOT.OLD");
		if(icon_offset) get_icon = 1;
		iso.close();
		if(!zipped_offset)
			return entry138::LOAD_ERROR;
	}
	else if(this->type == entry138::CSO)
	{
		gbCso cso;
		if(cso.open(file)!=gbCso::GBCSO_OK)
		{
			cso.clear();
			return entry138::LOAD_ERROR;
		};
		cso.getPrimaryVolumeDescriptor();
		
		unsigned cso_SFO_size, cso_SFO_off = cso.findFile("PSP_GAME/PARAM.SFO", &cso_SFO_size), dummy;
		unsigned cso_icon0_size, cso_icon0_off = cso.findFile("PSP_GAME/ICON0.PNG", &cso_icon0_size);
		this->patched = cso.findFile("SYSDIR/EBOOT.OLD", &dummy);
		
		if(cso_SFO_off) 
		{
			cso.extractFile(TEMP_PARAM, cso_SFO_off, cso_SFO_size);
			parse_file = (char *)TEMP_PARAM;
		}
		else
		{
			cso.clear();
			return entry138::LOAD_ERROR;
		};
		
		if(cso_icon0_off)
		{
			cso.extractFile(TEMP_ICON, cso_icon0_off, cso_icon0_size);
			icon_file = (char *)TEMP_ICON;
			get_icon = 1;
		}
		else get_icon = 0;
		cso.clear();
	};
#endif
	
	int ret = this->parseSfo(parse_file, ebootData.param+zipped_offset); //Load entry name
	if(get_icon && ret!=entry138::PARSE_ERROR) //Load entry icon
	{
		this->icon = loadImage(icon_file, icon_offset);
		if(this->icon)
		{
			if(this->icon->imageWidth > MAX_IMG_WIDTH) this->icon->imageWidth = MAX_IMG_WIDTH;
			if(this->icon->imageHeight > MAX_IMG_HEIGHT) this->icon->imageHeight = MAX_IMG_HEIGHT;
		};
	};
	if(parse_file!=(char *)file && parse_file!=(char *)TEMP_PARAM) free(parse_file);
	if(icon_file!=(char *)file && icon_file!=(char *)TEMP_ICON) free(icon_file);
	
	if(ret==entry138::PARSE_ERROR) return entry138::LOAD_ERROR;

	this->path = (char *)malloc(strlen(file)+1);
	if(this->path) strcpy(this->path, file);
	else return entry138::LOAD_ERROR;

	//Various string fixes
	normalize(this->name);
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


		if(this->type!=entry138::CSO && this->type!=entry138::ISO && !strcmp("CATEGORY", tmp))
		{
			unsigned cat = 0;
			fseek(a, sfo.data_table + index.data_offset + offset, 0);
			fread(&cat, sizeof(unsigned), 1, a);
			if(cat==POPS_CATEG) this->type = entry138::POPS;
			else if(cat==PSN_CATEG) this->type = entry138::PSN;
#ifndef CEF
			if(this->type!=entry138::HOMEBREW && this->type != entry138::INSTALLER_HB)
				return entry138::PARSE_ERROR;
#endif
		};
	
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
		fseek(a, seek, SEEK_SET);
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
	if(inp!=EBOOT_MAGIC && inp!=ZIP_MAGIC && inp!=CISO_MAGIC)
	{
		fseek(a, 0x8000, SEEK_SET);
		fread(&inp, sizeof(unsigned), 1, a);
	};
	fclose(a);
	return 	inp==EBOOT_MAGIC? entry138::IS_EBOOT: inp==ZIP_MAGIC? entry138::IS_ZIP: 
			inp==ISO_MAGIC? entry138::IS_ISO: inp==CISO_MAGIC? entry138::IS_CSO: entry138::INVALID;
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

unsigned entry138 :: isPatched()
{
	return this->patched;
};

int entry138 :: sort_entry138(const void * a, const void * b)
{
	entry138 * c1 = (entry138 *)a;
	entry138 * c2 = (entry138 *)b;
	int prior1 = 0, prior2 = 0;
	if(c1->getType()==entry138::HOMEBREW) prior1 = 3;
	if(c1->getType()==entry138::POPS) prior1 = 2;
	if(c1->getType()==entry138::PSN) prior1 = 1;
	if(c1->getType()==entry138::ISO || c1->getType()==entry138::CSO) prior1 = 0;
	
	if(c2->getType()==entry138::HOMEBREW) prior2 = 3;
	if(c2->getType()==entry138::POPS) prior2 = 2;
	if(c2->getType()==entry138::PSN) prior2 = 1;
	if(c2->getType()==entry138::ISO || c2->getType()==entry138::CSO) prior2 = 0;

	return prior2-prior1;	
};

void entry138 :: setPath(const char * path)
{
	if(this->path) free(this->path);
	this->path = (char *)malloc(strlen(path)+1);
	strcpy(this->path, path);
};

void entry138 :: setName(const char * name)
{
	if(this->name) free(this->name);
	this->name = (char *)malloc(strlen(name)+1);
	strcpy(this->name, name);
};

void entry138 :: setType(int type)
{
	this->type = type;
};