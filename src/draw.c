#include "draw.h"

/* Обновление текстуры с очками и учётом цветов из Assets рисовальщиком rend, 
из игры Game, с учётом параметров Params. Возвращает код ошибки, либо 0 при штатной работе*/
static int_fast8_t UpdateScore(SDL_Renderer *rend, Game *Game, Params *Params, Assets *Assets)
{
	//Освобождение старой текстуры
	SDL_DestroyTexture(Assets->textures[0].tex);

	//Обновление рекорда
	if (Game->Score > Game->MaxScore)
		Game->MaxScore = Game->Score;	

	//Создание новой рисовальщиком, с учётом очков, как старых, так и новых, размеров площадки, цветов
	SDL_Rect scoreField = {.h = (Params->WinSize.y - (int)Params->FieldSize) /2, .w = (int)Params->FieldSize};
	if(!(Assets->textures[0].tex = CreateScoreTexture(rend, Assets->cols, &scoreField, Game)))
		return ERR_SDL;
	
	return ERR_NO;
}

//Подбор индекса цвета для тайла с соответствующим значением TileValue
static int_fast8_t MatchColForTile(Sint64 TileValue)
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
	default://>2048
		return COL_MAX;
	}
}

/*Рисование сетки на фоне окна размера WinSize, светлой при Col_Mode = 0,
 * тёмной при Col_Mode в противном случае */
static int_fast8_t DrawBackground(SDL_Renderer *rend, int_fast8_t TileCount, Params *Params,
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
		{(Params->WinSize.x - (int)Params->FieldSize) / 2,
		 (Params->WinSize.y - (int)Params->FieldSize) / 2};

	// Задание цвета фона
	if (SDL_SetRenderDrawColor(rend, SPLIT_COL_VAL(Assets->cols[COL_FG])))
		return ERR_SDL;

	// Цикл, рисующий линии поля
	for (int_fast8_t i = 0; i <= TileCount; i++)
	{
		if (SDL_RenderDrawLine(rend, Corner.x,
				(Corner.y + (int)(i * TileSize)), // Начало горизонатльной линии
				(Corner.x + (int)Params->FieldSize),
				(Corner.y + (int)(i * TileSize)))) // Конец горизонтальной линии
			return ERR_SDL;

		if (SDL_RenderDrawLine(rend, Corner.x + (int)(i * TileSize),
				Corner.y, // Начало вертикальной линии
				(Corner.x + (int)(i * TileSize)),
				(Corner.y + (int)Params->FieldSize))) // Конец вертикальной линии
			return ERR_SDL;
	}
	return ERR_NO;
}

/*Функция отрисовки одного движущегося элемента Index рисовальщиком rend 
с учётом параметров Params, используя цвета и текстуры из Assets, поля Game, */
static int_fast8_t DrawSingleMovingElement(SDL_Renderer *rend, Params *Params,
									 Game *Game, Assets *Assets, int_fast8_t Index)
{
	SDL_Rect Tile;
	// Размер одной ячейки хранится в h
	Tile.h = (int)Params->CellWidth;
	Tile.w = (int)((float)Tile.h * TILE_SIZE_COEFFICIENT);

	// Положение угла поля в координатах
	Tile.x = (Params->WinSize.x - (int)Params->FieldSize) / 2;
	Tile.y = (Params->WinSize.y - (int)Params->FieldSize) / 2;

	// Сдвиг координаты угла плитки на её положение в матрице, плюс разницу
	// размеров плитки и ячейки
	Tile.x += (Tile.h * (Index % Game->FieldSize)) + (Tile.h - Tile.w) / 2;
	Tile.y += (Tile.h * (Index / Game->FieldSize)) + (Tile.h - Tile.w) / 2;

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

	// Создание нового элемента, если не нашёлся среди существующих
	if (!tile_texture)
	{
		tile_texture = CreateTileTexture(rend, Game->Field[Index].val, Assets,
										 Params->CellWidth);
		// Если элемент не удалось создать
		if (!tile_texture)
			return ERR_SDL;
		else
		{
			Assets->textures_count++;//Увеличение счётчика текстур, проверка на корректность
			if(Assets->textures_count < 0)
			{
				SDL_SetError("ошибка выделения памяти!");
				return ERR_MALLOC;
			}
			// Перевыделение увеличенного вектора текстур, проверка и перезапись указателя
			TileTexture *newTexs = SDL_realloc(Assets->textures, 
				sizeof(TileTexture) * (Uint64)Assets->textures_count);
			if (!newTexs)
			{
				SDL_SetError("ошибка выделения памяти!");
				return ERR_MALLOC;
			}
			Assets->textures = newTexs;

			//Запись в крайний тайл его значения и новой текстуры
			Assets->textures[Assets->textures_count - 1].val = Game->Field[Index].val;
			Assets->textures[Assets->textures_count - 1].tex = tile_texture;
		}
	}

	//Отрисовка элемента
	if (SDL_RenderCopy(rend, tile_texture, NULL, &Tile))
		return ERR_SDL;
	return ERR_NO;
}

