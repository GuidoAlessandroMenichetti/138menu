#include "menu.h"

menu138 :: menu138()
{
	active = false;
	exploitPath = NULL;
	main_menu = true;
	menu = GAMES;
	rotateTo = rotation = rotation_flag = 0;
	circlex = CIRCLE_INITIAL_X; 
	circley = 100;
	circle_move = 0;
	list_move = listTo = 0;
	listx = 500;
	title_move = titley = titleTo = 0;
	list_option = offset = 0;
	message_active = 0;
	DEBUG_LOAD = 0;
};

int menu138 :: start()
{
	ctrl.update();
	if(ctrl.get()->rtrigger) //if R is pressed activate debug load mode
		DEBUG_LOAD = 1;
	
	//create the game category list
	pspDebugScreenPrintf("Creating app list...\n");
	updateGameList();
	
	//create the installer category list
	pspDebugScreenPrintf("Creating installer list...\n");
	updateInstallerList();
	
	//load theme files
	pspDebugScreenPrintf("Creating theme list...\n");
	themes.load(exploitPath, "138");
	
	theme_current = themes.find_file(cfg.custom_theme, 1);
	if(theme_current < 0)
		theme_current = themes.find_file(DEFAULT_THEME, 1);;
		
	theme_selected = theme_current;
	
	//load wallpapers
	pspDebugScreenPrintf("Creating wallpaper list...\n");
	wallpapers.load(exploitPath, "PNG");
	
	wallpapers.delete_file("ICON0.PNG", 1);
	
	wall_current = wallpapers.find_file(cfg.custom_wall, 1);
	wall_selected = wall_current;
	
	//load default path in explorer
	pspDebugScreenPrintf("Creating explorer at %s...\n", EXPLORER_PATH);
	explorer.browse(EXPLORER_PATH);

	DEBUG_LOAD = 0; //disable debug loading
	active = true;
	
	while(active == true) //start main loop
	{
		//check button input and animations state
		ctrl.update();
		check();
		
		clrscr(); //clear screen
		
		//draw everything
		guStart();
		drawImages();
		drawText();
		flip(); //swap buffers
	};

	//clear resources
	game_list.clear();
	installer_list.clear();
	explorer.clear();
	wallpapers.clear();
	themes.clear();
	
	//destroy graphics
	disableGraphics();
	
	//delete stored app path
	if(exploitPath) 
		delete [] exploitPath;

	return 1;
};

void menu138 :: set_app_path(const char * path)
{
	//store app path
	exploitPath = new char[strlen(path) + 1];
	strcpy(exploitPath, path);
};

void menu138 :: load_app(const entry138 * entry)
{
	strcpy(app_start_path, entry->getPath());
	app_start_type = entry->getType();
	app_start_extra = entry->getExtra();
	
	active = false;
};

void menu138 :: clrscr()
{
	clearScreen(BLACK);
};

void menu138 :: flip()
{
	//finish and sync GU
	sceGuFinish();
	sceGuSync(0,0);
	
	//wait for VBlank
	sceDisplayWaitVblankStart(); 
	flipScreen();
	
	//writeback cache
	sceKernelDcacheWritebackInvalidateAll();
	
	//delay thread
	sceKernelDelayThread(THREAD_DELAY);
};

void menu138 :: printText(const char * text, int x, int y, unsigned color, unsigned shadow, float size)
{
	if(!text)
		return;
		
	intraFontSetStyle(res.font, size, color, shadow, 0.0f, 0); //set text style
	intraFontPrint(res.font, x, y, text); //print text
};

void menu138 :: printImg(Image * img, int x, int y)
{	
	if(!img)	
		return;

	//blit image
	blitAlphaImageToScreen(0, 0, img->imageWidth, img->imageHeight, img, x, y);
};

