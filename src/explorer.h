#ifndef EXPLORER_HEADER
#define EXPLORER_HEADER

#include "common.h"
#include "filelist.h"
#include "ctrl.h"


#define COPY_BUFFER_SIZE 0x8000

extern input138 ctrl;

enum E_COMMANDS
{
	NONE = -1,
	COPY = 0,
	CUT = 1,
	PASTE = 2,
	DELETE = 3,
	RENAME = 4,	
	MKDIR = 5,
	CANCEL = 6,
	RETURN = 7
};

typedef struct s_tool_option
{
	const char * name;
	int enabled;
	int command;
} t_tool_option;

static t_tool_option tool_options[] = 
{
	{NULL, 0, COPY},
	{NULL, 0, CUT},
	{NULL, 0, PASTE},
	{NULL, 0, DELETE},
	{NULL, 0, RENAME},
	{NULL, 0, MKDIR},
	{NULL, 0, CANCEL},
	{NULL, 0, RETURN}
};

static const int tool_count = sizeof(tool_options) / sizeof(t_tool_option);



class explorer138
{
	public: 
	
	static unsigned copy_progress;
	static unsigned copy_total;
	static char * copy_name;
	static bool copy_active;
	
	static int create_dummy(const char * path);
	
    static int file_exists(const char * path);
	static int folder_exists(const char * path);
	
	static int delete_file(const char * path);
	static int delete_folder(const char * path);
	
	static int copy_bytes(SceUID source, SceUID destination, unsigned bytes);
	static int copy_file(const char * source, const char * destination, unsigned source_offset = 0, unsigned source_size = 0);
	static int copy_folder_recursive(const char * source, const char * destination);
	static int copy_folder(const char * source, const char * destination);
	
	static int move_file(const char * source, const char * destination);
	static int move_folder(const char * source, const char * destination);
	
	static int create_folder(const char * path);
	static int rename(const char * old_name, const char * new_name);
	static unsigned file_size(const char * path);
	
	explorer138();
	~explorer138();
	void clear();
	void browse(const char * path);
	void refresh();
	int show(const void * owner);
	
	void draw_all();
	
	private:
	
	void check();
	void draw();
	
	void dofile(const char * path);
	int show_image(const char * path);
	
	void update_language();
	void tools_draw();
	void tools_check();
	void tools_open();
	void tools_docommand(int command);
	struct
	{
		bool enabled;
		int option;
		int command;
	} tools;
	
	bool active;
	char * current_path;
	int list_option, offset;
	filelist138 files;
	filelist138 clipboard;
	
	const void * owner;
};

#endif
