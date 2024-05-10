#ifndef DRAW_H
#define DRAW_H
#include "main.h"

Uint8 DrawOldElements(SDL_Renderer *rend, Params *Params, Game *Game);
Uint8 DrawNewElement(SDL_Renderer *rend,Params *Params, Game *Game, Sint8 Index, float *size);

/*Рисование сетки на фоне окна размера WinSize, светлой при Col_Mode = 0, 
 * тёмной при Col_Mode в противном случае */ 
Uint8 DrawBackground(SDL_Renderer *rend, Uint8 TileCount, Params *Params);

/*Рисование приветственного сообщения message рисовальщиком rend шрифтом font_name
 * на основе пареметров Params, c записью размеров текстуры в txt_size, */
SDL_Texture *CreateMessageTexture(SDL_Renderer *rend, SDL_Colour const *txt_col, SDL_Colour *bg_col,
								  SDL_Rect *txt_size, const char *font_name, const char *message, Uint8 IsCentred);

#endif // !DRAW_H
