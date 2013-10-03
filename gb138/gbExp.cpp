#include "gbExp.h"

int gbExplorer :: folderExist(const char * path)
{
	int ret = gbExplorer::FILE_NOT_FOUND;
	SceUID d; 
	d = sceIoDopen(path);
	if(d>=0) 
	{
		ret = gbExplorer::FILE_OK;
		sceIoDclose(d);
	};
	return ret;
}

int gbExplorer :: fileExist(const char * path)
{
    FILE * a;
    a = fopen(path, "rb");
    if(!a) return gbExplorer::FILE_NOT_FOUND;
    fclose(a);
    return gbExplorer::FILE_OK;
};

int gbExplorer :: createFolder(const char * path)
{
	if(!sceIoMkdir(path, 0)) return gbExplorer::DIR_CREATED;
	return gbExplorer::ERROR;
};

int gbExplorer :: deleteFile(const char * path)
{
	if(sceIoRemove(path)>=0) return gbExplorer::DELETED;
	return gbExplorer::ERROR;
};

int gbExplorer :: deleteFolder(const char * path)
{
	SceUID d; 
	SceIoDirent entry;
	
	d = sceIoDopen(path);
	if(d>=0)
	{
		char * new_path = (char *)malloc(strlen(path)+100);
		while((sceIoDread(d, &entry) > 0))
		{
			if (!strcmp(".", entry.d_name) || !strcmp("..", entry.d_name)) continue;
			
			strcpy(new_path, path);
			strcat(new_path, entry.d_name);
			if(gbExplorer::deleteFile(new_path)!=gbExplorer::DELETED) 
			{
				strcat(new_path, "/");
				gbExplorer::deleteFolder(new_path);
			};
		};
		sceIoDclose(d);
		sceIoRmdir(path);
		free(new_path);
	};
	
	return gbExplorer::DELETED;
};