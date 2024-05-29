#include "misc.h"

Uint8 UpdateWindowTitle(SDL_Window *win, Uint64 Score)
{
	char *buf;
	SDL_asprintf(&buf, "2048 | Очков: %lu", Score);
	if(!buf)
		return ERR_MALLOC;
	SDL_SetWindowTitle(win, buf);
	SDL_free(buf);
	return ERR_NO;
}

void ChangeCombinedToOld(Game *Game)
{
	// Цикл перебора каждой строки
	for (Sint8 i = 0; i < Game->FieldSize; i++)
	{ // Цикл перебора каждого столбца с конца
		for (Sint8 j = Game->FieldSize - 1; j >= 0; j--)
			if (Game->Field[i * Game->FieldSize + j].mode == TILE_JUSTCOMBINED)
				Game->Field[i * Game->FieldSize + j].mode = TILE_OLD;
	}
}

Uint8 CheckRightCombo(Game *Game, Params *Params)
{
	Uint8 MoveFlag = 0;
	// Цикл перебора каждой строки
	for (Sint8 i = 0; i < Game->FieldSize; i++)
	{ // Цикл перебора каждого столбца с конца
		for (Sint8 j = Game->FieldSize - 1; j > 0; j--)
		{ // Если данная ячейка пустая, можно пропускать
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
				continue;

			// Если элементы были только созданы, их можно пропускать
			if (Game->Field[i * Game->FieldSize + j].mode ==
					TILE_JUSTCOMBINED ||
				Game->Field[i * Game->FieldSize + j - 1].mode ==
					TILE_JUSTCOMBINED)
				continue;

			// Если же соседние элементы равны, но не равны нулю
			if (Game->Field[i * Game->FieldSize + j].val ==
				Game->Field[i * Game->FieldSize + j - 1].val)
			{
				Game->Field[i * Game->FieldSize + j].mode = TILE_COMBINED;
				Game->Field[i * Game->FieldSize + j - 1].mode = TILE_MOVE_X;
				Game->Field[i * Game->FieldSize + j - 1].offset =
					Params->CellWidth;
				j--; // Проверка через один элемент, а не следующего
				MoveFlag++; // Подъём флага движения
			}
		}
	}
	return MoveFlag;
}

Uint8 CheckLeftCombo(Game *Game, Params *Params)
{
	Uint8 MoveFlag = 0;
	// Цикл перебора каждой строки
	for (Sint8 i = 0; i < Game->FieldSize; i++)
	{ // Цикл перебора каждого столбца с начала
		for (Sint8 j = 0; j < Game->FieldSize - 1; j++)
		{ // Если данная ячейка пустая, можно пропускать
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
				continue;

			// Если элементы были только созданы, их можно пропускать
			if (Game->Field[i * Game->FieldSize + j].mode ==
					TILE_JUSTCOMBINED ||
				Game->Field[i * Game->FieldSize + j + 1].mode ==
					TILE_JUSTCOMBINED)
				continue;

			// Если же соседние элементы равны, но не равны нулю
			if (Game->Field[i * Game->FieldSize + j].val ==
				Game->Field[i * Game->FieldSize + j + 1].val)
			{
				Game->Field[i * Game->FieldSize + j].mode = TILE_COMBINED;
				Game->Field[i * Game->FieldSize + j + 1].mode = TILE_MOVE_X;
				Game->Field[i * Game->FieldSize + j + 1].offset =
					-1 * Params->CellWidth;
				j++; // Проверка через один элемент, а не следующего
				MoveFlag++; // Подъём флага движения
			}
		}
	}
	return MoveFlag;
}

