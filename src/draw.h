#ifndef DRAW_H
#define DRAW_H
#include "main.h"

/*Отрисовка статичных элементов из Game и фона рисовальщиком rend,
с учётом параметров Params. Возврат кода ошибки ERR_SDL либо 0*/
Uint8 DrawOldElements(SDL_Renderer *, Params *, Game *, Assets *);

/*Отрисовка нового элемента Index из Game и фона рисовальщиком rend, с учётом
параметров Params. size определяет текущий размер в цикле отрисовки. Возврат
кода ошибки ERR_SDL либо 0*/
Uint8 DrawNewElement(SDL_Renderer *, Params *, Game *, Assets *, Sint8 Index);

/*Обновление OldTexture или, если передан NULL, создание тексутры с числом очков
из Game, рисовальщиком rend, с цветами из набора ColourSet, вписанный в Tile.
Возврат кода ошибки ERR_SDL либо 0*/
SDL_Texture *GetScoreTexture(SDL_Renderer *, SDL_Texture *OldTexture,
							 SDL_Colour *ColourSet, SDL_Rect *Tile, Game *);

/*Создание рисовальщиком rend набора текстур для тайлов 2, 4, и очков,
 * используемая игрой, из набора цветов ColourSet,
с учётом размера экрана WinSize, размеров поля и очков игры из Game*/
Uint8 InitTextureSet(SDL_Renderer *, Assets *, Params *, Game *);

/*Создание текстуры тайла со значением TileValue, размера CellWidth,
 * рисовальщиком rend, из набора цветов cols*/
SDL_Texture *CreateTileTexture(SDL_Renderer *rend, Uint64 TileValue,
							   Assets *Assets, float CellWidth);

/*Обновление рисовальщиком rend набора текстур, используемых игрой,
с учётом размера экрана, размеров поля и очков игры из Game*/
Uint8 UpdateTextureSet(SDL_Renderer *, Params *, Game *, Assets *Assets);

// Подбор текстуры из набора textures для ячейки размерности TileValue
SDL_Texture *GetTextureForTile(Uint64 TileValue, Assets *);

/*Рисование приветственного сообщения message рисовальщиком rend шрифтом
 * font_name на основе пареметров Params, c записью размеров текстуры в
 * txt_size, */
SDL_Texture *CreateMessageTexture(SDL_Renderer *, SDL_Colour const *txt_col,
								  SDL_Colour *bg_col, SDL_Rect *txt_size,
								  const char *font_name, const char *message,
								  Uint8 IsCentred);

#endif // !DRAW_H
