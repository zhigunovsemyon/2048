#include "draw.h"
/*Рисование сетки на фоне окна размера WinSize, светлой при Col_Mode = 0,
 * тёмной при Col_Mode в противном случае */
static Uint8 DrawBackground(SDL_Renderer* rend, Uint8 TileCount, Params* Params);

SDL_Texture* GetScoreTexture(SDL_Renderer* rend, SDL_Texture* OldTexture,
	SDL_Colour* ColourSet, SDL_Rect* Tile, Game* Game)
{
	if (OldTexture)
		SDL_DestroyTexture(OldTexture);

	char text[80];
	sprintf(text, "Число очков:\n%lu\nРекорд:\n%lu", Game->Score, Game->MaxScore);
	return CreateMessageTexture(rend, ColourSet + COL_BG,
		ColourSet + COL_FG, Tile, FONT, text, SDL_FALSE);
}

SDL_Texture** CreateTextureSet(SDL_Renderer* rend, SDL_Colour* ColourSet, SDL_Point* WinSize, Game* Game)
{
	char text[32];
	Uint64 TileValue = 1;
	SDL_Colour txt_col = { 0xFF, 0xFF, 0xFF, 0xFF };
	SDL_Texture** set = (SDL_Texture**)SDL_malloc(TEXTURES_COUNT * sizeof(SDL_Texture*));
	if (!set)
		return NULL;

	// Размер поля
	float FieldSize = FIELD_SIZE_COEFFICIENT * // Отношение размера поля к размеру экрана
		MinOfTwo(WinSize->x, WinSize->y); // Меньший и размеров окон

	SDL_Rect Tile;
	Tile.w = Tile.h = TILE_SIZE_COEFFICIENT * FieldSize / Game->FieldSize;

	for (Uint8 i = TEX_SQ2; i < TEXTURES_COUNT; ++i)
	{
		TileValue <<= 1;
		sprintf(text, "%lu", TileValue);
		set[i] = CreateMessageTexture(rend, &txt_col, ColourSet + i + 1, &Tile, FONT, text, SDL_TRUE);
		if (!(set[i]))
		{	//Очистка всех остальных текстур
			SDL_DestroyTexture(set[i]);
			for (; i; i--)
				SDL_DestroyTexture(set[i - 1]);
			SDL_free(set);
			return NULL;
		}
	}

	//Создание текстуры числа очков
	Tile.w *= 2;
	if (!(set[TEX_SCORE] = GetScoreTexture(rend, NULL, ColourSet, &Tile, Game)))
	{	//Очистка всех остальных текстур
		for (Uint8 i = TEX_SCORE; i; i--)
			SDL_DestroyTexture(set[i - 1]);
		SDL_free(set);
		return NULL;
	}
	return set;
}

SDL_Texture** UpdateTextureSet(SDL_Renderer* rend, SDL_Texture** OldSet,
	SDL_Colour* ColourSet, SDL_Point* WinSize, Game* Game)
{
	/*Освобождение всех текстур*/
	for (Uint8 i = 0; i < TEXTURES_COUNT; ++i)
		SDL_DestroyTexture(OldSet[i]);

	//Освобождать сам массив из памяти на данном этапе не нужно,
	//так как он заменится новыми текстурами.
	//Его освобождение должно происходить только на выходе из программы
	return CreateTextureSet(rend, ColourSet, WinSize, Game);
}

SDL_Texture *GetTextureForTile(Uint64 TileValue, SDL_Texture **textures)
{
	switch (TileValue)
	{
	case 2:
		return textures[TEX_SQ2];
	case 4:
		return textures[TEX_SQ4];
	case 8:
		return textures[TEX_SQ8];
	case 16:
		return textures[TEX_SQ16];
	case 32:
		return textures[TEX_SQ32];
	case 64:
		return textures[TEX_SQ64];
	case 128:
		return textures[TEX_SQ128];
	case 256:
		return textures[TEX_SQ256];
	case 512:
		return textures[TEX_SQ512];
	case 1024:
		return textures[TEX_SQ1024];
	case 2048:
		return textures[TEX_SQ2048];
	default: //>2048
		return textures[TEX_MAX];
	}
}

