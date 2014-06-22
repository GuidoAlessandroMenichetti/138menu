#ifndef SFO_HEADER
#define SFO_HEADER

#include <stdio.h>
#include <string.h>
#include <pspdebug.h>
#include <pspkernel.h>

class sfo
{
	public:

	sfo();
	sfo(const char * path);
	~sfo();
	int load(const char * path, unsigned force_offset = NOT_FORCED);
	char * get_data(const char * key);
	bool is_loaded();

	private:

	typedef struct s_eboot_header
	{
		unsigned magic;
		unsigned version;
		unsigned sfo;
		unsigned icon0;
		unsigned icon1;
		unsigned pic0;
		unsigned pic1;
		unsigned snd0;
		unsigned pspdat;
		unsigned psar;
	} t_eboot_header;

	typedef struct s_index_table_entry
	{
		unsigned short offset;
		unsigned short type;
		unsigned len;
		unsigned max_len;
		unsigned data_offset;
	} t_index_table_entry;

	typedef struct s_sfo_header
	{
		unsigned sign;
		unsigned version;
		unsigned key_table;
		unsigned data_table;
		unsigned entries;
	} t_sfo_header;

	typedef struct s_entry
	{
		char * key;
		char * data;
	} t_entry;

	enum E_FILE_TYPES
	{
		TYPE_UNKNOWN = -1,
		TYPE_PBP = 0,
		TYPE_SFO = 1
	};

	enum E_MAGICS
	{
		SFO_MAGIC = 0x46535000, //PSF
		PBP_MAGIC = 0x50425000, //PBP
	};

	static const unsigned NOT_FORCED = -1;

	void init_values();
	void clear();

	inline int get_file_type();
	inline unsigned get_sfo_offset();
	inline int read_index_table(unsigned start_offset = 0);
	inline int get_entry(unsigned index, unsigned key_table_offset, unsigned data_table_offset);

	char * read_bytes(unsigned offset, unsigned short size);
	inline bool open_file(const char * path, const char * mode);

	FILE * stream;
	t_entry * entries;
	unsigned short total_entries;
	bool loaded;
};

#endif
