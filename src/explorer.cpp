#include "explorer.h"
#include "menu.h"
#include "osk.h"
#include "progress.h"

unsigned explorer138::copy_progress;
unsigned explorer138::copy_total;
char * explorer138::copy_name;
bool explorer138::copy_active;

int explorer138 :: create_dummy(const char * path)
{
	SceUID f = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT, 0777);
	if(f < 0)
		return 0;
		
	sceIoClose(f);
	return 1;
};

unsigned explorer138 :: file_size(const char * path)
{
	//try to open file
	SceUID f = sceIoOpen(path, PSP_O_RDONLY, 0777);
	
	if(f < 0) //failed
		return 0;

	//get file size
	unsigned size = sceIoLseek(f, 0, SEEK_END); 
	
	//close file
	sceIoClose(f);
	
	return size;
};

int explorer138 :: folder_exists(const char * path)
{
	//try to open directory
	SceUID d = sceIoDopen(path);
	
	if(d >= 0) 
	{
		//success
		sceIoDclose(d);
		return 1;
	};
	
	//failed
	return 0;
}

int explorer138 :: file_exists(const char * path)
{
	//try to open file
	SceUID f = sceIoOpen(path, PSP_O_RDONLY, 0777);
	
	if(f >= 0)
	{	
		//success
		sceIoClose(f);
		return 1;
	};
	
	//failed
	return 0;
};

int explorer138 :: delete_file(const char * path)
{
	return sceIoRemove(path) >= 0; //try to remove file
};

int explorer138 :: delete_folder(const char * path)
{
	 //protect some folders
	if(!strcmp(path, "ms0:/PSP/") || !strcmp(path, "ms0:/PSP/GAME/") || !strcmp(path, "ms0:/PSP/LICENSE/"))
		return 0;

	//try to open directory
	SceUID d = sceIoDopen(path);
	
	if(d >= 0)
	{
		SceIoDirent entry;
		memset(&entry, 0, sizeof(SceIoDirent));
		
		//allocate memory to store the full file paths
		char * new_path = new char[strlen(path) + 256];

		//start reading directory entries
		while(sceIoDread(d, &entry) > 0)
		{
			//skip . and .. entries
			if (!strcmp(".", entry.d_name) || !strcmp("..", entry.d_name)) 
			{
				memset(&entry, 0, sizeof(SceIoDirent));
				continue;
			};
			
			//build new file path
			strcpy(new_path, path);
			strcat(new_path, entry.d_name);
			
			if(!delete_file(new_path)) //try to delete file
			{
				//not a file? must be a folder
				strcat(new_path, "/");
				delete_folder(new_path); //try to delete folder content
			};
			
		};
		
		sceIoDclose(d); //close directory
		sceIoRmdir(path); //delete empty folder

		delete [] new_path; //clear allocated memory
	};

	return 1;
};

int explorer138 :: create_folder(const char * path)
{
	return !sceIoMkdir(path, 0); //try to create folder
};

int explorer138 :: rename(const char * old_name, const char * new_name)
{
	 //protect some folders
	if(!strcmp(old_name, "ms0:/PSP") || !strcmp(old_name, "ms0:/PSP/GAME"))
		return 0;

	return sceIoRename(old_name, new_name) >= 0; //try to rename
};

int explorer138 :: copy_bytes(SceUID source, SceUID destination, unsigned bytes)
{
	char buffer[COPY_BUFFER_SIZE];
	
	while(bytes)
	{
		//calculate the read/write size
		int copy_size = bytes > sizeof(buffer)? sizeof(buffer): bytes;
		
		//read bytes to buffer
		if(sceIoRead(source, buffer, copy_size) < copy_size)
			break;
		
		//write bytes to file
		if(sceIoWrite(destination, buffer, copy_size) < copy_size)
			break;
		
		bytes -= copy_size;
		copy_progress += copy_size;
	};
	
	return bytes == 0;
};

