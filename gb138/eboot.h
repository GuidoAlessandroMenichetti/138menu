#ifndef EBOOT_HEADER
#define EBOOT_HEADER

typedef struct
{
	unsigned sign;
	unsigned version;
	unsigned key_table;
	unsigned data_table;
	unsigned entries;
} sfoHeader;

typedef struct
{
	unsigned short offset;
	unsigned short type;
	unsigned len;
	unsigned max_len;
	unsigned data_offset;
} indexTable;

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
} ebootHeader;

#endif