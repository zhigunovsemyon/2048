#include "draw.h"

static Uint8 UpdateScore(SDL_Renderer *rend, Game *Game, Params *Params, Assets *Assets)
{
	//Освобождение старой текстуры
	SDL_DestroyTexture(Assets->textures[0].tex);

	//Обновление рекорда
	if (Game->Score > Game->MaxScore)
		Game->MaxScore = Game->Score;	

	//Создание новой рисовальщиком, с учётом очков, как старых, так и новых, размеров площадки, цветов
	SDL_Rect scoreField = {.h = (Params->WinSize.y - (int)Params->FieldSize) /2,// Params->CellWidth,
	.w = Params->FieldSize};
	if(!(Assets->textures[0].tex = CreateScoreTexture(rend, Assets->cols, &scoreField, Game)))
		return ERR_SDL;
	
	return ERR_NO;
}

static Uint8 MatchColForTile(Uint64 TileValue)
{
	switch (TileValue)
	{
	case 2:
		return COL_SQ2;
	case 4:
		return COL_SQ4;
	case 8:
		return COL_SQ8;
	case 16:
		return COL_SQ16;
	case 32:
		return COL_SQ32;
	case 64:
		return COL_SQ64;
	case 128:
		return COL_SQ128;
	case 256:
		return COL_SQ256;
	case 512:
		return COL_SQ512;
	case 1024:
		return COL_SQ1024;
	case 2048:
		return COL_SQ2048;
	default:
		return COL_MAX;
	}
}

SDL_Texture *CreateTileTexture(SDL_Renderer *rend, Uint64 TileValue,
							   Assets *Assets, float CellWidth)
{
	if (!TileValue /* == 0*/)
		return NULL;

	SDL_Colour txt_col = {0xFF, 0xFF, 0xFF, 0xFF};
	SDL_Rect txt_size;
	txt_size.h = txt_size.w = (int)(TILE_SIZE_COEFFICIENT * CellWidth);
	char *stringForTex;
	SDL_asprintf(&stringForTex, "%lu", TileValue);
	if (!stringForTex)
		return NULL;

	SDL_Texture *tmp = CreateMessageTexture(
		rend, &txt_col, Assets->cols + MatchColForTile(TileValue), &txt_size,
		FONT, stringForTex, SDL_TRUE);

	SDL_free(stringForTex);
	return tmp;
}

/*Рисование сетки на фоне окна размера WinSize, светлой при Col_Mode = 0,
 * тёмной при Col_Mode в противном случае */
static Uint8 DrawBackground(SDL_Renderer *rend, Uint8 TileCount, Params *Params,
							Assets *Assets);
Uint8 DrawSingleMovingElement(SDL_Renderer *rend, Params *Params, Game *Game,
							  Assets *Assets, Sint8 Index);

