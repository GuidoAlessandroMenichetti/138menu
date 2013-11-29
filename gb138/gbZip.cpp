#include "gbZip.h"

int gbZip :: entryIsEboot(const char * name, int mode)
{
	char eboot_names[][13] = {"wmenu.bin\0", "EBOOT.PBP\0", "FBOOT.PBP\0", "VBOOT.PBP\0"}, * aux = NULL;
	int i;
	if(mode) aux = strrchr(name, '/');
	else aux = strchr(name, '/');
	if(!aux) return 0;
	for(i=0;i<4;i++) if(!strcmp(aux+1, eboot_names[i])) return 1;
	return 0;
};

void gbZip :: fixname(char * name)
{
	char replace[] = ":?*<>|";
	int i = 0, j;
	while(name[i]!='\0')
	{
		for(j=0;j<6;j++) if(name[i]==replace[j]) name[i] = '_';
		i++;
	};
};

void gbZip :: printInfo(const char * root, const char * entry_name)
{
	pspDebugScreenSetXY(0, 0);
	__printf("INSTALLING...\n");
	pspDebugScreenSetXY(0, 2);
	__printf("DESTINATION: %s\n", root);
};

int gbZip :: unzip(const char * file, const char * root, const char * eboot_name)
{
	structLocalFileHeader fileHeader;
	FILE * a, * b;
	int centralDirFound = 0, copy_count = 0, print_y = 0;
	char * save_path = NULL, * entry_name = NULL, copy_buffer[COPYING_BUFFER];
	unsigned magic_check, i;
	memset(&fileHeader, 0, sizeof(structLocalFileHeader));

	a = fopen(file, "rb");
	fseek(a, 0, SEEK_SET);
	pspDebugScreenInit();
	
	while(!centralDirFound)
	{
		fread(&magic_check, sizeof(unsigned), 1, a);
		if(magic_check == CENTRAL_DIR_MAGIC) centralDirFound = 1;
		else
		{
			fseek(a, 4, 1);
			fread(&fileHeader.compression, sizeof(unsigned short), 1, a);
			fseek(a, 8, 1);
			fread(&fileHeader.compressedSize, sizeof(unsigned), 1, a);
			fseek(a, 4, 1);
			fread(&fileHeader.filenameLen, sizeof(unsigned), 1, a);
			
			entry_name = (char *)malloc(fileHeader.filenameLen+1);
			fread(entry_name, fileHeader.filenameLen, 1, a);
			*(entry_name + fileHeader.filenameLen) = '\0';
			if(gbZip::entryIsEboot(entry_name, 1)) strcpy(strrchr(entry_name, '/')+1, eboot_name);
			gbZip::fixname(entry_name);
			
			save_path = (char *)malloc(strlen(root)+fileHeader.filenameLen+30);
			strcpy(save_path, root);
			strcat(save_path, entry_name);
			
			if(*(entry_name+strlen(entry_name)-1)=='/') //Is it a folder?
			{
				pspDebugScreenClear();
				gbZip::printInfo(root, entry_name);
				pspDebugScreenSetXY(0, 4);
				__printf("  /%s...", entry_name);
				print_y = 0;
				gbExplorer::createFolder(save_path);
			}
			else //Is it a file?
			{
				b = fopen(save_path, "wb");
				if(!b)
				{
					fclose(a);
					pspDebugScreenClear();
					pspDebugScreenSetTextColor(0xFF0000FF);
					__printf("\nERROR\n\n");
					__printf("\tDESTINATION: %s\n\tFILE: %s", save_path, entry_name);
					
					free(entry_name);
					free(save_path);
					sceKernelDelayThread(5000000);
					return gbZip::ZIP_ERROR;
				};
				
				memset(copy_buffer, 0, sizeof(copy_buffer));
				
				pspDebugScreenSetXY(0, 7 + print_y);
				pspDebugScreenSetTextColor(0xFF00FFFF);
				__printf("\t%s\t\t\t\t", strrchr(save_path, '/')+1);
				pspDebugScreenSetTextColor(0xFFFFFFFF);
				pspDebugScreenSetXY(42, 7+print_y);
				
				int div = 0;
				if(fileHeader.compressedSize>1024) div = 1;
				__printf("%-6u %s\t", fileHeader.compressedSize/(div?(div * 1024):1), div? "KB": " B");
				
				for(i=0;i<fileHeader.compressedSize;i+=copy_count)
				{
					if(fileHeader.compressedSize-i < sizeof(copy_buffer)) copy_count = fileHeader.compressedSize-i;
					else copy_count = sizeof(copy_buffer);
					
					pspDebugScreenSetXY(58, 7+print_y);
					__printf("%-3d%%", i*100/fileHeader.compressedSize);
					
					fread(copy_buffer, copy_count, 1, a);
					fwrite(copy_buffer, copy_count, 1, b);
				};
				pspDebugScreenSetXY(58, 7+print_y);
				__printf("%-3d%%", i*100/fileHeader.compressedSize);
				
				print_y++;
				if(print_y > 20) 
				{
					print_y = 0;
					gbZip::printInfo(root, entry_name);
				};
				fclose(b);
			};
			free(entry_name);
			free(save_path);
		};
	};
	fclose(a);
	pspDebugScreenClear();
	__printf("\n\tPLEASE WAIT...");
	return gbZip::ZIP_OK;
};

unsigned gbZip :: getEbootOffset(const char * file)
{
	structLocalFileHeader fileHeader;
	unsigned magic_check, offset = 0;
	int centralDirFound = 0;
	char * entry_name = NULL;
	FILE * a = fopen(file, "rb");

	fread(&magic_check, sizeof(unsigned), 1, a);
	if(magic_check != FILE_MAGIC)
	{
		fclose(a);
		return 0;
	};
	fseek(a, 0, SEEK_SET);
	memset(&fileHeader, 0, sizeof(structLocalFileHeader));
	while(!centralDirFound && !offset)
	{
		fread(&magic_check, sizeof(unsigned), 1, a);
		if(magic_check == CENTRAL_DIR_MAGIC) centralDirFound = 1;
		else
		{
			fseek(a, 4, SEEK_CUR);
			fread(&fileHeader.compression, sizeof(unsigned short), 1, a);
			if(fileHeader.compression) //Zip is compressed, discard file
			{
				fclose(a);
				return 0;
			};
			fseek(a, 8, SEEK_CUR);
			fread(&fileHeader.compressedSize, sizeof(unsigned), 1, a);
			fseek(a, 4, SEEK_CUR);
			fread(&fileHeader.filenameLen, sizeof(unsigned), 1, a);
			entry_name = (char *)malloc(fileHeader.filenameLen+1);
			fread(entry_name, fileHeader.filenameLen, 1, a);
			*(entry_name+fileHeader.filenameLen) = '\0';
			fseek(a, fileHeader.extrafieldLen, 1);
			if(gbZip::entryIsEboot(entry_name, 0)) offset = ftell(a);
			fseek(a, fileHeader.compressedSize, 1);
			free(entry_name);
		};
	};

	fclose(a);
	return offset;
};