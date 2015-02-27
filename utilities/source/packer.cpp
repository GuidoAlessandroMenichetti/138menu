#include <string.h>
#include <stdio.h>

typedef struct s_file
{
	unsigned size;
	char name[20];
} t_file;

char * read_line(char * line, int size, FILE * stream)
{
	char * ret = fgets(line, size, stream);
	
	//removes /n and /r
	if(ret)
	{
		char * fix = strchr(line, '\r');
		if(!fix)
			fix = strchr(line, '\n');
			
		if(fix)
			* fix = 0x0;
	};
		
	return ret;
};

int main()
{
	//opens config file
	FILE * names_stream = fopen("CONFIG.CFG", "rt");
	if(!names_stream)
	{
		printf("Cant find CONFIG.CFG file\n");
		return 1;
	};
	
	unsigned file_count = 0;
	char line[20];
	
	//read the output file name
	if(read_line(line, sizeof(line), names_stream))
	{
		//create output file
		FILE * output = fopen(line, "wb");
		fwrite(&file_count, sizeof(unsigned), 1, output);
		
		t_file current_file;
		while(read_line(current_file.name, sizeof(line), names_stream))
		{
			if(current_file.name[0] == 0x0) //skips empty lines
				continue;

			//open current file
			FILE * current_stream = fopen(current_file.name, "rb");
			if(!current_stream)
			{
				printf("WARNING: Missing file %s\n", current_file.name);
				continue;
			};

			//get file size
			fseek(current_stream, 0, SEEK_END);
			current_file.size = ftell(current_stream);

			//write file info
			fwrite(&current_file, sizeof(t_file), 1, output);
			
			fseek(current_stream, 0, SEEK_SET);
			
			//read and write file bytes
			char * buffer = new char[current_file.size];
			fread(buffer, current_file.size, 1, current_stream);
			fwrite(buffer, current_file.size, 1, output);
			delete [] buffer;
			
			fclose(current_stream);
			file_count++;
		};
		
		fseek(output, 0, SEEK_SET);
		fwrite(&file_count, sizeof(unsigned), 1, output);
	};
	
	fclose(names_stream);
	return 0;
};