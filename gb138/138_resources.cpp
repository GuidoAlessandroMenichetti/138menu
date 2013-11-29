#include "138_resources.h"

resources138 :: resources138()
{
	for(int i=0;i<resources138::IMAGE_COUNT;i++) this->img[i] = NULL;
	this->font = NULL;
};

int resources138 :: start()
{
	int ret;
	if(gbExplorer::fileExist(DEFAULT_THEME)!=gbExplorer::FILE_OK) return resources138::RESOURCES_ERROR;
	
	ret = this->loadTheme();
	if(ret == resources138::FILES_OK)
		return resources138::RESOURCES_OK;
	
	this->clear();
	return resources138::RESOURCES_ERROR;
};

void resources138 :: clearFont()
{
	if(this->font)
	{
		intraFontUnload(this->font);
		intraFontShutdown();
		this->font = NULL;
	};
};

void resources138 :: clearImages()
{
	for(int i=0;i<resources138::IMAGE_COUNT;i++) 
		if(this->img[i]) 
		{
			freeImage(this->img[i]);
			this->img[i] = NULL;
		};
};

void resources138 :: clear()
{
	this->clearImages();
	this->clearFont();
	gbExplorer::deleteFile(FONT_FILE);
};

unsigned resources138 :: search(const char * filename, const char * package, const char * extract)
{
	FILE * a;
	a = fopen(package, "rb");
	if(!a) return resources138::FILES_MISSING;

	unsigned i, fileCount, offset = 0;
	fileData f;

	fread(&fileCount, sizeof(unsigned), 1, a);
	offset += sizeof(unsigned);
	for(i=0;i<fileCount+1;i++)
	{
		fread(&f, sizeof(fileData), 1, a);
		offset += sizeof(fileData);
		if(!strcmp(filename, f.name))
		{
			if(extract)
			{
				char copy_buffer[4000];
				int copy_count = 0;
				FILE * b;
				b = fopen(extract, "wb");
				memset(copy_buffer, 0, sizeof(copy_buffer));
				for(i=0;i<f.size;i+=copy_count)
				{
					if(f.size-i < sizeof(copy_buffer)) copy_count = f.size-i;
					else copy_count = sizeof(copy_buffer);

					fread(copy_buffer, copy_count, 1, a);
					fwrite(copy_buffer, copy_count, 1, b);
				};
				
				fclose(b);
			};
			
			fclose(a);
			return offset;
		}
		else
		{
			fseek(a, f.size, SEEK_CUR);
			offset += f.size;
		};
	};

	fclose(a);
	return 0;
};

int resources138 :: loadTheme()
{
	int ret;
	this->clearImages();
	this->clearFont();
	
	ret = this->loadFont();
	gbExplorer::deleteFile(FONT_FILE);
	if(ret != resources138::FILES_OK) return ret;
	
	if(strcmp(DEFAULT_THEME, cfg.custom_theme))
	{
		ret = this->loadImages(cfg.custom_theme);
		if(ret != resources138::FILES_OK)
		{
			this->clearImages();
			ret = this->loadImages(DEFAULT_THEME);
		};
	}
	else
		ret = this->loadImages(DEFAULT_THEME);
	
	return ret;
};

int resources138 :: loadFont()
{
	if(!this->search("FONT.PGF", cfg.custom_theme, "FONT.TMP"))
	{
		if(!this->search("FONT.PGF", DEFAULT_THEME, "FONT.TMP"))
			return resources138::FILES_MISSING; 
	};
	
	FILE * a;
	a = fopen(FONT_FILE, "rb");
	if(!a) return resources138::FILES_MISSING;
	fclose(a);
	intraFontInit();
	this->font = intraFontLoad(FONT_FILE , 0);
	if(this->font) return resources138::FILES_OK;
	return resources138::FILES_MISSING; 
};

int resources138 :: loadImages(const char * file)
{
	int ret = resources138::FILES_OK;

	this->img[MENU_JOY] = loadImage(file, this->search("GAME.PNG", file, NULL));
	this->img[MENU_SET] = loadImage(file, this->search("SETTINGS.PNG", file, NULL));
	this->img[MENU_INS] = loadImage(file, this->search("INSTALL.PNG", file, NULL));
	this->img[MENU_BAR] = loadImage(file, this->search("TBAR.PNG", file, NULL));
	this->img[MENU_BSHADOW] = loadImage(file, this->search("SHADOW.PNG", file, NULL));
	this->img[MENU_CIRCLE] = loadImage(file, this->search("CIRCLE.PNG", file, NULL));
	this->img[MENU_LISTBACK] = loadImage(file, this->search("LIST.PNG", file, NULL));
	this->img[MENU_GAME_TILE] = loadImage(file, this->search("TILE.PNG", file, NULL));
	this->img[MENU_DEFAULT] = loadImage(file, this->search("DEFAULT.PNG", file, NULL));
	this->img[MENU_MESSAGE] = loadImage(file, this->search("MESSAGE.PNG", file, NULL));
	this->img[MENU_BACKGROUND] = loadImage(file, this->search("BACK1.PNG", file, NULL));
	this->img[OSK_INPUTBACK] = loadImage(file, this->search("INPUT.PNG", file, NULL));
	this->img[OSK_KEYBOARD] = loadImage(file, this->search("KEYB.PNG", file, NULL));
	this->img[OSK_KEYBOARDBACK] = loadImage(file, this->search("KEYBB.PNG", file, NULL));

	this->loadCustomBackground();

	for(int i=0;i<resources138::IMAGE_COUNT-1;i++)
	{
		if(!this->img[i]) 
		{
			ret = resources138::FILES_MISSING;
			break;
		};
	};
	return ret;
};

void resources138 :: loadCustomBackground()
{
	if(this->img[MENU_CUSTOMBACK]) freeImage(this->img[MENU_CUSTOMBACK]);
	
	if(strcmp(cfg.custom_wall, DEFAULT_BACK))
		this->img[MENU_CUSTOMBACK] = loadImage(cfg.custom_wall, 0);
	else 
		this->img[MENU_CUSTOMBACK] = NULL;
};