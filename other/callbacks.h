#ifndef CALLBACKS_HEADER
#define CALLBACKS_HEADER

#include <pspkernel.h>

int exit_callback(int arg1, int arg2, void * common);
int CallbackThread(SceSize args, void * argp);
int SetupCallbacks();

#endif