void menu138 :: updateGameList()
{
	clean_list(game_list); //clear previous entries
	
#ifdef CEF

	//create ms0:/ISO folder if it's not there
	explorer138::create_folder(ISO_PATH);

	//load ISO/CSO files from ms0:/ISO folder
	fillList(game_list, ISO_PATH, NULL, 0, 0);
	
	//load ISO/CSO from exploit(savedata)/app path
	if(exploitPath) 
		fillList(game_list, exploitPath, NULL, 0, 1);

#endif

	//create homebrew folder if it does not exist:
	explorer138::create_folder(cfg.appPath);
		
	//check to load /GAME content only once:
	if(cfg.list_GAME && strcmp(cfg.appPath, "ms0:/PSP/GAME/")) 
		fillList(game_list, "ms0:/PSP/GAME/", NULL, 1);
	
	//add homebrew in located in app path
	fillList(game_list, cfg.appPath, NULL, 1);
	
	//sort games
	sort(game_list.begin(), game_list.end(), entry138::entry_sort);
	
	if(menu != SETTINGS) //go to list start
		list_option = offset = 0;
};

void menu138 :: updateInstallerList()
{
	clean_list(installer_list); //clear previous installer entries

	//add installers to list (only from INSTALLERS_PATH subdirectories)
	fillList(installer_list, INSTALLERS_PATH, ZIP_NAME, 1);
	
	if(menu != SETTINGS) //go to list start
		list_option = offset = 0;
};

void menu138 :: setMessage(const char * caption, const char * cat1, const char * cat2, int action_code)
{
	//build message
	strcpy(message, caption);
	if(cat1) 
		strcat(message, cat1);
	if(cat2) 
		strcat(message, cat2);
		
	message_option = 1;
	message_active = 1;
	message_action = action_code;
};

void menu138 :: doAction(int action_code)
{
	entry138 * entry = actualEntry(menu, list_option);
	
	switch(action_code)
	{
		case INSTALL_SELECTED:
		{
			gbzip installer;
			
			//setup progress screen
			explorer138::copy_active = true;
			start_progress_thread(&installer.current_file, &explorer138::copy_progress, &explorer138::copy_total, &explorer138::copy_active);
		
			//try to unzip
			installer.extract(entry->getPath(), cfg.appPath, cfg.appName);
			
			//terminate progress thread
			explorer138::copy_active = false;
			finish_progress_thread();
			
			updateGameList();

			break;
		}
		case DELETE_SELECTED:
		{
			char * entry_path = entry->getPath();
		
			if(entry->getType() == CSO || entry->getType() == ISO)
				explorer138::delete_file(entry->getPath()); //delete a single file
			else //delete a folder
			{
				char * get_folder = strrchr(entry_path, '/'); //find last '/'
				if(get_folder)
				{
					*(get_folder + 1) = 0x0; //overwrite with 0x0 and delete
					explorer138::delete_folder(entry_path);
				};
			};
			
			//update list
			if(menu == GAMES)
				updateGameList();
			else
				updateInstallerList();
			
			break;
		}
		case FORMAT_HBFOLDER:
		
			if(strcmp(cfg.appPath, "ms0:/PSP/GAME/")) //never format /GAME/
			{
				explorer138::delete_folder(cfg.appPath); //delete folder
				explorer138::create_folder(cfg.appPath); //create it again
				updateGameList(); //update list
			};

			break;
			
		case RESTORE_DEFAULTS:
		
			//restore default theme			
			theme_current = themes.find_file(DEFAULT_THEME, 1);
			theme_selected = theme_current;
			
			cfg.setCustomTheme(themes.get(theme_current)->name);
			res.start_theme();
		
			//restore wallpaper
			wall_current = wall_selected = -1;
			cfg.setBackground(DEFAULT_BACK);
			res.loadCustomBackground();
		
			cfg.makeDefault();
			cfg.save();
	};
	
	explorer.browse(EXPLORER_PATH);
};

entry138 * menu138 :: actualEntry(int menu, int index)
{
	if(menu == GAMES) //return pointer to object in games list
		return (game_list[index]);
	else if(menu == INSTALL) //in installer list
		return (installer_list[index]);
	
	//no valid menu
	return NULL;	
};

int menu138 :: get_items_on_list()
{
	//return total items in current submenu
	return (menu == GAMES)? game_list.size(): (menu == INSTALL)? installer_list.size(): 0;
};

