#ifndef CONTROLER_HEADER
#define CONTROLER_HEADER

#include "common.h"

#define WAIT_TIME 10

typedef struct
{
	int cross, square, circle, triangle,
		up, down, left, right,
		ltrigger, rtrigger, start, select;

	unsigned char xstick, ystick;
} control;

class input138
{
	public:

	input138();
	void update();
	void flush();
	int isPressed();
	
	void waitButtonsPressed();
	control * get();
	
	private:
	
	void setWait(int n);
	void restoreWait();
	
	control ctrl;
	int pressed, wait, read, wait_time;
};

#endif