Uint8 CheckDownCombo(Game *Game, Params *Params)
{
	Uint8 MoveFlag = 0;
	// Цикл перебора каждого столбца
	for (Sint8 j = 0; j < Game->FieldSize; j++)
	{ // Цикл перебора каждой строки
		for (Sint8 i = Game->FieldSize - 1; i > 0; i--)
		{ // Если данная ячейка пустая
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
				continue;

			// Если элементы были только созданы, их можно пропускать
			if (Game->Field[i * Game->FieldSize + j].mode ==
					TILE_JUSTCOMBINED ||
				Game->Field[(i - 1) * Game->FieldSize + j].mode ==
					TILE_JUSTCOMBINED)
				continue;

			// Если же соседние элементы равны, но не равны нулю
			if (Game->Field[i * Game->FieldSize + j].val ==
				Game->Field[(i - 1) * Game->FieldSize + j].val)
			{
				Game->Field[i * Game->FieldSize + j].mode = TILE_COMBINED;
				Game->Field[(i - 1) * Game->FieldSize + j].mode = TILE_MOVE_Y;
				Game->Field[(i - 1) * Game->FieldSize + j].offset =
					Params->CellWidth;
				i--; // Проверка через один элемент, а не следующего
				MoveFlag++; // Подъём флага движения
			}
		}
	}
	return MoveFlag;
}

Uint8 CheckUpCombo(Game *Game, Params *Params)
{
	Uint8 MoveFlag = 0;
	// Цикл перебора каждого столбца
	for (Sint8 j = 0; j < Game->FieldSize; j++)
	{ // Цикл перебора каждой строки
		for (Sint8 i = 0; i < Game->FieldSize - 1; i++)
		{ // Если данная ячейка пустая
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
				continue;

			// Если элементы были только созданы, их можно пропускать
			if (Game->Field[i * Game->FieldSize + j].mode ==
					TILE_JUSTCOMBINED ||
				Game->Field[(i + 1) * Game->FieldSize + j].mode ==
					TILE_JUSTCOMBINED)
				continue;

			// Если же соседние элементы равны, но не равны нулю
			if (Game->Field[i * Game->FieldSize + j].val ==
				Game->Field[(i + 1) * Game->FieldSize + j].val)
			{
				Game->Field[i * Game->FieldSize + j].mode = TILE_COMBINED;
				Game->Field[(i + 1) * Game->FieldSize + j].mode = TILE_MOVE_Y;
				Game->Field[(i + 1) * Game->FieldSize + j].offset =
					-1 * Params->CellWidth;
				i++; // Проверка через один элемент, а не следующего
				MoveFlag++; // Подъём флага движения
			}
		}
	}
	return MoveFlag;
}
Uint8 CheckRightMove(Game *Game, Params *Params)
{
	Uint8 QuitFlag = 1, MoveFlag = 0;
	SDL_Log("Проверка справа");
	// Цикл перебора каждой строки
	for (Sint8 i = 0; i < Game->FieldSize; i++)
	{ // Цикл перебора каждого столбца с конца
		for (Sint8 j = Game->FieldSize - 1; j >= 0; j--)
		{ // Если данная ячейка пустая
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
			{
				QuitFlag = 0;
				// Всем предшествующим не пустым ячейкам проставляется параметр
				// TILE_MOVE_X и оффсет
				Uint8 NonEmptyLine = 0;
				for (Sint8 j2 = j - 1; j2 >= 0; j2--)
				{ // Если ячейка не пустая
					if (Game->Field[i * Game->FieldSize + j2].val /* != 0 */)
					{
						NonEmptyLine++;
						Game->Field[i * Game->FieldSize + j2].mode =
							TILE_MOVE_X;
						// Оффсет выставляется в единицах. При отрисовке он
						// будет умножен на размер ячейки
						Game->Field[i * Game->FieldSize + j2].offset =
							Params->CellWidth;
					}
				}
				if (NonEmptyLine)
					MoveFlag++; // Подъём флага движения
			}
		}
	}
	return (MoveFlag) ? MODE_MOVE_RIGHT : (QuitFlag) ? MODE_QUIT : MODE_WAIT;
}

