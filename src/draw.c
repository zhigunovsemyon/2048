#include "draw.h"

Uint8 DrawBackground(SDL_Renderer *rend, const SDL_Point *WinSize, Uint8 Col_Mode)
{
	Uint8 BG_brightness;
	if (Col_Mode)
		BG_brightness = BG_DARK_BRIGHTNESS;
	else 
		BG_brightness = BG_LIGHT_BRIGHTNESS;
	

	if(SDL_SetRenderDrawColor(rend, BG_brightness, BG_brightness, BG_brightness, 0xff))
		return SDLERR;
	if(SDL_RenderClear(rend))
		return SDLERR;

	return NOERR;
}