void menu138 :: settingsCtrl()
{
	char * aux;
	switch(list_option)
	{
		case 0: //Language change
			if(++cfg.lang == LANG_COUNT) 
				cfg.lang = 0;
				
			break;
		case 1: //App icon load
		
			TOGGLE(cfg.app_icon);
			break;
			
		case 2: //Zip icon load
		
			TOGGLE(cfg.zip_icon);
			break;
			
		case 3: //Load mode
		
			TOGGLE(cfg.load_mode);
			break;
			
		case 4: //Install name
		
			osk.activate(lang_settings[cfg.lang][4], cfg.appName, MAX_EBOOT_NAME);
			if(!osk.Cancelled()) 
				cfg.setAppName(osk.getOutput());
				
			break;
			
		case 5: //Install path
		
			aux = strchr(strchr(cfg.appPath, '/') + 1, '/') + 1;
			osk.activate(lang_settings[cfg.lang][5], aux, MAX_PATH_NAME);
			if(!osk.Cancelled())
			{
				if(strlen(osk.getOutput()) == MAX_PATH_NAME && strncmp(aux, osk.getOutput(), MAX_PATH_NAME))
				{
					strncpy(aux, osk.getOutput(), MAX_PATH_NAME);
					updateGameList();
				};
			};
			
			break;
			
		case 6: //List /GAME/ apps
			TOGGLE(cfg.list_GAME);
			
			break;
		
		case 7: //Theme
	
			if(theme_selected != theme_current)
			{
				cfg.setCustomTheme(themes.get(theme_selected)->name);
				res.start_theme();
				
				theme_current = theme_selected;
				//theme_current = themes.find_file(cfg.custom_theme, 1);
				//theme_selected = theme_current;
			};
			
			break;
			
		case 8: //Background
			
			if(wall_selected != wall_current)
			{
				if(wall_selected < 0) //is default wallpaper
					cfg.setBackground(DEFAULT_BACK);
				else
					cfg.setBackground(wallpapers.get(wall_selected)->name);
				
				res.loadCustomBackground();
				wall_current = wall_selected;
			};

			break;
			
		case 9: //Clean apps folder
		
			setMessage(lang_msg_delete[cfg.lang], cfg.appPath, " ?", FORMAT_HBFOLDER);
			
			break;
			
		case 10: //Make defaults
		
			setMessage(lang_settings[cfg.lang][10], NULL, " ?", RESTORE_DEFAULTS);			
	};
	
	cfg.save();
};