Uint8 CheckLeftMove(Game *Game, Params *Params)
{
	Uint8 MoveFlag = 0, QuitFlag = 1;
	SDL_Log("Проверка слева");
	// Цикл перебора каждой строки
	for (Sint8 i = 0; i < Game->FieldSize; i++)
	{ // Цикл перебора каждого столбца
		for (Sint8 j = 0; j < Game->FieldSize; j++)
		{ // Если данная ячейка пустая
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
			{ // Всем следующим не пустым ячейкам проставляется параметр
			  // TILE_MOVE_X и оффсет
				Uint8 NonEmptyLine = 0;
				QuitFlag = 0;
				for (Sint8 j2 = j + 1; j2 < Game->FieldSize; j2++)
				{ // Если ячейка не пустая
					if (Game->Field[i * Game->FieldSize + j2].val /* != 0 */)
					{
						NonEmptyLine++;
						Game->Field[i * Game->FieldSize + j2].mode =
							TILE_MOVE_X;
						// Оффсет выставляется в единицах. При отрисовке он
						// будет умножен на размер ячейки
						Game->Field[i * Game->FieldSize + j2].offset =
							-1 * Params->CellWidth;
					}
				}
				if (NonEmptyLine)
					MoveFlag++; // Подъём флага движения
			}
		}
	}
	return (MoveFlag) ? MODE_MOVE_LEFT : (QuitFlag) ? MODE_QUIT : MODE_WAIT;
}

Uint8 CheckUpMove(Game *Game, Params *Params)
{
	Uint8 MoveFlag = 0, QuitFlag = 1;
	SDL_Log("Проверка сверху");
	// Цикл перебора каждого столбца
	for (Sint8 j = 0; j < Game->FieldSize; j++)
	{ // Цикл перебора каждой строки
		for (Sint8 i = 0; i < Game->FieldSize; i++)
		{ // Если данная ячейка пустая
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
			{ // Всем следующим не пустым ячейкам проставляется параметр
			  // TILE_MOVE_X и оффсет
				Uint8 NonEmptyLine = 0;
				QuitFlag = 0;
				for (Sint8 i2 = i + 1; i2 < Game->FieldSize; i2++)
				{ // Если ячейка не пустая
					if (Game->Field[i2 * Game->FieldSize + j].val /* != 0 */)
					{
						NonEmptyLine++;
						Game->Field[i2 * Game->FieldSize + j].mode =
							TILE_MOVE_Y;
						// Оффсет выставляется в единицах. При отрисовке он
						// будет умножен на размер ячейки
						Game->Field[i2 * Game->FieldSize + j].offset =
							-1 * Params->CellWidth;
					}
				}
				if (NonEmptyLine)
					MoveFlag++; // Подъём флага движения
			}
		}
	}
	return (MoveFlag) ? MODE_MOVE_UP : (QuitFlag) ? MODE_QUIT : MODE_WAIT;
}

Uint8 CheckDownMove(Game *Game, Params *Params)
{
	Uint8 QuitFlag = 1;
	Uint8 MoveFlag = 0;
	SDL_Log("Проверка снизу");
	// Цикл перебора каждого столбца
	for (Sint8 j = 0; j < Game->FieldSize; j++)
	{ // Цикл перебора каждой строки
		for (Sint8 i = Game->FieldSize - 1; i >= 0; i--)
		{ // Если данная ячейка пустая
			if (!Game->Field[i * Game->FieldSize + j].val /* == 0 */)
			{ // Всем следующим не пустым ячейкам проставляется параметр
			  // TILE_MOVE_X и оффсет
				Uint8 NonEmptyLine = 0;
				QuitFlag = 0;
				for (Sint8 i2 = i - 1; i2 >= 0; i2--)
				{ // Если ячейка не пустая
					if (Game->Field[i2 * Game->FieldSize + j].val /* != 0 */)
					{
						NonEmptyLine++;
						Game->Field[i2 * Game->FieldSize + j].mode =
							TILE_MOVE_Y;
						// Оффсет выставляется в единицах. При отрисовке он
						// будет умножен на размер ячейки
						Game->Field[i2 * Game->FieldSize + j].offset =
							Params->CellWidth;
					}
				}
				if (NonEmptyLine)
					MoveFlag++; // Подъём флага движения
			}
		}
	}
	return (MoveFlag) ? MODE_MOVE_DOWN : (QuitFlag) ? MODE_QUIT : MODE_WAIT;
}

