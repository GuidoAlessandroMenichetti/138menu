#ifndef CONFIG_HEADER_138
#define CONFIG_HEADER_138

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../config.h"

#define DEFAULT_BACK "DEFAULT"

class config138
{
	public:

	enum ENUM_CONFIG_MODES {MODE_ACCURATE = 0, MODE_FAST = 1};
	enum ENUM138_CONFIG {LOADED, GENERATED, CORRUPT, BUFFER_SIZE = 200, LANG_COUNT = 6};

	config138();
	int start();
	void clear();
	int load();
	void save();
	void makeDefault();
	void setInstallPath(const char * str);
	void setAppName(const char * str);
	void setCustomTheme(const char * str);
	void setBackground(const char * str);
	
	char * appPath;
	char * appName;
	int lang;
	int zip_icon;
	int app_icon;
	int load_mode;
	int use_wall;
	char * custom_wall;
	char * custom_theme;
	int list_GAME;   
};

#endif