static Uint8 DoRightMove(SDL_Renderer *rend, Game *Game, Params *Params,
						 Assets *Assets)
{
	if (DrawOldElements(rend, Params, Game, Assets))
		return ERR_SDL;

	Uint8 Flag = 0;
	// Размер поля
	float change = (ANIM_SPEED * dtCount() / 1000.0f);

	/*Умножение всех оффсетов на ширину ячейки */
	// Цикл перебора каждой строки
	for (Sint8 i = 0; i < Game->FieldSize; i++)
	{ // Цикл перебора каждого столбца с конца
		for (Sint8 j = Game->FieldSize - 1; j >= 0; j--)
		{ // Если данная ячейка пустая, или она не движется по горизонтали
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
				continue;
			if (Game->Field[i * Game->FieldSize + j].mode != TILE_MOVE_X)
				continue;

			if (0 < SDL_roundf(Game->Field[i * Game->FieldSize + j].offset))
			{
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											i * Game->FieldSize + j))
					return ERR_SDL;

				Flag++;
				Game->Field[i * Game->FieldSize + j].offset -= change;
			}
			else // Если элемент сдвинулся на целую ячейку
			{
				if (Game->Field[i * Game->FieldSize + j + 1].mode !=
					TILE_COMBINED)
				{
					// Копирование текущего элемента в следующий
					Game->Field[i * Game->FieldSize + j + 1] =
						Game->Field[i * Game->FieldSize + j];
					Game->Field[i * Game->FieldSize + j + 1].mode = TILE_OLD;
				}
				else
				{
					Game->Field[i * Game->FieldSize + j + 1].mode =
						TILE_JUSTCOMBINED;
					Game->Field[i * Game->FieldSize + j + 1].val <<= 1;
					Game->Score += Game->Field[i * Game->FieldSize + j + 1].val;
					if(UpdateScore(rend, Game, Params, Assets))
						return ERR_SDL;
					SDL_Log("Очков: %lu", Game->Score);
				}
				Game->Field[i * Game->FieldSize + j + 1].offset = 0;

				// Зануление прошлого элемента
				SDL_memset(Game->Field + (i * Game->FieldSize + j), 0,
						   sizeof(Tile));
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											i * Game->FieldSize + j + 1))
					return ERR_SDL;
			}
		}
	}
	if (Flag)
	{
		Params->Mode = MODE_MOVE_RIGHT;
		return ERR_NO;
	}
	/* Если CheckMove вернул флаг возможности смещения элемента,
	 * он записывается в глобальный параметр режима
		Если поле заполнено, будет включен режим выхода, если движение
	 невозможно, но поле содержит пустые ячейки, будет включен режим добавления
	 нового элемента*/
	if (CheckRightMove(Game, Params) == MODE_MOVE_RIGHT)
	{
		Params->Mode = MODE_MOVE_RIGHT;
		return ERR_NO;
	}

	// Если возможно сложение вправо
	if (CheckRightCombo(Game, Params))
	{
		Params->Mode = MODE_MOVE_RIGHT;
		return ERR_NO;
		// return (UpdateScore(rend, Game, Params, Assets)) ? ERR_SDL : ERR_NO;
	}
	// Если же нет, осуществляется проверка на добавление нового элемента
	Params->Mode = MODE_ADD;
	return ERR_NO;
}

static Uint8 DoLeftMove(SDL_Renderer *rend, Game *Game, Params *Params,
						Assets *Assets)
{
	if (DrawOldElements(rend, Params, Game, Assets))
		return ERR_SDL;

	Uint8 Flag = 0;
	// Размер поля
	float change = (ANIM_SPEED * dtCount() / 1000.0f);

	/*Умножение всех оффсетов на ширину ячейки */
	// Цикл перебора каждой строки
	for (Sint8 i = 0; i < Game->FieldSize; i++)
	{ // Цикл перебора каждого столбца с конца
		for (Sint8 j = 0; j < Game->FieldSize; j++)
		{ // Если данная ячейка пустая, или она не движется по горизонтали
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
				continue;
			if (Game->Field[i * Game->FieldSize + j].mode != TILE_MOVE_X)
				continue;

			if (0 > SDL_roundf(Game->Field[i * Game->FieldSize + j].offset))
			{
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											i * Game->FieldSize + j))
					return ERR_SDL;

				Flag++;
				Game->Field[i * Game->FieldSize + j].offset += change;
			}
			else
			{
				if (Game->Field[i * Game->FieldSize + j - 1].mode !=
					TILE_COMBINED)
				{
					// Копирование текущего элемента в следующий
					Game->Field[i * Game->FieldSize + j - 1] =
						Game->Field[i * Game->FieldSize + j];
					Game->Field[i * Game->FieldSize + j - 1].mode = TILE_OLD;
				}
				else
				{
					Game->Field[i * Game->FieldSize + j - 1].mode =
						TILE_JUSTCOMBINED;
					Game->Field[i * Game->FieldSize + j - 1].val <<= 1;
					Game->Score += Game->Field[i * Game->FieldSize + j - 1].val;
					if(UpdateScore(rend, Game, Params, Assets))
						return ERR_SDL;
					SDL_Log("Очков: %lu", Game->Score);
				}
				// Копирование текущего элемента в следующий
				Game->Field[i * Game->FieldSize + j - 1].offset = 0;

				// Зануление прошлого элемента
				SDL_memset(Game->Field + (i * Game->FieldSize + j), 0,
						   sizeof(Tile));
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											i * Game->FieldSize + j - 1))
					return ERR_SDL;
			}
		}
	}
	if (Flag)
	{
		Params->Mode = MODE_MOVE_LEFT;
		return ERR_NO;
	}
	/* Если CheckMove вернул флаг возможности смещения элемента,
	 * он записывается в глобальный параметр режима
		Если поле заполнено, будет включен режим выхода, если движение
	 невозможно, но поле содержит пустые ячейки, будет включен режим добавления
	 нового элемента*/
	if (CheckLeftMove(Game, Params) == MODE_MOVE_LEFT)
	{
		Params->Mode = MODE_MOVE_LEFT;
		return ERR_NO;
	}

	// Если возможно сложение вправо
	if (CheckLeftCombo(Game, Params))
	{
		Params->Mode = MODE_MOVE_LEFT;
		return ERR_NO;
		// return (UpdateScore(rend, Game, Params, Assets)) ? ERR_SDL : ERR_NO;
	}
	// Если же нет, осуществляется проверка на добавление нового элемента
	Params->Mode = MODE_ADD;
	return ERR_NO;
}