// Подсчёт длинны файла, сравнение с максимальным
static Uint32 FileLen(SDL_RWops *f)
{
	SDL_RWseek(f, RW_SEEK_SET, RW_SEEK_END); // Перемотка в конец
	Uint32 len = SDL_RWtell(f); // Чтение размера файла
	SDL_RWseek(f, 0, RW_SEEK_SET); // Перемотка обратно в начало
	return len;
}

SDL_Colour *CreateColourSet(Uint8 DarkModeFlag)
{
	// Список названий цветов в файле
	const char *ColNameList[] = COLOURS_LIST;

	// Выделение памяти под вектор цветов, проверка
	SDL_Colour *set =
		(SDL_Colour *)SDL_malloc(COLOURS_COUNT * sizeof(SDL_Colour));
	if (!set)
		return NULL;

	// Открытие файла, из которого будет осуществлятся чтение цветов
	SDL_RWops *ColFile =
		SDL_RWFromFile((DarkModeFlag) ? DARK_SCHEME : LIGHT_SCHEME, "rb");
	if (!ColFile)
	{ // Очитка теперь безполезного набора цветов
		SDL_free(set);
		return NULL;
	}
	// Объём файла + терм. символ
	Uint32 fLen = FileLen(ColFile) + 1;

	// Выделение памяти под копию файла в памяти, проверка
	char *fCopy = (char *)SDL_malloc(fLen);
	if (!fCopy)
	{ // Очитка теперь безполезного набора цветов
		SDL_free(set);
		// Закрытие файла с цветами
		SDL_RWclose(ColFile);
		return NULL;
	}

	// Копирование содержимого файла в память
	SDL_RWread(ColFile, fCopy, sizeof(char), fLen);

	// Закрытие файла с цветами
	SDL_RWclose(ColFile);

	// Пропуск шапки файла
	char *text = SDL_strchr(fCopy, '\n') + 1;

	// Цикл чтения строки
	for (Uint8 cur = 0; cur < COLOURS_COUNT; ++cur)
	{ // Если не удалось найти какой-либо цвет, цикл прерывается
		if (!(text = SDL_strstr(text, ColNameList[cur])))
			break;

		// Если строка нашлась -- чтение значений
		SDL_sscanf(text + SDL_strlen(ColNameList[cur]), "%hhX %hhX %hhX",
				   &set[cur].r, &set[cur].g, &set[cur].b);
		// Задание всем цветам непрозрачности
		set[cur].a = 0xFF;
	}

	/* Если цикл был прерван из - за недостатка цветов в файле,
	то память очищается, осуществляется возврат NULL */
	if (!text)
	{
		// Очитка теперь безполезного набора цветов
		SDL_free(set);
		// Очистка памяти, завершение функции
		SDL_free(fCopy);
		return NULL;
	}

	// Очистка памяти, завершение функции
	SDL_free(fCopy);
	return set;
}

Uint8 CountLines(const char *source)
{
	Uint8 i = 1; // Если в тексте нет переносов, значит там одна строка
	/*Когда функция находит перенос, она возвращает указатель на него,
	в противном случае -- NULL, цикл завершается */
	while ((source = SDL_strchr(source, '\n')))
	{
		source++;
		if (*source /*!= 0*/)
			i++;
	}
	return i;
}

void GetFieldAndTileSize(Game *Game, Params *Params)
{
	Params->FieldSize =
		FIELD_SIZE_COEFFICIENT * // Отношение размера поля к размеру экрана
		MinOfTwo(Params->WinSize.x,
				 Params->WinSize.y); // Меньший и размеров окон

	Params->CellWidth = Params->FieldSize / Game->FieldSize;
}

