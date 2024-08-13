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
/*Если сдвиг возможен -- возврат соответствующего флага, в противном случае
  если поле заполнено -- возврат флага завершения игры, иначе -- флаг ожидания*/
/*Выставление оффсета в соответствии с параметрами игры в Game и Params
 * Возврат флага сдвига, завершения игры, либо ожидания*/
int_fast8_t CheckRightMove(Game *, Params *);
int_fast8_t CheckLeftMove(Game *, Params *);
int_fast8_t CheckUpMove(Game *, Params *);
int_fast8_t CheckDownMove(Game *, Params *);

void ChangeCombinedToOld(Game *Game);
int_fast8_t UpdateWindowTitle(SDL_Window *win, Sint64 Score);

/*Сохранение игрового процесса game в файл finename*/
int_fast8_t SaveGame(Game *, const char *filename);

//Считает число переносов строки 
int_fast8_t CountLines(const char *source);

/*Возвращает случайное число из диапазона. 
Требует предварительной инициализации через srand*/
Sint32 RandomInt(Sint32, Sint32);

/*Создание набора цветов для текстур*/
SDL_Colour* CreateColourSet(int_fast8_t DarkModeFlag);

/*Считает и возвращает длинну кадра в мс*/
int_fast8_t dtCount(void);

/*Добавление нового элемента в игровое поле. Возврат индекса нового тайла*/
int_fast8_t AddElement(Game *Game);

//Создание размеров поля и тайла
void GetFieldAndTileSize(Game *Game, Params *Params);

/* На основе параметров запуска игры argv, в количестве argc, подбирается схема управления
 * и размер игрового поля. По умолчанию используются стрелки с мышью, поле имеет  размер 4x4*/
Game InitParamsAndGame(int argc, const char **argv, Params *Settings, const char *filename);

/*Функция меняет режим работы программы на основе
 уже выставленного режима работы и некоторого события */
void SetMode(SDL_Event *, Game *, Params *);

//Вызов SDL, создание окна под данным названием и рисовальщика 
int_fast8_t CreateWorkspace(SDL_Window **win, SDL_Renderer **rend, const char *title, 
					  const SDL_Point *WinSize, uint_fast8_t Flags);

//Вывести сообщение об ошибке и выйти из программы
int_fast8_t PrintErrorAndLeaveWithCode(int_fast8_t code, SDL_Window* win, SDL_Renderer* rend, Game* Game, Assets *Assets);

//Закрытие всех окон и рисовальщиков
int_fast8_t SilentLeaveWithCode(int_fast8_t code, SDL_Window* win, SDL_Renderer* rend, Game* Game, Assets *Assets);

/*Проверка окна win на изменение его размеров в Params, через ивенты ev.
Меняет на win_min при чрезмерном уменьшении. Возврат true, если размер изменён, либо false*/
int_fast8_t CheckForResize(SDL_Window* win, Params* Params, SDL_Event* ev, Uint16 win_min);

#endif // !MISC_H