static Uint8 DoUpMove(SDL_Renderer *rend, Game *Game, Params *Params,
					  Assets *Assets)
{
	if (DrawOldElements(rend, Params, Game, Assets))
		return ERR_SDL;

	Uint8 Flag = 0;
	// Размер поля
	float change = (ANIM_SPEED * dtCount() / 1000.0f);

	/*Умножение всех оффсетов на ширину ячейки */
	// Цикл перебора каждой строки
	for (Sint8 j = 0; j < Game->FieldSize; j++)
	{ // Цикл перебора каждого столбца с конца
		for (Sint8 i = 0; i < Game->FieldSize; i++)
		{ // Если данная ячейка пустая, и она движется не по горизонтали,
		  // пропуск
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
				continue;
			if (Game->Field[i * Game->FieldSize + j].mode != TILE_MOVE_Y)
				continue;

			if (0 > SDL_roundf(Game->Field[i * Game->FieldSize + j].offset))
			{
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											i * Game->FieldSize + j))
					return ERR_SDL;

				Flag++;
				Game->Field[i * Game->FieldSize + j].offset += change;
			}
			else
			{
				if (Game->Field[(i - 1) * Game->FieldSize + j].mode !=
					TILE_COMBINED)
				{
					// Копирование текущего элемента в следующий
					Game->Field[(i - 1) * Game->FieldSize + j] =
						Game->Field[i * Game->FieldSize + j];
					Game->Field[(i - 1) * Game->FieldSize + j].mode = TILE_OLD;
				}
				else
				{
					Game->Field[(i - 1) * Game->FieldSize + j].mode =
						TILE_JUSTCOMBINED;
					Game->Field[(i - 1) * Game->FieldSize + j].val <<= 1;
					Game->Score += Game->Field[(i - 1) * Game->FieldSize + j].val;
					if(UpdateScore(rend, Game, Params, Assets))
						return ERR_SDL;
					SDL_Log("Очков: %lu", Game->Score);
				}

				Game->Field[(i - 1) * Game->FieldSize + j].offset = 0;

				// Зануление прошлого элемента
				SDL_memset(Game->Field + (i * Game->FieldSize + j), 0,
						   sizeof(Tile));
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											(i - 1) * Game->FieldSize + j))
					return ERR_SDL;
			}
		}
	}
	if (Flag)
	{
		Params->Mode = MODE_MOVE_UP;
		return ERR_NO;
	}
	/* Если CheckMove вернул флаг возможности смещения элемента,
	 * он записывается в глобальный параметр режима
		Если поле заполнено, будет включен режим выхода, если движение
	 невозможно, но поле содержит пустые ячейки, будет включен режим добавления
	 нового элемента*/
	if (CheckUpMove(Game, Params) == MODE_MOVE_UP)
	{
		Params->Mode = MODE_MOVE_UP;
		return ERR_NO;
	}

	// Если возможно сложение вправо
	if (CheckUpCombo(Game, Params))
	{
		Params->Mode = MODE_MOVE_UP;
		return ERR_NO;
		// return (UpdateScore(rend, Game, Params, Assets)) ? ERR_SDL : ERR_NO;
	}
	// Если же нет, осуществляется проверка на добавление нового элемента
	Params->Mode = MODE_ADD;
	return ERR_NO;
}