int explorer138 :: copy_file(const char * source, const char * destination, unsigned source_offset, unsigned source_size)
{
	if(!strcmp(source, destination)) //paths are the same?
		return 0;
		
	SceUID fd_source, fd_destination;
	
	char * new_destination = new char[strlen(destination) + 0x10];
	strcpy(new_destination, destination);
	
	char * find_name = strstr(new_destination, "EBOOT.PBP"); //find and replace eboot.pbp names
	if(find_name)
		strcpy(find_name, cfg.appName); 
	
	//try to open files
	fd_source = sceIoOpen(source, PSP_O_RDONLY, 0777);
	fd_destination = sceIoOpen(new_destination, PSP_O_WRONLY | PSP_O_CREAT, 0777);
	
	delete [] new_destination;
	
	//some file didnt open properly?
	if(fd_source < 0 || fd_destination < 0)
	{
		sceIoClose(fd_source);
		sceIoClose(fd_destination);
		return 0;
	};
	
	unsigned copy_size;
	
	if(!source_size)
	{
		//get source file size
		copy_size = sceIoLseek(fd_source, 0, SEEK_END);
		
		//go back to start
		sceIoLseek(fd_source, 0, SEEK_SET);
	}
	else
		copy_size = source_size;
	
	if(source_offset)
		sceIoLseek(fd_source, source_offset, SEEK_SET);
	
	//update status
	
	explorer138::copy_name = strrchr(source, '/') + 1;
	explorer138::copy_progress = 0;
	explorer138::copy_total = copy_size;
	
	//copy all the bytes
	int ret = copy_bytes(fd_source, fd_destination, copy_size);
	
	//close files
	sceIoClose(fd_destination);
	sceIoClose(fd_source);
	
	return ret;
};

int explorer138 :: copy_folder(const char * source, const char * destination)
{
	/*
	arg1: ms0:/stuff/
	arg2: ms0:/PSP/GAME/
	
	will copy as
	ms0:/PSP/GAME/stuff/...
	*/

	if(!strcmp(source, destination) || !strcmp(destination, "ms0:/PSP/GAME/"))
		return 0;
	
	if(!strncmp(source, destination, strlen(source))) //avoid inception
		return 0;
	
	//get a pointer to the folder name
	char * new_folder = strrchr(source, '/') - 1;
	while(* new_folder != '/')
		new_folder--;
	new_folder++;

	char * new_destination = new char[strlen(destination) + 256];
	
	//create destination folder if it doesnt exist
	create_folder(destination);
	
	//create new dir on destination
	strcpy(new_destination, destination);
	strcat(new_destination, new_folder);
	
	int ret = copy_folder_recursive(source, new_destination);
	
	delete [] new_destination;
	
	return ret;
};

int explorer138 :: copy_folder_recursive(const char * source, const char * destination)
{
	//create new folder
	create_folder(destination);
	
	char * new_destination = new char[strlen(destination) + 256];
	strcpy(new_destination, destination);
	
	//store where the new names should be copied
	char * copy_name = new_destination + strlen(destination);
	
	//try to open source folder
	SceUID dir = sceIoDopen(source);
	
	if(dir >= 0)
	{
		SceIoDirent entry;
		memset(&entry, 0, sizeof(SceIoDirent));
		
		char * read_path = new char[strlen(source) + 256];
		
		//start reading directory entries
		while(sceIoDread(dir, &entry) > 0)
		{
			//skip . and .. entries
			if (!strcmp(".", entry.d_name) || !strcmp("..", entry.d_name)) 
			{
				memset(&entry, 0, sizeof(SceIoDirent));
				continue;
			};
		
			//build read path
			strcpy(read_path, source);
			strcat(read_path, entry.d_name);
		
			//pspDebugScreenPrintf("file copy from: %s\n", read_path);
		
			strcpy(copy_name, entry.d_name); //new file

			//pspDebugScreenPrintf("to %s\n", new_destination);
			
			if(file_exists(read_path)) //is it a file
				copy_file(read_path, new_destination); //copy file
			else
			{
				//try to copy as a folder
				strcat(new_destination, "/");
				strcat(read_path, "/");
				copy_folder_recursive(read_path, new_destination);
			};

		};
		
		delete [] read_path;
		
		//close folder
		sceIoDclose(dir);
	};
	
	//free allocated path
	delete [] new_destination;
	
	return 1;
};

