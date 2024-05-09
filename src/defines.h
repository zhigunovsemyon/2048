#ifndef DEFINES_H
#define DEFINES_H

// Минимальные размеры окна
#define WIN_MIN 320
#define WIN_HEIGHT (WIN_MIN * 2)
#define WIN_WIDTH (WIN_MIN * 3)

//Коэффициенты
#define FIELD_SIZE_COEFFICIENT 0.9f
#define TILE_SIZE_COEFFICIENT 0.9f
#define ANIM_SPEED 500.0f
#define CHANCE_OF_FOUR 10

#define BG_DARK_BRIGHTNESS		0x20
#define BG_LIGHT_BRIGHTNESS		0xdf

#define MSG_LEN  400

#if WIN32
#define FONT "..\\..\\..\\monospace"
#else
#define FONT "monospace"
#endif

enum Textues
{
	TEX_SCORE,
	TEX_SQ2
};

enum Errors
{
	ERR_NO,
	ERR_SDL,
	ERR_MALLOC,
	ERR_TTF
};

enum Flags
{
	FLAG_DARKMODE = 1,	//Флаг тёмной/светлой темы
	FLAG_MOUSEOFF = 2,	//Флаг включенной/отключенной мыши
	FLAG_ARROWKEY = 4,	//Флаг управления со стрелок
	FLAG_VIMKEY = 8,	//Флаг управления с клавиш vi
	FLAG_WASDKEY = 0x10,//Флаг управления с клавиш WASD
	FLAG_VSYNC = 0x20	//Флаг использования VSync
};

enum Modes 
{
	MODE_QUIT,		//Выход из игры
	MODE_DRAW,		//Рисование нового элемента
	MODE_MOVE_RIGHT,//перемещение вправо
	MODE_MOVE_LEFT,	//перемещение влево
	MODE_MOVE_DOWN,	//перемещение вниз
	MODE_MOVE_UP,	//перемещение вверх
	MODE_CHECK_RIGHT,//проверка хода вправо
	MODE_CHECK_LEFT,//проверка хода влево
	MODE_CHECK_DOWN,//проверка хода вних
	MODE_CHECK_UP,	//проверка хода вверх
	MODE_WAIT,		//ожидание ввода
	MODE_ADD,		//добавление нового элемента
};

#endif // !DEFINES_H
