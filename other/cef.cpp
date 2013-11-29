#include "cef.h"
#ifdef CEF
int cef_launch(char * file, int type, int patched)
{
	int runlevel;	
	SceKernelLoadExecVSHParam param;
	memset(&param, 0, sizeof(param));
	param.size = sizeof(param);
	char disc[60];
	if(!patched) strcpy(disc, "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN");
	else strcpy(disc, "disc0:/PSP_GAME/SYSDIR/EBOOT.OLD");
	
	char * exec = file;
	
	switch(type)
	{
		param.size = sizeof(param);
		case entry138::HOMEBREW:
			runlevel = HOMEBREW_RUNLEVEL;
			param.args = strlen(file) + 1;
			param.argp = file;
			param.key = "game";
		break;
		case entry138::POPS:
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
		case entry138::CSO:
		case entry138::ISO:
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

		break;
		default:
			return 0;
	};
	sctrlKernelLoadExecVSHWithApitype(runlevel, exec, &param);
	return 1;
};

#endif