int explorer138 :: move_file(const char * source, const char * destination)
{
	if(copy_file(source, destination)) //copy file to destination
		return delete_file(source); //remove source file
	
	return 0;
};

int explorer138 :: move_folder(const char * source, const char * destination)
{
	if(copy_folder(source, destination)) //copy folder
		return delete_folder(source); //delete source
		
	return 0;
};

explorer138 :: explorer138()
{
	current_path = NULL;
	active = false;
	list_option = offset = 0;
};

explorer138 :: ~explorer138()
{
	clear();
};

void explorer138 :: clear()
{
	//clean lists
	files.clear();
	clipboard.clear();
	
	if(current_path)
		delete [] current_path;
		
	current_path = NULL;
};

void explorer138 :: browse(const char * path)
{
	//copy new path
	char * new_path = new char[strlen(path) + 1];
	strcpy(new_path, path);
	
	//delete previous path
	if(current_path)
		delete [] current_path;
	
	//assign new path
	current_path = new_path;
	
	//clean file list
	files.clear();
	files.load(new_path);
	
	offset = list_option = 0;
};

void explorer138 :: refresh()
{
	browse(current_path);
};

void explorer138 :: update_language()
{
	tool_options[COPY].name = lang_explorer_tools[cfg.lang][0];
	tool_options[CUT].name = lang_explorer_tools[cfg.lang][1];
	tool_options[PASTE].name = lang_explorer_tools[cfg.lang][2];
	tool_options[DELETE].name = lang_explorer_tools[cfg.lang][3];
	tool_options[RENAME].name = lang_explorer_tools[cfg.lang][4];
	tool_options[MKDIR].name = lang_explorer_tools[cfg.lang][5];
	tool_options[CANCEL].name = lang_explorer_tools[cfg.lang][6];
	tool_options[RETURN].name = lang_explorer_tools[cfg.lang][7];
};

void explorer138 :: draw_all()
{
	menu138 * menu = (menu138 *)owner;

	//draw menu graphics
	guStart();
	menu->drawImages();
	menu->drawText();
	
	//draw explorer graphics
	draw();
	if(tools.enabled)
		tools_draw();
};

int explorer138 :: show(const void * owner)
{
	active = true;
	tools.enabled = false;
	tools.command = NONE;
	this->owner = owner;

	update_language();
	
	while(active == true)
	{
		ctrl.update();
		if(tools.enabled == false)
			check(); //check button press
		else
			tools_check(); //check button press on toolbox
		
		menu138::clrscr(); //clean screen
		
		draw_all();
		
		menu138::flip();
	};
	
	//ctrl.update();
	
	//we dont want to keep files in clipboard
	clipboard.clear();
	
	return 1;
};

int explorer138 :: show_image(const char * path)
{
	//try to load image
	Image * image = loadImage(path, 0);
	
	if(!image)
		return 0;
	
	if(image->imageWidth <= 480 && image->imageHeight <= 272)
	{	
		//clean screen
		menu138::clrscr(); 
		guStart();
		
		//print image
		menu138::printImg(image, 240 - image->imageWidth / 2, 136 - image->imageHeight / 2);
		
		//print name
		menu138::printText(strrchr(path, '/') + 1,  40, 230, WHITE, BLACK, SIZE_LITTLE);
		
		menu138::flip();

		//wait for button input
		bool finished = false;
		do
		{
			ctrl.update();
			if(ctrl.get()->circle)
				finished = true;
				
		} while(!finished);
		
	};
	
	//free image
	freeImage(image);
	
	ctrl.waitButtonsPressed();
	
	return 1;
};

