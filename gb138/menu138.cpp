#include "menu138.h"

menu138 :: menu138()
{
	op = GAMES; //Initial menu
	active = 1;
	menu = -1;
	rotateTo = rotation = rotation_flag = 0;
	circlex = 50; 
	circley = 100;
	circle_move = 0;
	list_move = listTo = 0;
	listx = 500;
	title_move = titley = titleTo = 0;
	list_option = offset = 0;
	hb_count = zip_count = 0;
	message_active = 0;
	press_time = 0;
};

int menu138 :: start()
{
	pspDebugScreenPrintf("Starting graphics...\n");
	this->startGl();
	pspDebugScreenPrintf("Creating app list...\n");
	this->updateGameList();
	pspDebugScreenPrintf("Creating installer list...\n");
	this->updateInstallerList();

	while(active)
	{
		ctrl.update();
		this->clrscr();
		
		this->check();
		this->drawImages();
		this->drawText();
		
		this->flip();
		ctrl.flush();
	};

	game_list.FreeMemory();
	installer_list.FreeMemory();
	this->killGl();
	return menu138::MENU_OK;
};

void menu138 :: loadEBOOT(const char * path)
{
	strcpy(ebootAddress, path);
	active = 0;
};

void menu138 :: cefStart(entry138 * entry)
{
	#ifdef ARK
	ark_launch(entry->getPath(), entry->getType(), entry->isPatched());
	#endif
};

void menu138 :: setEbootAddress(char * address)
{
	ebootAddress = address;
};

void menu138 :: startGl()
{
	initGraphics();
};

void menu138 :: killGl()
{
	disableGraphics();
};

void menu138 :: clrscr()
{
	clearScreen(BLACK);
};

void menu138 :: flip()
{
	sceGuFinish();
	sceGuSync(0,0);
	guStart();
	sceGuTexMode(GU_PSM_8888, 0, 0, 0);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	sceGuFinish();
	sceGuSync(0,0); 

	sceDisplayWaitVblankStart(); 
	flipScreen();
	sceKernelDcacheWritebackInvalidateAll();
	sceKernelDelayThread(THREAD_DELAY);
};

void menu138 :: printText(const char * text, int x, int y, unsigned color, unsigned shadow, float size)
{
	intraFontSetStyle(res.font, size, color, shadow, 0.0f, 0);
	intraFontPrint(res.font, x, y, text);
};

void menu138 :: printImg(Image * img, int x, int y)
{
	blitAlphaImageToScreen(0, 0, img->imageWidth, img->imageHeight, img, x, y);
};

void menu138 :: updateGameList()
{
	game_list.FreeMemory();
#ifdef CEF
	hb_count = this->fillList(&game_list, "ms0:/ISO/", NULL, 0);
#endif
	if(gbExplorer::folderExist(cfg.appPath)!=gbExplorer::FILE_OK) gbExplorer::createFolder(cfg.appPath);
	if(cfg.list_GAME) hb_count = this->fillList(&game_list, "ms0:/PSP/GAME/", NULL, 1);
	hb_count = this->fillList(&game_list, cfg.appPath, NULL, 1);
};

void menu138 :: updateInstallerList()
{
	installer_list.FreeMemory();
	zip_count = this->fillList(&installer_list, INSTALLERS_PATH, ZIP_NAME, 1);
};

void menu138 :: waitButtonsPressed()
{
	while(ctrl.isPressed()) ctrl.update();
};

void menu138 :: roll(int dir)
{
	op=op+dir;
	rotateTo=rotateTo-(dir*PI*2/3);
	rotation_flag = -dir;
	if(op<0) op = 2; 
	else if(op>2) op = 0;
	list_option = offset = 0;
};

int menu138 :: animating()
{
	return rotation_flag || circle_move || list_move || title_move;
};

void menu138 :: setMessage(const char * caption, const char * cat1, const char * cat2, int action_code)
{
	strcpy(message, caption);
	if(cat1) strcat(message, cat1);
	if(cat2) strcat(message, cat2);
	message_option = 1;
	message_active = 1;
	message_action = action_code;
};

