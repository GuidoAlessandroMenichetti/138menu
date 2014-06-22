#ifndef COMMON_H
#define COMMON_H

//headers used by most of the source
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspgum.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <math.h>
#include <pspctrl.h>
#include <psploadexec_kernel.h>
#include <pspinit.h>
#include <pspumd.h>
#include <vector>
#include <algorithm>

#include "../config.h"
#include "design.h"
#include "lang.h"

//various settings
#define CREDITS "138Menu by GUIDOBOT"
#define CFG_FILE "138CFG.TXT" //config file name
#define ISO_PATH "ms0:/ISO/" //backups path
#define INSTALLERS_PATH "ms0:/PSP/SAVEDATA/" //installers path
#define DEFAULT_PATH "ms0:/PSP/VHBL/" //eboot/games path
#define ZIP_NAME "INSTALL.ZIP" //name for installers
#define DEFAULT_NAME "wmenu.bin" //default app name
#define DEFAULT_THEME "DATA.138" //main theme name
#define DEFAULT_BACK "DEFAULT" //default background name
#define TEMPORAL_ICON "ms0:/tmp_ICON0.PNG" //temporal icon to be extracted from a CSO
#define TEMPORAL_PARAM "ms0:/tmp_PARAM.SFO" //temporal sfo for CSOs
#define TEMPORAL_FONT "FONT.TMP" //temporal font name extracted from theme
#define EXPLORER_PATH "ms0:/" //default explorer start path

#define MAX_MESSAGE	0x80 //message text arrays
#define MAX_EBOOT_NAME 9 //executable apps name (EBOOT.PBP)
#define MAX_PATH_NAME 4 //install path name (VHBL, GAME, etc)

#define TOGGLE(A) ((A) = (A)? 0: 1) //boolean toggle
#define THREAD_DELAY 1000
#define PROGRESS_SCREEN_DELAY 50000//1000000
#define PI 3.14159265

#define BUILD_VHBL "VHBL"
#define BUILD_ARK "ARK CEF"
#define BUILD_TN "TN CEF"

#if defined(ARK) && defined(TN)
#undef TN
#endif

#if defined(ARK) || defined(TN)
#define CEF
#endif

enum E_FILE_TYPES //files magic number
{
	PBP = 0x50425000,
	ZIP = 0x04034B50,
	SFO = 0x46535000,
	ISO = 0x30444301,
	CSO = 0x4F534943,
	PNG = 0x474E5089
};

using namespace std;

#endif