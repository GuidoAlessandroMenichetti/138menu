#include "138_osk.h"
#include "138_menu.h"

osk138 :: osk138()
{
	this->enabled = 0;
};

void osk138 :: work()
{
	while(this->enabled)
	{
		ctrl.update();
		menu138::clrscr();
		this->check();
		this->draw();
		menu138::flip();
	};
	while(ctrl.isPressed()) ctrl.update();
};

void osk138 :: activate(const char * message, const char * value, int limit)
{
	this->limit = limit;
	strcpy(this->caption, message);
	memset(this->input, 0, sizeof(this->input));
	strcpy(this->input, value);
	*(this->input+limit) = '\0';
	this->enabled = 1;
	this->upper = 0;
	this->cancel = 0;
	this->y = this->x = 0;
	this->animation = -60;
	this->len = (int)strlen(this->input);
	this->work();
};

void osk138 :: printChar(char character, int x, int y, unsigned color, unsigned shadow, float size)
{
	intraFontSetStyle(res.font, size, color, shadow, 0.0f, 0);
	intraFontPrintf(res.font, x, y, "%c", character);
};

void osk138 :: draw()
{
	unsigned color, x_r;
	int i, j;
	char letter;
	Image * back_show = res.img[MENU_CUSTOMBACK]? res.img[MENU_CUSTOMBACK]: res.img[MENU_BACKGROUND];

	menu138::printImg(back_show, 0, 0);	
	menu138::printImg(res.img[OSK_KEYBOARDBACK], 24, 4+animation);
	menu138::printImg(res.img[OSK_INPUTBACK], 33, 40+animation);
	menu138::printImg(res.img[OSK_KEYBOARD], 33, 83+animation);

	guStart();
	menu138::printText(this->caption, 33, 25+animation, menu138::WHITE, menu138::BLACK, SIZE_BIG);
	for(i=0;i<len;i++) //input drawing
	{
		letter = *(this->input+i);
		x_r = 40+i*14;
		for(j=0;j<(int)sizeof(special_chars);j++) 
			if(letter == special_chars[j]) x_r += 4;
		this->printChar(letter, x_r, animation+60, menu138::BLACK, 0x00000000, SIZE_BIG);
	};

	for(i=0;i<4;i++) //Keyboard letters drawing
		for(j=0;j<11;j++)
		{
			letter = keyboard[upper][i][j];
			color = (y==i && x==j)? menu138::YELLOW: menu138::WHITE;
			this->printChar(letter, 42+i*7+j*37, animation+100+i*25, color, menu138::BLACK, SIZE_BIG);
		};

	color = y==4? menu138::YELLOW: menu138::WHITE; //Space bar drawing
	for(j=0;j<100;j++) this->printChar('-', 65+i*7+j*2, animation+100+i*25, color, menu138::BLACK, SIZE_BIG);
	color = y<0? menu138::YELLOW: menu138::BLACK;
	this->printChar('x', 430, animation+20, color, menu138::BLACK, SIZE_BIG);
};

void osk138 :: addChar(char character)
{
	if(len<limit)
	{
		input[len] = character;
		input[len+1] = '\0';
		len++;
	};
};

void osk138 :: check()
{
	if(ctrl.getCtrl()->up) 
	{
		if(y>-1) y--;
		while(keyboard[upper][y][x]=='\0') x--;
	};

	if(ctrl.getCtrl()->down)
	{
		if(y<4) y++;
		while(keyboard[upper][y][x]=='\0') x--;
	};

	if(y>-1 && y<4)
	{
		if(ctrl.getCtrl()->left)
			if(x>0) x--;
			
		if(ctrl.getCtrl()->right)
			if(keyboard[upper][y][x+1]!='\0') x++;
	};
	
	if(ctrl.getCtrl()->cross) 
	{
		if(y<0)
		{
			this->enabled = 0;
			this->cancel = 1;
			return;
		};
		if(y==4) this->addChar(' ');
		else if(y==3 && x==0) upper = upper? 0: 1;
		else if(y==0 && x==10 && len>0) input[--len] = '\0';
		else this->addChar(keyboard[upper][y][x]);
	};

	if((ctrl.getCtrl()->circle || ctrl.getCtrl()->ltrigger) && len>0) input[--len] = '\0';
	if(ctrl.getCtrl()->start) enabled = 0;
	if(animation!=30) animation+=10;
};

char * osk138 :: getOutput()
{
	return this->input;
};

int osk138 :: Cancelled()
{
	return this->cancel;
};