void explorer138 :: dofile(const char * path)
{
	int type = entry138::get_file_type(path); //get file magic/type
	
	if(type == PNG) //is it a png file?
		show_image(path); //open image viewer
	else
	{
		//try to create new entry
		entry138 * new_entry = new entry138();
		
		if(new_entry->create(path)) //entry is valid?
		{
			//load file
			if(new_entry->getType() != ZIP)
			{
				menu138 * menu = (menu138 *)owner;
				
				//load app and close explorer
				menu->load_app(new_entry);
				active = false;
			};
		};
		
		delete new_entry;
	};	
	
	ctrl.waitButtonsPressed();
};

void explorer138 :: tools_docommand(int command)
{
	int total_in_clipboard = clipboard.get_count();
	
	char destination[1024];
	strcpy(destination, current_path);
	
	bool update_games = false;
	bool update_installers = false;
	
	if(!strncmp(current_path, cfg.appPath, strlen(cfg.appPath)))
		update_games = true;
	if(!strncmp(current_path, INSTALLERS_PATH, strlen(INSTALLERS_PATH)))
		update_installers = true;
	
	switch(command)
	{
		case RETURN:
			return;
		case CANCEL:
			break;
		case PASTE:
		{
			copy_active = true;
			explorer138::copy_name = NULL;
			
			start_progress_thread(&explorer138::copy_name, &explorer138::copy_progress, &explorer138::copy_total, &explorer138::copy_active);

			for(int i = 0; i < total_in_clipboard; i++)
			{
				t_file * file = clipboard.get(i);
				
				strcpy(destination, current_path);	
				
				if(tools.command == COPY)
				{
					if(file->type == filelist138::FILES)
					{
						strcat(destination, strrchr(file->path, '/') + 1);
						copy_file(file->path, destination);
					}
					else
						copy_folder(file->path, destination);
				}
				else if(tools.command == CUT)
				{
					if(file->type == filelist138::FILES)
					{
						strcat(destination, strrchr(file->path, '/') + 1);
						move_file(file->path, destination);
					}
					else
						move_folder(file->path, destination);
				};
				
				//update lists?
				if(!strncmp(cfg.appPath, file->path, strlen(cfg.appPath)))
					update_games = true;
				else if(!strncmp(INSTALLERS_PATH, file->path, strlen(INSTALLERS_PATH)))
					update_installers = true;
				
			};
			
			copy_active = false;
			finish_progress_thread();
			
			break;
		}
		case MKDIR:
		{
			osk138 osk;
			osk.activate(tool_options[MKDIR].name, "New Folder", 26);
			if(!osk.Cancelled())
			{
				strcat(destination, osk.getOutput());
				strcat(destination, "/");
				create_folder(destination);
			};

			browse(current_path);
			return;
		}
		case RENAME:
		{
			t_file * file = files.get(list_option);
			char * find_slash = strchr(file->name, '/');
			if(find_slash)
				* find_slash = 0x0;
			
			osk138 osk;
			osk.activate(tool_options[RENAME].name, file->name, 26);
			
			if(!osk.Cancelled())
			{
				strcat(destination, osk.getOutput());
				
				if(find_slash)
				{
					find_slash = strrchr(file->path, '/');
					* find_slash = 0x0;
				};
				
				rename(file->path, destination);
				
			};

			browse(current_path);
			
			return;
		}
		case DELETE:
			for(int i = 0; i < total_in_clipboard; i++)
			{
				t_file * file = clipboard.get(i);
			
				if(file->type == filelist138::FILES)
					delete_file(file->path);
				else
					delete_folder(file->path);
			};
			
			break;
			
		case COPY:
		case CUT:
			
			tools.command = command; //store current command
			return;
	};
	
	//refresh lists if the operation affected something important
	menu138 * menu = (menu138 *)owner;
	
	if(update_games == true)
		menu->updateGameList();
	if(update_installers == true)
		menu->updateInstallerList();
	
	tools.command = NONE;
	clipboard.clear();
	browse(current_path);
};