static Uint8 DoDownMove(SDL_Renderer *rend, Game *Game, Params *Params,
						Assets *Assets)
{
	if (DrawOldElements(rend, Params, Game, Assets))
		return ERR_SDL;

	Uint8 Flag = 0;
	// Размер поля
	float change = (ANIM_SPEED * dtCount() / 1000.0f);

	// Цикл перебора каждого столбца
	for (Sint8 i = Game->FieldSize - 1; i >= 0; i--)
	{ // Цикл перебора каждой строки
		for (Sint8 j = Game->FieldSize - 1; j >= 0; j--)
		{ // Если данная ячейка пустая, или она не движется по горизонтали
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
				continue;
			if (Game->Field[i * Game->FieldSize + j].mode != TILE_MOVE_Y)
				continue;

			if (0 < SDL_roundf(Game->Field[i * Game->FieldSize + j].offset))
			{
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											i * Game->FieldSize + j))
					return ERR_SDL;

				Flag++;
				Game->Field[i * Game->FieldSize + j].offset -= change;
			}
			else
			{
				if (Game->Field[(i + 1) * Game->FieldSize + j].mode !=
					TILE_COMBINED)
				{
					// Копирование текущего элемента в следующий
					Game->Field[(i + 1) * Game->FieldSize + j] =
						Game->Field[i * Game->FieldSize + j];
					Game->Field[(i + 1) * Game->FieldSize + j].mode = TILE_OLD;
				}
				else
				{
					Game->Field[(i + 1) * Game->FieldSize + j].mode =
						TILE_JUSTCOMBINED;
					Game->Field[(i + 1) * Game->FieldSize + j].val <<= 1;
					Game->Score += Game->Field[(i + 1) * Game->FieldSize + j].val;
					if(UpdateScore(rend, Game, Params, Assets))
						return ERR_SDL;
					SDL_Log("Очков: %lu", Game->Score);
				}
				// Копирование текущего элемента в следующий
				Game->Field[(i + 1) * Game->FieldSize + j].offset = 0;

				// Зануление прошлого элемента
				SDL_memset(Game->Field + (i * Game->FieldSize + j), 0,
						   sizeof(Tile));
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											(i + 1) * Game->FieldSize + j))
					return ERR_SDL;
			}
		}
	}
	if (Flag)
	{
		Params->Mode = MODE_MOVE_DOWN;
		return ERR_NO;
	}
	/* Если CheckMove вернул флаг возможности смещения элемента,
	 * он записывается в глобальный параметр режима
		Если поле заполнено, будет включен режим выхода, если движение
	 невозможно, но поле содержит пустые ячейки, будет включен режим добавления
	 нового элемента*/
	if (CheckDownMove(Game, Params) == MODE_MOVE_DOWN)
	{
		Params->Mode = MODE_MOVE_DOWN;
		return ERR_NO;
	}

	// Если возможно сложение вправо
	if (CheckDownCombo(Game, Params))
	{
		Params->Mode = MODE_MOVE_DOWN;
		return ERR_NO;
		// return (UpdateScore(rend, Game, Params, Assets)) ? ERR_SDL : ERR_NO;
	}
	// Если же нет, осуществляется проверка на добавление нового элемента
	Params->Mode = MODE_ADD;
	return ERR_NO;
}

