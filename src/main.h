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

//Текстура некоторого тайла
typedef struct
{
	Uint64 val;//Значение тайла, для которого используется текстура
	SDL_Texture *tex;//Сама текстура
}
TileTexture;

//Структура, описывающая один тайл
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

//Структура с полем и очками
typedef struct
{
	Tile* Field;	//Поле ячеек
	Uint8 FieldSize;//Размер поля
	Uint64 Score;	//Текущий счёт
	Uint64 MaxScore;//Максимальный счёт
}
Game;

//Параметры игры
typedef struct 
{
	Uint8 Flags;
	Uint8 Mode;
	SDL_Point WinSize;
	float CellWidth;
	float FieldSize;
}
Params;

//Ассеты игры
typedef struct
{
	TileTexture *textures;
	Uint8 textures_count;
	SDL_Colour *cols;
}
Assets;

#include "defines.h"
#include "misc.h"
#include "draw.h"

Uint8 Greeting(SDL_Window *window, SDL_Renderer *rend, SDL_Event *ev, Assets *Assets,
			   Params *Params, Game *Game, Uint8 NextMode);


/*Набор функций расстановки сдвигов тайлов поля Game.
используются номера MODE_CHECK_RIGHT, MODE_CHECK_LEFT, MODE_CHECK_DOWN, MODE_CHECK_UP*/
extern Uint8(**CheckMove)(Game*, Params*);

/*Набор функций отрисовки сдвигов тайлов поля Game.
используются номера MODE_MOVE_RIGHT, MODE_MOVE_LEFT, MODE_MOVE_DOWN, MODE_MOVE_UP*/
extern Uint8 (**DoMove)(SDL_Renderer*, Game *, Params *, Assets *);
extern Uint8 (**CheckCombo)(Game *, Params *);

#endif // !_MAIN_H_