void explorer138 :: tools_check()
{
	//exit tools
	if(ctrl.get()->circle || ctrl.get()->triangle)
		tools.enabled = false;
	
	if(ctrl.get()->cross)
	{
		tools_docommand(tool_options[tools.option].command);
		tools.enabled = false;
	};
		
	//scroll tools
	if(ctrl.get()->down)
	{	
		do
		{
			tools.option++;
			if(tools.option > tool_count)
				tools.option = 0;
		} while(tool_options[tools.option].enabled == 0);
	}
	else if(ctrl.get()->up)
	{
		do
		{
			tools.option--;
			if(tools.option < 0)
				tools.option = tool_count - 1;
		} while(tool_options[tools.option].enabled == 0);
	};
};

void explorer138 :: tools_draw()
{
	//tools back
	menu138::printImg(res.img[EXPLORER_TOOLS], TOOLS_X - 5, LIST_START_Y + 3 + LIST_SEPARATION);

	for(int i = 0; i < tool_count; i++)
	{
		//define color
		unsigned color = (tools.option == i)? YELLOW: (tool_options[i].enabled)? WHITE: GREY;
		
		//draw text
		menu138::printText(tool_options[i].name, TOOLS_X, TOOLS_Y + 2 * LIST_SEPARATION + LIST_SEPARATION * i, color, BLACK, SIZE_LITTLE);
	};
};

void explorer138 :: tools_open()
{
	//disable all tools
	for(int i = 0; i < tool_count; i++)
		tool_options[i].enabled = 0;
	
	//enable certain tools depending on the situation
	tool_options[RETURN].enabled = 1;

	if(strncmp(current_path, "ms0:/PSP/GAME/", 14)) //no write access here
	{
		tool_options[MKDIR].enabled = 1;
		
		if(files.get_count())
			tool_options[RENAME].enabled = 1;
		
		if(tools.command == COPY || tools.command == CUT)
		{
			tool_options[PASTE].enabled = 1;
			tool_options[CANCEL].enabled = 1;
			tool_options[COPY].enabled = 0;
		}
		else
		{
			if(clipboard.get_count())
			{
				tool_options[DELETE].enabled = 1;
				tool_options[CUT].enabled = 1;
				tool_options[COPY].enabled = 1;
			};
		};
	}
	else
	{
		if(clipboard.get_count())
			tool_options[COPY].enabled = 1;
	};

	tools.enabled = true;
	tools.option = 0;
	
	//search first enabled option
	while(tool_options[tools.option].enabled == 0)
		tools.option++;
};

