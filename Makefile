TARGET = 138MENUCPP
OBJS = main.o src/resources.o src/menu.o src/progress.o src/explorer.o src/entry.o src/filelist.o src/gb/gbsfo.o src/gb/gbiso.o src/gb/gbcso.o src/ctrl.o src/gb/gbzip.o src/osk.o src/cfg.o src/other.o src/graphics/graphics.o src/graphics/framebuffer.o src/graphics/libccc.o src/graphics/intraFont.o
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
