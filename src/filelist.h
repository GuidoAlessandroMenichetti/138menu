#ifndef FILELIST138_H
#define FILELIST138_H

#include "common.h"

typedef struct s_file
{
	char * path;
	char * name;
	unsigned size;
	int type;
} t_file;

class filelist138
{
	public:
	
	enum E_FILE_TYPE
	{
		FILES = 0,
		FOLDER = 1
	};
	
	filelist138();
	~filelist138();
	
	int load(const char * path, const char * extension_filter = NULL);
	void clear();
	
	void add_file(const char * path, const char * name, int type, int size);
	int delete_file(const char * path, int COMPARE_NAME = 0);
	int find_file(const char * path, int COMPARE_NAME = 0);
	
	static bool sort_function(const t_file * file1, const t_file * file2);
	
	t_file * get(unsigned index);
	int get_count();
	
	private:
	
	void delete_entry(t_file * file);
	vector<t_file *> files;
};

#include "explorer.h"

#endif