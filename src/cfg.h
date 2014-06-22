#ifndef CONFIG_HEADER_138
#define CONFIG_HEADER_138

#include "common.h"

class config138
{
	public:

	enum E_CONFIG_RETURNS
	{
		LOADED = 1, 
		GENERATED = 2, 
		CORRUPT = 0
	};

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
