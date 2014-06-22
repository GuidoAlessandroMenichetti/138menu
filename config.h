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

#define VERSION "L4 (22/06/14)"

//for design settings check src/design.h
//for language check src/lang.h
//for other settings check src/common.h

#endif