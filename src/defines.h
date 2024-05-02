#ifndef DEFINES_H
#define DEFINES_H

// Минимальные размеры окна
#define WIN_MIN 200
#define WIN_HEIGHT (WIN_MIN * 3)
#define WIN_WIDTH (WIN_MIN * 4)
#define FIELD_SIZE_COEFFICIENT 0.9

#define BG_DARK_BRIGHTNESS		0x20
#define BG_LIGHT_BRIGHTNESS		0xdf

enum Errors
{
	ERR_NO,
	ERR_SDL,
	ERR_MALLOC
};

enum Flags
{
	FLAG_DARKMODE = 1,
	FLAG_MOUSEOFF = 2,
	FLAG_ARROWKEY = 4,
	FLAG_VIMKEY   = 8,
	FLAG_WASDKEY  = 0x10
};

enum Modes 
{
	MODE_QUIT,
	MODE_MOVE_RIGHT,
	MODE_MOVE_LEFT,
	MODE_MOVE_DOWN,
	MODE_MOVE_UP,
	MODE_CHECK_RIGHT,
	MODE_CHECK_LEFT,
	MODE_CHECK_DOWN,
	MODE_CHECK_UP,
	MODE_DO_NOTHING,
	MODE_DRAW,
	MODE_RESIZE
};

#endif // !DEFINES_H