static int_fast8_t DoRightMove(SDL_Renderer *rend, Game *Game, Params *Params,
						 Assets *Assets)
{	//Отрисовка фона и старых элементов
	if (DrawOldElements(rend, Params, Game, Assets))
		return ERR_SDL;

	//Флаг необходимости продолжения анимации
	int_fast8_t Flag = 0;
	// Размер сдвига
	float change = (ANIM_SPEED * dtCount() / 1000.0f);

	// Цикл перебора каждой строки
	for (Sint8 i = 0; i < Game->FieldSize; i++)
	{ // Цикл перебора каждого столбца с конца
		for (Sint8 j = Game->FieldSize - 1; j >= 0; j--)
		{ // Если данная ячейка пустая, или она не движется по горизонтали
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
				continue;
			if (Game->Field[i * Game->FieldSize + j].mode != TILE_MOVE_X)
				continue;

			//Если элемент ещё не сдвинулся на размер целой ячейки, он отрисовывается
			if (0 < SDL_roundf(Game->Field[i * Game->FieldSize + j].offset))
			{
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											(int_fast8_t)(i * Game->FieldSize + j)))
					return ERR_SDL;

				Flag++;//Подъём флага для продолжения анимации на следующем витке цикла
				Game->Field[i * Game->FieldSize + j].offset -= change;//Сдвиг оффсета
			}
			else // Если элемент сдвинулся на целую ячейку
			{	//Если элемент получился не в результате сложения
				if (Game->Field[i * Game->FieldSize + j + 1].mode !=
					TILE_COMBINED)
				{
					// Копирование текущего элемента в следующий
					Game->Field[i * Game->FieldSize + j + 1] =
						Game->Field[i * Game->FieldSize + j];
					Game->Field[i * Game->FieldSize + j + 1].mode = TILE_OLD;
				}
				else	//Если элемент получился в результате сложения
				{	//В него записывается признак сложенности
					Game->Field[i * Game->FieldSize + j + 1].mode =
						TILE_JUSTCOMBINED;
					//Увеличение значения сложенного тайла в 2 раза
					Game->Field[i * Game->FieldSize + j + 1].val <<= 1;
					//Увеличение числа очков, обновление текстуры очков
					Game->Score += Game->Field[i * Game->FieldSize + j + 1].val;
					if (UpdateScore(rend, Game, Params, Assets))
						return ERR_SDL;
				}
				//Сброс оффсета
				Game->Field[i * Game->FieldSize + j + 1].offset = 0;

				// Зануление прошлого элемента
				SDL_memset(Game->Field + (i * Game->FieldSize + j), 0,
						   sizeof(Tile));
				//Отрисовка нового элемента
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											(int_fast8_t)(i * Game->FieldSize + j + 1)))
					return ERR_SDL;
			}
		}
	}

	/* Если был поднят флаг необходимости продолжения сдвига,
		устанавливается соответствующий режим */
	if (Flag)	
	{
		Game->Mode = MODE_MOVE_RIGHT;
		return ERR_NO;
	}
	/* Если CheckMove вернул флаг возможности смещения элемента,
	 * он записывается в глобальный параметр режима
		Если поле заполнено, будет включен режим выхода, если движение
	 невозможно, но поле содержит пустые ячейки, будет включен режим добавления
	 нового элемента*/
	if ((Game->Mode = CheckRightMove(Game, Params)) == MODE_WAIT)
	{
		Game->Mode = MODE_ADD;
		return ERR_NO;
	}

	// Если же нет, осуществляется проверка на добавление нового элемента
	return ERR_NO;
}

