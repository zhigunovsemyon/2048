#include "draw.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

Uint8 DrawBackground(SDL_Renderer *rend, Uint8 TileCount, Params *Params)
{
	// Определение яркости фона и сетки
	Uint8 BG_brightness = (Params->Flags & FLAG_DARKMODE) ? BG_DARK_BRIGHTNESS : BG_LIGHT_BRIGHTNESS;
	Uint8 FG_brightness = (!(Params->Flags & FLAG_DARKMODE)) ? BG_DARK_BRIGHTNESS : BG_LIGHT_BRIGHTNESS;

	// Заливка фона
	if (SDL_SetRenderDrawColor(rend, BG_brightness, BG_brightness, BG_brightness, 0xff))
		return SDLERR;
	if (SDL_RenderClear(rend))
		return SDLERR;

	// Размер поля и плитки
	float FieldSize = FIELD_SIZE_COEFFICIENT * // Отношение размера поля к размеру экрана
					  MinOfTwo(Params->WinSize.x, Params->WinSize.y); // Меньший и размеров окон
	float TileSize = FieldSize / TileCount;							  // Размер одного поля
	SDL_Point Corner =												  // Координаты угла поля
		{(Params->WinSize.x - FieldSize) * 0.5, (Params->WinSize.y - FieldSize) * 0.5};
	SDL_Log("Размер поля: %f\n", FieldSize);
	SDL_Log("Размер плитки: %f\n", TileSize);

	// Задание цвета фона
	if (SDL_SetRenderDrawColor(rend, FG_brightness, FG_brightness, FG_brightness, 0xff))
		return SDLERR;

	// Цикл, рисующий линии поля
	for (Uint8 i = 0; i <= TileCount; i++)
	{
		if (SDL_RenderDrawLine(rend, Corner.x, Corner.y + i * TileSize, // Начало горизонатльной линии
							   Corner.x + FieldSize, Corner.y + i * TileSize)) // Конец горизонтальной линии
			return SDLERR;
		if (SDL_RenderDrawLine(rend, Corner.x + i * TileSize, Corner.y, // Начало вертикальной линии
							   Corner.x + i * TileSize, Corner.y + FieldSize)) // Конец вертикальной линии
			return SDLERR;
	}
	return NOERR;
}