Uint8 DrawSingleMovingElement(SDL_Renderer *rend, Params *Params, Game *Game,
							  Assets *Assets, Sint8 Index)
{
	SDL_Rect Tile;
	// Размер одной ячейки хранится в h
	Tile.h = Params->CellWidth;
	Tile.w = Tile.h * TILE_SIZE_COEFFICIENT;

	// Положение угла поля в координатах
	Tile.x = (Params->WinSize.x - Params->FieldSize) * 0.5;
	Tile.y = (Params->WinSize.y - Params->FieldSize) * 0.5;

	// Сдвиг координаты угла плитки на её положение в матрице, плюс разницу
	// размеров плитки и ячейки
	Tile.x += (Tile.h * (Index % Game->FieldSize)) + (Tile.h - Tile.w) * 0.5f;
	Tile.y += (Tile.h * (Index / Game->FieldSize)) + (Tile.h - Tile.w) * 0.5f;

	// Сдвиг на оффсет
	if (Game->Field[Index].mode == TILE_MOVE_X)
	{
		if (Game->Field[Index].offset > 0)
			Tile.x += Tile.h - (int)Game->Field[Index].offset;
		else
			Tile.x -= Tile.h + (int)Game->Field[Index].offset;
	}
	if (Game->Field[Index].mode == TILE_MOVE_Y)
	{
		if (Game->Field[Index].offset > 0)
			Tile.y += Tile.h - (int)Game->Field[Index].offset;
		else
			Tile.y -= Tile.h + (int)Game->Field[Index].offset;
	}

	Tile.h = Tile.w; // Запись корректной высоты плитки

	// Отрисовка конечного тайла
	SDL_Texture *tile_texture =
		GetTextureForTile(Game->Field[Index].val, Assets);

	// Создание нового элемента, если не нашёлся
	if (!tile_texture)
	{
		tile_texture = CreateTileTexture(rend, Game->Field[Index].val, Assets,
										 Params->CellWidth);
		// Если элемент не удалось создать
		if (!tile_texture)
			return ERR_SDL;
		else
		{
			Assets->textures_count++;
			TileTexture *newTexs = SDL_realloc(
				Assets->textures, sizeof(TileTexture) * Assets->textures_count);
			if (!newTexs)
				return ERR_MALLOC;
			Assets->textures = newTexs;
			Assets->textures[Assets->textures_count - 1].val =
				Game->Field[Index].val;
			Assets->textures[Assets->textures_count - 1].tex = tile_texture;
		}
	}

	if (SDL_RenderCopy(rend, tile_texture, NULL, &Tile))
		return ERR_SDL;
	return ERR_NO;
}

SDL_Texture *CreateScoreTexture(SDL_Renderer *rend, SDL_Colour *ColourSet,
							 SDL_Rect *Tile, Game *Game)
{
	char *text;
	SDL_asprintf(&text, "Число очков: %lu\nРекорд: %lu", Game->Score,
				 Game->MaxScore);
	if (!text)
		return NULL;
	SDL_Texture *ret =
		CreateMessageTexture(rend, ColourSet + COL_FG, ColourSet + COL_BG, Tile,
							 FONT, text, SDL_TRUE);
	SDL_free(text);
	return ret;
}