static int_fast8_t DoLeftMove(SDL_Renderer *rend, Game *Game, Params *Params,
						Assets *Assets)
{	//Отрисовка старых элементов
	if (DrawOldElements(rend, Params, Game, Assets))
		return ERR_SDL;

	//Флаг необходимости продолжения анимации
	int_fast8_t Flag = 0;
	// Размер сдвига
	float change = (ANIM_SPEED * dtCount() / 1000.0f);

	// Цикл перебора каждой строки
	for (Sint8 i = 0; i < Game->FieldSize; i++)
	{ // Цикл перебора каждого столбца с конца
		for (Sint8 j = 0; j < Game->FieldSize; j++)
		{ // Если данная ячейка пустая, или она не движется по горизонтали
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
				continue;
			if (Game->Field[i * Game->FieldSize + j].mode != TILE_MOVE_X)
				continue;

			/* Если элемент ещё не сдвинулся на размер целой ячейки, 
			он отрисовывается, флаг анимации поднимается, сдвиг уменьшается*/
			if (0 > SDL_roundf(Game->Field[i * Game->FieldSize + j].offset))
			{
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											(int_fast8_t)(i * Game->FieldSize + j)))
					return ERR_SDL;

				Flag++;
				Game->Field[i * Game->FieldSize + j].offset += change;
			}
			else // Если элемент сдвинулся на целую ячейку
			{	// Если элемент получился не в результате сложения
				if (Game->Field[i * Game->FieldSize + j - 1].mode !=
					TILE_COMBINED)
				{
					// Копирование текущего элемента в следующий
					Game->Field[i * Game->FieldSize + j - 1] =
						Game->Field[i * Game->FieldSize + j];
					Game->Field[i * Game->FieldSize + j - 1].mode = TILE_OLD;
				}
				else // Если элемент получился в результате сложения
				{	// В него записывается признак сложенности
					Game->Field[i * Game->FieldSize + j - 1].mode =
						TILE_JUSTCOMBINED;
					//Увеличение его размерности в два раза
					Game->Field[i * Game->FieldSize + j - 1].val <<= 1;
					//Увеличение числа очков, обновление соответствующей текстуры
					Game->Score += Game->Field[i * Game->FieldSize + j - 1].val;
					if (UpdateScore(rend, Game, Params, Assets))
						return ERR_SDL;
				}
				// сброс оффсета
				Game->Field[i * Game->FieldSize + j - 1].offset = 0;

				// Зануление прошлого элемента
				SDL_memset(Game->Field + (i * Game->FieldSize + j), 0,
						   sizeof(Tile));
				//Отрисовка нового элемента
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											(int_fast8_t)(i * Game->FieldSize + j - 1)))
					return ERR_SDL;
			}
		}
	}

	/* Если был поднят флаг необходимости продолжения сдвига,
		устанавливается соответствующий режим */
	if (Flag)
	{
		Game->Mode = MODE_MOVE_LEFT;
		return ERR_NO;
	}
	/* Если CheckMove вернул флаг возможности смещения элемента,
	 * он записывается в глобальный параметр режима
		Если поле заполнено, будет включен режим выхода, если движение
	 невозможно, но поле содержит пустые ячейки, будет включен режим добавления
	 нового элемента*/
	if (CheckLeftMove(Game, Params) == MODE_MOVE_LEFT)
	{
		Game->Mode = MODE_MOVE_LEFT;
		return ERR_NO;
	}

	// Если же нет, осуществляется проверка на добавление нового элемента
	Game->Mode = MODE_ADD;
	return ERR_NO;
}

