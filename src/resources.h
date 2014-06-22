#ifndef RESOURCES_HEADER
#define RESOURCES_HEADER

#include "common.h"
#include "cfg.h"
#include "explorer.h"

extern config138 cfg;

extern "C"
{
#include "graphics/graphics.h"
#include "graphics/intraFont.h"
}

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
	
	MENU_EXP = 14,
	EXPLORER_FOLDER = 15,
	EXPLORER_FILE = 16,
	EXPLORER_CHECK = 17,
	EXPLORER_TOOLS = 18,
	PROGRESS_BACK = 19,
	PROGRESS_BAR = 20,
	MENU_CUSTOMBACK = 21
};

typedef struct
{
	unsigned size;
    char name[20];
} fileData;

#define IMAGE_COUNT 22

class resources138
{
	public:
	
	resources138();
	int start();
	void clear();
	
	int loadTheme();
	void loadCustomBackground();
	
	int start_theme();
	int reload_theme(const char * path);
	
	Image * img[IMAGE_COUNT];
	intraFont * font;
	
	private:
	
	void clearFont();
	void clearImages();
	
	int loadImages(const char * file);
	int loadFont(const char * path);
	
	unsigned search(const char * filename, const char * package, const char * extract = NULL);
};

#endif
