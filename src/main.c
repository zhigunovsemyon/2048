#include "main.h"

Uint8 SaveGame(Game *Game, Params *Params, const char *filename)
{
	/*Возможные пути программы
	 * 1. Игра окончилась геймовером, рекорд не обновлён (вынесен из функции)
	 *		сохранять ничего не нужно
	 * 2. Выход запрошен пользователем, рекорд не обновлён
	 *		нужно открыть файл и сохранить всю игру -- очки, поле
	 * 3. Выход запрошен пользователем, рекорд обновлён
	 *		нужно открыть файл и сохранить всю игру -- очки, поле
	 * 4. Игра окончилась геймовером, рекорд обновлён
	 *		нужно открыть файл и сохранить только рекорд, остальное -- признаком незагрузки
	 * */

	//Открытие файла, в который будет записан прогресс
	// SDL_RWops *fptr = SDL_RWFromFile(filename, "wb");
	//
	// //Закрытие файла
	// SDL_RWclose(fptr);
	return ERR_NO;
}

int main(int argc, const char **args)
{
	srand(time(NULL));
	Uint8 errCode;
	Sint8 NewElementIndex = -1;
	Params Params;
	Assets Assets;
	SDL_Event Events;
	Params.WinSize.x = WIN_MIN * 1.5;
	Params.WinSize.y = WIN_MIN * 2;
	Assets.textures = NULL;
	Assets.cols = NULL;
	SDL_Window *window = NULL;
	SDL_Renderer *rend = NULL;

	/*Установка всех флагов в нужное положение в соответствие с параметрами запуска,
	 * подгрузка файла с прогрессом*/
	Game Game = InitParamsAndGame(argc, args, &Params);
	if (!Game.FieldSize)
	{
		SDL_SetError("ошибка выделения памяти!");
		return PrintErrorAndLeaveWithCode(ERR_MALLOC, window, rend, &Game, &Params, &Assets);
	}

	/*Создание набора цветов*/
	if (!(Assets.cols = CreateColourSet(FLAG_DARKMODE & Params.Flags)))
	{
		SDL_SetError("ошибка выделения памяти!");
		return PrintErrorAndLeaveWithCode(ERR_MALLOC, window, rend, &Game, &Params, &Assets);
	}

	// Создание окна и рисовальщика
	if ((errCode = CreateWorkspace(&window, &rend, "Добро пожаловать", &Params.WinSize, Params.Flags)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);

	// Вывод приветствия
	if ((errCode = Greeting(window, rend, &Events, &Assets, &Params, &Game, MODE_ADD)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);

	// Подсчёт размера поля и каждой ячейки поля. Создание соответствующих текстур
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
		case MODE_USERQUIT: // 0
		case MODE_QUIT:		// 1
			//Сохранение прогресса, либо только лишь рекорда

			//Если игра закончилась переполнением поля, при этом рекорд 
			//не был обновлён, сохранять прогресс нет необходимости
			if (Params.Mode == MODE_QUIT && Game.Score < Game.MaxScore)
				SilentLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);

			errCode = SaveGame(&Game, &Params, SAVE_FILE);
			return (errCode) ? //Если произошла ошибка
				PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets) :
				SilentLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);

		//Режим ожидания, во время которого пользователь может выбрать направление движения
		case MODE_WAIT: // 11
			continue;

		//Добавление нового элемента
		case MODE_ADD: // 12
			// преобразование сложенного на прошлом цикле элемента в обычный старый
			ChangeCombinedToOld(&Game); 
			NewElementIndex = AddElement(&Game);	//Добавление нового элемента, возврат его индекса
			Game.Field[NewElementIndex].size = 0;	//сброс размера для дальнейшей отрисовки

			/*Если было найдено место для нового элемента, оно хранится в NewElementIndex.
			В противном случае там -1, что приведёт к выходу из программы*/
			Params.Mode = (NewElementIndex < 0) ? MODE_QUIT : MODE_DRAW_NEW;
			dtCount(); // Сброс счётчика времени кадра перед отрисовкой
			break;

		//Режимы проверки поля в каждом из направлений
		case MODE_CHECK_RIGHT: // 7
		case MODE_CHECK_LEFT:  // 8
		case MODE_CHECK_DOWN:  // 9
		case MODE_CHECK_UP: {  // 10
			// Может вернуть move, quit или wait
			Uint8 tmpMode = CheckMove[Params.Mode](&Game, &Params);
			dtCount();
			if (tmpMode == MODE_WAIT || tmpMode == MODE_QUIT)
			{
				Params.Mode = (CheckCombo[Params.Mode](&Game, &Params))
									//Если есть комбинации, осуществляется движение 
									//вправо, влево, вверх, вниз
								  ? Params.Mode - (MODE_CHECK_RIGHT - MODE_MOVE_RIGHT)
								  : tmpMode;//Если комбинаций нет -- режим ожидания или выхода
			}
			else // Если плитки всё ещё движутся
				Params.Mode = tmpMode;
			break;
		}

		// Режимы отрисовки сдвига ячеек в каждом из направлений
		case MODE_MOVE_RIGHT: // 3
		case MODE_MOVE_LEFT:  // 4
		case MODE_MOVE_DOWN:  // 5
		case MODE_MOVE_UP:	  // 6
			//Вызов нужной функции сдвига, в соответствии с направлением
			if ((errCode = DoMove[Params.Mode](rend, &Game, &Params, &Assets)))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);
			//Обновление заголовка на случай сложенной комбинации
			if((errCode = UpdateWindowTitle(window, Game.Score)))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);
			//Отображение изменений на экране
			SDL_RenderPresent(rend);
			break;

		//Режим отрисовки добавления нового элемента
		case MODE_DRAW_NEW: // 2
			// Рисование поля со старыми элементами
			if ((errCode = DrawOldElements(rend, &Params, &Game, &Assets) /*== ERR_SDL*/))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);

			//Отрисовка нового элемента
			if ((errCode = DrawNewElement(rend, &Params, &Game, &Assets, NewElementIndex)))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);
			//Отображение изменений на экране
			SDL_RenderPresent(rend);

			/*Если размер был сброшен, значит цикл отрисовки пора прервать,
				выставив соответстветствующие флаги */
			if (!Game.Field[NewElementIndex].size)
			{
				NewElementIndex = -1;
				Params.Mode = MODE_WAIT;
			}
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
	SDL_asprintf(&message, "%s\n%s%s%s\n%s%s\n%s%s\n%s%hhu\n%s\n", "Добро пожаловать в игру 2048!", //Заголовок
				"Включён ", (Params->Flags & FLAG_DARKMODE) ? "тёмный" : "светлый", " режим",	//Цветовой режим
				"V-Sync ", (Params->Flags & FLAG_VSYNC) ? "включен" : "отключен",	//v-sync
				"Используется управление ",(Params->Flags & FLAG_WASDKEY) ? "WASD" //схема управления wasd
				: (Params->Flags & FLAG_VIMKEY) ? "vi" : "стрелками",			// либо vi, либо стрелки
				"Используется размер поля ", Game->FieldSize, //Информация о размерах поля
				"Для выхода нажмите q.\nДля продолжения нажмите любую клавишу");
	if (!message)
	{
		SDL_SetError("ошибка выделения памяти!");
		return ERR_MALLOC;
	}

	SDL_Rect txt_size;
	txt_size.x = 0, txt_size.y = 0, txt_size.w = Params->WinSize.x, txt_size.h = Params->WinSize.y;

	SDL_Texture *greet =
		CreateMessageTexture(rend, &Assets->cols[COL_FG], &Assets->cols[COL_BG], &txt_size, FONT, message, SDL_FALSE);
	if (!greet)
	{
		SDL_SetError("ошибка создания текстуры приветствия!");
		return ERR_SDL;
	}

	if (SDL_RenderCopy(rend, greet, NULL, &txt_size))
	{
		SDL_SetError("ошибка вставки текстуры!");
		return ERR_SDL;
	}
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
			{
				SDL_SetError("ошибка создания текстуры приветствия!");
				SDL_free(message);
				return ERR_SDL;
			}

			// Отрисовка новой текстуры приветствия
			if (SDL_RenderCopy(rend, greet, NULL, &txt_size))
			{
				SDL_free(message);
				SDL_SetError("ошибка вставки текстуры!");
				return ERR_SDL;
			}
			SDL_RenderPresent(rend);
		}

		//Возможные события
		switch (ev->type)
		{
		default:
			continue;

		// Если был запрошен выход из программы
		case SDL_QUIT:
			SDL_DestroyTexture(greet);
			SDL_free(message);
			Params->Mode = MODE_QUIT;
			return ERR_NO;

		case SDL_KEYUP: // Если была нажата любая клавиша
			SDL_DestroyTexture(greet);
			//Если нажата клавиша Q -- осуществляется выход из игры, в противном случае -- начало игры
			Params->Mode = (ev->key.keysym.scancode == SDL_SCANCODE_Q) ? MODE_USERQUIT : NextMode;
			SDL_SetWindowTitle(window, "2048 | Очков: 0");
			SDL_free(message);
			return ERR_NO;
		}
	}
}