Sint8 AddElement(Game *Game)
{
	Sint8 pos;
	Uint8 tries = 0;
	/*Случайный перебор*/
	for (; tries < _SQ(Game->FieldSize); tries++)
	{
		pos = RandomInt(0, _SQ(Game->FieldSize));
		if (Game->Field[pos].val)
			continue; // Если там уже есть значение, то перебор продолжается
		/*В противном случае в эту позицию сохраняется число*/
		Game->Field[pos].val = (RandomInt(0, CHANCE_OF_FOUR)) ? 2 : 4;
		Game->Field[pos].mode = TILE_NEW;
		return pos; // Позиция нового элемента возвращается
	}
	/*Если случайно подобрать положение не удалось, функция перебирает ячейки по
	очереди с конца, проводя аналогичную проверку*/
	for (pos = tries - 1; pos >= 0; pos--)
	{
		if (Game->Field[pos].val /*!= 0*/)
			continue;
		/*else*/
		Game->Field[pos].val = (RandomInt(0, CHANCE_OF_FOUR)) ? 2 : 4;
		Game->Field[pos].mode = TILE_NEW;
		return pos;
	}
	// Если свободных ячеек не нашлось, значит возвращается соответствующий флаг
	return -1;
}

Uint8 dtCount(void)
{
	static Uint32 lasttime = 0;
	Uint32 newtime = SDL_GetTicks();
	Uint8 ret = newtime - lasttime;
	if (ret < MIN_FRAMETIME)
	{
		SDL_Delay(MIN_FRAMETIME);
		newtime = SDL_GetTicks();
		ret = newtime - lasttime;
	}
	lasttime = newtime;
	return ret; // Если прошло мало времени, возврат 1
}

Sint32 RandomInt(Sint32 a, Sint32 b)
{
	//"Переворот" чисел при необходимости
	if (a > b)
	{
		Sint32 tmp = a;
		a = b;
		b = tmp;
	}
	return (rand() % (b - a)) + a;
}

void SetMode(SDL_Event *event, Params *Params)
{
	// Определение событий
	while (!SDL_PollEvent(event))
	{ // Если событий не было, сразу осуществляется выход, режим не меняется
		return;
	}

	switch (event->type)
	{
	// Если был запрошен выход из программы
	case SDL_QUIT:
		Params->Mode = MODE_QUIT;
		return;

		break;

	case SDL_KEYUP: // Если была нажата клавиша

		// Если програма отображает перемещение ячейки, то возможно только выйти
		// из программы
		if (event->key.keysym.scancode == SDL_SCANCODE_Q)
		{
			Params->Mode = MODE_QUIT;
			return;
		}
		if (Params->Mode != MODE_WAIT)
			return;
		/*дальнейший код выполняется только в режиме MODE_WAIT*/
		switch (event->key.keysym.scancode)
		{
			/*Наборы клавиш "ВПРАВО" для разных схем*/
		case SDL_SCANCODE_L:
			if ((Params->Flags & FLAG_VIMKEY))
				Params->Mode = MODE_CHECK_RIGHT;
			return;
		case SDL_SCANCODE_D:
			if ((Params->Flags & FLAG_WASDKEY))
				Params->Mode = MODE_CHECK_RIGHT;
			return;
		case SDL_SCANCODE_RIGHT:
			if ((Params->Flags & FLAG_ARROWKEY))
				Params->Mode = MODE_CHECK_RIGHT;
			return;

			/*Наборы клавиш "ВЛЕВО" для разных схем*/
		case SDL_SCANCODE_H:
			if ((Params->Flags & FLAG_VIMKEY))
				Params->Mode = MODE_CHECK_LEFT;
			return;
		case SDL_SCANCODE_A:
			if ((Params->Flags & FLAG_WASDKEY))
				Params->Mode = MODE_CHECK_LEFT;
			return;
		case SDL_SCANCODE_LEFT:
			if ((Params->Flags & FLAG_ARROWKEY))
				Params->Mode = MODE_CHECK_LEFT;
			return;

			/*Наборы клавиш "ВВЕРХ" для разных схем*/
		case SDL_SCANCODE_K:
			if ((Params->Flags & FLAG_VIMKEY))
				Params->Mode = MODE_CHECK_UP;
			return;
		case SDL_SCANCODE_W:
			if ((Params->Flags & FLAG_WASDKEY))
				Params->Mode = MODE_CHECK_UP;
			return;
		case SDL_SCANCODE_UP:
			if ((Params->Flags & FLAG_ARROWKEY))
				Params->Mode = MODE_CHECK_UP;
			return;

			/*Наборы клавиш "ВНИЗ" для разных схем*/
		case SDL_SCANCODE_J:
			if ((Params->Flags & FLAG_VIMKEY))
				Params->Mode = MODE_CHECK_DOWN;
			return;
		case SDL_SCANCODE_S:
			if ((Params->Flags & FLAG_WASDKEY))
				Params->Mode = MODE_CHECK_DOWN;
			return;
		case SDL_SCANCODE_DOWN:
			if ((Params->Flags & FLAG_ARROWKEY))
				Params->Mode = MODE_CHECK_DOWN;
			return;

		default:
			break;
		}
		break;
	default:
		break;
	}

	// Если произошло не значимое событие
	return;
}

