#include "ark.h"

int ark_launch(char * file, int type, int patched)
{
	int runlevel;	
	SceKernelLoadExecVSHParam param;
	memset(&param, 0, sizeof(param));
	param.size = sizeof(param);
	char disc[60];
	
	if(!patched) strcpy(disc, "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN");
	else strcpy(disc, "disc0:/PSP_GAME/SYSDIR/EBOOT.OLD");
	
	switch(type)
	{
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
			runlevel = ISO_RUNLEVEL;
			param.args = strlen(disc) + 1;
			param.argp = disc;
			param.key = "umdemu";
			sctrlSESetBootConfFileIndex(MODE_NP9660);
			sctrlSESetUmdFile("");
		break;
		case entry138::CSO:
		case entry138::ISO:
			runlevel = ISO_RUNLEVEL;
			param.args = strlen(disc) + 1;
			param.argp = disc;
			param.key = "umdemu";
			sctrlSESetBootConfFileIndex(MODE_INFERNO);
			sctrlSESetUmdFile(file);
		break;
		default:
			return 0;
	};
	sctrlKernelLoadExecVSHWithApitype(runlevel, file, &param);
	return 1;
};