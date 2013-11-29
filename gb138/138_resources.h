#ifndef RESOURCES_HEADER
#define RESOURCES_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "138_cfg.h"
#include "gbExp.h"

extern config138 cfg;

extern "C"
{
#include "../graphics/graphics.h"
#include "../graphics/intraFont.h"
}

//#define IMAG_FILE "DATA.138"
//#define FONT_FILE "LTN9.PGF"
#define FONT_FILE "FONT.TMP"

enum IMAGE_IDS
{
	MENU_JOY = 0,
	MENU_SET = 1,
	MENU_INS = 2,
	MENU_BAR = 3,
	MENU_BSHADOW = 4,
	MENU_CIRCLE = 5,
	MENU_LISTBACK = 6,
	MENU_GAME_TILE = 7,
	MENU_DEFAULT = 8,
	MENU_MESSAGE = 9,
	MENU_BACKGROUND = 10,
	
	OSK_INPUTBACK = 11,
	OSK_KEYBOARD = 12,
	OSK_KEYBOARDBACK = 13,
	
	MENU_CUSTOMBACK = 14,
};

typedef struct
{
	unsigned size;
    char name[20];
} fileData;

class resources138
{
	public:
	
	enum RESOURCES_RETURNS
	{
		FILES_MISSING, FILES_OK, RESOURCES_ERROR, RESOURCES_OK
	};
	
	enum RESOURCES_DATA
	{
		IMAGE_COUNT = 15
	};
	
	int start();
	void clear();
	void clearFont();
	void clearImages();
	int loadTheme();
	void loadCustomBackground();
	resources138();
	
	Image * img[IMAGE_COUNT];
	intraFont * font;
	
	private:
	int loadImages(const char * file);
	int loadFont();
	unsigned search(const char * filename, const char * package, const char * extract);
};

#endif
