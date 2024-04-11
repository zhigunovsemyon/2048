#include "misc.h"

Uint8 PrintErrorAndLeaveWithCode (Uint8 code, SDL_Window* win, SDL_Renderer* rend)
{
	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s\n", SDL_GetError());
	return SilentLeaveWithCode(code, win, rend);
}

Uint8 SilentLeaveWithCode (Uint8 code, SDL_Window* win, SDL_Renderer* rend)
{
	//Уничтожение рисовальщика
	if(rend)
		SDL_DestroyRenderer(rend);

	//Уничтожение окна
	if(win)
		SDL_DestroyWindow(win);
	
	//Возврат кода ошибки для выхода
	return code;
}

Uint8 CreateWorkspace(SDL_Window **win, SDL_Renderer **rend, const char *title, const SDL_Point *WinSize)
{
	// Вызов SDL
	if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
	{
		// Завершение работы, если не удалось вызвать SDL
		return SDLERR;
	}

	//Создание окна
	*win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
					 WinSize->x, WinSize->y,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(!(*win))
	{
		return PrintErrorAndLeaveWithCode(SDLERR, *win, *rend);
	}

	//Создание рисовальщика
	*rend = SDL_CreateRenderer(*win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(!(*rend))
	{
		return PrintErrorAndLeaveWithCode(SDLERR, *win, *rend);
	}

	//Возврат кода штатной работы
	return NOERR;
}
