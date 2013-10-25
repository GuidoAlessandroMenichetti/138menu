#ifndef ARK_HEADER
#define ARK_HEADER

#define HOMEBREW_RUNLEVEL 0x141
#define ISO_RUNLEVEL 0x123
#define POPS_RUNLEVEL 0x144

#include "../gb138/138_entry.h"
#include <pspkernel.h>
#include <psploadexec_kernel.h>
#include <pspsystemctrl_user.h>

enum launch_types
{
	HOMEBREW = 0,
	POPS = 1,
	ISO = 2,
	ISO_PATCHED = 3,
	PSN = 4
};

int ark_launch(char * file, int type, int patched);

#endif