#ifndef DEFINES_H
#define DEFINES_H

// Минимальные размеры окна
#define WIN_MIN 200
#define WIN_HEIGHT (WIN_MIN * 3)
#define WIN_WIDTH (WIN_MIN * 4)

#define BG_DARK_BRIGHTNESS		0x60
#define BG_LIGHT_BRIGHTNESS		0xA0

enum Errors
{
	NOERR,
	SDLERR
};

enum Flags
{
	GLOB_FLAG_DARKMODE = 1,
	GLOB_FLAG_MOUSEOFF = 2
};

// enum Modes 
// {
// 	
// };

#endif // !DEFINES_H