Uint8 InitTextureSet(SDL_Renderer *rend, Assets *Assets, Params *Params,
					 Game *Game)
{
	SDL_Colour txt_col = {0xFF, 0xFF, 0xFF, 0xFF};

	// Начальное число текстур (очки, двойка, четвёрка)
	Assets->textures_count = 3;

	// Начальный буфер текстур
	if (!(Assets->textures = (TileTexture *)SDL_malloc(Assets->textures_count *
													   sizeof(TileTexture))))
		return ERR_MALLOC;

	SDL_Rect Tile; // рект с размером текста
	//(размер ячейки * отношение размера тайла к размеру ячейки)
	Tile.w = Tile.h = (int)(TILE_SIZE_COEFFICIENT * Params->CellWidth);

	// Текстура очков (пока заглушка)
	Assets->textures[0].val = 0;
	SDL_Rect scoreField = {.h = (Params->WinSize.y - (int)Params->FieldSize) / 2, 
		.w = (int)Params->FieldSize };
	Assets->textures[0].tex = CreateScoreTexture(rend,Assets->cols, &scoreField, Game);
	// Assets->textures[0].tex = 0;

	// Текстура двойки
	Assets->textures[1].val = 2;
	if (!(Assets->textures[1].tex =
			  CreateMessageTexture(rend, &txt_col, &Assets->cols[COL_SQ2],
								   &Tile, FONT, "2", SDL_TRUE)))
		return ERR_SDL;

	// Текстура четвёрки
	Assets->textures[2].val = 4;
	if (!(Assets->textures[2].tex =
			  CreateMessageTexture(rend, &txt_col, &Assets->cols[COL_SQ4],
								   &Tile, FONT, "4", SDL_TRUE)))
		return ERR_SDL;

	return ERR_NO;
}

Uint8 UpdateTextureSet(SDL_Renderer *rend, Params *Params, Game *Game,
					   Assets *Assets)
{
	/*Освобождение всех текстур*/
	for (Uint8 i = 0; i < Assets->textures_count; ++i)
		if (Assets->textures[i].tex /*!= NULL*/)
		{
			SDL_DestroyTexture(Assets->textures[i].tex);
			Assets->textures[i].tex = NULL;
		}

	SDL_Rect scoreField = {.h = (Params->WinSize.y - (int)Params->FieldSize) / 2, //(int)Params->CellWidth, 
		.w = (int)Params->FieldSize };
	Assets->textures[0].tex = CreateScoreTexture(rend,Assets->cols, &scoreField, Game);

	Uint8 oldCount = Assets->textures_count;
	Assets->textures_count = 1;
	for (Uint8 i = 1; i < oldCount; ++i)
	{
		if (!Assets->textures[i].tex /*==NULL*/)
			if (!(Assets->textures[i].tex =
					  CreateTileTexture(rend, Assets->textures[i].val, Assets,
										Params->CellWidth)))
				return ERR_SDL;
	}
	// Освобождать сам массив из памяти на данном этапе не нужно,
	// так как он заменится новыми текстурами.
	// Его освобождение должно происходить только на выходе из программы
	return ERR_NO;
}

/*Функция поиска текстуры для bsearch*/
static Sint32 FindTexture(void const *l, void const *r)
{
	return ((TileTexture const *)l)->val - ((TileTexture const *)r)->val;
}

SDL_Texture *GetTextureForTile(Uint64 TileValue, Assets *Assets)
{
	TileTexture key = {.val = TileValue};
	TileTexture *needed =
		SDL_bsearch(&key, Assets->textures + 1, Assets->textures_count - 1,
					sizeof(TileTexture), FindTexture);
	// Если текстура нашлась, она возвращается, в противном случае возврат NULL
	if (needed)
		return needed->tex;
	/*else */ return NULL;
}

SDL_Texture *CreateMessageTexture(SDL_Renderer *rend, SDL_Colour const *txt_col,
								  SDL_Colour *bg_col, SDL_Rect *txt_size,
								  const char *font_name, const char *message,
								  Uint8 IsCentred)
{
	TTF_Font *font;
	SDL_Surface *txt_surf;

	if (!message)
		return NULL;

	Uint8 scaler =
		CountLines(message); // Отношение размера буквы к размеру окна
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
	SDL_Surface *bg = SDL_CreateRGBSurfaceWithFormat(
		0, txt_size->w, txt_size->h, 32, SDL_PIXELFORMAT_ARGB8888);
	if (!bg)
	{
		SDL_FreeSurface(txt_surf);
		TTF_CloseFont(font);
		return NULL;
	}

	// Заливка поверхности с фоном
	if (SDL_FillRect(bg, &bg->clip_rect,
					 SDL_MapRGBA(bg->format, SPLIT_COL_LINK(bg_col))))
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

	// Создание текстуры из поверхности с надписью, проверка на корректность
	// осуществляется вовне
	SDL_Texture *ret = SDL_CreateTextureFromSurface(rend, bg);
	TTF_CloseFont(font); // Закрытие шрифта
	SDL_FreeSurface(txt_surf);
	SDL_FreeSurface(bg); // Очистка поверхностей
	return ret;			 // Возврат тексутры, либо NULL
}

