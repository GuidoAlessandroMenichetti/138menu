#ifndef OSK_HEADER
#define OSK_HEADER

#include <pspgum.h>
#include <string.h>
#include <stdio.h>
#include "gbCtrl.h"
#include "resources_138.h"
extern "C"
{
#include "../graphics/graphics.h"
}

extern resources138 res; 
extern gbControl ctrl;

static const char keyboard[][4][13] = 
{
	{"1234567890<\0", "QWERTYUIOP\0", "ASDFGHJKL\0 ", "^ZXCVBNM,.\0"}, 
	{"1234567890<\0", "qwertyuiop\0", "asdfghjkl\0 ", "^zxcvbnm,.\0"}
}, 	special_chars[] = {"iIljftr"};

class osk138
{
	public:

	enum OSK_CONSTANTS {MAX_BUFFER = 30};

	osk138();
	void activate(const char * message, const char * value, int limit);
	char * getOutput();
	int Cancelled();

	private:
	void work();
	void draw();
	void shutdown();
	void check();
	void addChar(char character);
	void printChar(char character, int x, int y, unsigned color, unsigned shadow, float size);

	char caption[MAX_BUFFER];
	char default_value[MAX_BUFFER];
	char input[MAX_BUFFER];
	int limit, animation,
		enabled, upper, cancel,
		y, x, len;
};

#endif