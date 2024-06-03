#include "main.h"
#include "defines.h"
int main(int argc, const char **args)
{
	srand(time(NULL));	//Инициализация рандомайзера
	Uint8 errCode = 0;	//Код ошибок
	
	//Парамерты игры
	Params Params = {	.WinSize.x = WIN_MIN * 1.5,
						.WinSize.y = WIN_MIN * 2};

	//Ассеты с текстурами и цветами
	Assets Assets = {	.textures = NULL,
						.cols = NULL };
		
	/*Игровое поле, очки, режим игры*/
	Game Game;

	SDL_Window *window = NULL;//Окно с игрой
	SDL_Renderer *rend = NULL;//Рисовальщик

	/*Установка всех флагов в нужное положение в соответствие с параметрами запуска,
	 * подгрузка файла с прогрессом*/
	Game = InitParamsAndGame(argc, args, &Params, SAVE_FILE);
	if (!Game.Field)
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
	
	/* Вывод приветствия.Если пользователь захотел выйти уже с экрана приветствия,
		в игровом цикле он сразу выйдет на MODE_QUIT */
	if ((errCode = Greeting(window, rend, &Assets, &Params, &Game)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);

	// Подсчёт размера поля и каждой ячейки поля. Создание соответствующих текстур
	GetFieldAndTileSize(&Game, &Params);
	if ((errCode =  InitTextureSet(rend, &Assets, &Params, &Game)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);

	// Игровой цикл. Если он завершился ошибкой, аварийное завершение	
	if ((errCode = GameCycle(window, rend, &Assets, &Params, &Game)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);

	//Сохранение прогресса, либо только лишь рекорда
	return ((errCode = SaveGame(&Game, SAVE_FILE))) ? //Если произошла ошибка
		PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets) :
		SilentLeaveWithCode(errCode, window, rend, &Game, &Params, &Assets);
}

/* Цикл игры Game в окне window рисовальщиком rend,
	с параметрами Params и применением текстур и цветов в Assets */
Uint8 GameCycle(SDL_Window *window, SDL_Renderer *rend,
				Assets *Assets, Params *Params, Game *Game)
{
	Uint8 errCode = 0;	//Код ошибки
	SDL_Event Events;	//Юнион событий
	Sint8 NewElementIndex = -1;	//Индекс нового элемента (-1 если такого нет)

	//Непосредственно цикл
	while (SDL_TRUE)
	{
		SetMode(&Events, Game, Params); // Выбор режима работы в данный момент

		// Проверка на изменение размера
		if (CheckForResize(window, Params, &Events, WIN_MIN))
		{	//Получение размера поля и тайлов в пикселях, обновление набора текстур
			GetFieldAndTileSize(Game, Params);
			if ((errCode = UpdateTextureSet(rend, Params, Game, Assets)))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, Game,
												  Params, Assets);

			// Рисование поля со старыми элементами
			if ((errCode = DrawOldElements(rend, Params, Game, Assets)))
				PrintErrorAndLeaveWithCode(errCode, window, rend, Game,
										   Params, Assets);
			SDL_RenderPresent(rend);
		}

		//Обработка возможных событий
		switch (Game->Mode)
		{
		//Выход из программы по заполнению поля, или же по желанию
		case MODE_QUIT:		// 0
		case MODE_GAMEOVER: // 1
			return errCode;

		/* Режим ожидания, во время которого пользователь
		может выбрать направление движения */
		case MODE_WAIT: // 11
			continue;

		// Добавление нового элемента
		case MODE_ADD: // 12
			/* преобразование сложенного на прошлом цикле элемента
			в обычный старый */
			ChangeCombinedToOld(Game);

			// Добавление нового элемента, возврат его индекса
			NewElementIndex = AddElement(Game); 
			// сброс размера для дальнейшей отрисовки
			Game->Field[NewElementIndex].size = 0; 

			/*Если было найдено место для нового элемента, оно хранится в
			NewElementIndex. В противном случае там -1, что приведёт к выходу из
			программы*/
			Game->Mode = (NewElementIndex < 0) ? MODE_GAMEOVER : MODE_DRAW_NEW;
			dtCount(); // Сброс счётчика времени кадра перед отрисовкой
			break;

		// Режимы проверки поля в каждом из направлений
		case MODE_CHECK_RIGHT: // 7
		case MODE_CHECK_LEFT:  // 8
		case MODE_CHECK_DOWN:  // 9
		case MODE_CHECK_UP: {  // 10
			// Может вернуть move, quit или wait
			Uint8 tmpMode = CheckMove[Game->Mode](Game, Params);
			dtCount();
			if (tmpMode == MODE_WAIT || tmpMode == MODE_GAMEOVER)
			{
				Game->Mode =
					(CheckCombo[Game->Mode](Game, Params))
						/* Если есть комбинации, осуществляется движение
						вправо, влево, вверх, вниз */
						? Game->Mode - (MODE_CHECK_RIGHT - MODE_MOVE_RIGHT)
						/* Если комбинаций нет -- режим ожидания или выхода*/
						: tmpMode; 
			}
			else // Если плитки всё ещё движутся
				Game->Mode = tmpMode;
			break;
		}

		// Режимы отрисовки сдвига ячеек в каждом из направлений
		case MODE_MOVE_RIGHT: // 3
		case MODE_MOVE_LEFT:  // 4
		case MODE_MOVE_DOWN:  // 5
		case MODE_MOVE_UP:	  // 6
			// Вызов нужной функции сдвига, в соответствии с направлением
			if ((errCode = DoMove[Game->Mode](rend, Game, Params, Assets)))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, Game,
												  Params, Assets);
			// Обновление заголовка на случай сложенной комбинации
			if ((errCode = UpdateWindowTitle(window, Game->Score)))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, Game,
												  Params, Assets);
			// Отображение изменений на экране
			SDL_RenderPresent(rend);
			break;

		// Режим отрисовки добавления нового элемента
		case MODE_DRAW_NEW: // 2
			// Рисование поля со старыми элементами
			if ((errCode = DrawOldElements(rend, Params, Game,
										   Assets) /*== ERR_SDL*/))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, Game,
												  Params, Assets);

			// Отрисовка нового элемента, отображение
			if ((errCode = DrawNewElement(rend, Params, Game, Assets,
										  NewElementIndex)))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, Game,
												  Params, Assets);
			SDL_RenderPresent(rend);

			/*Если был передан индекс -1, значит нужно
			 * переключится в режим ожидания */
			if(NewElementIndex == -1)
			{
				Game->Mode = MODE_WAIT;
				break;
			}

			/*Если размер был сброшен, значит цикл отрисовки пора прервать,
				выставив соответстветствующий флаг */
			if (!Game->Field[NewElementIndex].size)
			{
				NewElementIndex = -1;
				Game->Mode = MODE_WAIT;
			}
			break;
		}
	}
}

