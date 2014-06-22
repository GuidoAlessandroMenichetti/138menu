#ifndef PROGRESS_H
#define PROGRESS_H

#include "common.h"
#include "resources.h"

extern resources138 res;

extern "C"
{
#include "graphics/graphics.h"
#include "graphics/intraFont.h"
};

typedef struct s_progress_info
{
	char ** name;
	unsigned * status;
	unsigned * total;
	bool * active;
} t_progress_info;

int progress_thread(SceSize args, void * argp);
int start_progress_thread(char ** name, unsigned * state, unsigned * total, bool * active);
void finish_progress_thread();

#endif