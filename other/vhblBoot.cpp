#include "vhblBoot.h"

int xstrtoi(char * str, int len) 
{
	int val;
	int c;
	int i;
	val = 0;
	for (i = 0; i < len; i++){
		c = *(str + i);
		if (c >= '0' && c <= '9') {
			c -= '0';
		} else if (c >= 'A' && c <= 'F') {
			c = (c - 'A') + 10;
		} else if (c >= 'a' && c <= 'f') {
			c = (c - 'a') + 10;
		} else {
			return 0;
		}
		val *= 16;
		val += c;
	}
	return val;
}