static int_fast8_t DoUpMove(SDL_Renderer *rend, Game *Game, Params *Params,
					  Assets *Assets)
{
	//Отрисовка старых элементов
	if (DrawOldElements(rend, Params, Game, Assets))
		return ERR_SDL;

	// Флаг необходимости продолжения анимации
	int_fast8_t Flag = 0;
	// Размер сдвига
	float change = (ANIM_SPEED * dtCount() / 1000.0f);

	// Цикл перебора каждой строки
	for (Sint8 j = 0; j < Game->FieldSize; j++)
	{ // Цикл перебора каждого столбца с конца
		for (Sint8 i = 0; i < Game->FieldSize; i++)
		{ /* Если данная ячейка пустая, и она движется 
			не по горизонтали, пропуск*/
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
				continue;
			if (Game->Field[i * Game->FieldSize + j].mode != TILE_MOVE_Y)
				continue;

			/* Если элемент ещё не сдвинулся на размер целой ячейки, 
			 * он отрисовывается, флаг анимации поднимается, сдвиг уменьшается*/
			if (0 > SDL_roundf(Game->Field[i * Game->FieldSize + j].offset))
			{
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											(int_fast8_t)(i * Game->FieldSize + j)))
					return ERR_SDL;

				Flag++;
				Game->Field[i * Game->FieldSize + j].offset += change;
			}
			else // Если элемент сдвинулся на целую ячейку
			{	// Если элемент получился не в результате сложения
				if (Game->Field[(i - 1) * Game->FieldSize + j].mode !=
					TILE_COMBINED)
				{
					// Копирование текущего элемента в следующий
					Game->Field[(i - 1) * Game->FieldSize + j] =
						Game->Field[i * Game->FieldSize + j];
					Game->Field[(i - 1) * Game->FieldSize + j].mode = TILE_OLD;
				}
				else // Если элемент получился в результате сложения
				{	// В него записывается признак сложенности
					Game->Field[(i - 1) * Game->FieldSize + j].mode =
						TILE_JUSTCOMBINED;
					//Увеличение размерности тайла
					Game->Field[(i - 1) * Game->FieldSize + j].val <<= 1;
					//Увеличение числа очков, обновление соответствующей текстуры
					Game->Score += Game->Field[(i - 1) * Game->FieldSize + j].val;
					if (UpdateScore(rend, Game, Params, Assets))
						return ERR_SDL;
				}

				//сброс оффсета
				Game->Field[(i - 1) * Game->FieldSize + j].offset = 0;

				// Зануление прошлого элемента
				SDL_memset(Game->Field + (i * Game->FieldSize + j), 0,
						   sizeof(Tile));
				//Отрисовка нового элемента
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											(int_fast8_t)((i - 1) * Game->FieldSize + j)))
					return ERR_SDL;
			}
		}
	}

	/* Если был поднят флаг необходимости продолжения сдвига,
		устанавливается соответствующий режим */
	if (Flag)
	{
		Game->Mode = MODE_MOVE_UP;
		return ERR_NO;
	}
	/* Если CheckMove вернул флаг возможности смещения элемента,
	 * он записывается в глобальный параметр режима
		Если поле заполнено, будет включен режим выхода, если движение
	 невозможно, но поле содержит пустые ячейки, будет включен режим добавления
	 нового элемента*/
	if (CheckUpMove(Game, Params) == MODE_MOVE_UP)
	{
		Game->Mode = MODE_MOVE_UP;
		return ERR_NO;
	}

	// Если же нет, осуществляется проверка на добавление нового элемента
	Game->Mode = MODE_ADD;
	return ERR_NO;
}

