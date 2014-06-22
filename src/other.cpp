#include "other.h"

int exit_callback(int arg1, int arg2, void *common) 
{
	sceKernelExitGame();
	return 0;
};

int CallbackThread(SceSize args, void *argp) 
{
	int cbid;
	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
};

int SetupCallbacks() 
{
	int thid = 0;
	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0) sceKernelStartThread(thid, 0, 0);
	return thid;
};

int xstrtoi(char * str, int len) 
{
	int val;
	int c;
	int i;
	val = 0;
	for (i = 0; i < len; i++){
		c = *(str + i);
		if (c >= '0' && c <= '9') {
			c -= '0';
		} else if (c >= 'A' && c <= 'F') {
			c = (c - 'A') + 10;
		} else if (c >= 'a' && c <= 'f') {
			c = (c - 'a') + 10;
		} else {
			return 0;
		}
		val *= 16;
		val += c;
	}
	return val;
};

#ifdef CEF

int cef_launch(char * file, int type, int extra)
{
	int runlevel = 0;	
	
	//create loadexec struct
	SceKernelLoadExecVSHParam param;
	memset(&param, 0, sizeof(param));
	param.size = sizeof(param);
	
	char disc[34];
	char * exec = file;
	
	strcpy(disc, "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN");
	
	if(type == PBP) //pbp files
	{
		switch(extra)
		{
			case entry138::APP:
				runlevel = HOMEBREW_RUNLEVEL;
				param.args = strlen(file) + 1;
				param.argp = file;
				param.key = "game";
			
				break;
				
			case entry138::PS1:
				runlevel = POPS_RUNLEVEL;
				param.args = strlen(file) + 1;
				param.argp = file;
				param.key = "pops";
				
				break;
			
			case entry138::PSN:
				runlevel = PSN_RUNLEVEL;
				param.args = strlen(disc) + 1;
				param.argp = disc;
				param.key = "umdemu";
				
#ifdef ARK
				sctrlSESetBootConfFileIndex(MODE_NP9660);
				sctrlSESetUmdFile("");
#endif

#ifdef TN
				sctrlSEMountUmdFromFile(file, MODE_NP9660);
#endif
				break;
		};
	}
	else //backups
	{
		if(extra == entry138::PATCHED)
			strcpy(disc, "disc0:/PSP_GAME/SYSDIR/EBOOT.OLD");

		runlevel = ISO_RUNLEVEL;
		param.args = strlen(disc) + 1;
		param.argp = disc;	
		
#ifdef ARK
		param.key = "umdemu";
		sctrlSESetBootConfFileIndex(MODE_INFERNO);
		sctrlSESetUmdFile(file);
#endif

#ifdef TN

		param.key = "game";
		sctrlSEMountUmdFromFile(file, MODE_MARCH33);
		exec = disc;
#endif		

	};
	
	//start app
	return sctrlKernelLoadExecVSHWithApitype(runlevel, exec, &param);
};

#endif