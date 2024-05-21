#ifndef DEFINES_H
#define DEFINES_H

//Макросы
#define _SQ(A) (A) * (A)//Квадрат числа
#define SPLIT_COL_LINK(A) A->r, A->g, A->b, A->a//Разложение ссылки на цвет на отдельные аргументы
#define SPLIT_COL_VAL(A)  A.r, A.g, A.b, A.a	//Разложение значения цвета на отдельные аргументы


#define WIN_MIN 320	// Минимальные размеры окна
#define WIN_HEIGHT (WIN_MIN * 2)	//Базовый размер окна по высоте (640)
#define WIN_WIDTH (WIN_MIN * 3)		//Базовый размер окна по ширине (960)

//Коэффициенты
#define FIELD_SIZE_COEFFICIENT 0.9f	//Отношение размера поля к меньшему из размеров экрана
#define TILE_SIZE_COEFFICIENT 0.9f	//Отношение размер тайла к размеру ячейки с тайлом
#define ANIM_SPEED 500.0f			//Скорость анимации
#define MIN_FRAMETIME 1				//Минимальное время кадра (1 = 1000FPS, 16 = 60 FPS)
#define CHANCE_OF_FOUR 10			//Шанс выпадения 4ки

#define BG_DARK_BRIGHTNESS		0x20//Яркость тёмного цвета
#define BG_LIGHT_BRIGHTNESS		0xdf//Яркость светлого цвета

//Используемый шрифт
#if WIN32
#define FONT "..\\..\\..\\assets\\monospace"
#else
#define FONT "assets/monospace"
#endif

//Используемые цветовые схемы
#if WIN32
#define LIGHT_SCHEME "..\\..\\..\\assets\\light"
#define DARK_SCHEME "..\\..\\..\\assets\\dark"
#else
#define LIGHT_SCHEME "assets/light"
#define DARK_SCHEME "assets/dark"
#endif

enum TileMode
{
	TILE_NEW,	//Новый тайл
	TILE_OLD,	//Статичный тайл
	TILE_MOVE_X,//Тайл, движимый по горизонтали
	TILE_MOVE_Y,//Тайл, движимый по вертикали
	TILE_COMBINED,		//Тайл, на который будет наложен соседний 
	TILE_JUSTCOMBINED	//Тайл, который был получен наложением
};

//Число всех цветов в игре
#define COLOURS_COUNT 14
enum ColourIndexes
{
	COL_BG,
	COL_FG,
	COL_SQ2,
	COL_SQ4,
	COL_SQ8,
	COL_SQ16,
	COL_SQ32,
	COL_SQ64,
	COL_SQ128,
	COL_SQ256,
	COL_SQ512,
	COL_SQ1024,
	COL_SQ2048,
	COL_MAX
};

//Число всех текстур в игре
#define TEXTURES_COUNT 13
enum TextureIndexes
{
	TEX_SCORE,
	TEX_SQ2,
	TEX_SQ4,
	TEX_SQ8,
	TEX_SQ16,
	TEX_SQ32,
	TEX_SQ64,
	TEX_SQ128,
	TEX_SQ256,
	TEX_SQ512,
	TEX_SQ1024,
	TEX_SQ2048,
	TEX_MAX
};

enum Errors
{
	ERR_NO,		//Отсутствие ошибок
	ERR_SDL,	//Ошибка SDL
	ERR_MALLOC,	//Ошибка malloc
	ERR_TTF		//Ошибка SDL_ttf
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
	MODE_DRAW_NEW,	//Рисование нового элемента
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
