#include "resources_138.h"

resources138 :: resources138()
{
	for(int i=0;i<resources138::IMAGE_COUNT;i++) this->img[i] = NULL;
	this->font = NULL;
};

int resources138 :: start()
{
	if(this->loadImages() != resources138::FILES_OK || this->loadFont() != resources138::FILES_OK)
	{
		this->clear();
		return resources138::RESOURCES_ERROR;
	};
	return resources138::RESOURCES_OK;
};

void resources138 :: clear()
{
	for(int i=0;i<resources138::IMAGE_COUNT;i++) if(this->img[i]) freeImage(this->img[i]);
	if(this->font)
	{
		intraFontUnload(this->font);
		intraFontShutdown();
	};
};

unsigned resources138 :: search(const char * filename)
{
	FILE * a;
	a = fopen(IMAG_FILE, "rb");
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

int resources138 :: loadFont()
{
	FILE * a;
	a = fopen(FONT_FILE, "rb");
	if(!a) return resources138::FILES_MISSING;
	fclose(a);
	intraFontInit();
	this->font = intraFontLoad(FONT_FILE , 0);
	if(this->font) return resources138::FILES_OK;
	return resources138::FILES_MISSING; 
};

int resources138 :: loadImages()
{
	int ret = resources138::FILES_OK;

	this->img[MENU_JOY] = loadImage(IMAG_FILE, this->search("GAME.PNG"));
	this->img[MENU_SET] = loadImage(IMAG_FILE, this->search("SETTINGS.PNG"));
	this->img[MENU_INS] = loadImage(IMAG_FILE, this->search("INSTALL.PNG"));
	this->img[MENU_BAR] = loadImage(IMAG_FILE, this->search("TBAR.PNG"));
	this->img[MENU_BSHADOW] = loadImage(IMAG_FILE, this->search("SHADOW.PNG"));
	this->img[MENU_CIRCLE] = loadImage(IMAG_FILE, this->search("CIRCLE.PNG"));
	this->img[MENU_LISTBACK] = loadImage(IMAG_FILE, this->search("LIST.PNG"));
	this->img[MENU_GAME_TILE] = loadImage(IMAG_FILE, this->search("TILE.PNG"));
	this->img[MENU_DEFAULT] = loadImage(IMAG_FILE, this->search("DEFAULT.PNG"));
	this->img[MENU_MESSAGE] = loadImage(IMAG_FILE, this->search("MESSAGE.PNG"));
	this->img[MENU_BACKGROUND] = loadImage(IMAG_FILE, this->search("BACK1.PNG"));
	this->img[OSK_INPUTBACK] = loadImage(IMAG_FILE, this->search("INPUT.PNG"));
	this->img[OSK_KEYBOARD] = loadImage(IMAG_FILE, this->search("KEYB.PNG"));
	this->img[OSK_KEYBOARDBACK] = loadImage(IMAG_FILE, this->search("KEYBB.PNG"));

	if(strcmp(cfg.custom_wall, "DEFAULT")) this->img[MENU_CUSTOMBACK] = loadImage(cfg.custom_wall, 0);
	else this->img[MENU_CUSTOMBACK] = NULL;

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