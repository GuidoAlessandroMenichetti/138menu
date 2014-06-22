#include "resources.h"

resources138 :: resources138()
{
	//initialize pointers
	for(int i = 0; i < IMAGE_COUNT; i++) 
		img[i] = NULL;
		
	font = NULL;
};

int resources138 :: start()
{
	if(!explorer138::file_exists(DEFAULT_THEME)) //check for DATA.138 file
		return 0;

	if(!start_theme()) //load theme
	{
		clear(); //failed
		return 0;
	};
	
	return 1;
};

void resources138 :: clearFont()
{
	//clear font data
	if(font)
	{
		intraFontUnload(font);
		intraFontShutdown();
		font = NULL;
	};
};

void resources138 :: clearImages()
{
	for(int i = 0; i < IMAGE_COUNT; i++)
	{
		if(img[i]) 
		{
			freeImage(img[i]); //release image data
			img[i] = NULL;
		};
	};
};

void resources138 :: clear()
{
	clearImages(); //clear images
	clearFont(); //clear font
};

unsigned resources138 :: search(const char * filename, const char * package, const char * extract)
{

	//try to open file
	SceUID file = sceIoOpen(package, PSP_O_RDONLY, 0777); 

	if(file < 0)
		return 0;
		
	//get file count
	unsigned file_count;
	sceIoRead(file, &file_count, sizeof(unsigned));

	file_count++;
	
	bool finished = false;
	unsigned result = 0;
	
	//start reading entries
	while(file_count && finished == false)
	{
		//read file data
		fileData file_data;
		sceIoRead(file, &file_data, sizeof(fileData));
	
		if(!strcmp(filename, file_data.name)) //file found
		{
			result = sceIoLseek(file, 0, SEEK_CUR); //store offset
			
			if(extract) //file extract
			{
				SceUID output = sceIoOpen(extract, PSP_O_WRONLY | PSP_O_CREAT, 0777); //try to open output
				
				if(output >= 0)
				{
					explorer138::copy_bytes(file, output, file_data.size); //copy bytes
					sceIoClose(output); //close file
				};
			};
			
			finished = true;
		};
	
		sceIoLseek(file, file_data.size, SEEK_CUR); //skip file data bytes
		
		file_count--;
	};
	
	sceIoClose(file);

	return result;
};

int resources138 :: reload_theme(const char * path)
{
	clear();
	
	if(!loadImages(path))
		return 0;
		
	if(!loadFont(path))
		return 0;
	
	cfg.setCustomTheme(path);
	
	return 1;
};

int resources138 :: start_theme()
{
	if(!reload_theme(cfg.custom_theme))
		return reload_theme(DEFAULT_THEME);
	
	return 1;
};

int resources138 :: loadFont(const char * path)
{
	if(!search("FONT.PGF", path, TEMPORAL_FONT)) //try to extract font from custom theme
		return 0; 

	//check that file was extracted correctly
	if(!explorer138::file_exists(TEMPORAL_FONT))
		return 0;
	
	intraFontInit(); //start intrafont
	font = intraFontLoad(TEMPORAL_FONT , 0); //try to load font
	
	explorer138::delete_file(TEMPORAL_FONT); //delete font file
	
	return font != NULL;
};

int resources138 :: loadImages(const char * file)
{
	//load files from container
	img[MENU_JOY] = loadImage(file, search("GAME.PNG", file));
	img[MENU_SET] = loadImage(file, search("SETTINGS.PNG", file));
	img[MENU_INS] = loadImage(file, search("INSTALL.PNG", file));
	img[MENU_BAR] = loadImage(file, search("TBAR.PNG", file));
	img[MENU_BSHADOW] = loadImage(file, search("SHADOW.PNG", file));
	img[MENU_CIRCLE] = loadImage(file, search("CIRCLE.PNG", file));
	img[MENU_LISTBACK] = loadImage(file, search("LIST.PNG", file));
	img[MENU_GAME_TILE] = loadImage(file, search("TILE.PNG", file));
	img[MENU_DEFAULT] = loadImage(file, search("DEFAULT.PNG", file));
	img[MENU_MESSAGE] = loadImage(file, search("MESSAGE.PNG", file));
	img[MENU_BACKGROUND] = loadImage(file, search("BACK1.PNG", file));
	img[OSK_INPUTBACK] = loadImage(file, search("INPUT.PNG", file));
	img[OSK_KEYBOARD] = loadImage(file, search("KEYB.PNG", file));
	img[OSK_KEYBOARDBACK] = loadImage(file, search("KEYBB.PNG", file));
	img[MENU_EXP] = loadImage(file, search("EXPLORER.PNG", file));
	img[EXPLORER_FOLDER] = loadImage(file, search("FOLDER.PNG", file));
	img[EXPLORER_FILE] = loadImage(file, search("FILE.PNG", file));
	img[EXPLORER_CHECK] = loadImage(file, search("CHECK.PNG", file));
	img[EXPLORER_TOOLS] = loadImage(file, search("TOOLS.PNG", file));
	img[PROGRESS_BACK] = loadImage(file, search("PROGBACK.PNG", file));
	img[PROGRESS_BAR] = loadImage(file, search("PROGBAR.PNG", file));
	
	loadCustomBackground();

	//check that all images are loaded
	for(int i = 0; i < IMAGE_COUNT - 1; i++)
	{
		if(!img[i]) 
			return 0; //this file didnt load
	};
	
	return 1;
};

void resources138 :: loadCustomBackground()
{
	if(img[MENU_CUSTOMBACK]) //free previous custom background
		freeImage(img[MENU_CUSTOMBACK]);
	
	if(strcmp(cfg.custom_wall, DEFAULT_BACK)) //is custom wall different from default one?
		img[MENU_CUSTOMBACK] = loadImage(cfg.custom_wall, 0);
	else //no custom background
		img[MENU_CUSTOMBACK] = NULL;
};