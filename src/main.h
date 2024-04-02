#ifndef _MAIN_H_
#define _MAIN_H_

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
}
Controls;

#include "logic.h"
#endif // !_MAIN_H_
