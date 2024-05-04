#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
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
	Uint64* Field;
	Uint8 FieldSize;
	Uint64 Score;
	Uint64 MaxScore;
}
Game;

typedef struct 
{
	Uint8 Flags;
	Uint8 Mode;
	SDL_Point WinSize;
}
Params;

#include "defines.h"
#include "misc.h"
#include "draw.h"

#endif // !_MAIN_H_