Uint8 PrintErrorAndLeaveWithCode(Uint8 code, SDL_Window *win,
								 SDL_Renderer *rend, Game *Game, Params *Params,
								 Assets *Assets)
{
	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s\n", SDL_GetError());
	return SilentLeaveWithCode(code, win, rend, Game, Params, Assets);
}

Uint8 SilentLeaveWithCode(Uint8 code, SDL_Window *win, SDL_Renderer *rend,
						  Game *Game, Params *Params, Assets *Assets)
{
	// Освобождение цветов
	if (Assets->cols)
		SDL_free(Assets->cols);

	// Освобождение текстур
	if (Assets->textures)
	{
		for (Uint8 i = 0; i < Assets->textures_count; ++i)
			SDL_DestroyTexture(Assets->textures[i].tex);
		SDL_free(Assets->textures);
	}

	// Освобождение поля
	if (Game->Field)
		SDL_free(Game->Field);

	// Уничтожение рисовальщика
	if (rend)
		SDL_DestroyRenderer(rend);

	// Уничтожение окна
	if (win)
		SDL_DestroyWindow(win);

	// Завершение работы SDL и SDL_ttf
	TTF_Quit();
	SDL_Quit();

	// Возврат кода ошибки для выхода
	return code;
}

Uint8 CreateWorkspace(SDL_Window **win, SDL_Renderer **rend, const char *title,
					  const SDL_Point *WinSize, Uint8 Flag)
{
	// Вызов SDL
	if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
		return ERR_SDL; // Завершение работы, если не удалось вызвать SDL

	// Вызов SDL_ttf
	if (TTF_Init())
		return ERR_TTF; // Завершение работы, если не удалось вызвать TTF

	// Создание окна
	if (!(*win = SDL_CreateWindow(
			  title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WinSize->x,
			  WinSize->y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)))
		return ERR_SDL;

	// Создание рисовальщика
	if (!(*rend = SDL_CreateRenderer(*win, -1,
									 SDL_RENDERER_ACCELERATED |
										 ((Flag & FLAG_VSYNC)
											  ? SDL_RENDERER_PRESENTVSYNC
											  : 0)))) // Флаг вкл/выкл v-sync
		return ERR_SDL;

	// Возврат кода штатной работы
	return ERR_NO;
}

Sint32 MinOfTwo(Sint32 a, Sint32 b)
{
	return (a > b) ? b : a;
}

Sint32 MaxOfTwo(Sint32 a, Sint32 b)
{
	return (a < b) ? b : a;
}