static int_fast8_t DoDownMove(SDL_Renderer *rend, Game *Game, Params *Params,
						Assets *Assets)
{	//Отрисовка старых элементов
	if (DrawOldElements(rend, Params, Game, Assets))
		return ERR_SDL;

	//Флаг продолжения анимации
	int_fast8_t Flag = 0;
	// Размер оффсета
	float change = (ANIM_SPEED * dtCount() / 1000.0f);

	// Цикл перебора каждого столбца
	for (int_fast8_t i = Game->FieldSize - 1; i >= 0; i--)
	{ // Цикл перебора каждой строки
		for (int_fast8_t j = Game->FieldSize - 1; j >= 0; j--)
		{ /* Если данная ячейка пустая, и она движется 
			не по горизонтали, пропуск*/
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
				continue;
			if (Game->Field[i * Game->FieldSize + j].mode != TILE_MOVE_Y)
				continue;

			/* Если элемент ещё не сдвинулся на размер целой ячейки,
			 * он отрисовывается, флаг анимации поднимается, сдвиг уменьшается*/
			if (0 < SDL_roundf(Game->Field[i * Game->FieldSize + j].offset))
			{
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											(int_fast8_t)(i * Game->FieldSize + j)))
					return ERR_SDL;

				Flag++;
				Game->Field[i * Game->FieldSize + j].offset -= change;
			}
			else // Если элемент сдвинулся на целую ячейку
			{	// Если элемент получился не в результате сложения
				if (Game->Field[(i + 1) * Game->FieldSize + j].mode !=
					TILE_COMBINED)
				{
					// Копирование текущего элемента в следующий
					Game->Field[(i + 1) * Game->FieldSize + j] =
						Game->Field[i * Game->FieldSize + j];
					Game->Field[(i + 1) * Game->FieldSize + j].mode = TILE_OLD;
				}
				else // Если элемент получился в результате сложения
				{	// В него записывается признак сложенности
					Game->Field[(i + 1) * Game->FieldSize + j].mode =
						TILE_JUSTCOMBINED;
					//Увеличение размерности тайла
					Game->Field[(i + 1) * Game->FieldSize + j].val <<= 1;
					//Обновление числа очков
					Game->Score += Game->Field[(i + 1) * Game->FieldSize + j].val;
					if (UpdateScore(rend, Game, Params, Assets))
						return ERR_SDL;
				}
				// Сброс оффсета
				Game->Field[(i + 1) * Game->FieldSize + j].offset = 0;

				// Зануление прошлого элемента, отрисовка нового элемента
				SDL_memset(Game->Field + (i * Game->FieldSize + j), 0,
						   sizeof(Tile));
				if (DrawSingleMovingElement(rend, Params, Game, Assets,
											(int_fast8_t)((i + 1) * Game->FieldSize + j)))
					return ERR_SDL;
			}
		}
	}

	/* Если был поднят флаг необходимости продолжения сдвига,
		устанавливается соответствующий режим */
	if (Flag)
	{
		Game->Mode = MODE_MOVE_DOWN;
		return ERR_NO;
	}
	/* Если CheckMove вернул флаг возможности смещения элемента,
	 * он записывается в глобальный параметр режима
		Если поле заполнено, будет включен режим выхода, если движение
	 невозможно, но поле содержит пустые ячейки, будет включен режим добавления
	 нового элемента*/
	if (CheckDownMove(Game, Params) == MODE_MOVE_DOWN)
	{
		Game->Mode = MODE_MOVE_DOWN;
		return ERR_NO;
	}

	// Если же нет, осуществляется проверка на добавление нового элемента
	Game->Mode = MODE_ADD;
	return ERR_NO;
}

SDL_Texture *CreateTileTexture(SDL_Renderer *rend, Sint64 TileValue,
							   Assets *Assets, float CellWidth)
{
	if (!TileValue /* == 0*/)
	{
		SDL_SetError("Попытка подобрать текстуру для 0!");
		return NULL;
	}

	/*Цвет текста. Если тайл имеет значение 2 или 4, 
	то он заливаётся тёмным цветом, в противном случае -- белым */
	SDL_Colour txt_col = {.a = 0xFF}; 
	txt_col.b = txt_col.g = txt_col.r = (TileValue == 2 || TileValue == 4)
							 ? 0x50 : 0xFF;
	//Тайл размеры тайла
	SDL_Rect tile_size = {.w = (int)(TILE_SIZE_COEFFICIENT * CellWidth)};
	tile_size.h = tile_size.w;

	//Создание текста со значением тайла
	char *stringForTex;
	SDL_asprintf(&stringForTex, "%ld", TileValue);
	if (!stringForTex)
	{
		SDL_SetError("ошибка выделения памяти!");
		return NULL;
	}

	//Создание текстуры, размеров tile_size, соответствующего цвета
	SDL_Texture *tmp = CreateMessageTexture(
		rend, &txt_col, Assets->cols + MatchColForTile(TileValue), &tile_size,
		FONT, stringForTex, SDL_TRUE);

	//Освобождение текста из памяти, выход
	SDL_free(stringForTex);
	return tmp;
}

