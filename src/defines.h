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
	MODE_QUIT,		//Выход из игры
	MODE_MOVE_RIGHT,//перемещение вправо
	MODE_MOVE_LEFT,	//перемещение влево
	MODE_MOVE_DOWN,	//перемещение вниз
	MODE_MOVE_UP,	//перемещение вверх
	MODE_CHECK_RIGHT,//проверка хода вправо
	MODE_CHECK_LEFT,//проверка хода влево
	MODE_CHECK_DOWN,//проверка хода вних
	MODE_CHECK_UP,	//проверка хода вверх
	MODE_WAIT,		//ожидание ввода
	//MODE_DRAW,		//
	MODE_RESIZE		//изменение размеров окна
};

#endif // !DEFINES_H