void menu138 :: check()
{
	//control animations status
	animCheck();
	
	if(animating()) //abort if still animating
		return;
	
	//start explorer
	if(menu == EXPLORER && main_menu == false)
		explorer.show(this);
	
	if(message_active) //check message button pressing
	{
		if(ctrl.get()->up || ctrl.get()->down) //change option inside a message
			TOGGLE(message_option);
		else if(ctrl.get()->cross) //choose option
		{
			if(!message_option) 
				doAction(message_action);
				
			message_active = 0;
		}
		else if(ctrl.get()->circle) //cancel message
			message_active = 0;

		return;
	};
	
	//menu switch
	if(ctrl.get()->rtrigger) 
		roll(-1); //previous
	else if(ctrl.get()->ltrigger) //pressed L
		roll(1); //next
	
	if(main_menu == true) //main menu
	{
		//menu switch
		if(ctrl.get()->up || ctrl.get()->left) 
			roll(-1); //previous
			
		if(ctrl.get()->down || ctrl.get()->right) 
			roll(1); //next
		
		//menu selected
		if(ctrl.get()->cross)
		{
			//start "menu open" animations
			start_animation(&circle_move, &circleTo, -1, -55);
			start_animation(&title_move, &titleTo, -1, -110);
			start_animation(&list_move, &listTo, -1, 93);
			
			main_menu = false;
		};	
	}
	else
	{
		int count = (menu != SETTINGS)? get_items_on_list(): SETTINGS_COUNT;
		
		if(menu == SETTINGS)
		{
			
			if(ctrl.get()->up || ctrl.get()->down)
			{
				theme_selected = theme_current;
				wall_selected = wall_current;
			};
			
			if(list_option == 7) //Pressed left or right in Theme
			{
				if(ctrl.get()->left)
				{
					if(--theme_selected < 0) 
						theme_selected = themes.get_count() - 1;
				}
				else if(ctrl.get()->right)
				{
					if(++theme_selected == themes.get_count()) 
						theme_selected = 0;
				};
			}
			else if(list_option == 8) //Pressed left or right in Background
			{
			
				if(ctrl.get()->left)
				{
					if(--wall_selected < -1) 
						wall_selected = wallpapers.get_count() - 1;
				}
				else if(ctrl.get()->right)
				{
					if(++wall_selected == wallpapers.get_count()) 
						wall_selected = -1;
				};
				
			};
		};
		
		//do entry
		if(ctrl.get()->cross) //press cross in any submenu
		{
			if(menu == SETTINGS)
				settingsCtrl(); //do current setting
			else if(count)
			{
				entry138 * entry = actualEntry(menu, list_option);
				
				if(entry->getType() != ZIP) //entry is not an installer
					load_app(entry);
				else //ask to install
					setMessage(lang_msg_install[cfg.lang], entry->getName(), " ?", INSTALL_SELECTED);
			};
		};
		
		//delete entry
		if(ctrl.get()->triangle) //triangle pressed
		{
			if(count && menu != SETTINGS)
			{
				entry138 * entry = actualEntry(menu, list_option);
				
				//set delete message
				setMessage(lang_msg_delete[cfg.lang], entry->getName(), " ?", DELETE_SELECTED);
			};
		};
		
		//return to main menu:
		if(ctrl.get()->circle) //pressed circle in any submenu
		{
			//start "menu close" animations
			start_animation(&circle_move, &circleTo, 1, CIRCLE_INITIAL_X);
			start_animation(&list_move, &listTo, 1, 500);
			start_animation(&title_move, &titleTo, 1, 0);
			
			main_menu = true; 
		};
		
		//update list
		if(ctrl.get()->select)
		{
			if(menu == GAMES) 
				updateGameList(); //refresh game list
			else if(menu == INSTALL) 
				updateInstallerList(); //refresh installer list
		};
		
		//list scrolling:
		if(ctrl.get()->down) //press down in any submenu
		{
			if(list_option < count - 1)
			{
				if(++list_option == MAX_ON_SCREEN + offset) 
					offset++;
			}
			else 
				list_option = offset = 0;
		}
		else if(ctrl.get()->up) //press up in any submenu
		{
			if(list_option > 0)
			{
				list_option--;
				if(offset && (list_option == offset - 1)) 
					offset--;
			}
			else
			{
				list_option = count - 1;
				if(list_option >= MAX_ON_SCREEN) 
					offset=count - MAX_ON_SCREEN;
			};
		};
		
	};
};

