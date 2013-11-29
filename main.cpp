#include "main.h"

void finish()
{
	cfg.clear();
	res.clear();
};

int begin()
{
	SetupCallbacks();
	pspDebugScreenInit();
	pspDebugScreenClear();
	
	const char * build;
#ifdef ARK
	build = BUILD_ARK;
#else
#ifdef TN
	build = BUILD_TN;
#else
	build = BUILD_VHBL;
#endif
#endif	

	
	pspDebugScreenPrintf("Starting 138Menu %s [%s] by GUIDOBOT\n", VERSION, build);
	pspDebugScreenPrintf("Loading config file\n");
	
	if(cfg.start()==config138::GENERATED)
		pspDebugScreenPrintf("Config file not found or corrupted. Generating defaults...\n");
		
	pspDebugScreenPrintf("Loading menu resources...\n");
	if(res.start()==resources138::RESOURCES_ERROR)
	{
		pspDebugScreenPrintf("ERROR: Resources missing/invalid!\n");
		sceKernelDelayThread(3000000);
		return 0;
	};
	pspDebugScreenPrintf("Resources loaded!\n");
	return 1;
};

int main(int argc, char * argv[])
{
	int apiAddr = 0;
	char * ebootPath = NULL;
	tMenuApi * settings;
	
	if(argc>1)
	{
		char * hex = argv[1];
        *(hex + 8) = 0;
        apiAddr = xstrtoi(hex, 8);	
	};
	
	if(apiAddr)
	{
		settings = (tMenuApi*) apiAddr;
		ebootPath = settings->filename;
	};

	
	if(begin())
	{
		menu138 menu;
#ifndef CEF
		menu.setEbootAddress(ebootPath);
#endif
		menu.setExploitPath(argv[0]);
		menu.start();
	};

	finish();
	sceKernelExitGame();
    return 0;
};
