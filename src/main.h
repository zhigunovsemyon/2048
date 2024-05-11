#ifndef MAIN_H
#define MAIN_H

#include <time.h>

#include "SDL.h"
#ifndef SDL_h_
#include <SDL2/SDL.h>
#endif // !SDL_h_

#include "SDL_ttf.h"
#ifndef SDL_TTF_H_
#include <SDL2/SDL_ttf.h>
#endif // !SDL_h_

typedef struct
{
	Uint64 val;//Значение ячейки
	Uint8 mode;//Состояние данной плитки
	union
	{
		float size;		//Размеры новой ячейки
		float offset;	//Сдвиг перемещаемой ячейки
	};
}
Tile;

typedef struct
{
	Tile* Field;	//Поле ячеек
	Uint8 FieldSize;//Размер поля
	Uint64 Score;	//Текущий счёт
	Uint64 MaxScore;//Максимальный счёт
}
Game;

typedef struct 
{
	Uint8 Flags;
	Uint8 Mode;
	SDL_Point WinSize;
	SDL_Texture **textures;
	SDL_Colour *cols;
}
Params;

#include "defines.h"
#include "misc.h"
#include "draw.h"

/*Набор функций расстановки сдвигов тайлов поля Game.
используются номера MODE_CHECK_RIGHT, MODE_CHECK_LEFT, MODE_CHECK_DOWN, MODE_CHECK_UP*/
extern Uint8(**CheckMove)(Game*);

#endif // !_MAIN_H_
