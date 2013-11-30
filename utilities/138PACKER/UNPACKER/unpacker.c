#include <stdio.h>
#include <string.h>

typedef struct s_file_struct
{
	unsigned file_size;
	char name[20];
} file_struct;

int unpack(const char * to, const char * file)
{
	FILE * a, * b;
	a = fopen(file, "rb");
	if(!a) return 0;

	unsigned file_count;
	unsigned j, i;
	unsigned char bu;
	file_struct fs;

	char save_path[300];
	strcpy(save_path, to);
	char * name_ad = strrchr(save_path, '\\')+1;
	* name_ad = '\0';

	fread(&file_count, sizeof(unsigned), 1, a);
	for(i=0; i<file_count; i++)
	{
		fread(&fs, sizeof(file_struct), 1, a);
		printf("Unpacking %-20s (0x%08X bytes)..\n", fs.name, fs.file_size);

		strcpy(name_ad, fs.name);
		b = fopen(save_path, "wb");
		for(j=0;j<fs.file_size;j++)
		{
			fread(&bu, 1, 1, a);
			fwrite(&bu, 1, 1, b);
		};
		fclose(b);
	};

	fclose(a);
	return 1;
};

int main(int argc, char * argv[])
{
	if(argc>0)
		return unpack(argv[0], argv[1]);
	return 0;
};