SDL_Texture *CreateMessageTexture(SDL_Renderer *rend, SDL_Colour const *txt_col, SDL_Colour *bg_col, SDL_Rect *txt_size,
								  const char *font_name, const char *message, Uint8 IsCentred)
{
	TTF_Font *font;
	SDL_Surface *txt_surf;

	Uint8 scaler = CountLines(message); // Отношение размера буквы к размеру окна
	do
	{
		// Открытие шрифта, проверка
		font = TTF_OpenFont(font_name, txt_size->h / scaler);
		if (!font)
			return NULL;

		// Создание поверхности с надписью, проверка
		txt_surf = TTF_RenderUTF8_Solid_Wrapped(font, message, *txt_col, 0);
		if (!txt_surf)
		{
			TTF_CloseFont(font);
			return NULL;
		}
		// Если размер текста меньше размера окна, цикл прерывается
		if (txt_surf->h <= txt_size->h && txt_surf->w <= txt_size->w)
			break;

		/*Если размер текста больше по одной из плоскостей, старый текст и
		поверхность очищаются, коэфициент увеличивается*/
		++scaler;
		TTF_CloseFont(font);
		SDL_FreeSurface(txt_surf);
	} while (SDL_TRUE); // Условие завершения описано внутри

	if (IsCentred) // Если был передан флаг выравнивания по центу
	{ /*К положению поверхностей прибавляется половина разницы между
	  размером внешенего прямоугольника и прямоугольника с текстом */
		txt_surf->clip_rect.x += ((txt_size->w - txt_surf->w) / 2);
		txt_surf->clip_rect.y += ((txt_size->h - txt_surf->h) / 2);
	}

	// Создание поверхности с фоном, проверка
	SDL_Surface *bg = SDL_CreateRGBSurfaceWithFormat(0, txt_size->w, txt_size->h, 32, SDL_PIXELFORMAT_ARGB8888);
	if (!bg)
	{
		SDL_FreeSurface(txt_surf);
		TTF_CloseFont(font);
		return NULL;
	}

	// Заливка поверхности с фоном
	if (SDL_FillRect(bg, &bg->clip_rect, SDL_MapRGBA(bg->format, SPLIT_COL_LINK(bg_col))))
	{
		SDL_FreeSurface(txt_surf);
		SDL_FreeSurface(bg);
		TTF_CloseFont(font);
		return NULL;
	}

	// Склейка поверхности с текстом на прямоугольник, проверка
	if (SDL_BlitSurface(txt_surf, NULL, bg, &txt_surf->clip_rect))
	{
		SDL_FreeSurface(bg);
		SDL_FreeSurface(txt_surf);
		TTF_CloseFont(font);
		return NULL;
	}

	// Создание текстуры из поверхности с надписью, проверка на корректность осуществляется вовне
	SDL_Texture *ret = SDL_CreateTextureFromSurface(rend, bg);
	TTF_CloseFont(font); // Закрытие шрифта
	SDL_FreeSurface(txt_surf);
	SDL_FreeSurface(bg); // Очистка поверхностей
	return ret;			 // Возврат тексутры, либо NULL
}

