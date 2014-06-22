#ifndef MENU_HEADER
#define MENU_HEADER

#include "common.h"
#include "resources.h"
#include "entry.h"
#include "cfg.h"
#include "osk.h"
#include "ctrl.h"
#include "explorer.h"
#include "filelist.h"
#include "progress.h"

extern "C"
{
#include "graphics/graphics.h"
#include "graphics/intraFont.h"
};

#define RD 1.570796325 

extern config138 cfg;
extern resources138 res;
extern input138 ctrl;

extern char app_start_path[256];
extern int app_start_type;
extern int app_start_extra;

class menu138
{
	public:

	enum E_ACTIONS 
	{
		INSTALL_SELECTED, 
		DELETE_SELECTED, 
		FORMAT_HBFOLDER, 
		RESTORE_DEFAULTS
	};
	
	enum E_CONSTANTS
	{
		SETTINGS_COUNT = 11
	};

	enum E_ORDER 
	{
		SETTINGS = 0, 
		GAMES = 1, 
		INSTALL = 2, 
		EXPLORER = 3
	};

	menu138();
	int start();
	
	void set_app_path(const char * path);
	void load_app(const entry138 * entry);
	void drawImages();
	void drawText();
	
	static void clrscr();
	static void printText(const char * text, int x, int y, unsigned color, unsigned shadow, float size);
	static void printImg(Image * img, int x, int y);
	static void flip();
	
	void updateGameList();
	void updateInstallerList();
	
	osk138 osk;

	private:

	entry138 * actualEntry(int menu, int index);
	
	int fillList(vector<entry138 *> & list, const char * start, const char * forced_name, int append, int ONLY_ISOS = 0);
	void clean_list(vector<entry138 *> & list);
	
	void settingsCtrl();
	
	int get_items_on_list();
	
	void check();
	void roll(int dir);
	
	void animCheck();
	inline int animating() const;
	void animation_control(int * flag, float * status, float limit, float step);
	inline void start_animation(int * flag, float * limit, int direction, float destination);
	
	//message related
	void setMessage(const char * caption, const char * cat1, const char * cat2, int action_code);
	void doAction(int action_code);

	bool active;
	bool main_menu;
	
	int menu, list_option, offset, //list data
		circle_move, rotation_flag, list_move, title_move, //animation flags
		message_active, message_action, message_option; //message information
		
	float rotateTo, rotation, //rotation data
		circleTo, circlex, circley, //circle animation data
		listTo, listx, //list animation data
		titleTo, titley; //title animation data

	int theme_current, theme_selected;
	int wall_current, wall_selected;
	
	char message[MAX_MESSAGE];
	char * exploitPath;
	int DEBUG_LOAD;
	
	vector<entry138 *> game_list; 
	vector<entry138 *> installer_list;
	filelist138 wallpapers;
	filelist138 themes;
	explorer138 explorer;
};

#endif
