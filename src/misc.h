#ifndef MISC_H
#define MISC_H
#include "main.h"

enum SetFlags
{
	KEY_UNSET	= 1,
	COL_UNSET	= 2,	
	SIZE_UNSET	= 4,
	VSYNC_UNSET = 8
};
Uint8 CheckRightMove(Game *Game, Params *Params);
Uint8 CheckLeftMove(Game *Game, Params *Params);
Uint8 CheckUpMove(Game *Game, Params *Params);
Uint8 CheckDownMove(Game *Game, Params *Params);

void ChangeCombinedToOld(Game *Game);
Uint8 UpdateWindowTitle(SDL_Window *win, Uint64 Score);

/*Сохранение игрового процесса game в файл finename*/
Uint8 SaveGame(Game *, const char *filename);

//Считает число переносов строки 
Uint8 CountLines(const char *source);

/*Возвращает случайное число из диапазона. 
Требует предварительной инициализации через srand*/
Sint32 RandomInt(Sint32, Sint32);

/*Создание набора цветов для текстур*/
SDL_Colour* CreateColourSet(Uint8 DarkModeFlag);

/*Считает и возвращает длинну кадра в мс*/
Uint8 dtCount(void);

/*Добавление нового элемента в игровое поле. Возврат индекса нового тайла*/
Sint8 AddElement(Game *Game);

//Создание размеров поля и тайла
void GetFieldAndTileSize(Game *Game, Params *Params);

/* На основе параметров запуска игры argv, в количестве argc, подбирается схема управления
 * и размер игрового поля. По умолчанию используются стрелки с мышью, поле имеет  размер 4x4*/
Game InitParamsAndGame(int argc, const char **argv, Params *Settings, const char *filename);

/*Функция меняет режим работы программы на основе
 уже выставленного режима работы и некоторого события */
void SetMode(SDL_Event *, Game *, Params *);

//Вызов SDL, создание окна под данным названием и рисовальщика 
Uint8 CreateWorkspace(SDL_Window **win, SDL_Renderer **rend, const char *title, 
					  const SDL_Point *WinSize, Uint8 Flags);

//Вывести сообщение об ошибке и выйти из программы
Uint8 PrintErrorAndLeaveWithCode(Uint8 code, SDL_Window* win, SDL_Renderer* rend, Game* Game, Params *Params, Assets *Assets);

//Закрытие всех окон и рисовальщиков
Uint8 SilentLeaveWithCode(Uint8 code, SDL_Window* win, SDL_Renderer* rend, Game* Game, Params *Params, Assets *Assets);

/*Проверка окна win на изменение его размеров в Params, через ивенты ev.
Меняет на win_min при чрезмерном уменьшении. Возврат true, если размер изменён, либо false*/
Uint8 CheckForResize(SDL_Window* win, Params* Params, SDL_Event* ev, Uint16 win_min);

#endif // !MISC_H
