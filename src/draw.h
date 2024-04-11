#ifndef DRAW_H
#define DRAW_H
#include "main.h"

/*Рисование сетки на фоне окна размера WinSize, светлой при Col_Mode = 0, 
 * тёмной при Col_Mode в противном случае */ 
Uint8 DrawBackground(SDL_Renderer *rend, const SDL_Point *WinSize, Uint8 Col_Mode);

#endif // !DRAW_H
