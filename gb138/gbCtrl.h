#ifndef CONTROLER_HEADER
#define CONTROLER_HEADER

#include <pspctrl.h>
#include <string.h>

#define WAIT_TIME 10

typedef struct
{
	int cross, square, circle, triangle,
		up, down, left, right,
		ltrigger, rtrigger, start, select;

	unsigned char xstick, ystick;
} control;

class gbControl
{
	public:
	
	void update();
	void flush();
	int isPressed();
	void setWait(int n);
	void restoreWait();
	control * getCtrl();
	gbControl();
	
	private:
	control ctrl;
	int pressed, wait, get, wait_time;
};

#endif