static Uint8 DrawBackground(SDL_Renderer *rend, Uint8 TileCount, Params *Params,
							Assets *Assets)
{
	// Заливка фона
	if (SDL_SetRenderDrawColor(rend, SPLIT_COL_VAL(Assets->cols[COL_BG])))
		return ERR_SDL;
	if (SDL_RenderClear(rend))
		return ERR_SDL;

	// Размер плитки
	float TileSize = Params->FieldSize / TileCount; // Размер одного тайла
	SDL_Point Corner = // Координаты угла поля
		{(Params->WinSize.x - Params->FieldSize) * 0.5,
		 (Params->WinSize.y - Params->FieldSize) * 0.5};

	// Задание цвета фона
	if (SDL_SetRenderDrawColor(rend, SPLIT_COL_VAL(Assets->cols[COL_FG])))
		return ERR_SDL;

	// Цикл, рисующий линии поля
	for (Uint8 i = 0; i <= TileCount; i++)
	{
		if (SDL_RenderDrawLine(
				rend, Corner.x,
				Corner.y + i * TileSize, // Начало горизонатльной линии
				Corner.x + Params->FieldSize,
				Corner.y + i * TileSize)) // Конец горизонтальной линии
			return ERR_SDL;
		if (SDL_RenderDrawLine(
				rend, Corner.x + i * TileSize,
				Corner.y, // Начало вертикальной линии
				Corner.x + i * TileSize,
				Corner.y + Params->FieldSize)) // Конец вертикальной линии
			return ERR_SDL;
	}
	return ERR_NO;
}

Uint8 DrawOldElements(SDL_Renderer *rend, Params *Params, Game *Game,
					  Assets *Assets)
{
	SDL_Rect Tile;
	// Рисование поля
	if (DrawBackground(rend, Game->FieldSize, Params, Assets) /*== ERR_SDL*/)
		return ERR_SDL;

	//Положение и размер очков
	Tile.w = (int)Params->FieldSize;
	Tile.h =
		(Params->WinSize.y - (int)Params->FieldSize) / 2; //(int)Params->CellWidth;
	Tile.x = (Params->WinSize.x - Params->FieldSize) * 0.5;
	Tile.y = (Params->WinSize.y - Params->FieldSize) * 0.5 - Tile.h;

	//Отрисовка очков
	if(SDL_RenderCopy(rend, Assets->textures[0].tex, NULL, &Tile))
		return ERR_SDL;
	

	//Сброс размеров перед отрисовкой тайлов
	Tile.w = Tile.h = TILE_SIZE_COEFFICIENT * Params->CellWidth;

	for (Uint8 i = 0; i < _SQ(Game->FieldSize); i++)
	{
		if (!(Game->Field[i].mode == TILE_OLD ||
			  Game->Field[i].mode == TILE_COMBINED ||
			  Game->Field[i].mode == TILE_JUSTCOMBINED))
			continue;
		// Положение угла поля в координатах +
		// Сдвиг координаты угла плитки на её положение в матрице, плюс разницу
		// размеров плитки и ячейки
		Tile.x = (Params->WinSize.x - Params->FieldSize) * 0.5 +
				 (Params->CellWidth - Tile.w) * 0.5 +
				 Params->CellWidth * (i % Game->FieldSize);
		Tile.y = (Params->WinSize.y - Params->FieldSize) * 0.5 +
				 (Params->CellWidth - Tile.w) * 0.5 +
				 Params->CellWidth * (i / Game->FieldSize);
		SDL_Texture *tile_texture =
			GetTextureForTile(Game->Field[i].val, Assets);
		if (!tile_texture)
			tile_texture = CreateTileTexture(rend, Game->Field[i].val, Assets,
											 Params->CellWidth);
		if (!tile_texture)
			return ERR_SDL;

		if (SDL_RenderCopy(rend, tile_texture, NULL, &Tile))
			return ERR_SDL;
	}
	return ERR_NO;
}

