#include "draw.h"

SDL_Texture *CreateGreetingTexture(SDL_Renderer *rend, Params *Params, SDL_Rect *txt_size, 
								   const char *font_name, const char *message)
{
	TTF_Font *font = TTF_OpenFont(font_name, Params->WinSize.y / 20);
	if (!font)
		return NULL;

	SDL_Colour txt_col;
	txt_col.a = 0xFF;
	if (FLAG_DARKMODE & Params->Flags)
	{
		txt_col.r = BG_LIGHT_BRIGHTNESS;
		txt_col.g = BG_LIGHT_BRIGHTNESS;
		txt_col.b = BG_LIGHT_BRIGHTNESS;
	}
	else
	{
		txt_col.r = BG_DARK_BRIGHTNESS;
		txt_col.b = BG_DARK_BRIGHTNESS;
		txt_col.g = BG_DARK_BRIGHTNESS;
	}

	SDL_Surface *txt_surf = TTF_RenderUTF8_Solid_Wrapped(font, message, txt_col, 0);
	if (!txt_surf)
	{
		TTF_CloseFont(font);
		return NULL;
	}
	SDL_Texture *ret = SDL_CreateTextureFromSurface(rend, txt_surf);
	TTF_CloseFont(font);
	txt_size->w = txt_surf->w;
	txt_size->h = txt_surf->h;
	SDL_FreeSurface(txt_surf);
	return ret;
}

Uint8 DrawBackground(SDL_Renderer *rend, Uint8 TileCount, Params *Params)
{
	// Определение яркости фона и сетки
	Uint8 BG_brightness = (Params->Flags & FLAG_DARKMODE) ? BG_DARK_BRIGHTNESS : BG_LIGHT_BRIGHTNESS;
	Uint8 FG_brightness = (!(Params->Flags & FLAG_DARKMODE)) ? BG_DARK_BRIGHTNESS : BG_LIGHT_BRIGHTNESS;

	// Заливка фона
	if (SDL_SetRenderDrawColor(rend, BG_brightness, BG_brightness, BG_brightness, 0xff))
		return ERR_SDL;
	if (SDL_RenderClear(rend))
		return ERR_SDL;

	// Размер поля и плитки
	float FieldSize = FIELD_SIZE_COEFFICIENT * // Отношение размера поля к размеру экрана
					  MinOfTwo(Params->WinSize.x, Params->WinSize.y); // Меньший и размеров окон
	float TileSize = FieldSize / TileCount;							  // Размер одного поля
	SDL_Point Corner =												  // Координаты угла поля
		{(Params->WinSize.x - FieldSize) * 0.5, (Params->WinSize.y - FieldSize) * 0.5};

	// Задание цвета фона
	if (SDL_SetRenderDrawColor(rend, FG_brightness, FG_brightness, FG_brightness, 0xff))
		return ERR_SDL;

	// Цикл, рисующий линии поля
	for (Uint8 i = 0; i <= TileCount; i++)
	{
		if (SDL_RenderDrawLine(rend, Corner.x, Corner.y + i * TileSize, // Начало горизонатльной линии
							   Corner.x + FieldSize, Corner.y + i * TileSize)) // Конец горизонтальной линии
			return ERR_SDL;
		if (SDL_RenderDrawLine(rend, Corner.x + i * TileSize, Corner.y, // Начало вертикальной линии
							   Corner.x + i * TileSize, Corner.y + FieldSize)) // Конец вертикальной линии
			return ERR_SDL;
	}
	return ERR_NO;
}
