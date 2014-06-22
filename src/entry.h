#ifndef ENTRY138
#define ENTRY138

#include "common.h"
#include "gb/gbiso.h"
#include "gb/gbcso.h"
#include "gb/gbsfo.h"
#include "gb/gbzip.h"
#include "cfg.h"

extern "C"
{
#include "graphics/graphics.h"
};

extern config138 cfg;

typedef struct
{
	unsigned sign;
	unsigned version;
	unsigned param;
	unsigned icon0;
	unsigned icon1;
	unsigned pic0;
	unsigned pic1;
	unsigned snd0;
	unsigned pspdat;
	unsigned psar;
} eboot_header;

class entry138
{
	public:
	
	enum E_ENTRY_LOADMODES
	{
		MODE_ACCURATE = 0,
		MODE_FAST = 1
	};

	enum E_ENTRY_EXTRA
	{
		//APP = 0,
		PATCHED = 1,
		NOT_PATCHED = 2,
		PS1 = 0x454D,
		PSN = 0x4745,
		APP = 0x474D
	};
	
	enum E_ENTRY_RETURNS
	{
		LOAD_ERROR, 
		LOAD_OK
	};
	
	entry138();
	~entry138();
	
	int create(const char * path);
	static int get_file_type(const char * path);
	
	char * getPath() const;
	char * getName() const;
	int getType() const;
	int getExtra() const;
	Image * getIcon();
	
	static int get_priority(const entry138 * entry);
	static bool entry_sort(const entry138 * a, const entry138 * b);
	
	private:
	
	void clear();
	int load(const char * path);
	
	inline bool type_is_valid(int type);
	eboot_header get_eboot_header(const char * path, unsigned offset = 0);
	
	void fix_name_string(char * str);
	
	char * path;
	char * name;
	Image * icon;
	
	int type;
	int pbp_type;
	int extra;
	
};

#endif