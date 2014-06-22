#include "cfg.h"

config138 :: config138()
{
	appName = NULL;
	appPath = NULL;
	custom_wall = NULL;
	custom_theme = NULL;
};

void config138 :: clear()
{
	//free pending strings
	if(appName) 
		delete [] appName;
	if(appPath) 
		delete [] appPath;
	if(custom_wall)
		delete [] custom_wall;
	if(custom_theme)
		delete [] custom_theme;
};

int config138 :: start()
{
	//try to load config data
    if(load() == CORRUPT)
    {
		//file is corrupt, generate one and save it
        makeDefault(); 
		save();
		
        return GENERATED;
    };
	
    return LOADED;
};

int config138 :: load()
{
	//try to open config file
	FILE * stream = fopen(CFG_FILE, "rb");
	if(!stream) 
		return CORRUPT; //couldn't open it

	char buffer[256], * aux;
	
	unsigned count = 0;

	while(fgets(buffer, sizeof(buffer), stream))
	{
		if(* buffer == '#') //skip comments 
			continue;

		//remove extra chars
		aux = strrchr(buffer, '\r'); //find '\r' char (windows)
		if(!aux) 
			aux = strrchr(buffer, '\n'); //find new line char
		
		if(aux) //remove that extra char
			* aux = '\0';
			
		//parse entries
		aux = strchr(buffer, '='); //find '=' char
		if(!aux)
			continue; //not found, ignore this line
		
		aux++;
		
		while(* aux == ' ') //skip whitespaces
			aux++;
		
		if(!strncmp(buffer, "PATH", 4))
			setInstallPath(aux); //apps path
		else if(!strncmp(buffer, "RENAME", 6))
			setAppName(aux); //apps name
		else if(!strncmp(buffer, "USEWALL", 7))
			setBackground(aux); //wallpaper
		else if(!strncmp(buffer, "USETHEME", 8))
			setCustomTheme(aux); //theme
		else if(!strncmp(buffer, "LANG", 4))
			sscanf(aux, "%d", &lang); //language
		else if(!strncmp(buffer, "APPICON", 7))
			sscanf(aux, "%d", &app_icon); //show app icon
		else if(!strncmp(buffer, "PKGICON", 7))
			sscanf(aux, "%d", &zip_icon); //show installer icon
		else if(!strncmp(buffer, "LOADMODE", 8))
			sscanf(aux, "%d", &load_mode); //load mode
		else if(!strncmp(buffer, "GAMEFOLDER", 10))
			sscanf(aux, "%d", &list_GAME); //list /game/ homebrew
		else
			continue; //data was not valid
			
		count++;
	};
	
	//close file
	fclose(stream);
	
	//check all needed entries were readen
	if(count != 9 || strncmp("ms0:/", appPath, 5)) 
		return CORRUPT;
		
	//append a '/' char to app path if needed
	unsigned len = strlen(appPath);
	if(appPath[len - 1] != '/') 
		strcat(appPath, "/");
	
	//check and fix invalid values
	if(lang > LANG_COUNT - 1 || lang < 0) 
		lang = 0;
	if(app_icon > 1 || app_icon < 0) 
		app_icon = 1;
	if(zip_icon > 1 || zip_icon < 0) 
		zip_icon = 1;
	if(load_mode > 1 || load_mode < 0) 
		load_mode = 1;
	if(list_GAME > 1 || list_GAME < 0) 
		list_GAME = 1;
		
	return LOADED;
};

void config138 :: save()
{
	FILE * stream = fopen(CFG_FILE, "wb"); //create config file
	
	//print config text
	fprintf(stream,
"#138Menu %s\r\n\
PATH = %s\r\n\
RENAME = %s\r\n\
LANG = %d\r\n\
APPICON = %d\r\n\
PKGICON = %d\r\n\
LOADMODE = %d\r\n\
GAMEFOLDER = %d\r\n\
USEWALL = %s\r\n\
USETHEME = %s\r\n",
	VERSION, appPath, appName, lang, app_icon, zip_icon,
	load_mode, list_GAME, custom_wall, custom_theme);
	
	fclose(stream);	
};

void config138 :: makeDefault()
{
	//set default paths and names
	setAppName(DEFAULT_NAME);
	setInstallPath(DEFAULT_PATH);
	setBackground(DEFAULT_BACK);
	setCustomTheme(DEFAULT_THEME);

	//default values
	lang = 0;
	zip_icon = 1;
	app_icon = 1;
	load_mode = 0;
	list_GAME = 0;
	
#ifdef CEF

	list_GAME = 1;
	
#endif
};

void config138 :: setAppName(const char * str)
{
	//allocate and copy new data
	char * new_name = new char[strlen(str) + 2];
	strcpy(new_name, str);
	
	if(appName) //delete previous name
		delete [] appName;
	
	appName = new_name;
};

void config138 :: setInstallPath(const char * str)
{	
	//allocate and copy new data
	char * new_path = new char[strlen(str) + 1];
	strcpy(new_path, str);
	
	if(appPath) //delete previous path
		delete [] appPath;
	
	appPath = new_path;
};

void config138 :: setCustomTheme(const char * str)
{
	//allocate and copy new data
	char * new_path = new char[strlen(str) + 1];
	strcpy(new_path, str);
	
	if(custom_theme) //delete previous theme path
		delete [] custom_theme;
	
	custom_theme = new_path;
};

void config138 :: setBackground(const char * str)
{
	//allocate and copy new data
	char * new_path = new char[strlen(str) + 1];
	strcpy(new_path, str);
	
	if(custom_wall) //delete previous theme path
		delete [] custom_wall;
		
	custom_wall = new_path;
};

