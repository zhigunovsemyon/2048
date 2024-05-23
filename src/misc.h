#ifndef MISC_H
#define MISC_H
#include "main.h"

enum SetFlags
{
	KEY_UNSET	= 1,
	COL_UNSET	= 2,
	MOUSE_UNSET = 4,	
	SIZE_UNSET	= 8,
	VSYNC_UNSET = 0x10
};
Uint8 CheckRightMove(Game *Game, Params *Params);
Uint8 CheckLeftMove(Game *Game, Params *Params);
Uint8 CheckUpMove(Game *Game, Params *Params);
Uint8 CheckDownMove(Game *Game, Params *Params);

Uint8 CheckRightCombo(Game *Game, Params *Params);
Uint8 CheckLeftCombo(Game *Game, Params *Params);
Uint8 CheckUpCombo(Game *Game, Params *Params);
Uint8 CheckDownCombo(Game *Game, Params *Params);

void ChangeCombinedToOld(Game *Game);
Uint8 UpdateWindowTitle(SDL_Window *win, Uint64 Score);

//Считает число переносов строки 
Uint8 CountLines(const char *source);

/*Возвращает случайное число из диапазона. 
Требует предварительной инициализации через srand*/
Sint32 RandomInt(Sint32, Sint32);

/*Создание набора цветов для текстур*/
SDL_Colour* CreateColourSet(Uint8 DarkModeFlag);

/*Считает и возвращает длинну кадра в мс*/
Uint8 dtCount(void);

/*Добавление нового элемента в игровое поле. Возврат индекса нового тайла,
 *  если нет свободного места, возврат -1*/
Sint8 AddElement(Game *Game);

//Создание размеров поля и тайла
void GetFieldAndTileSize(Game *Game, Params *Params);

/* На основе параметров запуска игры argv, в количестве argc, подбирается схема управления
 * и размер игрового поля. По умолчанию используются стрелки с мышью, поле имеет  размер 4x4*/
Game StartUp(int argc, const char **argv, Params *Settings);

/*Функция меняет режим работы программы на основе
 уже выставленного режима работы и некоторого события */
void SetMode(SDL_Event *event, Params *Params);

//Вычисление меньшего из двух чисел
int MinOfTwo(int,int);

//Вычисление большего из двух чисел
int MaxOfTwo(int,int);

//Вызов SDL, создание окна под данным названием и рисовальщика 
Uint8 CreateWorkspace(SDL_Window **win, SDL_Renderer **rend, const char *title, 
					  const SDL_Point *WinSize, Uint8 Flags);

//Вывести сообщение об ошибки и выйти из программы
Uint8 PrintErrorAndLeaveWithCode(Uint8 code, SDL_Window* win, SDL_Renderer* rend, Game* Game, Params *Params, Assets *Assets);

//Закрытие всех окон и рисовальщиков
Uint8 SilentLeaveWithCode(Uint8 code, SDL_Window* win, SDL_Renderer* rend, Game* Game, Params *Params, Assets *Assets);

/*Проверка окна win на изменение его размеров в Params, через ивенты ev.
Меняет на win_min при чрезмерном уменьшении. Возврат true, если размер изменён, либо false*/
Uint8 CheckForResize(SDL_Window* win, Params* Params, SDL_Event* ev, Uint16 win_min);

#endif // !MISC_H
