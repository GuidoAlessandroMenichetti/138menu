#ifndef CEF_HEADER
#define CEF_HEADER

#include "../config.h"
#include "../gb138/138_entry.h"
#include <pspkernel.h>
#include <psploadexec_kernel.h>
#include <pspinit.h>
#include <pspumd.h>

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

int cef_launch(char * file, int type, int patched);

#endif