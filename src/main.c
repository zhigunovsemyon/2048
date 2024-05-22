#include "main.h"
int main(int argc, const char **args)
{
	srand(time(NULL));
	Uint8 errCode;
	Sint8 NewElementIndex = -1;
	Game Game;
	Params Params;
	Assets Assets;
	SDL_Event Events;
	Params.WinSize.x = WIN_MIN * 1.5;
	Params.WinSize.y = WIN_MIN * 2;
	Assets.textures = NULL;
	SDL_Window *window = NULL;
	SDL_Renderer *rend = NULL;
	// Забил очки максимальными числами для проверки строк с ними
	Game.Score = 0;//UINT64_MAX;
	Game.MaxScore = 0;UINT64_MAX;

	/*Установка всех флагов в нужное положение в соответствие с параметрами запуска*/
	Game.FieldSize = LaunchOptions(argc, args, &Params);
	Game.Field = (Tile *)SDL_calloc(sizeof(Tile), // Выделение памяти под игровое поле
									Game.FieldSize * Game.FieldSize);
	if (!Game.Field)
		return ERR_MALLOC;

	/*Создание набора цветов*/
	if (!(Assets.cols = CreateColourSet(FLAG_DARKMODE & Params.Flags)))
		return ERR_MALLOC;

	// Создание окна и рисовальщика
	if ((errCode = CreateWorkspace(&window, &rend, "Добро пожаловать", &Params.WinSize, Params.Flags)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);

	// Вывод приветствия
	if ((errCode = Greeting(window, rend, &Events, &Assets, &Params, &Game, MODE_ADD)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);

	// Подсчёт размера поля и каждой ячейки поля
	GetFieldAndTileSize(&Game, &Params);

	if ((errCode =  InitTextureSet(rend, &Assets, &Params, &Game)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);

	// Игровой цикл
	while (SDL_TRUE)
	{
		SetMode(&Events, &Params); // Выбор режима работы в данный момент
		if (CheckForResize(window, &Params, &Events, WIN_MIN)) // Проверка на изменение размера
		{
			GetFieldAndTileSize(&Game, &Params);
			if((errCode = UpdateTextureSet(rend, &Params, &Game, &Assets)))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game,
										   &Params, &Assets);

			// Рисование поля со старыми элементами
			if ((errCode = DrawOldElements(rend, &Params, &Game, &Assets) /*== ERR_SDL*/))
				PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);
			SDL_RenderPresent(rend);
		}

		switch (Params.Mode)
		{
		case MODE_QUIT: // 0
			return SilentLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);

		case MODE_WAIT: // 10
			continue;

		case MODE_ADD: // 11
			ChangeCombinedToOld(&Game);
			NewElementIndex = AddElement(&Game);
			Game.Field[NewElementIndex].size = 0;
			/*Если было найдено место для нового элемента, оно хранится в NewElementIndex.
			В противном случае там -1, что приведёт к выходу из программы*/
			Params.Mode = (NewElementIndex < 0) ? MODE_QUIT : MODE_DRAW_NEW;
			dtCount(); // Сброс счётчика времени кадра перед отрисовкой
			break;

		case MODE_CHECK_RIGHT: // 6
		case MODE_CHECK_LEFT:  // 7
		case MODE_CHECK_DOWN:  // 8
		case MODE_CHECK_UP: {  // 9
			// Может вернуть move, quit или wait
			Uint8 tmpMode = CheckMove[Params.Mode](&Game, &Params);
			dtCount();
			if (tmpMode == MODE_WAIT || tmpMode == MODE_QUIT)
			{
				Params.Mode = (CheckCombo[Params.Mode](&Game, &Params))
									//Если есть комбинации, осуществляется движение 
									//вправо, влево, вверх, вниз
								  ? Params.Mode - (MODE_CHECK_RIGHT - MODE_MOVE_RIGHT)
								  : tmpMode == MODE_WAIT;//Если комбинаций нет -- режим ожидания
			}
			else // Если плитки всё ещё движутся
				Params.Mode = tmpMode;
			break;
		}

		case MODE_MOVE_RIGHT: // 2
		case MODE_MOVE_LEFT:  // 3
		case MODE_MOVE_DOWN:  // 4
		case MODE_MOVE_UP:	  // 5
			if ((errCode = DoMove[Params.Mode](rend, &Game, &Params, &Assets)))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);
			if((errCode = UpdateWindowTitle(window, Game.Score)))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);
			SDL_RenderPresent(rend);
			break;

		case MODE_DRAW_NEW: // 1
			// Рисование поля со старыми элементами
			if ((errCode = DrawOldElements(rend, &Params, &Game, &Assets) /*== ERR_SDL*/))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);

			if ((errCode = DrawNewElement(rend, &Params, &Game, &Assets, NewElementIndex)))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);
			SDL_RenderPresent(rend);

			/*Если размер был сброшен, значит цикл отрисовки пора прервать,
				выставив соответстветствующие флаги */
			if (!Game.Field[NewElementIndex].size)
			{
				NewElementIndex = -1;
				Params.Mode = MODE_WAIT;
			}
			break;

		default:
			break;
		}
	}
}

