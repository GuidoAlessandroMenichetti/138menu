/****************************************
	*	  138Menu Libre by GUIDOBOT		*
	*	  http://guidobot.hostoi.com	*
	****************************************/

#ifndef MAIN_HEADER
#define MAIN_HEADER

#include <pspkernel.h>
#include <pspdebug.h>
#include "other/vhblBoot.h"
#include "other/callbacks.h"
#include "gb138/resources_138.h"
#include "gb138/138_cfg.h"
#include "gb138/gbCtrl.h"
#include "gb138/menu138.h"

#define VERSION "L2"

config138 cfg;
resources138 res;
gbControl ctrl;

PSP_MODULE_INFO("138MENUCPP", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

int main(int argc, char * argv[]);
int begin();
void finish();

#endif