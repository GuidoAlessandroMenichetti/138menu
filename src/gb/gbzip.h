#ifndef GBZIP_H
#define GBZIP_H

#include "../common.h"
#include "../explorer.h"

typedef struct s_end_of_central_directory
{
	unsigned magic;
	unsigned short disk_id;
	unsigned short disk_cd;
	unsigned short disk_cd_count;
	unsigned short cd_count;
	unsigned cd_size;
	unsigned cd_offset;
	unsigned short comment_length;
	//comment
} __attribute__ ((__packed__)) t_end_of_central_directory;

typedef struct s_zip_file_header
{
	unsigned magic;
	unsigned short version;
	unsigned short flag;
	unsigned short compression;
	unsigned short time;
	unsigned short date;
	unsigned crc;
	unsigned size_compressed;
	unsigned size_uncompressed;
	unsigned short name_length;
	unsigned short extra_length;
	//name
	//extra field
	//data
} __attribute__ ((__packed__)) t_zip_file_header;

typedef struct s_central_directory
{
	unsigned magic;
	unsigned short version_made;
	unsigned short version_min;
	unsigned short flag;
	unsigned short compression;
	unsigned short modif_time;
	unsigned short modif_date;
	unsigned crc;
	unsigned size_compressed;
	unsigned size_uncompressed;
	unsigned short name_length;
	unsigned short extra_length;
	unsigned short comment_length;
	unsigned short disk_id;
	unsigned short internal_attrib;
	unsigned external_attrib;
	unsigned offset;
	//file name
	//extra field
	//comment
} __attribute__ ((__packed__)) t_central_directory;

class gbzip
{
	public:
	
	gbzip();
	~gbzip();

	int extract(const char * file, const char * destination, const char * rename);
	unsigned fast_search(const char * file, const char * search);

	char * current_file;
	
	private:

	enum E_ZIP_MAGICS
	{
		EOCD_MAGIC = 0x06054b50,
		CD_MAGIC = 0x02014b50
	};

	int load(const char * path);
	void clear();
	void initial_values();
	void fix_path(char * path);
	inline bool open_file(const char * path, const char * mode);

	int get_end_of_cd();
	int extract_file(char * name, unsigned offset, unsigned size);
	int start_reading_cds();

	FILE * stream;
	t_end_of_central_directory end_of_cd;
	const char * destination;
	const char * source;
	const char * rename;
	
};

#endif
