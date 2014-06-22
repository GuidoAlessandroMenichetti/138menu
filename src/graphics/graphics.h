#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <pspgum.h>
#include <psptypes.h>
#include <pspdebug.h>

#define	PSP_LINE_SIZE 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

typedef u32 Color;
#define A(color) ((u8)(color >> 24 & 0xFF))
#define B(color) ((u8)(color >> 16 & 0xFF))
#define G(color) ((u8)(color >> 8 & 0xFF))
#define R(color) ((u8)(color & 0xFF))

typedef struct
{
	int textureWidth;
	int textureHeight;
	int imageWidth;
	int imageHeight;
	Color* data;
} Image;

extern Image * loadImage(const char* filename, unsigned offset);
extern void blitAlphaImageToScreen(int sx, int sy, int width, int height, Image* source, int dx, int dy);
extern void freeImage(Image* image);
extern void clearScreen(Color color);
extern void flipScreen();
extern void initGraphics();
extern void disableGraphics();
extern Color * getVramDrawBuffer();
extern Color * getVramDisplayBuffer();
extern void guStart();

#endif
