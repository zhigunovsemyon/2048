#ifndef MAIN_H
#define MAIN_H

#include "SDL.h"
#ifndef SDL_h_
#include <SDL2/SDL.h>
#endif // !SDL_h_

enum Errors {NOERR, SDLERR};

typedef struct
{
	Uint64* Field;
	Uint8 FieldSize;
	Uint64 Score;
}
Game;

typedef struct 
{
	Uint8 quit;
	Uint8 left;
	Uint8 right;
	Uint8 up;
	Uint8 down;
	Uint8 mouse;
	Uint8 Dark;
}
Settings;

#include "defines.h"
#include "logic.h"
#include "misc.h"
#include "draw.h"

#endif // !_MAIN_H_
