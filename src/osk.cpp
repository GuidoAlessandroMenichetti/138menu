#include "osk.h"
#include "menu.h"

osk138 :: osk138()
{
	enabled = 0;
};

void osk138 :: work()
{
	ctrl.waitButtonsPressed();

	while(enabled)
	{
		ctrl.update();
		check();
		
		//clean screen
		menu138::clrscr();
		guStart();
		
		//draw osk
		draw();
		
		menu138::flip();
	};
};

void osk138 :: activate(const char * message, const char * value, int limit)
{
	this->limit = limit;
	
	strcpy(caption, message); //copy caption
	
	if(value) //copy default value
		strcpy(input, value);
	else 
		* input = 0x0;
	
	*(input + limit) = 0x0; //cut input string
	
	//default values
	enabled = 1;
	upper = 0;
	cancel = 0;
	y = x = 0;
	animation = -60;
	len = (int)strlen(input);
	
	//start
	work();
};

void osk138 :: printChar(char character, int x, int y, unsigned color, unsigned shadow, float size)
{
	//print a single character
	intraFontSetStyle(res.font, size, color, shadow, 0.0f, 0);
	intraFontPrintf(res.font, x, y, "%c", character);
};

void osk138 :: draw()
{
	//select background
	Image * back_show = res.img[MENU_CUSTOMBACK]? res.img[MENU_CUSTOMBACK]: res.img[MENU_BACKGROUND];

	//print images
	menu138::printImg(back_show, 0, 0);	
	menu138::printImg(res.img[OSK_KEYBOARDBACK], 24, 4 + animation);
	menu138::printImg(res.img[OSK_INPUTBACK], 33, 40 + animation);
	menu138::printImg(res.img[OSK_KEYBOARD], 33, 83 + animation);
	
	//print caption
	menu138::printText(caption, 33, 25 + animation, WHITE, BLACK, SIZE_BIG);
	
	char letter;
	unsigned color;
	int i, j;
	
	menu138::printText(input, 40, animation + 60, BLACK, 0x00000000, SIZE_LITTLE);

	for(i = 0; i < 4; i++) //draw keyboard letters
	{
		for(j = 0; j < 11; j++)
		{
			letter = keyboard[upper][i][j];
			color = (y == i && x == j)? YELLOW: WHITE;
			printChar(letter, 42 + i * 7 + j * 37, animation + 100 + i * 25, color, BLACK, SIZE_LITTLE);
		};
	};
	
	color = (y == 4)? YELLOW: WHITE; //choose color for spacebar
	for(j = 0; j < 100; j++) //draw spacebar
		printChar('-', 65 + i * 7 + j * 2, animation + 100 + i * 25, color, BLACK, SIZE_BIG);
		
	color = (y < 0)? YELLOW: BLACK; 
	printChar('x', 430, animation + 20, color, BLACK, SIZE_BIG); //draw upper cross
};

void osk138 :: addChar(char character)
{
	//add a new char to the text
	if(len < limit)
	{
		input[len] = character;
		input[len + 1] = 0x0;
		len++;
	};
};

void osk138 :: check()
{
	if(ctrl.get()->up) 
	{
		if(y > -1) //go to line above
			y--;
			
		while(keyboard[upper][y][x] == 0x0) //move to the next valid space
			x--;
	}
	else if(ctrl.get()->down)
	{
		if(y < 4) //go to the line below
			y++;
			
		while(keyboard[upper][y][x] == 0x0) //move to valid space
			x--;
	};

	if(y > -1 && y < 4) //left right movement
	{
		if(ctrl.get()->left)
		{
			if(x > 0) 
				x--;
		}
		else if(ctrl.get()->right)
		{
			if(keyboard[upper][y][x + 1] != 0x0) 
				x++;
		};
	};
	
	//cross pressing
	if(animation == 30 && ctrl.get()->cross) 
	{
		if(y < 0) //exit/cancel selected
		{
			enabled = 0;
			cancel = 1;
			return;
		};
		
		if(y == 4) 
			addChar(' '); //add new space
		else if(y == 3 && x == 0) 
			TOGGLE(upper);//Toggle upper/lower case //upper = upper? 0: 1;
		else if(y == 0 && x == 10 && len > 0) 
			input[--len] = 0x0; //go back one char
		else 
			addChar(keyboard[upper][y][x]); //add new char
	};

	if((ctrl.get()->circle || ctrl.get()->ltrigger) && len > 0) 
		input[--len] = 0x0; //go back one char
		
	if(ctrl.get()->start) 
		enabled = 0; //accept string
	
	if(animation != 30) 
		animation += 10; //animation control
};

char * osk138 :: getOutput()
{
	return input;
};

int osk138 :: Cancelled()
{
	return cancel;
};
