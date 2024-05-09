#include "misc.h"
#define _SQ(A) (A) * (A)

Uint8 CountLines(const char *source)
{
	Uint8 i = 1;//Если в тексте нет переносов, значит там одна строка
	/*Когда функция находит перенос, она возвращает указатель на него,
	в противном случае -- NULL, цикл завершается */
	while((source = SDL_strchr(source, '\n')))
	{
		source++;
		if(*source/*!= 0*/)
			i++;
	}
	return i;
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
		return pos; // Позиция нового элемента возвращается
	}
	/*Если случайно подобрать положение не удалось, функция перебирает ячейки по очереди
	с конца, проводя аналогичную проверку*/
	for (pos = tries - 1; pos >= 0; pos--)
	{
		if (Game->Field[pos].val /*!= 0*/)
			continue;
		/*else*/
		Game->Field[pos].val = (RandomInt(0, CHANCE_OF_FOUR)) ? 2 : 4;
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
	lasttime = newtime;
	return ret;
}

Sint32 RandomInt(Sint32 a, Sint32 b)
{
	//"Переворот чисел при необходимости"
	if (a > b)
	{
		Sint32 tmp = a;
		a = b;
		b = tmp;
	}
	return (rand() % (b - a)) + a;
}

Uint8 Greeting(SDL_Window *window, SDL_Renderer *rend, SDL_Event *ev, Params *Params, Game *Game, Uint8 NextMode)
{
	char message[MSG_LEN] = "Добро пожаловать в игру 2048!\n";
	if (Params->Flags & FLAG_DARKMODE)
		SDL_strlcat(message, "Включён тёмный режим\n", MSG_LEN);
	else
		SDL_strlcat(message, "Включён светлый режим\n", MSG_LEN);

	if (Params->Flags & FLAG_MOUSEOFF)
		SDL_strlcat(message, "Управление мышью отключено\n", MSG_LEN);
	else
		SDL_strlcat(message, "Включено управление мышью\n", MSG_LEN);

	if (Params->Flags & FLAG_WASDKEY)
		SDL_strlcat(message, "Используется управление WASD\n", MSG_LEN);
	if (Params->Flags & FLAG_VIMKEY)
		SDL_strlcat(message, "Используется управление vi\n", MSG_LEN);
	if (Params->Flags & FLAG_ARROWKEY)
		SDL_strlcat(message, "Используется управление стрелками\n", MSG_LEN);

	sprintf(message + SDL_strlen(message), "Используется размер поля: %u\n", Game->FieldSize);
	SDL_strlcat(message, "Для выхода нажмите q.\nДля прододжения нажмите любую клавишу\n", MSG_LEN);

	SDL_Rect txt_size;
	txt_size.x = 0, txt_size.y = 0, txt_size.w = Params->WinSize.x, txt_size.h = Params->WinSize.y;

	SDL_Colour txt_col, bg_col;
	txt_col.a = 0xFF;
	bg_col.a = 0xFF;
	if (Params->Flags & FLAG_DARKMODE)
	{
		txt_col.r = BG_LIGHT_BRIGHTNESS;
		txt_col.g = BG_LIGHT_BRIGHTNESS;
		txt_col.b = BG_LIGHT_BRIGHTNESS;
	}
	else
	{
		txt_col.r = BG_DARK_BRIGHTNESS;
		txt_col.g = BG_DARK_BRIGHTNESS;
		txt_col.b = BG_DARK_BRIGHTNESS;
	}

	if (Params->Flags & FLAG_DARKMODE)
	{
		bg_col.r = BG_DARK_BRIGHTNESS;
		bg_col.g = BG_DARK_BRIGHTNESS;
		bg_col.b = BG_DARK_BRIGHTNESS;
	}
	else
	{
		bg_col.r = BG_LIGHT_BRIGHTNESS;
		bg_col.g = BG_LIGHT_BRIGHTNESS;
		bg_col.b = BG_LIGHT_BRIGHTNESS;
	}
// SDL_Texture *CreateMessageTexture(SDL_Renderer *rend, SDL_Colour txt_col, SDL_Colour *bg_col,
// 								  SDL_Rect *txt_size, const char *font_name, const char *message);
	SDL_Texture *greet = CreateMessageTexture(rend, &txt_col, &bg_col, &txt_size, FONT, message, SDL_FALSE);
	if (!greet)
		return ERR_SDL;

	Uint8 BG_brightness = (Params->Flags & FLAG_DARKMODE) ? BG_DARK_BRIGHTNESS : BG_LIGHT_BRIGHTNESS;
	SDL_SetWindowTitle(window, "Добро пожаловать");
	SDL_RenderCopy(rend, greet, NULL, &txt_size);
	SDL_RenderPresent(rend);

	while (SDL_TRUE)
	{
		while (!SDL_PollEvent(ev))
		{ // Если событий не было, сразу осуществляется выход, режим не меняется
			continue;
		}

		/*Если, на экране приветствия, изменился размер окна, надпись отрисовывается по новой*/
		if (CheckForResize(window, Params, ev, WIN_MIN))
		{
			SDL_DestroyTexture(greet);
			greet = CreateMessageTexture(rend, &txt_col, &bg_col, &txt_size, FONT, message, SDL_FALSE);
			// greet = CreateMessageTexture(rend, Params, &txt_size, FONT, message);
			if (!greet)
				return ERR_SDL;

			// Заливка фона
			if (SDL_SetRenderDrawColor(rend, BG_brightness, BG_brightness, BG_brightness, 0xff))
				return ERR_SDL;
			if (SDL_RenderClear(rend))
				return ERR_SDL;
			SDL_RenderCopy(rend, greet, NULL, &txt_size);
			SDL_RenderPresent(rend);
		}

		switch (ev->type)
		{
		default:
			continue;

		// Если был запрошен выход из программы
		case SDL_QUIT:
			SDL_DestroyTexture(greet);
			Params->Mode = MODE_QUIT;
			return ERR_NO;

		case SDL_MOUSEBUTTONUP:
			if (Params->Flags & FLAG_MOUSEOFF)
				continue;
			SDL_DestroyTexture(greet);
			Params->Mode = NextMode;
			SDL_SetWindowTitle(window, "2048 | Очков: 0");
			return ERR_NO;

		case SDL_KEYUP: // Если была нажата клавиша
			SDL_DestroyTexture(greet);
			Params->Mode = (ev->key.keysym.scancode == SDL_SCANCODE_Q) ? MODE_QUIT : NextMode;
			SDL_SetWindowTitle(window, "2048 | Очков: 0");
			return ERR_NO;
		}
	}
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

	case SDL_MOUSEBUTTONUP:
		if (Params->Flags & FLAG_MOUSEOFF)
			return;
		break;

	case SDL_KEYUP: // Если была нажата клавиша

		// Если програма отображает перемещение ячейки, то возможно только выйти из программы
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

Uint8 PrintErrorAndLeaveWithCode(Uint8 code, SDL_Window *win, SDL_Renderer *rend, Game *Game)
{
	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s\n", SDL_GetError());
	return SilentLeaveWithCode(code, win, rend, Game);
}

Uint8 SilentLeaveWithCode(Uint8 code, SDL_Window *win, SDL_Renderer *rend, Game *Game)
{
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

Uint8 CreateWorkspace(SDL_Window **win, SDL_Renderer **rend, const char *title, const SDL_Point *WinSize, Uint8 Flag)
{
	// Вызов SDL
	if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
		return ERR_SDL; // Завершение работы, если не удалось вызвать SDL

	// Вызов SDL_ttf
	if (TTF_Init())
		return ERR_TTF; // Завершение работы, если не удалось вызвать TTF

	// Создание окна
	*win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WinSize->x, WinSize->y,
							SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!(*win))
	{
		return ERR_SDL;
	}

	// Создание рисовальщика
	*rend =
		SDL_CreateRenderer(*win, -1, SDL_RENDERER_ACCELERATED | ((Flag & FLAG_VSYNC) ? SDL_RENDERER_PRESENTVSYNC : 0));
	if (!(*rend))
	{
		return ERR_SDL;
	}

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

Uint8 LaunchOptions(int argc, const char **argv, Params *Settings)
{
	// Базовые параметры работы игры
	Uint8 FieldSize = 4;
	Settings->Flags = (FLAG_VSYNC | FLAG_DARKMODE | FLAG_ARROWKEY);

	// Если игра была запущена без флагов,
	Uint8 Setters = (argc == 1) ? 0 : // то используется стандартная раскладка
						(VSYNC_UNSET | COL_UNSET | KEY_UNSET | MOUSE_UNSET | SIZE_UNSET);

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

		if (!SDL_strcmp(argv[i], "--nomouse") && (Setters & MOUSE_UNSET))
		{
			Settings->Flags |= FLAG_MOUSEOFF;
			Setters &= ~MOUSE_UNSET;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--mouse") && (Setters & MOUSE_UNSET))
		{
			Setters &= ~MOUSE_UNSET;
			Settings->Flags &= ~FLAG_MOUSEOFF;
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
	return FieldSize;
}

Uint8 CheckForResize(SDL_Window *win, Params *Params, SDL_Event *ev, Uint16 win_min)
{
	// Если был изменён размер окна
	if (ev->type != SDL_WINDOWEVENT)
		return SDL_FALSE;

	if (ev->window.event != SDL_WINDOWEVENT_RESIZED)
		return SDL_FALSE;

	/*Если ev->type != SDL_WINDOWEVENT и ev->window.event != SDL_WINDOWEVENT_RESIZED*/
	SDL_GetWindowSize(win, &Params->WinSize.x, &Params->WinSize.y);
	if (Params->WinSize.x < win_min)
		Params->WinSize.x = win_min;
	if (Params->WinSize.y < win_min)
		Params->WinSize.y = win_min;

	SDL_SetWindowSize(win, Params->WinSize.x, Params->WinSize.y);
	return SDL_TRUE;
}