void menu138 :: drawText()
{
	//print category name
	printText(lang_main_menu[cfg.lang][menu], circlex + 160, circley + titley + 41, WHITE, BLACK, SIZE_BIG);

	// do not draw texts while there are animations in course or it is inside the explorer
	if(animating() || main_menu == true) 
		return;
	
	if(menu == SETTINGS) //settings menu draw
	{
		char * fix_dot_theme = strrchr(themes.get(theme_selected)->name, '.'), * fix_dot_wall = NULL;
		* fix_dot_theme = 0x0;
	
		if(wall_selected >= 0)
		{
			fix_dot_wall = strrchr(wallpapers.get(wall_selected)->name, '.');
			* fix_dot_wall = 0x0;
		};
		
		//update settings status string pointers according to language
		const char * setting_status[SETTINGS_COUNT] = 
		{
			lang_names[cfg.lang], //language
			lang_settings_yesno[cfg.lang][cfg.app_icon], //show app icon
			lang_settings_yesno[cfg.lang][cfg.zip_icon], //show installer icon
			lang_settings_modes[cfg.lang][cfg.load_mode], //load mode
			cfg.appName, //custom pbp name
			strchr(cfg.appPath, '/'), //app path
			lang_settings_yesno[cfg.lang][cfg.list_GAME], //load /GAME apps
			strncmp(themes.get(theme_selected)->name, DEFAULT_THEME, strlen(DEFAULT_THEME) - 4)? themes.get(theme_selected)->name: "138Menu", //theme
			(wall_selected >= 0)? wallpapers.get(wall_selected)->name: "138Menu", //background
			NULL, //clean homebrew folder
			NULL //restore defaults
		};
		
		for(int i = 0; i < SETTINGS_COUNT; i++)
		{
			//define color
			unsigned color_text = (list_option == i)? YELLOW: WHITE;
			unsigned color_shadow = (list_option == i)? WHITE: BLACK;
		
			//print setting
			printText(lang_settings[cfg.lang][i], LIST_START_X, LIST_START_Y + 12 * i, color_text, color_shadow, SIZE_LITTLE);
			
			//print status
			printText(setting_status[i], 380, LIST_START_Y + 12 * i, GREY, color_shadow, SIZE_LITTLE);
		};
		
		* fix_dot_theme = '.';
		
		if(fix_dot_wall)
			* fix_dot_wall = '.';
		
		if(!message_active) //show credits if message is not active
			printText(CREDITS, LIST_START_X, 242, GREY, WHITE, SIZE_LITTLE);
			
	}
	else if(menu == GAMES || menu == INSTALL)//other menus draw
	{
		int count = get_items_on_list();
		
		if(count) //there are items to display
		{
			//define how many items should be printed
			int show_count = (count > MAX_ON_SCREEN)? MAX_ON_SCREEN: count;
		
			if(show_count > 10 && message_active) //reduce amount of items if a message will be printed
				show_count = 10;
			
			for(int i = 0; i < show_count; i++)
			{
				//define text color for this entry
				unsigned color_text = (list_option == i + offset)? YELLOW: WHITE;
				unsigned color_shadow = (list_option == i + offset)? WHITE: BLACK;
				
				//get current entry
				entry138 * entry = actualEntry(menu, i + offset);
				
				//print text
				printText(entry->getName(), LIST_START_X, LIST_START_Y + LIST_SEPARATION * i, color_text, color_shadow, SIZE_LITTLE);
			};
		}
		else //no items to display
		{
			//define which text should be displayed
			const char * no_apps_text = (menu == GAMES)? lang_no_apps[cfg.lang]: lang_no_installers[cfg.lang];
			
			//print text
			printText(no_apps_text, LIST_START_X, LIST_START_Y, WHITE, BLACK, SIZE_LITTLE);
		};
	};

	if(message_active)
	{
		//print message text
		printText(message, MESSAGE_X + 9, MESSAGE_Y + 16, WHITE, BLACK, SIZE_LITTLE);
		
		//print options
		for(int i = 0; i < 2; i++) 
		{
			//define option color
			unsigned color_text = (i == message_option)? YELLOW: WHITE;
			unsigned color_shadow = (i == message_option)? WHITE: BLACK;
			
			//print
			printText(lang_msg_options[cfg.lang][i], MESSAGE_X + 20, MESSAGE_Y + 32 + LIST_SEPARATION * i, color_text, color_shadow, SIZE_LITTLE);
		};
	};
	
};

