#include "progress.h"

#include "menu.h"

static t_progress_info progress_info;
static SceUID pr_thread;

int progress_thread(SceSize args, void * argp)
{
	//this will constantly draw the bytes copy progress
	char files[MAX_ON_SCREEN + 1][ENTRY_NAME_LIMIT];
	
	//empty all names in the list
	for(int i = 0; i < MAX_ON_SCREEN + 1; i++) 
		files[i][0] = 0x0;
	
	int x = 0;
	int last = 0;
	
	//select background
	Image * back_show = res.img[MENU_CUSTOMBACK]? res.img[MENU_CUSTOMBACK]: res.img[MENU_BACKGROUND];
	
	sceKernelDelayThread(PROGRESS_SCREEN_DELAY * 4); //delay
	
	while(* progress_info.active == true)
	{
		if(* progress_info.name)
		{
			if(strcmp(* progress_info.name, files[last])) //current file has changed
			{
				last = x;
				strncpy(files[x], * progress_info.name, ENTRY_NAME_LIMIT - 1); //store new name
				x++; //go to next line

				if(x > MAX_ON_SCREEN)
					x = 0;
			};
		};
		
		menu138::clrscr(); //clear screen
		
		//draw everything
		guStart();
		
		menu138::printImg(back_show, 0, 0);
		menu138::printImg(res.img[MENU_LISTBACK], 240 - res.img[MENU_LISTBACK]->imageWidth / 2, 45);
		
		int total_progress = 100;
		
		//draw file names
		for(int i = 0; i < x; i++)
		{
			unsigned color = (i == last)? YELLOW: GREY; //choose color
			
			//calculate progress
			if(i == last)
			{
				if(* progress_info.total) //fix for 0 bytes files
					total_progress = 100.0f * (* progress_info.status) / (* progress_info.total);
			};
			
			//print file name
			menu138::printText(files[i], 60, LIST_START_Y + LIST_SEPARATION * i, color, BLACK, SIZE_LITTLE);
			
			//print progress
			menu138::printImg(res.img[PROGRESS_BACK], 325, LIST_START_Y + LIST_SEPARATION * i - 10);
			blitAlphaImageToScreen(0, 0, total_progress * 100.0f / res.img[PROGRESS_BACK]->imageWidth, res.img[PROGRESS_BACK]->imageHeight, res.img[PROGRESS_BAR], 325, LIST_START_Y + LIST_SEPARATION * i - 10);
		};
		
		menu138::flip(); //swap buffers
		sceKernelDelayThread(PROGRESS_SCREEN_DELAY); //delay
	};
	
	return 0;
};

int start_progress_thread(char ** name, unsigned * state, unsigned * total, bool * active)
{
	pr_thread = sceKernelCreateThread("Progress", progress_thread, 0x18, 0x10000, 0, NULL);

	progress_info.name = name;
	progress_info.status = state;
	progress_info.total = total;
	progress_info.active = active;

	sceKernelStartThread(pr_thread, 0, NULL);
	
	if(pr_thread >= 0)
		return pr_thread;
	
	return 0;
};

void finish_progress_thread()
{
	if(pr_thread >= 0)
		sceKernelTerminateDeleteThread(pr_thread);
	
	pr_thread = 0;
};