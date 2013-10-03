#include "gbCtrl.h"

gbControl :: gbControl()
{
	this->flush();
	wait = 0;
	get = 1;
	this->restoreWait();
};

int gbControl :: isPressed()
{
	return this->pressed;
};

control * gbControl :: getCtrl()
{
	return &this->ctrl;
};

void gbControl :: flush()
{
	memset(&this->ctrl, 0, sizeof(control));
};

void gbControl :: setWait(int n)
{
	this->wait_time = n;
};

void gbControl :: restoreWait()
{
	this->wait_time = WAIT_TIME;
};

void gbControl :: update()
{
	SceCtrlData entry;
	sceCtrlReadBufferPositive(&entry, 1);
	if(entry.Buttons) pressed = 1;
	if(get)
	{		
		if(entry.Buttons)
		{
			this->flush();
			if(entry.Buttons & PSP_CTRL_CROSS) ctrl.cross = 1;
			if(entry.Buttons & PSP_CTRL_CIRCLE) ctrl.circle = 1;
			if(entry.Buttons & PSP_CTRL_SQUARE) ctrl.square = 1;
			if(entry.Buttons & PSP_CTRL_TRIANGLE) ctrl.triangle = 1;
			if(entry.Buttons & PSP_CTRL_LTRIGGER) ctrl.ltrigger = 1;
			if(entry.Buttons & PSP_CTRL_RTRIGGER) ctrl.rtrigger = 1;
			if(entry.Buttons & PSP_CTRL_START) ctrl.start = 1;
			if(entry.Buttons & PSP_CTRL_SELECT) ctrl.select = 1;
			if(entry.Buttons & PSP_CTRL_UP) ctrl.up = 1;
			if(entry.Buttons & PSP_CTRL_DOWN) ctrl.down = 1;
			if(entry.Buttons & PSP_CTRL_LEFT) ctrl.left = 1;
			if(entry.Buttons & PSP_CTRL_RIGHT) ctrl.right = 1;
			//pressed = 1;
			get = wait = 0;
		};
		if(!entry.Buttons) 
		{
			pressed = 0;
			get = 1;
		};
		ctrl.xstick = entry.Lx;
		ctrl.ystick = entry.Ly;
	}
	else
	{
		if(wait<wait_time) 
		{	
			wait++;
			this->flush();
		}
		else get = 1;
		if(!entry.Buttons) get = 1;
	};
};