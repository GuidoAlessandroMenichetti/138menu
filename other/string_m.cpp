#include "string_m.h"

char * normalize(char * str)
{
	char * ret = str;
	int flag = 0;
	while(*str)
	{
		if(*str <= 'Z' && *str >= 'A')
		{
			if(flag) *str = *str+0x20;
			else flag = 1;
		}
		else if(*str==' ') flag = 0;
		str++;
	};
	return ret;
};