void menu138 :: deleteItemFromSD(entry138 * entry)
{
	char * abs_delete = (char *)malloc(strlen(entry->getPath()));
	strcpy(abs_delete, entry->getPath());
	
	if(entry->getType()==entry138::ISO || entry->getType()==entry138::CSO) gbExplorer::deleteFile(abs_delete);
	else
	{
		*(strrchr(abs_delete, '/')+1) = '\0';
		gbExplorer::deleteFolder(abs_delete);
	};
	
	free(abs_delete);
	if(menu==GAMES) this->updateGameList();
	else this->updateInstallerList();
};

void menu138 :: doAction(int action_code)
{
	entry138 * entry = this->actualEntry(menu, list_option);;
	switch(action_code)
	{
		case INSTALL_SELECTED:
			if(gbZip::unzip(entry->getPath(), cfg.appPath, cfg.appName)!=gbZip::ZIP_ERROR) this->updateGameList();
		break;
		case DELETE_SELECTED:
			this->deleteItemFromSD(entry);
		break;
		case FORMAT_HBFOLDER:
			gbExplorer::deleteFolder(cfg.appPath);
			gbExplorer::createFolder(cfg.appPath);
			this->updateGameList();
		break;
	};
	offset = 0;
	list_option = 0;
};

entry138 * menu138 :: actualEntry(int menu, int index)
{
	tData * tmp;
	if(menu==GAMES && hb_count) tmp = game_list.Get(index);
	else if(menu==INSTALL && zip_count) tmp = installer_list.Get(index);
	else return NULL;
	entry138 * ret = tmp->entry;
	return ret;
};

void menu138 :: settingsCtrl()
{
	char * aux;
	switch(list_option)
	{
		case 0: //Language change
			if(++cfg.lang==config138::LANG_COUNT) cfg.lang = 0;
		break;
		case 1: //App icon load
			cfg.app_icon = cfg.app_icon? 0: 1;
		break;
		case 2: //Zip icon load
			cfg.zip_icon = cfg.zip_icon? 0: 1;
		break;
		case 3: //Load mode
			cfg.load_mode = cfg.load_mode? 0: 1;
		break;
		case 4: //Install name
			osk.activate(lang_settings[cfg.lang][4], cfg.appName, MAX_EBOOT_NAME);
			if(!osk.Cancelled()) cfg.setAppName(osk.getOutput());
		break;
		case 5: //Install path
			aux = strchr(strchr(cfg.appPath, '/')+1,'/')+1;
			osk.activate(lang_settings[cfg.lang][5], aux, MAX_PATH_NAME);
			if(!osk.Cancelled())
			{
				if(strlen(osk.getOutput())==MAX_PATH_NAME && strncmp(aux, osk.getOutput(), MAX_PATH_NAME))
				{
					strncpy(aux, osk.getOutput(), MAX_PATH_NAME);
					this->updateGameList();
				};
			};
		break;
		case 6: //List /GAME/ apps
			cfg.list_GAME = cfg.list_GAME? 0: 1;
		break;
		case 7: //Clean apps folder
			this->setMessage(lang_msg_delete[cfg.lang], cfg.appPath, " ?", FORMAT_HBFOLDER);
		break;
		case 8: //Make defaults
			cfg.makeDefault();
		break;
	};
	cfg.save();
};

void menu138 :: animCheck()
{
	//Back circle move control
	if(circle_move) 
	{
		if(circlex<circleTo) circlex+=5;
		if(circlex>circleTo) circlex-=5;
		if(circlex==circleTo) circle_move=0;
	};

	//Rotation animation control
	if(rotation_flag==1) 
	{
		if(rotation<rotateTo) rotation+=0.1;
		if(rotation>=rotateTo) 
		{
			rotation = rotateTo;
			rotation_flag = 0;
		};
	}
	else if(rotation_flag==-1)
	{
		if(rotation>rotateTo) rotation-=0.1;
		if(rotation<=rotateTo) 
		{
			rotation = rotateTo;
			rotation_flag = 0;
		};
	};

	//List animation control
	if(list_move==-1) 
	{
		if(listx>listTo) listx-=15;
		if(listx<=listTo)
		{
			listx=listTo;
			list_move=0;
		};
	}
	else if(list_move==1)
	{
		if(listx<listTo) listx+=15;
		if(listx>=listTo)
		{
			listx=listTo;
			list_move=0;
		};
	};

	//Title (Y) animation control
	if(title_move==1) 
	{
		if(titley<titleTo) titley+=5;
		if(titley>=0)
		{
			titley=titleTo;
			title_move=0;
		};
	}
	else if(title_move==-1)
	{
		if(titley>titleTo) titley-=5;
		if(titley<=titleTo)
		{
			titley=titleTo;
			title_move=0;
		};
	};
};

