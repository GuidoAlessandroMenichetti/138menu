#include "ctrl.h"

input138 :: input138()
{
	flush();
	wait = 0;
	read = 1;
	restoreWait();
};

int input138 :: isPressed()
{
	return pressed;
};

control * input138 :: get()
{
	return &ctrl;
};

void input138 :: flush()
{
	memset(&ctrl, 0, sizeof(control));
};

void input138 :: setWait(int n)
{
	wait_time = n;
};

void input138 :: restoreWait()
{
	wait_time = WAIT_TIME;
};

void input138 :: waitButtonsPressed()
{
	SceCtrlData entry;
	do
	{
		sceCtrlReadBufferPositive(&entry, 1);
	} while(entry.Buttons); //wait until there is no input
};

void input138 :: update()
{
	SceCtrlData entry;
	sceCtrlReadBufferPositive(&entry, 1);
	
	if(entry.Buttons) 
		pressed = 1;
		
	if(read)
	{		
		if(entry.Buttons) //there is input
		{
			ctrl.cross = entry.Buttons & PSP_CTRL_CROSS;
			ctrl.circle = entry.Buttons & PSP_CTRL_CIRCLE;
			ctrl.square = entry.Buttons & PSP_CTRL_SQUARE;
			ctrl.triangle = entry.Buttons & PSP_CTRL_TRIANGLE;
			ctrl.ltrigger = entry.Buttons & PSP_CTRL_LTRIGGER;
			ctrl.rtrigger = entry.Buttons & PSP_CTRL_RTRIGGER;
			ctrl.start = entry.Buttons & PSP_CTRL_START;
			ctrl.select = entry.Buttons & PSP_CTRL_SELECT;
			ctrl.up = entry.Buttons & PSP_CTRL_UP;
			ctrl.down = entry.Buttons & PSP_CTRL_DOWN;
			ctrl.left = entry.Buttons & PSP_CTRL_LEFT;
			ctrl.right = entry.Buttons & PSP_CTRL_RIGHT;
			read = wait = 0;
		}
		else //no input
		{
			pressed = 0;
			read = 1;
		};
		
		//get sticks position
		ctrl.xstick = entry.Lx;
		ctrl.ystick = entry.Ly;
	}
	else
	{
		if(!wait)
			flush();
			
		if(wait < wait_time) 
			wait++;
		else 
			read = 1;
			
		if(!entry.Buttons) 
			read = 1;
	};
};