#ifndef ENTRY_HEADER_138
#define ENTRY_HEADER_138

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pspdebug.h>

#include "gbZip.h"
#include "gbExp.h"
#include "138_cfg.h"
#include "eboot.h"

extern "C"
{
#include "graphics/graphics.h"
};

#define _printf 	pspDebugScreenPrintf
#define EBOOT_MAGIC 0x50425000
#define ZIP_MAGIC	0x04034B50
#define PARAM_MAGIC 0x46535000
#define FAST_PARAM 	"PARAM.SFO"
#define FAST_ICON	"ICON0.PNG"

extern config138 cfg;

class entry138
{
    public:

    enum ENUM138_ENTRY_TYPES {UNKNOWN = -1, HOMEBREW = 0, INSTALLER_HB = 1};
    enum ENUM138_RETURNS {INVALID = 0, IS_EBOOT = 1, IS_ZIP=8, PARSE_ERROR = 2, PARSE_OK = 3, LOAD_ERROR = 4, LOAD_OK = 5};
    enum ENUM138_MODES {MODE_ACCURATE = 0, MODE_FAST = 1};
    enum ENUM138_LIMITS {NAME_LIMIT = 30, PARSE_BUF = 40, MAX_IMG_WIDTH = 144, MAX_IMG_HEIGHT = 80};

	entry138();
    int create(const char * file);
    static int getFileType(const char * file, unsigned offset);
	static int getEbootHeader(const char * file, ebootHeader * eb, unsigned offset);
	char * getName();
    char * getPath();
    int getType();
	Image * getIcon();
    void clear();

    private:

    int load(const char * file);
	int parseSfo(const char * file, unsigned offset);
    char * name;
    char * path;
	Image * icon;
    int type;
};

#endif