Game StartUp(int argc, const char **argv, Params *Settings)
{
	Game Game;
	// Базовые параметры работы игры
	Uint8 FieldSize = 4;
	Settings->Flags = (FLAG_VSYNC | FLAG_DARKMODE | FLAG_ARROWKEY);

	// Если игра была запущена без флагов,
	Uint8 Setters =
		(argc == 1) ? 0 : // то используется стандартная раскладка
			(VSYNC_UNSET | COL_UNSET | KEY_UNSET | SIZE_UNSET);

	/*Перебор аргументов, с которыми была запущена игра. Если их не было,
	 * цикл ниже будет пропущен*/
	for (Uint8 i = 1; Setters && (i < argc); ++i)
	{
		if (!SDL_strcmp(argv[i], "--vsync-off") && (Setters & VSYNC_UNSET))
		{
			Setters &= ~VSYNC_UNSET;
			Settings->Flags &= ~FLAG_VSYNC;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--vsync-on") && (Setters & VSYNC_UNSET))
		{
			Setters &= ~VSYNC_UNSET;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--size=3") && (Setters & SIZE_UNSET))
		{
			Setters &= ~SIZE_UNSET;
			FieldSize = 3;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--size=4") && (Setters & SIZE_UNSET))
		{
			Setters &= ~SIZE_UNSET;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--size=5") && (Setters & SIZE_UNSET))
		{
			Setters &= ~SIZE_UNSET;
			FieldSize = 5;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--size=6") && (Setters & SIZE_UNSET))
		{
			Setters &= ~SIZE_UNSET;
			FieldSize = 6;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--light") && (Setters & COL_UNSET))
		{
			Setters &= ~COL_UNSET;
			Settings->Flags &= ~FLAG_DARKMODE;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--dark") && (Setters & COL_UNSET))
		{
			Setters &= ~COL_UNSET;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--arrows") && (Setters & KEY_UNSET))
		{
			Setters &= ~KEY_UNSET;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--wasd") && (Setters & KEY_UNSET))
		{
			Setters &= ~KEY_UNSET;
			Settings->Flags &= ~FLAG_ARROWKEY;
			Settings->Flags |= FLAG_WASDKEY;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--vi") && (Setters & KEY_UNSET))
		{
			Setters &= ~KEY_UNSET;
			Settings->Flags &= ~FLAG_ARROWKEY;
			Settings->Flags |= FLAG_VIMKEY;
			continue;
		}
	}

	Game.Score = 0,	Game.MaxScore = 0;
	// Выделение памяти под игровое поле
	Game.Field = (Tile *)SDL_calloc(sizeof(Tile), _SQ(FieldSize));
	Game.FieldSize = (Game.Field/* != NULL */) ? FieldSize : 0;
	return Game;
}

Uint8 CheckForResize(SDL_Window *win, Params *Params, SDL_Event *ev,
					 Uint16 win_min)
{
	// Если был изменён размер окна
	if (ev->type != SDL_WINDOWEVENT)
		return SDL_FALSE;

	if (ev->window.event != SDL_WINDOWEVENT_RESIZED)
		return SDL_FALSE;

	/*Если ev->type != SDL_WINDOWEVENT и ev->window.event !=
	 * SDL_WINDOWEVENT_RESIZED*/
	SDL_GetWindowSize(win, &Params->WinSize.x, &Params->WinSize.y);
	if (Params->WinSize.x < win_min)
		Params->WinSize.x = win_min;
	if (Params->WinSize.y < win_min)
		Params->WinSize.y = win_min;

	SDL_SetWindowSize(win, Params->WinSize.x, Params->WinSize.y);
	return SDL_TRUE;
}

static Uint8 (*int_CheckMove[])(Game *, Params *) = {
	CheckRightMove, CheckLeftMove, CheckDownMove, CheckUpMove};

static Uint8 (*int_CheckCombo[])(Game *, Params *) = {
	CheckRightCombo, CheckLeftCombo, CheckDownCombo, CheckUpCombo};
/*Набор функций расстановки сдвигов тайлов поля Game.
используются номера MODE_CHECK_RIGHT, MODE_CHECK_LEFT, MODE_CHECK_DOWN,
MODE_CHECK_UP*/
Uint8 (**CheckMove)(Game *, Params *) = int_CheckMove - MODE_CHECK_RIGHT;
Uint8 (**CheckCombo)(Game *, Params *) = int_CheckCombo - MODE_CHECK_RIGHT;