void menu138 :: check()
{
	this->animCheck();
	if(!this->animating()) //check for button press only if its not animating graphics
	{
		if(message_active) //check message button pressing
		{
			if(ctrl.getCtrl()->up || ctrl.getCtrl()->down) 
				if(++message_option>1) message_option = 0;
			if(ctrl.getCtrl()->cross)
			{
				if(!message_option) this->doAction(message_action);
				message_active = 0;
			};
			if(ctrl.getCtrl()->circle) message_active = 0;
			this->waitButtonsPressed();
			return;
		};

		if(menu < 0) //main menu
		{
			if(ctrl.getCtrl()->up || ctrl.getCtrl()->left || ctrl.getCtrl()->rtrigger) this->roll(-1);
			if(ctrl.getCtrl()->down || ctrl.getCtrl()->right || ctrl.getCtrl()->ltrigger) this->roll(1);

			if(ctrl.getCtrl()->cross)
			{
				menu=op;
				circle_move=1; 	circleTo=-55;
				title_move=-1; 	titleTo = -110;
				list_move=-1; 	listTo=93;
				list_option=0; 	offset = 0;
			};	
		}
		else
		{
			int count = menu==GAMES? hb_count: menu==INSTALL? zip_count: SETTINGS_COUNT;
			entry138 * entry = this->actualEntry(menu, list_option);
			
			if(ctrl.getCtrl()->cross) //Press cross in any submenu
			{
				this->waitButtonsPressed();
				if(menu==SETTINGS) this->settingsCtrl();
				else if(count)
				{
					if(entry->getType()!=entry138::INSTALLER_HB) 
					{
						#ifdef CEF
							this->cefStart(entry);
						#else
							this->loadEBOOT(entry->getPath());
						#endif
					}
					else
						this->setMessage(lang_msg_install[cfg.lang], entry->getName(), " ?", INSTALL_SELECTED);
				};
			};
			
			if(menu!=SETTINGS && count && ctrl.getCtrl()->triangle) //Triangle pressed (delete)
				this->setMessage(lang_msg_delete[cfg.lang], entry->getName(), " ?", DELETE_SELECTED);
			
			if(ctrl.getCtrl()->down) //Press down in any submenu
			{
				if(list_option<count-1)
				{
					if(++list_option==MAX_ON_SCREEN+offset) offset++;
				}
				else list_option = offset = 0;
			};
			
			
			if(ctrl.getCtrl()->up) //Press up in any submenu
			{
				if(list_option>0)
				{
					list_option--;
					if(offset && (list_option == offset-1)) offset--;
				}
				else
				{
					list_option=count-1;
					if(list_option>=MAX_ON_SCREEN) offset=count-MAX_ON_SCREEN;
				};
			};
			
			if(ctrl.getCtrl()->rtrigger) //Press R in any submenu
			{
				this->roll(-1);
				menu=op;
			};
			
			if(ctrl.getCtrl()->ltrigger) //Press L in any submenu
			{
				this->roll(1);
				menu=op;
			};
			
			if(ctrl.getCtrl()->circle) //Press circle in any submenu
			{
				menu=-1;
				circle_move=1; 	circleTo=50;
				list_move=1; 	listTo=500;
				title_move=1; 	titleTo=0; 
			};
			
			if(menu!=SETTINGS && ctrl.getCtrl()->select) //Refresh lists
			{
				if(menu==GAMES) this->updateGameList();
				else if(menu==INSTALL) this->updateInstallerList();
				offset = 0;
				list_option = 0;
			};
		};
	};
};