void menu138 :: drawImages()
{
	//choose a custom background(if any) or use the default one
	Image * back_show = res.img[MENU_CUSTOMBACK]? res.img[MENU_CUSTOMBACK]: res.img[MENU_BACKGROUND];
	
	//calculate sphere positions and set data
	int 
	sphere_position_x[] =
	{
		circlex + CIRCLE_DISTANCE * cos(rotation + RD),
		circlex + CIRCLE_DISTANCE * cos(rotation),
		circlex + CIRCLE_DISTANCE * cos(rotation - RD),
		circlex - CIRCLE_DISTANCE * cos(-rotation)
	},
	sphere_position_y[] = 
	{
	
		circley + CIRCLE_DISTANCE * sin(rotation + RD),
		circley + CIRCLE_DISTANCE * sin(rotation),
		circley + CIRCLE_DISTANCE * sin(rotation - RD),
		circley - CIRCLE_DISTANCE * sin(rotation)
	},
	sphere_image[] = //image enum, see 138_resources
	{
		MENU_INS, 
		MENU_JOY,
		MENU_SET,
		MENU_EXP
	},
	sphere_code[] = //see ORDER enum
	{
		INSTALL,
		GAMES,
		SETTINGS,
		EXPLORER
	};
	
	//background drawing
	printImg(back_show, 0, 0);
	
	//middle circle
	printImg(res.img[MENU_CIRCLE], circlex - 8, circley - 10);
	
	//spheres drawing
	for(int i = 0; i < 4; i++)
	{
		//draw sphere
		printImg(res.img[sphere_image[i]], sphere_position_x[i], sphere_position_y[i]);
	
		if((menu != sphere_code[i]) || main_menu == false) //draw shadow
			printImg(res.img[MENU_BSHADOW], sphere_position_x[i], sphere_position_y[i]);
	};

	//List and bar drawing
	printImg(res.img[MENU_LISTBACK], listx, 45);
	printImg(res.img[MENU_BAR], circlex + 147, circley + titley + 23);

	//should it render the icon for this submenu?
	int draw_icon = (menu == GAMES)? cfg.app_icon: (menu != SETTINGS)? cfg.zip_icon: 0;
	
	//Game icon back square and icon drawing
	if(!animating() && get_items_on_list() && draw_icon && main_menu == false)
	{
		//int count = op==GAMES? hb_count: zip_count;
		entry138 * entry = actualEntry(menu, list_option);

		//print icons background square
		printImg(res.img[MENU_GAME_TILE], listx + 227, 51);
		
		if(entry->getIcon()) //draw app icon
			printImg(entry->getIcon(), 396 - entry->getIcon()->imageWidth / 2, 96 - entry->getIcon()->imageHeight / 2);
		else //draw default icon
			printImg(res.img[MENU_DEFAULT], 396 - res.img[MENU_DEFAULT]->imageWidth / 2, 96 - res.img[MENU_DEFAULT]->imageHeight / 2);
	};
	
	if(message_active) //Message background drawing
		printImg(res.img[MENU_MESSAGE], MESSAGE_X, MESSAGE_Y);
};

void menu138 :: clean_list(vector<entry138 *> & list)
{
	for(unsigned i = 0; i < list.size(); i++)
		delete list[i];
	
	list.clear();
};

