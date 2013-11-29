#ifndef EXPLORER_HEADER
#define EXPLORER_HEADER

#include <stdio.h>
#include <pspkernel.h>
#include <string.h>
#include <stdlib.h>
#include <pspdebug.h>

#define __printf pspDebugScreenPrintf

class gbExplorer
{
	public: 
    enum EXPLORER_RETURNS {FILE_OK, FILE_NOT_FOUND, FILE_CREATED, DIR_CREATED, DELETED, ERROR};

	static int isDir(const char * path);
    static int fileExist(const char * path);
	static int deleteFolder(const char * path);
	static int createFolder(const char * path);
	static int createDummy(const char * path);
	static int deleteFile(const char * path);
	static int folderExist(const char * path);
};

#endif