SDL_Texture *CreateScoreTexture(SDL_Renderer *rend, SDL_Colour *ColourSet,
							 SDL_Rect *Tile, Game *Game)
{	//Текст со значением очков и рекорда
	char *text;
	SDL_asprintf(&text, "Число очков: %ld\nРекорд: %ld", Game->Score,
				 Game->MaxScore);
	if (!text)
	{	
		SDL_SetError("Ошибка выделения памяти!");
		return NULL;
	}

	/*Создание текстуры с очками, фон сливается с основным фоном, 
	цвет текста соответствует цвету переднего плана. 
	Размер берётся извне через рект Tile*/
	SDL_Texture *ret =
		CreateMessageTexture(rend, ColourSet + COL_FG, ColourSet + COL_BG, Tile,
							 FONT, text, SDL_TRUE);

	// Освобождение текста из памяти, выход
	SDL_free(text);
	return ret;
}

int_fast8_t InitTextureSet(SDL_Renderer *rend, Assets *Assets, Params *Params,
					 Game *Game)
{	/* Нахождение номинала максимальной клетки. Если у некоторой ячейки он больше 
	максимальной, максимальный размер обновляется */
	Sint64 maxVal = 2;
	for (int_fast8_t cur = 0; cur < _SQ(Game->FieldSize); cur++)
		if (maxVal < Game->Field[cur].val)
			maxVal = Game->Field[cur].val;

	/* Выделение памяти под нужное число текстур -- текстуру очков +
	тексутры тайлов (log2(макс_значение)) */
	if (!(Assets->textures = (TileTexture *)SDL_malloc(sizeof(TileTexture) 
								* (size_t)((1 + (SDL_log((double)maxVal) / SDL_log(2.0)))))))
	{
		SDL_SetError("ошибка выделения памяти!");
		return ERR_MALLOC;
	}

	SDL_Rect Tile; // рект с размером текста
	//(размер ячейки * отношение размера тайла к размеру ячейки)
	Tile.w = Tile.h = (int)(TILE_SIZE_COEFFICIENT * Params->CellWidth);
	
	//Рект очков
	SDL_Rect scoreField = {
	.h = (Params->WinSize.y - (int)Params->FieldSize) / 2, //промежуток между краем экрана и поля
		.w = (int)Params->FieldSize };					//ширина поля
	
	//Создание текстуры очков
	Assets->textures[0].val = 0;
	Assets->textures[0].tex = CreateScoreTexture(rend,Assets->cols, &scoreField, Game);

	//Создание текстур всем ячейкам, вплоть до максимальной
	Assets->textures_count = 2;
	for (Sint64 val = 2; val <= maxVal; Assets->textures_count++, val <<= 1)
	{
		//Создание крайней текстуры
		Assets->textures[Assets->textures_count - 1].val = val;
		if (!(Assets->textures[Assets->textures_count - 1].tex = CreateTileTexture(rend, 
			Assets->textures[Assets->textures_count - 1].val, Assets, Params->CellWidth)))
			return ERR_SDL;
	}
	Assets->textures_count--;//Откат переполненного счётчика текстур

	return ERR_NO;
}

int_fast8_t UpdateTextureSet(SDL_Renderer *rend, Params *Params, Game *Game,
					   Assets *Assets)
{
	/*Обновление текстур тайлов*/
	for (int_fast8_t i = 1; i < Assets->textures_count; ++i)
		if (Assets->textures[i].tex /*!= NULL*/)
		{
			SDL_DestroyTexture(Assets->textures[i].tex);
			if (!(Assets->textures[i].tex =
					CreateTileTexture(rend, Assets->textures[i].val, Assets,
										Params->CellWidth)))
				return ERR_SDL;
		}

	//Рект текстуры очков
	SDL_Rect scoreField = {.h = (Params->WinSize.y - (int)Params->FieldSize) / 2,
		.w = (int)Params->FieldSize };

	//Обновление текcтуры очков
	SDL_DestroyTexture(Assets->textures[0].tex);
	if(!(Assets->textures[0].tex = CreateScoreTexture(rend,Assets->cols, &scoreField, Game)))
		return ERR_SDL;

	return ERR_NO;
}

/*Функция поиска текстуры для bsearch*/
static Sint32 FindTexture(void const *l, void const *r)
{
	return (Sint32)(((TileTexture const *)l)->val - ((TileTexture const *)r)->val);
}