void explorer138 :: check()
{
	//exit explorer
	if(ctrl.get()->rtrigger || ctrl.get()->ltrigger)
	{
		active = false;
		return;
	};
	
	if(ctrl.get()->circle)
	{
		if(!strcmp("ms0:/", current_path)) //pressed circle in root folder
		{
			active = false;
			return;
		}
		else //go to parent folder
		{
			*(strrchr(current_path, '/')) = 0x0;
			*(strrchr(current_path, '/') + 1) = 0x0;
			browse(current_path);
		};
		
		if(tools.command == NONE) //clean pending selected files if there is no operation
			clipboard.clear();
	};
	
	t_file * file = files.get(list_option);
	
	//show/hide tools
	if(ctrl.get()->triangle)
		tools_open();

	//open item
	if(files.get_count())
	{
		
		//select/unselect item
		if(ctrl.get()->square)
		{
			if(tools.command != NONE)
			{
				clipboard.clear();
				tools.command = NONE;
			};
		
			if(clipboard.find_file(file->path) >= 0) //is this already in clipboard?
				clipboard.delete_file(file->path); //remove from clipboard
			else //add to clipboard
				clipboard.add_file(file->path, file->name, file->type, file->size);
		};
		
		if(ctrl.get()->cross)
		{
			//get current file info
			if(file->type == filelist138::FOLDER) //browse it if its a folder
			{
				browse(file->path);
				if(tools.command == NONE) //clean pending selected files if there is no operation
					clipboard.clear();
			}
			else
				dofile(file->path); //try to open this file in any way
		};
		
		//files scrolling
		if(ctrl.get()->down)
		{
			if(list_option < files.get_count() - 1)
			{
				if(++list_option == MAX_ON_SCREEN_EXPLORER + offset) 
					offset++;
			}
			else 
				list_option = offset = 0;
		}
		else if(ctrl.get()->up)
		{
			if(list_option > 0)
			{
				list_option--;
				if(offset && (list_option == offset - 1)) //adjust offset
					offset--;
			}
			else
			{
				list_option = files.get_count() - 1; //go to last entry
				if(list_option >= MAX_ON_SCREEN_EXPLORER) //adjust offset
					offset = files.get_count() - MAX_ON_SCREEN_EXPLORER;
			};
		};
	};
	
};

void explorer138 :: draw()
{
	//choose how to show the current path
	char * path_show = current_path;
	while(strlen(path_show) > PATH_NAME_LIMIT)
		path_show = strchr(path_show + 1, '/'); //seek next '/'

	//print path
	menu138::printText(path_show, LIST_START_X, LIST_START_Y, WHITE, BLACK, SIZE_LITTLE);
	
	int items_show = (files.get_count() > MAX_ON_SCREEN_EXPLORER)? MAX_ON_SCREEN_EXPLORER: files.get_count();
	
	//draw files
	for(int i = 0; i < items_show; i++)
	{
		//get current file info
		t_file * file = files.get(i + offset);
		
		//choose file color
		unsigned color_text = WHITE;
		int move_x = 0;
		
		//is this file stored in the clipboard?
		if(clipboard.find_file(file->path) >= 0)
		{
			color_text = WEIRD_ORANGE;
			move_x += 20;
			
			//print checkbox
			menu138::printImg(res.img[EXPLORER_CHECK], LIST_START_X, 5 + LIST_START_Y + LIST_SEPARATION + LIST_SEPARATION * i);
		};
		
		if((list_option == i + offset)) //is this the current file?
			color_text = YELLOW;
			
		int icon = EXPLORER_FOLDER;
		
		//print file name
		menu138::printText(file->name, LIST_START_X + move_x + 20, LIST_START_Y + 2 * LIST_SEPARATION + LIST_SEPARATION * i, color_text, BLACK, SIZE_LITTLE);

		//special behaviour for files
		if(file->type == filelist138::FILES)
		{
			icon = EXPLORER_FILE;
		
			//show size (only for files)
			if(tools.enabled == false)
			{
				unsigned size_show = file->size;
				int divided = 0;
				
				while(size_show > 1024 * 10)
				{
					size_show /= 1024;
					divided++;
				};

				//convert integer to string
				char size_string[10];
				sprintf(size_string, "%d", size_show);
				
				//print size
				menu138::printText(size_string, LIST_START_X + 300, LIST_START_Y + 2 * LIST_SEPARATION + LIST_SEPARATION * i, GREY, BLACK, SIZE_LITTLE);
				
				static char size_caption[][4] = {"B", "KB\0", "MB\0", "GB\0"};
				
				//print unit
				menu138::printText(size_caption[divided], LIST_START_X + 340, LIST_START_Y + 2 * LIST_SEPARATION + LIST_SEPARATION * i, GREY, BLACK, SIZE_LITTLE);
			};
		};
		
		//print icon
		menu138::printImg(res.img[icon], LIST_START_X + move_x, 5 + LIST_START_Y + LIST_SEPARATION + LIST_SEPARATION * i);

	};
};