void menu138 :: drawText()
{
	//Draw the menu texts
	guStart();
	printText(lang_main_menu[cfg.lang][op], circlex+160, circley+titley+41, WHITE, BLACK, SIZE_BIG);

	if(this->animating()) return;
	if(menu==SETTINGS) //Settings menu draw
	{
		if(!message_active) printText(CREDITS, 107, 242, GREY, WHITE, SIZE_LITTLE);
		for(int i=0;i<SETTINGS_COUNT;i++)
		{
			if(list_option==i) printText(lang_settings[cfg.lang][i], 107, 62+12*i, YELLOW, WHITE, SIZE_LITTLE);
			else printText(lang_settings[cfg.lang][i], 107, 62+12*i, WHITE, BLACK, SIZE_LITTLE);
		};
		
		printText(lang_names[cfg.lang], 380, 62, GREY, BLACK, SIZE_LITTLE);
		printText(lang_settings_yesno[cfg.lang][cfg.app_icon], 380, 74, GREY, BLACK, SIZE_LITTLE);
		printText(lang_settings_yesno[cfg.lang][cfg.zip_icon], 380, 86, GREY, BLACK, SIZE_LITTLE);
		printText(lang_settings_modes[cfg.lang][cfg.load_mode], 380, 98, GREY, BLACK, SIZE_LITTLE);
		printText(cfg.appName, 380, 110, GREY, BLACK, SIZE_LITTLE);
		printText(strchr(cfg.appPath, '/'), 380, 122, GREY, BLACK, SIZE_LITTLE);
		printText(lang_settings_yesno[cfg.lang][cfg.list_GAME], 380, 134, GREY, BLACK, SIZE_LITTLE);
	}
	else //Other menus draw
	{
		int count = menu==GAMES? hb_count: menu==INSTALL? zip_count: 0, show_count;
		if(count)
		{
			entry138 * entry;
			if(count>MAX_ON_SCREEN) show_count = MAX_ON_SCREEN;
			else show_count = count;
			if(show_count>10 && message_active) show_count = 10;
			
			for(int i=0;i<show_count;i++)
			{
				entry = this->actualEntry(menu, i+offset);
				if(list_option==i+offset) printText(entry->getName(), 107, 62+14*i, YELLOW, WHITE, SIZE_LITTLE);
				else printText(entry->getName(), 107, 62+14*i, WHITE, BLACK, SIZE_LITTLE);
			};
		}
		else
		{
			if(menu==GAMES) //No homebrew
			{
				printText(lang_no_apps[cfg.lang], 107, 60, WHITE, BLACK, SIZE_LITTLE);
				printText(cfg.appPath, 107, 74, WHITE, BLACK, SIZE_LITTLE);
			};
			if(menu==INSTALL) //No installers
				printText(lang_no_installers[cfg.lang], 107, 60, WHITE, BLACK, SIZE_LITTLE);
		};
	};

	if(message_active)
	{
		printText(message, 17, 216, WHITE, BLACK, SIZE_LITTLE);
		for(int i=0;i<2;i++)
		{
			if(i==message_option) printText(lang_msg_options[cfg.lang][i], 28, 232+14*i, YELLOW, WHITE, SIZE_LITTLE);
			else printText(lang_msg_options[cfg.lang][i], 28, 232+14*i, WHITE, BLACK, SIZE_LITTLE);
		};
	};
};

