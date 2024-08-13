#ifndef DRAW_H
#define DRAW_H
#include "main.h"

/*Отрисовка статичных элементов из Game и фона рисовальщиком rend,
с учётом параметров Params. Возврат кода ошибки ERR_SDL либо 0*/
int_fast8_t DrawOldElements(SDL_Renderer *, Params *, Game *, Assets *);

/*Отрисовка нового элемента Index из Game и фона рисовальщиком rend, с учётом
параметров Params. Возврат кода ошибки ERR_SDL либо 0*/
int_fast8_t DrawNewElement(SDL_Renderer *, Params *, Game *, Assets *, Sint8 Index);

/*Cоздание тексутры с числом очков из Game, рисовальщиком rend, 
с цветами из набора ColourSet, вписанный в Tile.
Возврат кода ошибки ERR_SDL либо 0*/
SDL_Texture *CreateScoreTexture(SDL_Renderer *, SDL_Colour *ColourSet,
							 SDL_Rect *Tile, Game *);

/*Создание рисовальщиком rend набора текстур для тайлов 2, 4, и очков*/
int_fast8_t InitTextureSet(SDL_Renderer *, Assets *, Params *, Game *);

/*Создание текстуры тайла со значением TileValue, размера CellWidth,
 * рисовальщиком rend, из набора цветов cols*/
SDL_Texture *CreateTileTexture(SDL_Renderer *, Sint64 TileValue,
							   Assets *, float CellWidth);

/*Обновление рисовальщиком rend набора текстур, используемых игрой,
с учётом параметров игры Params, размеров поля и очков игры из Game*/
int_fast8_t UpdateTextureSet(SDL_Renderer *, Params *, Game *, Assets *Assets);

// Подбор текстуры из набора textures для ячейки размерности TileValue
SDL_Texture *GetTextureForTile(Sint64 TileValue, Assets *);

/*Рисование текстуры с текстом из message рисовальщиком rend шрифтом
 * font_name, вписанной в квадрат txt_size. Текст может быть отцентрован при
 передаче флага IsCentred*/
SDL_Texture *CreateMessageTexture(SDL_Renderer *, SDL_Colour const *txt_col,
								  SDL_Colour *bg_col, SDL_Rect *txt_size,
								  const char *font_name, const char *message,
								  int_fast8_t IsCentred);

#endif // !DRAW_H
