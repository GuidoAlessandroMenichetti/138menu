#include "src/common.h"
#include "src/other.h"
#include "src/resources.h"
#include "src/cfg.h"
#include "src/ctrl.h"
#include "src/menu.h"
#include "src/graphics/graphics.h"

PSP_MODULE_INFO("138MENUCPP", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

config138 cfg;
resources138 res;
input138 ctrl;

//store here the final result
char app_start_path[256];
int app_start_type;
int app_start_extra;

int begin()
{
	//start callbacks
	SetupCallbacks();
	
	//start debug screen
	pspDebugScreenInit();
	pspDebugScreenClear();
	
	//define the menu version (vhbl/cef) to print
	const char * build = BUILD_VHBL;
	
#ifdef ARK
	build = BUILD_ARK;
#else
#ifdef TN
	build = BUILD_TN;
#endif	
#endif

	pspDebugScreenPrintf("Starting 138Menu %s [%s] by GUIDOBOT\n", VERSION, build);
	pspDebugScreenPrintf("Loading config file\n");
	
	//try to load/generate config file
	if(cfg.start() == config138::GENERATED)
		pspDebugScreenPrintf("Config file not found or corrupted. Default file created...\n");
	
	//load resources
	pspDebugScreenPrintf("Loading menu resources...\n");
	
	if(!res.start())
	{
		//couldnt load all needed resources. Stop menu
		pspDebugScreenPrintf("ERROR: Resources missing/invalid!\n");
		sceKernelDelayThread(3000000);
		
		return 0;
	};
	
	//success
	pspDebugScreenPrintf("Resources loaded!\n");
	
	//try to start gu
	pspDebugScreenPrintf("Starting graphics...\n");
	initGraphics();
	
	return 1;
};

int main(int argc, char * argv[])
{
#ifndef CEF

	char * ebootPath = NULL;
	
	if(argc > 1) //parse second argument to get the vhbl eboot load path (hbl)
	{
		int apiAddr = 0;
		char * hex = argv[1];
		
        *(hex + 8) = 0;
        apiAddr = xstrtoi(hex, 8);
		
		if(apiAddr)
		{
			tMenuApi * settings;
			settings = (tMenuApi *)apiAddr;
			ebootPath = settings->filename;
		};
	};
	
#endif

	* app_start_path = 0x0;

	//load needed files
	if(begin())
	{
		menu138 menu; //create menu object

		*(strrchr(argv[0], '/') + 1) = 0x0;
		menu.set_app_path(argv[0]); //store app path
		menu.start(); //start menu
	};

	//clear any loaded resource
	cfg.clear();
	res.clear();
	
	if(* app_start_path)
	{
	
#ifndef CEF
		//for hbl
		strcpy(ebootPath, app_start_path);
#else 
		//for cef
		cef_launch(app_start_path, app_start_type, app_start_extra);
#endif

	};
	
	sceKernelExitGame();
	
    return 0;
};