SDL_Texture *GetTextureForTile(Sint64 TileValue, Assets *Assets)
{
	TileTexture key = {.val = TileValue};
	if (Assets->textures_count < 1)
		return NULL;
	TileTexture *needed =
		SDL_bsearch(&key, Assets->textures + 1, (Uint64)Assets->textures_count - 1,
					sizeof(TileTexture), FindTexture);
	// Если текстура нашлась, она возвращается, в противном случае возврат NULL
	if (needed)
		return needed->tex;
	/*else */ return NULL;
}

SDL_Texture *CreateMessageTexture(SDL_Renderer *rend, SDL_Colour const *txt_col,
								  SDL_Colour *bg_col, SDL_Rect *txt_size,
								  const char *font_name, const char *message,
								  int_fast8_t IsCentred)
{	//используемый шрифт
	TTF_Font *font;

	//Поверхность, в которой будет текст
	SDL_Surface *txt_surf, 
				*bg;//поверхность фона

	if (!message)
	{
		SDL_SetError("отсутствует текст для текстуры!");
		return NULL;
	}

	// Отношение размера буквы к размеру окна
	/*Для начала отношение берётся по числу строк, 
	Если текст не помещается в txt_size, скейлер наращивается*/
	int_fast8_t scaler = CountLines(message); 
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
	if (!(bg = SDL_CreateRGBSurfaceWithFormat(
		0, txt_size->w, txt_size->h, 32, SDL_PIXELFORMAT_ARGB8888)))
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

	/*Создание текстуры из поверхности с надписью, 
	проверка на корректность осуществляется вовне*/
	SDL_Texture *ret = SDL_CreateTextureFromSurface(rend, bg);
	TTF_CloseFont(font); // Закрытие шрифта
	// Очистка поверхностей
	SDL_FreeSurface(txt_surf);
	SDL_FreeSurface(bg); 
	return ret; // Возврат тексутры, либо NULL
}

int_fast8_t DrawOldElements(SDL_Renderer *rend, Params *Params, Game *Game,
					  Assets *Assets)
{	//Рект, используемый для отрисовки как очков, так и тайлов
	SDL_Rect Tile;
	// Рисование поля
	if (DrawBackground(rend, Game->FieldSize, Params, Assets) /*== ERR_SDL*/)
		return ERR_SDL;

	//Положение и размер очков
	Tile.w = (int)Params->FieldSize;
	Tile.h = (Params->WinSize.y - (int)Params->FieldSize) / 2;
	Tile.x = (Params->WinSize.x - (int)Params->FieldSize) / 2;
	Tile.y = (Params->WinSize.y - (int)Params->FieldSize) / 2 - Tile.h;

	//Отрисовка очков
	if(SDL_RenderCopy(rend, Assets->textures[0].tex, NULL, &Tile))
		return ERR_SDL;
	

	//Сброс размеров перед отрисовкой тайлов
	Tile.w = Tile.h = (int)(TILE_SIZE_COEFFICIENT * Params->CellWidth);

	//Цикл отрисовки каждого тайла
	for (int_fast8_t i = 0; i < _SQ(Game->FieldSize); i++)
	{	//Если элемент не является старым, складываемым, или сложенным...
		if (!(Game->Field[i].mode == TILE_OLD ||
			  Game->Field[i].mode == TILE_COMBINED ||
			  Game->Field[i].mode == TILE_JUSTCOMBINED))
			continue;//он пропускается
		
		/* Положение угла поля в координатах +
		Сдвиг координаты угла плитки на её положение в матрице, плюс разницу
		размеров плитки и ячейки */
		Tile.x = (int)(((float)Params->WinSize.x - Params->FieldSize) / 2 +
				 (Params->CellWidth - (float)Tile.w) / 2 +
				 Params->CellWidth * (float)(i % Game->FieldSize));
		Tile.y = (int)(((float)Params->WinSize.y - Params->FieldSize) / 2 +
				 (Params->CellWidth - (float)Tile.w) / 2 +
				 Params->CellWidth * (float)(i / Game->FieldSize));

		//Поиск текстуры среди уже существующих
		SDL_Texture *tile_texture =	GetTextureForTile(Game->Field[i].val, Assets);
		
		//Отрисовка её
		if (SDL_RenderCopy(rend, tile_texture, NULL, &Tile))
			return ERR_SDL;
	}
	return ERR_NO;
}

