#include "misc.h"

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
			if((Params->Flags & FLAG_WASDKEY)) 
				Params->Mode = MODE_CHECK_RIGHT;
			return;
		case SDL_SCANCODE_RIGHT:
			if((Params->Flags & FLAG_ARROWKEY)) 
				Params->Mode = MODE_CHECK_RIGHT;
			return;

			/*Наборы клавиш "ВЛЕВО" для разных схем*/
		case SDL_SCANCODE_H:
			if((Params->Flags & FLAG_VIMKEY))
				Params->Mode = MODE_CHECK_LEFT;
			return;
		case SDL_SCANCODE_A:
			if((Params->Flags & FLAG_WASDKEY))
				Params->Mode =	MODE_CHECK_LEFT;
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
		}
		break;
	default:
		break;
	}

	// Если произошло не значимое событие
	return;
}

Uint8 PrintErrorAndLeaveWithCode (Uint8 code, SDL_Window* win, SDL_Renderer* rend, Game *Game)
{
	SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s\n", SDL_GetError());
	return SilentLeaveWithCode(code, win, rend, Game);
}

Uint8 SilentLeaveWithCode (Uint8 code, SDL_Window* win, SDL_Renderer* rend, Game *Game)
{
	//Освобождение поля
	if (Game->Field)
		SDL_free(Game->Field);

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
		return ERR_SDL;
	}

	//Создание окна
	*win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
					 WinSize->x, WinSize->y,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(!(*win))
	{
		return ERR_SDL;
	}

	//Создание рисовальщика
	*rend = SDL_CreateRenderer(*win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(!(*rend))
	{
		return ERR_SDL;
	}

	//Возврат кода штатной работы
	return ERR_NO;
}

int MinOfTwo(int a, int b)
{
	return (a > b) ? b : a; 
}

int MaxOfTwo(int a, int b)
{
	return (a < b) ? b : a; 
}

Uint8 LaunchOptions(int argc, const char **argv, Params *Settings)
{
	//Базовые параметры работы игры
	Uint8 FieldSize = 4;
	Settings->Flags	= (FLAG_DARKMODE | FLAG_ARROWKEY);

	//Если игра была запущена без флагов, 
	//то используется стандартная раскладка
	Uint8 Setters = (argc != 1) ? 15 : 0;

	/*Перебор аргументов, с которыми была запущена игра. Если их не было,
	 * цикл ниже будет пропущен*/
	for (Uint8 i = 1; Setters && (i < argc); ++i)
	{
		if (!SDL_strcmp(argv[i],"--size=3") && (Setters & SIZE_UNSET))
		{
			Setters &= ~SIZE_UNSET;
			FieldSize = 3;
			continue;
		}

		if (!SDL_strcmp(argv[i],"--size=4") && (Setters & SIZE_UNSET))
		{
			Setters &= ~SIZE_UNSET;
			continue;
		}

		if (!SDL_strcmp(argv[i],"--size=5") && (Setters & SIZE_UNSET))
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

		if (!SDL_strcmp(argv[i],"--nomouse") && (Setters & MOUSE_UNSET))
		{
			Settings->Flags |= FLAG_MOUSEOFF;
			Setters &= ~MOUSE_UNSET;
			continue;
		}

		if (!SDL_strcmp(argv[i],"--mouse") && (Setters & MOUSE_UNSET)) 
		{
			Setters &= ~MOUSE_UNSET;
			Settings->Flags &= ~FLAG_MOUSEOFF;
			continue;
		}

		if(!SDL_strcmp(argv[i], "--light") && (Setters & COL_UNSET))
		{
			Setters &= ~COL_UNSET;
			Settings->Flags &= ~FLAG_DARKMODE;
			continue;
		}


		if(!SDL_strcmp(argv[i], "--dark") && (Setters & COL_UNSET))
		{
			Setters &= ~COL_UNSET;
			continue;
		}

		if(!SDL_strcmp(argv[i], "--arrows") && (Setters & KEY_UNSET))
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

void CheckForResize(SDL_Window* win, Params* Params, SDL_Event* ev, Uint16 win_min)
{
	// Если был изменён размер окна
	if (ev->type != SDL_WINDOWEVENT)
		return;
	
	if (ev->window.event != SDL_WINDOWEVENT_RESIZED)
		return;

	/*Если ev->type != SDL_WINDOWEVENT и ev->window.event != SDL_WINDOWEVENT_RESIZED*/
	SDL_GetWindowSize(win, &Params->WinSize.x, &Params->WinSize.y);
	if (Params->WinSize.x < win_min)
		Params->WinSize.x = win_min;
	if (Params->WinSize.y < win_min)
		Params->WinSize.y = win_min;

	SDL_SetWindowSize(win, Params->WinSize.x, Params->WinSize.y);
}
