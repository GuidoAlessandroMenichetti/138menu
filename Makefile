TARGET = 138MENUCPP
OBJS = main.o gb138/138_menu.o gb138/gbCtrl.o gb138/gbIso.o gb138/gbCso.o gb138/gbZip.o gb138/gbAlloc.o gb138/138_entry.o gb138/138_osk.o gb138/138_cfg.o gb138/138_resources.o gb138/gbExp.o other/vhblBoot.o other/cef.o other/callbacks.o other/string_m.o graphics/graphics.o graphics/framebuffer.o graphics/libccc.o graphics/intraFont.o
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
INCDIR = libs
LIBDIR = libs
LIBS = -lstdc++ -lpspgu -lpspgum -lpng -lz -lm -lpsploadexec_kernel -lpspsystemctrl_user -lpspsystemctrl_kernel -lpspumd
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 138MENU
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
