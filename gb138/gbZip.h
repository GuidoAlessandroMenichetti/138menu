#ifndef GBZIP_HEADER
#define GBZIP_HEADER

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pspdebug.h>
#include "gbExp.h"

#define __printf pspDebugScreenPrintf

#define FILE_MAGIC 			0x04034b50
#define CENTRAL_DIR_MAGIC 	0x02014b50
#define CENTRAL_END_MAGIC 	0x06054b50
#define COPYING_BUFFER 		2000

typedef struct 
{
    unsigned short compression;
    unsigned compressedSize;
    unsigned short filenameLen;
    unsigned short extrafieldLen;
} structLocalFileHeader;

class gbZip
{
	public:
	
	enum ZIP_ENTRY_TYPE {ZIP_FILE, ZIP_FOLDER};
	enum enumZIP {ZIP_OK, ZIP_NOT_ZIP, ZIP_ERROR, ZIP_COMPRESSED, ZIP_PACKAGE};
	
	static int entryIsEboot(const char * name, int mode);
	static unsigned getEbootOffset(const char * file);
	static int unzip(const char * file, const char * root, const char * eboot_name);
	static void fixname(char * name);
};

#endif