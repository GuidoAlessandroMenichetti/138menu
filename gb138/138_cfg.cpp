#include "138_cfg.h"

int config138 :: start()
{
    if(this->load()==config138::CORRUPT)
    {
        this->makeDefault();
		this->save();
        return config138::GENERATED;
    };
    return config138::LOADED;
};

void config138 :: save()
{
	FILE * a;
	a = fopen(CFG_FILE, "wb");
	fprintf(a,
"#138Menu L3\r\n\
PATH=%s\r\n\
RENAME=%s\r\n\
LANG=%d\r\n\
APPICON=%d\r\n\
PKGICON=%d\r\n\
LOADMODE=%d\r\n\
GAMEFOLDER=%d\r\n\
USEWALL=%s\r\n\
USETHEME=%s\r\n",
	this->appPath, this->appName, this->lang, this->app_icon, this->zip_icon,
	this->load_mode, this->list_GAME, this->custom_wall, this->custom_theme);
	fclose(a);
};

void config138 :: makeDefault()
{
	this->clear();
	this->appPath = (char*)malloc(strlen(DEFAULT_PATH)+1);
	this->appName = (char*)malloc(strlen(DEFAULT_NAME)+1);
	this->custom_wall = (char*)malloc(strlen(DEFAULT_BACK)+1);
	this->custom_theme = (char*)malloc(strlen(DEFAULT_THEME)+1);

	this->lang = 0;
	this->zip_icon = 1;
	this->app_icon = 1;
	this->load_mode = 0;
	this->list_GAME = 0;
#ifdef CEF
	this->list_GAME = 1;
#endif
	strcpy(this->appPath, DEFAULT_PATH);
	strcpy(this->appName, DEFAULT_NAME);
	strcpy(this->custom_wall, DEFAULT_BACK);
	strcpy(this->custom_theme, DEFAULT_THEME);
};

int config138 :: load()
{
	FILE * a;
	a = fopen(CFG_FILE, "rb");
	if(!a) return config138::CORRUPT;

	char buffer[config138::BUFFER_SIZE], * aux;
	unsigned c=0;

	while(fgets(buffer, sizeof(buffer), a))
	{
		if(*buffer=='#') continue;

		aux = strrchr(buffer, '\r');
		if(!aux) aux = strrchr(buffer, '\n');
		
		if(aux) *aux='\0';
		if(!strncmp(buffer, "PATH=", 5))
		{
			c++;
			this->appPath = (char*)malloc(strlen(buffer+5)+3);
			memset(this->appPath, 0, (strlen(buffer+5)+3));
			sscanf(buffer+5, "%s", this->appPath);
		};
		if(!strncmp(buffer, "RENAME=", 7))
		{
			c++;
			this->appName = (char*)malloc(strlen(buffer+7)+1);
			sscanf(buffer+7, "%s", this->appName);
		}
		if(!strncmp(buffer, "USEWALL=", 8))
		{
			c++;
			this->custom_wall = (char*)malloc(strlen(buffer+8)+1);
			sscanf(buffer+8, "%s", this->custom_wall);
		};
		if(!strncmp(buffer, "USETHEME=", 9))
		{
			c++;
			this->custom_theme = (char*)malloc(strlen(buffer+9)+1);
			sscanf(buffer+9, "%s", this->custom_theme);
		};
		if(!strncmp(buffer, "LANG=", 5))
		{
			c++;
			sscanf(buffer+5, "%d", &this->lang);
		};
		if(!strncmp(buffer, "APPICON=", 8))
		{
			c++;
			sscanf(buffer+8, "%d", &this->app_icon);
		}
		if(!strncmp(buffer, "PKGICON=", 8))
		{
			c++;
			sscanf(buffer+8, "%d", &this->zip_icon);
		}
		if(!strncmp(buffer, "LOADMODE=", 9))
		{
			c++;
			sscanf(buffer+9, "%d", &this->load_mode);
		}

		if(!strncmp(buffer, "GAMEFOLDER=", 11))
		{
			c++;
			sscanf(buffer+11, "%d", &this->list_GAME);
		};
	};
	fclose(a);
	if(c!=9) return config138::CORRUPT;
	if(strncmp("ms0:/", this->appPath, 5)) return config138::CORRUPT;
	unsigned len = strlen(this->appPath);
	if(this->appPath[len-1]!='/') 
	{
		this->appPath[len]='/';
		this->appPath[len+1]='\0';
	};
	if(this->lang > config138::LANG_COUNT-1 || this->lang <0) this->lang = 0;
	if(this->app_icon > 1 || this->app_icon <0) this->app_icon = 1;
	if(this->zip_icon > 1 || this->zip_icon <0) this->zip_icon = 1;
	if(this->load_mode > 1 || this->load_mode <0) this->load_mode = 1;
	if(this->list_GAME > 1 || this->list_GAME <0) this->list_GAME = 1;
	return config138::LOADED;
};

config138 :: config138()
{
	this->appName = NULL;
	this->appPath = NULL;
	this->custom_wall = NULL;
	this->custom_theme = NULL;
};

void config138 :: clear()
{
	if(this->appName) free(this->appName);
	if(this->appPath) free(this->appPath);
	if(this->custom_wall) free(this->custom_wall);
	if(this->custom_theme) free(this->custom_theme);
};

void config138 :: setAppName(const char * str)
{
	if(this->appName) free(this->appName);
	this->appName = (char *)malloc(strlen(str)+1);
	strcpy(this->appName, str);
};

void config138 :: setInstallPath(const char * str)
{
	if(this->appPath) free(this->appPath);
	this->appPath = (char *)malloc(strlen(str)+1);
	strcpy(this->appPath, str);
};

void config138 :: setCustomTheme(const char * str)
{
	if(this->custom_theme) free(this->custom_theme);
	this->custom_theme = (char *)malloc(strlen(str)+1);
	strcpy(this->custom_theme, str);
};

void config138 :: setBackground(const char * str)
{
	if(this->custom_wall) free(this->custom_wall);
	this->custom_wall = (char *)malloc(strlen(str)+1);
	strcpy(this->custom_wall, str);
};

