#ifndef _SYSTEMCTRL_H_
#define _SYSTEMCTRL_H_


#ifdef __cplusplus
extern "C" {
#endif


// Load Execute Module via Kernel Internal Function
int sctrlKernelLoadExecVSHWithApitype(int apitype, const char * file, struct SceKernelLoadExecVSHParam * param);

int sctrlSEMountUmdFromFile(const char * file, int index);


enum SEUmdModes
{
	MODE_UMD = 0,
	MODE_OE_LEGACY = 1,
	MODE_MARCH33 = 2,
	MODE_NP9660 = 3,
};


#ifdef __cplusplus
}
#endif


#endif
