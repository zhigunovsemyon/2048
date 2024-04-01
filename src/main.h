#include "SDL.h"
#ifndef SDL_h_
#include <SDL2/SDL.h>
#endif // !SDL_h_
#include <SDL2/SDL.h>

enum Errors {NOERR, SDLERR};

typedef struct
{
	Uint64* Field;
	Uint64 Score;
}
Game;

typedef struct 
{
	Uint8 quit;
	Uint8 left;
	Uint8 righ;
	Uint8 up;
	Uint8 down;
	Uint8 mouse;
}
Controls;

