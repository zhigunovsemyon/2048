#include "draw.h"

SDL_Texture *CreateMessageTexture(SDL_Renderer *rend, Params *Params, SDL_Rect *txt_size, 
								   const char *font_name, const char *message)
{
	TTF_Font* font;
	SDL_Surface* txt_surf;
	SDL_Colour txt_col;	//Определение цвета шрифта
	txt_col.a = 0xFF;
	if (FLAG_DARKMODE & Params->Flags)//Светлый текст для тёмного режима
	{
		txt_col.r = BG_LIGHT_BRIGHTNESS;
		txt_col.g = BG_LIGHT_BRIGHTNESS;
		txt_col.b = BG_LIGHT_BRIGHTNESS;
	}
	else	//Тёмный текст для светлого режима
	{
		txt_col.r = BG_DARK_BRIGHTNESS;
		txt_col.b = BG_DARK_BRIGHTNESS;
		txt_col.g = BG_DARK_BRIGHTNESS;
	}

	Uint8 scaler = 12; //Отношение размера буквы к размеру окна
	do
	{
		//Открытие шрифта, проверка
		font = TTF_OpenFont(font_name, Params->WinSize.y / scaler);
		if (!font)
			return NULL;

		//Создание поверхности с надписью, проверка
		txt_surf = TTF_RenderUTF8_Solid_Wrapped(font, message, txt_col, 0);
		if (!txt_surf)
		{
			TTF_CloseFont(font);
			return NULL;
		}
		//Если размер текста меньше размера окна, цикл прерывается
		if (txt_surf->h < Params->WinSize.y && txt_surf->w < Params->WinSize.x)
			break;

		/*Если размер текста больше по одной из плоскостей, старый текст и 
		поверхность очищаются, коэфициент увеличивается*/
		++scaler;
		TTF_CloseFont(font);
		SDL_FreeSurface(txt_surf);
	} while (SDL_TRUE); //Условие завершения описано внутри
	
	//Создание текстуры из поверхности с надписью
	SDL_Texture *ret = SDL_CreateTextureFromSurface(rend, txt_surf);
	TTF_CloseFont(font);		//Закрытие шрифта
	txt_size->w = txt_surf->w,	//Передача размера надписи во вне
	txt_size->h = txt_surf->h;
	SDL_FreeSurface(txt_surf);	//Очистка поверхности
	return ret;					//Возврат тексутры, либо NULL
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

Uint8 DrawNewElement(SDL_Renderer *rend,Params *Params, Game *Game, Sint8 Index)
{
	SDL_Rect Tile;
	// Размер поля
	float FieldSize = FIELD_SIZE_COEFFICIENT * // Отношение размера поля к размеру экрана
					  MinOfTwo(Params->WinSize.x, Params->WinSize.y); // Меньший и размеров окон

	Tile.w = 0;
	for (float size = 0; /*Перед циклом размер зануляется*/
		/*Пограничное условие*/
		size / TILE_SIZE_COEFFICIENT < (FieldSize / Game->FieldSize);
		/*Каждый виток размер растёт и записывается в Tile.w, хранящий размер плитки*/
		Tile.w = (int)(size += ANIM_SPEED * dtCount() / 1000.0f))
	{
		//Рисование поля
		if(DrawBackground(rend, Game->FieldSize, Params)/*== ERR_SDL*/)
			return ERR_SDL;
	
		// Задание цвета фона
		if (SDL_SetRenderDrawColor(rend, 0xFF, 0, 0, 0xff))
			return ERR_SDL;

		// Размер одного поля хранится в h
		Tile.h = FieldSize / Game->FieldSize;
		
		// Положение угла поля в координатах
		Tile.x = (Params->WinSize.x - FieldSize) * 0.5;
		Tile.y = (Params->WinSize.y - FieldSize) * 0.5;

		//Сдвиг координаты угла плитки на её положение в матрице, плюс разницу размеров плитки и ячейки
		Tile.x += (Tile.h * (Index % Game->FieldSize)) + (Tile.h - Tile.w) * 0.5;
		Tile.y += (Tile.h * (Index / Game->FieldSize)) + (Tile.h - Tile.w) * 0.5;

		Tile.h = Tile.w;//Запись корректной высоты плитки
		
		/*Рисование и отображение прямоугольника*/
		if (SDL_RenderFillRect(rend, &Tile))
			return ERR_SDL;
		
		SDL_RenderPresent(rend);
	}
	return ERR_NO;
}
