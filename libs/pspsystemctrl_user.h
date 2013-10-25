#ifndef _SYSTEMCTRL_H_
#define _SYSTEMCTRL_H_


#ifdef __cplusplus
extern "C" {
#endif


// Load Execute Module via Kernel Internal Function
int sctrlKernelLoadExecVSHWithApitype(int apitype, const char * file, struct SceKernelLoadExecVSHParam * param);

// Return Reboot Configuration UMD File
char * sctrlSEGetUmdFile(void);

// Set Reboot Configuration UMD File
void sctrlSESetUmdFile(const char * file);

// No MODE_OE_LEGACY any more
enum SEUmdModes
{
        MODE_UMD = 0,
        MODE_MARCH33 = 1,
        MODE_NP9660 = 2,
        MODE_INFERNO = 3,
};
/**
 * Sets the boot config file for next reboot
 *
 * @param index - The index identifying the file (0 -> normal bootconf, 1 -> march33 driver bootconf, 2 -> np9660 bootcnf, 3 -> inferno bootconf), 4 -> inferno vsh mount
*/
void sctrlSESetBootConfFileIndex(int index);


#ifdef __cplusplus
}
#endif


#endif