static Uint8 DrawBackground(SDL_Renderer *rend, Uint8 TileCount, Params *Params)
{
	// Заливка фона
	if (SDL_SetRenderDrawColor(rend, SPLIT_COL_VAL(Params->cols[COL_BG])))
		return ERR_SDL;
	if (SDL_RenderClear(rend))
		return ERR_SDL;

	// Размер поля и плитки
	float FieldSize = FIELD_SIZE_COEFFICIENT * // Отношение размера поля к размеру экрана
					  MinOfTwo(Params->WinSize.x, Params->WinSize.y); // Меньший и размеров окон
	float TileSize = FieldSize / TileCount;							  // Размер одного тайла
	SDL_Point Corner =												  // Координаты угла поля
		{(Params->WinSize.x - FieldSize) * 0.5, (Params->WinSize.y - FieldSize) * 0.5};

	// Задание цвета фона
	if (SDL_SetRenderDrawColor(rend, SPLIT_COL_VAL(Params->cols[COL_FG])))
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

Uint8 DrawOldElements(SDL_Renderer *rend, Params *Params, Game *Game)
{
	// Размер поля
	float FieldSize = FIELD_SIZE_COEFFICIENT * // Отношение размера поля к размеру экрана
					  MinOfTwo(Params->WinSize.x, Params->WinSize.y); // Меньший и размеров окон

	SDL_Rect Tile;
	Uint16 CellWidth = FieldSize / Game->FieldSize;
	Tile.w = Tile.h = TILE_SIZE_COEFFICIENT * CellWidth;

	// Рисование поля
	if (DrawBackground(rend, Game->FieldSize, Params) /*== ERR_SDL*/)
		return ERR_SDL;

	for (Uint8 i = 0; i < _SQ(Game->FieldSize); i++)
	{
		if (Game->Field[i].mode != TILE_OLD)
			continue;
		// Положение угла поля в координатах +
		// Сдвиг координаты угла плитки на её положение в матрице, плюс разницу размеров плитки и ячейки
		Tile.x = (Params->WinSize.x - FieldSize) * 0.5 + (CellWidth - Tile.w) * 0.5 + CellWidth * (i % Game->FieldSize);
		Tile.y = (Params->WinSize.y - FieldSize) * 0.5 + (CellWidth - Tile.w) * 0.5 + CellWidth * (i / Game->FieldSize);
		SDL_Texture *tile_texture = GetTextureForTile(Game->Field[i].val, Params->textures);
		if (SDL_RenderCopy(rend, tile_texture, NULL, &Tile))
			return ERR_SDL;
	}
	return ERR_NO;
}

Uint8 DrawNewElement(SDL_Renderer *rend, Params *Params, Game *Game, Sint8 Index)
{
	// Размер поля
	float FieldSize = FIELD_SIZE_COEFFICIENT * // Отношение размера поля к размеру экрана
					  MinOfTwo(Params->WinSize.x, Params->WinSize.y); // Меньший и размеров окон

	SDL_Rect Tile;
	SDL_Texture *tile_texture = GetTextureForTile(Game->Field[Index].val, Params->textures);

	/*Каждый виток размер растёт и записывается в Tile.w, хранящий размер плитки*/
	Tile.w = (int)(Game->Field[Index].size += ANIM_SPEED * dtCount() / 1000.0f);

	// Размер одной ячейки хранится в h
	Tile.h = FieldSize / Game->FieldSize;

	// Положение угла поля в координатах
	Tile.x = (Params->WinSize.x - FieldSize) * 0.5;
	Tile.y = (Params->WinSize.y - FieldSize) * 0.5;

	// Сдвиг координаты угла плитки на её положение в матрице, плюс разницу размеров плитки и ячейки
	Tile.x += (Tile.h * (Index % Game->FieldSize)) + (Tile.h - Tile.w) * 0.5;
	Tile.y += (Tile.h * (Index / Game->FieldSize)) + (Tile.h - Tile.w) * 0.5;
		
	Tile.h = Tile.w; // Запись корректной высоты плитки

	/*Если размер плитки ещё слишком мал*/
	if (Game->Field[Index].size / TILE_SIZE_COEFFICIENT < (FieldSize / Game->FieldSize))
	{	//Отрисовка промежуточного значения. Если SDL_RenderCopy упал, возврат кода ошибки
		if (SDL_RenderCopy(rend, tile_texture, NULL, &Tile))
			return ERR_SDL;
		return ERR_NO;
	}
		
	// Отрисовка окончательного положения квадрата
	Tile.h = FieldSize / Game->FieldSize;
	Tile.w = Tile.h * TILE_SIZE_COEFFICIENT;
	// Положение угла поля в координатах
	// Сдвиг координаты угла плитки на её положение в матрице, плюс разницу размеров плитки и ячейки
	Tile.x = (Params->WinSize.x - FieldSize) * 0.5 + (Tile.h * (Index % Game->FieldSize)) + (Tile.h - Tile.w) * 0.5;
	Tile.y = (Params->WinSize.y - FieldSize) * 0.5 + (Tile.h * (Index / Game->FieldSize)) + (Tile.h - Tile.w) * 0.5;
	Tile.h = Tile.w; // Запись корректной высоты плитки

	//Отрисовка конечного тайла
	if (SDL_RenderCopy(rend, tile_texture, NULL, &Tile))
		return ERR_SDL;
	Game->Field[Index].mode = TILE_OLD;	//Установка флага, что теперь эта ячейка отрисована
	Game->Field[Index].size = 0;		//Сброс параметра размера
	return ERR_NO;
}