void menu138 :: drawImages()
{
	//Background drawing
	Image * back_show = res.img[MENU_CUSTOMBACK]? res.img[MENU_CUSTOMBACK]: res.img[MENU_BACKGROUND];
	this->printImg(back_show, 0, 0);

	//Spheres drawing
	this->printImg(res.img[MENU_CIRCLE], circlex-8, circley-10);
	this->printImg(res.img[MENU_INS], circlex+CIRCLE_DISTANCE*cos(rotation+RD),circley+CIRCLE_DISTANCE*sin(rotation+RD));
	this->printImg(res.img[MENU_JOY], circlex+CIRCLE_DISTANCE*cos(rotation),circley+CIRCLE_DISTANCE*sin(rotation));
	this->printImg(res.img[MENU_SET], circlex+CIRCLE_DISTANCE*cos(rotation-RD),circley+CIRCLE_DISTANCE*sin(rotation-RD));

	//Shperes shadow drawing
	if(op!=SETTINGS || menu!=-1) this->printImg(res.img[MENU_BSHADOW], circlex+CIRCLE_DISTANCE*cos(rotation-RD),circley+CIRCLE_DISTANCE*sin(rotation-RD));
	if(op!=GAMES || menu!=-1) this->printImg(res.img[MENU_BSHADOW], circlex+CIRCLE_DISTANCE*cos(rotation),circley+CIRCLE_DISTANCE*sin(rotation));
	if(op!=INSTALL || menu!=-1) this->printImg(res.img[MENU_BSHADOW], circlex+CIRCLE_DISTANCE*cos(rotation+RD),circley+CIRCLE_DISTANCE*sin(rotation+RD));

	//List and bar drawing
	this->printImg(res.img[MENU_LISTBACK], listx, 45);
	this->printImg(res.img[MENU_BAR], circlex+145, circley+titley+23);

	//Game icon back square and icon drawing
	if(!this->animating() && (menu==GAMES? cfg.app_icon: cfg.zip_icon) && menu>0 && menu!=SETTINGS)
	{
		int count = op==GAMES? hb_count: zip_count;
		if(count)
		{
			entry138 * entry = this->actualEntry(menu, list_option);

			this->printImg(res.img[MENU_GAME_TILE], listx+227, 51);
			if(entry->getIcon()) this->printImg(entry->getIcon(), 396-entry->getIcon()->imageWidth/2, 96-entry->getIcon()->imageHeight/2);
			else this->printImg(res.img[MENU_DEFAULT], 396-res.img[MENU_DEFAULT]->imageWidth/2, 96-res.img[MENU_DEFAULT]->imageHeight/2);
		};
	};
	//Message background drawing
	if(message_active) this->printImg(res.img[MENU_MESSAGE], 8, 200);
};

int menu138 :: fillList(gbAlloc * list, const char * start, const char * forced_name, int append)
{
	SceUID dir;
	SceIoDirent entry; 
	tData new_node;
	int i, found, error, count = list->GetCount(), search_names = 4;
	char eboot_names[][15] = { "wmenu.bin\0", "FBOOT.PBP\0", "VBOOT.PBP\0", "CUSTOM\0", "EBOOT.PBP\0"}, buffer[500];
	entry138 * new_entry = NULL;

	strcpy(eboot_names[3], cfg.appName);
	memset(&entry, 0, sizeof(SceIoDirent));
	
#ifdef CEF
	search_names = 5;
#endif

	dir = sceIoDopen(start);
	if(dir >= 0)
	{
		while((sceIoDread(dir, &entry) > 0))
		{
			error = 0;
			found = -1;
			strcpy(buffer, start);
			strcat(buffer, entry.d_name);
			
			if(append)
			{
				strcat(buffer, "/");
				
				if(forced_name)
				{
					strcat(buffer, forced_name);
					if(gbExplorer::fileExist(buffer)==gbExplorer::FILE_OK) found = 5;
				}
				else
					for(i=0;i<search_names;i++)
					{
						strcpy(strrchr(buffer, '/')+1, eboot_names[i]);
						if(gbExplorer::fileExist(buffer)==gbExplorer::FILE_OK)
						{
							found = i;
							break;
						};
					};
			}
			else if(gbExplorer::fileExist(buffer)==gbExplorer::FILE_OK) found = 6;
				
			if(found < 0) continue;
			new_entry = new entry138();
			new_node.entry = new_entry;
			if(new_entry->create(buffer)==entry138::LOAD_ERROR) error = 1;
			if(error || list->Add(&new_node)==gbAlloc::GBALLOC_NO_MEMORY) error = 1;
			if(error) delete new_entry;
			else count++;
			//__printf("%s\n", new_entry->getName());
		};
		sceIoDclose(dir);
	};
	return count;
};
