#ifndef OTHER_H
#define OTHER_H

#include "common.h"
#include "entry.h"

//vhbl related
typedef struct 
{
	unsigned long APIVersion;
	char Credits[32];
	char VersionName[32];
	char *BackgroundFilename;
	char *filename;
} tMenuApi;

int xstrtoi(char * str, int len);

//for callback 
int exit_callback(int arg1, int arg2, void * common);
int CallbackThread(SceSize args, void * argp);
int SetupCallbacks();

//launcher related
#ifdef ARK
#include <ark_pspsystemctrl_user.h>
#endif
#ifdef TN
#include <pspsystemctrl_user.h>
#endif

#define HOMEBREW_RUNLEVEL 0x141
#define ISO_RUNLEVEL 0x123
#define POPS_RUNLEVEL 0x144
#define PSN_RUNLEVEL ISO_RUNLEVEL

int cef_launch(char * file, int type, int extra);

#endif