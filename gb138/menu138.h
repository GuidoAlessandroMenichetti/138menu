#ifndef MENU_HEADER
#define MENU_HEADER

#include <pspdebug.h>
#include <pspkernel.h>
#include <pspgum.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <math.h>

#include "lang.h"
#include "resources_138.h"
#include "138_entry.h"
#include "gbAlloc.h"
#include "138_cfg.h"
#include "138_osk.h"
#include "gbCtrl.h"

extern "C"
{
#include "../graphics/graphics.h"
#include "../graphics/intraFont.h"
};

#define __printf 		pspDebugScreenPrintf
#define PI 				3.14159265
#define RD 				2.093333
#define SIZE_LITTLE 	0.51f
#define	SIZE_BIG 		0.7f
#define MAX_MESSAGE		100
#define MAX_EBOOT_NAME	9
#define MAX_PATH_NAME	4	
#define INSTALLERS_PATH "ms0:/PSP/SAVEDATA/"
#define ZIP_NAME 		"INSTALL.ZIP"
#define CREDITS 		"138Menu by GUIDOBOT"


extern config138 cfg;
extern resources138 res;
extern gbControl ctrl;

class menu138
{
	public:
	
	enum ACTIONS {INSTALL_SELECTED, DELETE_SELECTED, FORMAT_HBFOLDER};
	enum MENU_RETURNS {MENU_GRAPHIC_ERROR, MENU_OK};
	enum MENU_CONSTANTS{SETTINGS_COUNT = 9, MAX_ON_SCREEN = 14, THREAD_DELAY = 1000};
	enum FONT_COLORS{BLACK = 0xFF000000, WHITE = 0xFFFFFFFF, GREY  = 0xFFCCCCCC, YELLOW = 0xFF00FFFF};
	enum ORDER {SETTINGS = 0, GAMES = 1, INSTALL = 2};
	
	menu138();
	int start();
	static void flip();
	static void printText(const char * text, int x, int y, unsigned color, unsigned shadow, float size);
	static void printImg(Image * img, int x, int y);
	void setEbootAddress(char * address);
	static void startGl();
	static void killGl();
	static void clrscr();
	
	private:
	
	entry138 * actualEntry(int menu, int index);
	void loadEBOOT(const char * path);
	void updateGameList();
	void updateInstallerList();
	int fillList(gbAlloc * list, const char * start, const char * forced_name);
	void deleteItemFromSD(entry138 * entry);
	void settingsCtrl();
	void animCheck();
	void waitButtonsPressed();
	
	void drawImages();
	void drawText();
	void check();
	void roll(int dir);
	int animating();
	
	void setMessage(const char * caption, const char * cat1, const char * cat2, int action_code);
	void doAction(int action_code);

	gbAlloc game_list, installer_list;
	osk138 osk;
	int active,
		osk_active,
		hb_count, zip_count,
		circlex, circley, circleTo, circle_move, rotation_flag,
		list_move, listTo, listx,
		title_move, titley, titleTo,
		menu, op, offset, list_option,
		message_active, message_action, message_option,
		press_time;
	double rotateTo, rotation;
	char * ebootAddress, message[MAX_MESSAGE];
};

#endif