/*Вывод приветствия в игру, отображённого в окне window, рисовальщиком rend,параметров игры Params, 
настроек игры Game. Возврат нуля при отсутствии ошибок, либо кода ошибки*/
Uint8 Greeting(SDL_Window *window, SDL_Renderer *rend, Assets *Assets, Params *Params, Game *Game)
{ // Создание сообщения
	char *message;
	SDL_asprintf(&message, "%s\n%s\n%s\n%s%s%s\n%s%s\n%s%s\n%s%hhu\n%s\n", 
		"Автор игры: Gabriele Cirulli",
		"Автор данной реализации: Жигунов С. Н.",
		"Добро пожаловать в игру 2048!", //Заголовок
		"Включён ", (Params->Flags & FLAG_DARKMODE) ? "тёмный" : "светлый", " режим",	//Цветовой режим
		"V-Sync ", (Params->Flags & FLAG_VSYNC) ? "включен" : "отключен",	//v-sync
		"Используется управление ",(Params->Flags & FLAG_WASDKEY) ? "WASD" //схема управления wasd
		: (Params->Flags & FLAG_VIKEY) ? "vi" : "стрелками",			// либо vi, либо стрелки
		"Используется размер поля ", Game->FieldSize, //Информация о размерах поля
		"Для выхода нажмите q.\nДля продолжения нажмите любую клавишу");
	if (!message)
	{
		SDL_SetError("ошибка выделения памяти!");
		return ERR_MALLOC;
	}
	SDL_Event ev;

	//Рект, в который будет вписан текст приветствия, равный размеру экрана
	SDL_Rect txt_size = {.x = 0, .y = 0};
	txt_size.w = Params->WinSize.x, txt_size.h = Params->WinSize.y;

	//Создание текстуры приветствия
	SDL_Texture *greet =
		CreateMessageTexture(rend, &Assets->cols[COL_FG], &Assets->cols[COL_BG], &txt_size, FONT, message, SDL_FALSE);
	if (!greet)
	{
		SDL_SetError("ошибка создания текстуры приветствия!");
		return ERR_SDL;
	}

	//Отрисовка текстуры приветствия, отображение
	if (SDL_RenderCopy(rend, greet, NULL, &txt_size))
	{
		SDL_free(message);
		SDL_DestroyTexture(greet); // Уничтожение старой надписи
		SDL_SetError("ошибка вставки текстуры!");
		return ERR_SDL;
	}
	SDL_RenderPresent(rend);

	//Цикл ожидания ввода пользователем
	while (SDL_TRUE)
	{
		// Если событий не было, цикл сразу идёт на следующий виток, режим не меняется
		while (!SDL_PollEvent(&ev))
			continue;

		/*Если, на экране приветствия, изменился размер окна, надпись отрисовывается по новой*/
		if (CheckForResize(window, Params, &ev, WIN_MIN))
		{
			SDL_DestroyTexture(greet);	//Уничтожение старой надписи

			//Задание новых размеров ректа, создание новой текстуры
			txt_size.w = Params->WinSize.x, txt_size.h = Params->WinSize.y;
			greet = CreateMessageTexture(rend, &Assets->cols[COL_FG], &Assets->cols[COL_BG], &txt_size, FONT, message,
										 SDL_FALSE);
			if (!greet)
			{
				SDL_SetError("ошибка создания текстуры приветствия!");
				SDL_free(message);
				return ERR_SDL;
			}

			// Отрисовка новой текстуры приветствия, отображение
			if (SDL_RenderCopy(rend, greet, NULL, &txt_size))
			{
				SDL_free(message);
				SDL_DestroyTexture(greet); // Уничтожение старой надписи
				SDL_SetError("ошибка вставки текстуры!");
				return ERR_SDL;
			}
			SDL_RenderPresent(rend);
		}

		//Обработка возможных событий
		switch (ev.type)
		{
		default:
			continue;
		
		case SDL_QUIT:	// Если был запрошен выход из программы
			SDL_DestroyTexture(greet);
			SDL_free(message);
			Game->Mode = MODE_QUIT;
			return ERR_NO;

		case SDL_KEYUP: // Если была нажата любая клавиша
			SDL_DestroyTexture(greet);
			//Если нажата клавиша Q -- осуществляется выход из игры, в противном случае -- начало игры
			if (ev.key.keysym.scancode == SDL_SCANCODE_Q) 
					Game->Mode = MODE_QUIT;
			SDL_SetWindowTitle(window, "2048 | Очков: 0");
			SDL_free(message);
			return ERR_NO;
		}
	}
}
