#ifndef MISC_H
#define MISC_H
#include "main.h"

enum SetFlags
{
	KEY_UNSET	= 1,
	COL_UNSET	= 2,
	MOUSE_UNSET = 4,	
	SIZE_UNSET	= 8
};

/*Добавление нового элемента в игровое поле. Возврат индекса нового тайла,
 *  если нет свободного места, возврат -1*/
Sint8 AddElement(Game *Game);

/* На основе параметров запуска игры argv, в количестве argc, подбирается схема управления
 * и размер игрового поля. По умолчанию используются стрелки с мышью, поле имеет  размер 4x4*/
Uint8 LaunchOptions(int argc, const char **argv, Params *Settings);

/*Функция меняет режим работы программы на основе
 уже выставленного режима работы и некоторого события */
void SetMode(SDL_Event *event, Params *Params);

//Вычисление меньшего из двух чисел
int MinOfTwo(int,int);

//Вычисление большего из двух чисел
int MaxOfTwo(int,int);

//Вызов SDL, создание окна под данным названием и рисовальщика 
Uint8 CreateWorkspace(SDL_Window **win, SDL_Renderer **rend, const char *title, const SDL_Point *WinSize);

//Вывести сообщение об ошибки и выйти из программы
Uint8 PrintErrorAndLeaveWithCode(Uint8 code, SDL_Window* win, SDL_Renderer* rend, Game* Game);

//Закрытие всех окон и рисовальщиков
Uint8 SilentLeaveWithCode(Uint8 code, SDL_Window* win, SDL_Renderer* rend, Game* Game);

/*Проверка окна win на изменение его размеров в Params, через ивенты ev.
Меняет на win_min при чрезмерном уменьшении*/
void CheckForResize(SDL_Window* win, Params* Params, SDL_Event* ev, Uint16 win_min);

#endif // !MISC_H