int_fast8_t DrawNewElement(SDL_Renderer *rend, Params *Params, Game *Game,
					 Assets *Assets, Sint8 Index)
{
	if (Index == -1) // Если был передан -1, значит нового элемента нет
		return ERR_NO;

	//Текстура тайла
	SDL_Texture *tile_texture;
	/*Поиск текстуры в ассетах. Если она не нашлась, она создаётся по новой*/
	tile_texture = GetTextureForTile(Game->Field[Index].val, Assets);
	if (!tile_texture)
		tile_texture = CreateTileTexture(rend, Game->Field[Index].val, Assets,
										 Params->CellWidth);
	//Если её не удалось создать, функция завершается с ошибкой
	if (!tile_texture)
		return ERR_SDL;

	//Рект с размерами тайла
	SDL_Rect Tile;
	/*Каждый вызов функции размер растёт и записывается в Tile.w, 
	хранящий размер плитки*/
	Tile.w =
		(int)(Game->Field[Index].size += (ANIM_SPEED * dtCount() / 1000.0f));

	// Размер одной ячейки хранится в h
	Tile.h = (int)(Params->CellWidth);

	// Положение угла поля в координатах
	Tile.x = (int)((float)Params->WinSize.x - Params->FieldSize) / 2;
	Tile.y = (int)((float)Params->WinSize.y - Params->FieldSize) / 2;

	/* Сдвиг координаты угла плитки на её положение в матрице, 
	плюс разницу размеров плитки и ячейки */
	Tile.x += (Tile.h * (Index % Game->FieldSize)) + (Tile.h - Tile.w) / 2;
	Tile.y += (Tile.h * (Index / Game->FieldSize)) + (Tile.h - Tile.w) / 2;

	Tile.h = Tile.w; // Запись корректной высоты плитки

	/*Если размер плитки ещё слишком мал*/
	if (Game->Field[Index].size / TILE_SIZE_COEFFICIENT < Params->CellWidth)
	{ /* Отрисовка промежуточного значения. Если SDL_RenderCopy упал, 
		возврат кода ошибки */
		return (SDL_RenderCopy(rend, tile_texture, NULL, &Tile)) ?
			ERR_SDL : ERR_NO;
	}

	// Отрисовка окончательного положения квадрата
	Tile.h = (int)(Params->FieldSize / Game->FieldSize);
	Tile.w = (int)((float)Tile.h * TILE_SIZE_COEFFICIENT);

	/* Положение угла поля в координатах
	Сдвиг координаты угла плитки на её положение в матрице, плюс разницу
	размеров плитки и ячейки */
	Tile.x = (int)(((float)Params->WinSize.x - Params->FieldSize) * 0.5 +
			 ((float)Tile.h * (Index % Game->FieldSize)) + (Tile.h - Tile.w) / 2);
	Tile.y = (int)(((float)Params->WinSize.y - Params->FieldSize) * 0.5 +
			 ((float)Tile.h * (Index / Game->FieldSize)) + (Tile.h - Tile.w) / 2);
	Tile.h = Tile.w; // Запись корректной высоты плитки

	// Отрисовка конечного тайла
	if (SDL_RenderCopy(rend, tile_texture, NULL, &Tile))
		return ERR_SDL;
	
	// Установка флага, что теперь эта ячейка отрисована
	Game->Field[Index].mode = TILE_OLD; 
	Game->Field[Index].size = 0; // Сброс параметра размера
	return ERR_NO;
}

static int_fast8_t (*int_DoMove[])(SDL_Renderer *, Game *, Params *, Assets *) = {
	DoRightMove, DoLeftMove, DoDownMove, DoUpMove};
/*Набор функций отрисовки сдвигов тайлов поля Game.
используются номера MODE_MOVE_RIGHT, MODE_MOVE_LEFT, MODE_MOVE_DOWN,
MODE_MOVE_UP*/
int_fast8_t (**DoMove)(SDL_Renderer *, Game *, Params *,
				 Assets *) = int_DoMove - MODE_MOVE_RIGHT;
