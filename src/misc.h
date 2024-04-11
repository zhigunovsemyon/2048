#ifndef MISC_H
#define MISC_H

#include "main.h"

//Вызов SDL, создание окна под данным названием и рисовальщика 
Uint8 CreateWorkspace(SDL_Window **win, SDL_Renderer **rend, const char *title, const SDL_Point *WinSize);

//Вывести сообщение об ошибки и выйти из программы
Uint8 PrintErrorAndLeaveWithCode (Uint8 code, SDL_Window* win, SDL_Renderer* rend);

//Закрытие всех окон и рисовальщиков
Uint8 SilentLeaveWithCode (Uint8 code, SDL_Window* win, SDL_Renderer* rend);

#endif // !MISC_H