int menu138 :: fillList(vector<entry138 *> & list, const char * start, const char * forced_name, int append, int ONLY_ISOS)
{
	int count = list.size(); //get current items count on vector
	
	static char eboot_names[][0x10] = //valid eboot/application names
	{
		"wmenu.bin\0", //VHBL EBOOT name
		"FBOOT.PBP\0", //TN CEF EBOOT name
		"VBOOT.PBP\0", //ARK EBOOT name
		
		"CUSTOM\0", //custom app name, should always be before EBOOT.PBP
		"EBOOT.PBP\0" //EBOOT.PBP should always be the last one in this array
	};
	
	int search_names = sizeof(eboot_names) / sizeof(eboot_names[0]); //total eboot names to check
	strcpy(eboot_names[search_names - 2], cfg.appName); //copy custom name into eboot_names array
	
#ifndef CEF

	search_names--; // do not include EBOOT.PBP file
	
#endif
	
	if(DEBUG_LOAD) //debug load enabled, clean screen
		pspDebugScreenClear();
	
	//try to open directory
	SceUID dir = sceIoDopen(start);
	
	if(dir >= 0)
	{
		SceIoDirent entry; 
		memset(&entry, 0, sizeof(SceIoDirent)); //clear directory entry
	
		char buffer[1024]; //buffer to create file path
	
		//start reading directory entries
		while(sceIoDread(dir, &entry) > 0)
		{
			//skip . and .. paths
			if(!strcmp(".", entry.d_name) || !strcmp("..", entry.d_name)) 
			{
				//clear directory entry and continue
				memset(&entry, 0, sizeof(SceIoDirent)); 
				continue;
			};
			
			int error = 0;
			int found = -1; //set to not-found
			
			//build full file path
			strcpy(buffer, start);
			strcat(buffer, entry.d_name);
			
			//categories plugin support
			if(!strncmp(entry.d_name, "CAT", 3))
			{	
				strcat(buffer, "/");
				fillList(list, buffer, forced_name, append, ONLY_ISOS);
				* strrchr(buffer, '/') = 0x0;
				//continue;
			};
			
			if(append) //should append a valid eboot name or a forced name
			{
				strcat(buffer, "/");
				
				if(forced_name) //the file name is forced (used for install.zip files)
				{
					strcat(buffer, forced_name); //append name
					
					if(explorer138::file_exists(buffer)) //try to open
						found = 1;
				}
				else 
				{
					for(int i = 0; i < search_names; i++)
					{
						//build path with a valid eboot file name
						strcpy(strrchr(buffer, '/') + 1, eboot_names[i]); 
						
						//try to open file
						if(explorer138::file_exists(buffer)) 
						{
							found = i;
							break;
						};
					};
				};
			}
			else if(explorer138::file_exists(buffer))
				found = 1;

			if(found < 0) //no file/valid file found
				continue;
			
			if(DEBUG_LOAD) //print the file path if debug loading is enabled
				pspDebugScreenPrintf("Processing %s\n", buffer);
			
			//create new object for the entry
			entry138 * new_entry = NULL;
			new_entry = new entry138();
			
			if(!new_entry) //object couldn't be created
				error |= 1;
			else
			{
				//try to create new entry
				if(new_entry->create(buffer) == entry138::LOAD_ERROR) 
					error |= 2;

				//Only isos filter
				if(!error && ONLY_ISOS)
				{
					int entry_type = new_entry->getType();
					if(entry_type != ISO && entry_type != CSO)
						error |= 4;
				};
			};

			if(DEBUG_LOAD) //print result
				pspDebugScreenPrintf("Result: %d\n", error);
			
			if(error) //skip this entry as it failed
			{
				if(error > 1)
					delete new_entry; //delete object
					
				continue; //go to next entry
			};
			
			list.push_back(new_entry); //add object to vector
			count++;
			
			sceKernelDcacheWritebackInvalidateAll();
		};
		
		//close directory
		sceIoDclose(dir);
	};
	
	return count; //return total entries
};

void menu138 :: animation_control(int * flag, float * status, float limit, float step)
{
	//generic animation control
	//will add or decrease step into status depending on flag value
	//sets flag to 0 if limit is reached
	
	if(* flag == 1) 
	{
		if(* status < limit) 
			* status += step;
			
		if(* status >= limit) 
		{
			* status = limit;
			* flag = 0;
		};
	}
	else if(* flag == -1)
	{
		if(* status > limit) 
			* status -= step;
			
		if(* status <= limit) 
		{
			* status = limit;
			* flag = 0;
		};
	};
};

inline void menu138 :: start_animation(int * flag, float * limit, int direction, float destination)
{
	* flag = direction;
	* limit = destination; //final position
};

void menu138 :: animCheck()
{
	//Back circle move control
	animation_control(&circle_move, &circlex, circleTo, 5);

	//Rotation animation control
	animation_control(&rotation_flag, &rotation, rotateTo, 0.1);

	//List animation control
	animation_control(&list_move, &listx, listTo, 15);

	//Title (Y) animation control
	animation_control(&title_move, &titley, titleTo, 5);
};

void menu138 :: roll(int dir)
{
	//rotate circle in dir direction
	rotateTo = rotateTo - (dir * PI * 2/4);
	rotation_flag = -dir;
	
	menu += dir;
	
	//check valid menu
	if(menu < 0)
		menu = 3;
	else if(menu > 3)
		menu = 0;
	
	list_option = offset = 0;
	theme_selected = theme_current;
	wall_selected = wall_current;

};

inline int menu138 :: animating() const
{
	//returns 1 if there is at least 1 animation active
	return rotation_flag || circle_move || list_move || title_move;
};

