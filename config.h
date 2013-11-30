#ifndef MENU138CONFIG
#define MENU138CONFIG

/** TO BUILD ARK/TN VERSIONS
	If you want to use the TN libs, just rename 
		libpspsystemctrl_user.a to ARK____libpspsystemctrl_user.a
		TN____libpspsystemctrl_user.a to libpspsystemctrl_user.a
		
	Same for ARK, the wanted lib should be named libpspsystemctrl_user.a, the default one is for ARK
	Then uncomment the correct define below
	NOTE: You can't use both at the same time
**/

//#define ARK
//#define TN

#define VERSION "L3 (29/11/13)"
#define CREDITS "138Menu by GUIDOBOT"

#define INSTALLERS_PATH "ms0:/PSP/SAVEDATA/"
#define ZIP_NAME "INSTALL.ZIP"
#define CFG_FILE "138CFG.TXT"
#define DEFAULT_PATH "ms0:/PSP/VHBL/"
#define DEFAULT_NAME "wmenu.bin"
#define DEFAULT_THEME "DATA.138"

#define BUILD_VHBL "VHBL"
#define BUILD_ARK "ARK CEF"
#define BUILD_TN "TN CEF"

#if defined(ARK) && defined(TN)
#undef TN
#endif

#if defined(ARK) || defined(TN)
#define CEF
#endif

#endif