/*Вывод приветствия в игру, отображённого в окне window, рисовальщиком rend, с учётом событий ev,
 * параметров игры Params, настроек игры Game. Возврат нуля при отсутствии ошибок, либо SDL_ERR*/
Uint8 Greeting(SDL_Window *window, SDL_Renderer *rend, SDL_Event *ev, Assets *Assets, Params *Params, Game *Game,
			   Uint8 NextMode)
{ // Создание сообщения
	char *message;
	SDL_asprintf(&message, "%s\n%s%s%s\n%s%s\n%s%s\n%s%s\n%s%hhu\n%s\n", "Добро пожаловать в игру 2048!", //Заголовок
				"Включён ", (Params->Flags & FLAG_DARKMODE) ? "тёмный" : "светлый", " режим",	//Цветовой режим
				"Управление мышью ",(Params->Flags & FLAG_MOUSEOFF) ? "отключено" : "включено", //Управление мышью
				"V-Sync ", (Params->Flags & FLAG_VSYNC) ? "включен" : "отключен",	//v-sync
				"Используется управление ",(Params->Flags & FLAG_WASDKEY) ? "WASD" //схема управления wasd
				: (Params->Flags & FLAG_VIMKEY) ? "vi" : "стрелками",			// либо vi, либо стрелки
				"Используется размер поля ", Game->FieldSize, //Информация о размерах поля
				"Для выхода нажмите q.\nДля продолжения нажмите любую клавишу");
	if (!message)
		return ERR_MALLOC;

	SDL_Rect txt_size;
	txt_size.x = 0, txt_size.y = 0, txt_size.w = Params->WinSize.x, txt_size.h = Params->WinSize.y;

	SDL_Texture *greet =
		CreateMessageTexture(rend, &Assets->cols[COL_FG], &Assets->cols[COL_BG], &txt_size, FONT, message, SDL_FALSE);
	SDL_free(message);
	if (!greet)
		return ERR_SDL;

	if (SDL_RenderCopy(rend, greet, NULL, &txt_size))
		return ERR_SDL;
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
			txt_size.w = Params->WinSize.x, txt_size.h = Params->WinSize.y;
			greet = CreateMessageTexture(rend, &Assets->cols[COL_FG], &Assets->cols[COL_BG], &txt_size, FONT, message,
										 SDL_FALSE);
			if (!greet)
				return ERR_SDL;

			// Заливка фона
			if (SDL_SetRenderDrawColor(rend, SPLIT_COL_VAL(Assets->cols[COL_BG])))
				return ERR_SDL;
			if (SDL_RenderClear(rend))
				return ERR_SDL;
			if (SDL_RenderCopy(rend, greet, NULL, &txt_size))
				return ERR_SDL;
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
