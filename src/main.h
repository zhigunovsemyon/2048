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
	Uint8 Mode;		//Текущий этап игры
}
Game;

//Параметры игры
typedef struct 
{
	/* Набор флагов : FLAG_DARKMODE, FLAG_ARROWKEY,
	FLAG_VIKEY, FLAG_WASDKEY, FLAG_VSYNC*/
	Uint8 Flags;
	SDL_Point WinSize;	//Размер окна по горизонтали и вертикали
	float CellWidth;	//Размер стороны ячейки с тайлом
	float FieldSize;	//Размер стороны поля
}
Params;

//Ассеты игры
typedef struct
{
	TileTexture *textures;	//Указатель на массив тексутр
	Uint8 textures_count;	//Количество сохранённых текстур
	SDL_Colour *cols;		//Указатель на массив цветов
}
Assets;

#include "defines.h"
#include "misc.h"
#include "draw.h"

Uint8 Greeting(SDL_Window *window, SDL_Renderer *rend, SDL_Event *ev, Assets *Assets,
			   Params *Params, Game *Game);


/*Набор функций расстановки сдвигов тайлов поля Game.
Используются номера MODE_CHECK_RIGHT, MODE_CHECK_LEFT, MODE_CHECK_DOWN, MODE_CHECK_UP*/
extern Uint8(**CheckMove)(Game*, Params*);

/*Набор функций отрисовки сдвигов тайлов поля Game.
Используются номера MODE_MOVE_RIGHT, MODE_MOVE_LEFT, MODE_MOVE_DOWN, MODE_MOVE_UP*/
extern Uint8 (**DoMove)(SDL_Renderer*, Game *, Params *, Assets *);

/*Набор функций тайлов поля Game на складываемость друг с другом.
Используются номера MODE_CHECK_RIGHT, MODE_CHECK_LEFT, MODE_CHECK_DOWN, MODE_CHECK_UP*/
extern Uint8 (**CheckCombo)(Game *, Params *);

#endif // !_MAIN_H_
