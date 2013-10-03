#ifndef VHBL_BOOT_HEADER
#define VHBL_BOOT_HEADER

typedef struct 
{
	unsigned long APIVersion;
	char Credits[32];
	char VersionName[32];
	char *BackgroundFilename;
	char *filename;
} tMenuApi;

int xstrtoi(char * str, int len);

#endif