Uint8 DrawNewElement(SDL_Renderer *rend, Params *Params, Game *Game,
					 Assets *Assets, Sint8 Index)
{
	if (Index == -1) // Если был передан -1, значит нового элемента нет
		return ERR_NO;

	SDL_Rect Tile;
	SDL_Texture *tile_texture =
		GetTextureForTile(Game->Field[Index].val, Assets);
	if (!tile_texture)
		tile_texture = CreateTileTexture(rend, Game->Field[Index].val, Assets,
										 Params->CellWidth);
	if (!tile_texture)
		return ERR_SDL;

	/*Каждый виток размер растёт и записывается в Tile.w, хранящий размер
	 * плитки*/
	Tile.w =
		(int)(Game->Field[Index].size += (ANIM_SPEED * dtCount() / 1000.0f));

	// Размер одной ячейки хранится в h
	Tile.h = Params->FieldSize / Game->FieldSize;

	// Положение угла поля в координатах
	Tile.x = (Params->WinSize.x - Params->FieldSize) * 0.5;
	Tile.y = (Params->WinSize.y - Params->FieldSize) * 0.5;

	// Сдвиг координаты угла плитки на её положение в матрице, плюс разницу
	// размеров плитки и ячейки
	Tile.x += (Tile.h * (Index % Game->FieldSize)) + (Tile.h - Tile.w) * 0.5;
	Tile.y += (Tile.h * (Index / Game->FieldSize)) + (Tile.h - Tile.w) * 0.5;

	Tile.h = Tile.w; // Запись корректной высоты плитки

	/*Если размер плитки ещё слишком мал*/
	if (Game->Field[Index].size / TILE_SIZE_COEFFICIENT < Params->CellWidth)
	{ // Отрисовка промежуточного значения. Если SDL_RenderCopy упал, возврат
	  // кода ошибки
		if (SDL_RenderCopy(rend, tile_texture, NULL, &Tile))
			return ERR_SDL;
		return ERR_NO;
	}

	// Отрисовка окончательного положения квадрата
	Tile.h = Params->FieldSize / Game->FieldSize;
	Tile.w = Tile.h * TILE_SIZE_COEFFICIENT;
	// Положение угла поля в координатах
	// Сдвиг координаты угла плитки на её положение в матрице, плюс разницу
	// размеров плитки и ячейки
	Tile.x = (Params->WinSize.x - Params->FieldSize) * 0.5 +
			 (Tile.h * (Index % Game->FieldSize)) + (Tile.h - Tile.w) * 0.5;
	Tile.y = (Params->WinSize.y - Params->FieldSize) * 0.5 +
			 (Tile.h * (Index / Game->FieldSize)) + (Tile.h - Tile.w) * 0.5;
	Tile.h = Tile.w; // Запись корректной высоты плитки

	// Отрисовка конечного тайла
	if (SDL_RenderCopy(rend, tile_texture, NULL, &Tile))
		return ERR_SDL;
	Game->Field[Index].mode =
		TILE_OLD; // Установка флага, что теперь эта ячейка отрисована
	Game->Field[Index].size = 0; // Сброс параметра размера
	return ERR_NO;
}

static Uint8 (*int_DoMove[])(SDL_Renderer *, Game *, Params *, Assets *) = {
	DoRightMove, DoLeftMove, DoDownMove, DoUpMove};
/*Набор функций отрисовки сдвигов тайлов поля Game.
используются номера MODE_MOVE_RIGHT, MODE_MOVE_LEFT, MODE_MOVE_DOWN,
MODE_MOVE_UP*/
Uint8 (**DoMove)(SDL_Renderer *, Game *, Params *,
				 Assets *) = int_DoMove - MODE_MOVE_RIGHT;
