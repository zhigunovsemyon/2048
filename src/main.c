#include "main.h"
int main(int argc, const char **args)
{
	srand(time(NULL));
	Uint8 errCode;
	Sint8 NewElementIndex = -1;
	Game Game;
	Params Params;
	SDL_Event Events;
	Params.WinSize.x = WIN_WIDTH;
	Params.WinSize.y = WIN_HEIGHT;
	Params.textures = NULL;
	SDL_Window *window = NULL;
	SDL_Renderer *rend = NULL;
	//Забил очки максимальными числами для проверки строк с ними
	Game.Score = UINT64_MAX;
	Game.Score = UINT64_MAX;

	/*Установка всех флагов в нужное положение в соответствие с параметрами запуска*/
	Game.FieldSize = LaunchOptions(argc, args, &Params);
	Game.Field = (Tile *)SDL_calloc(sizeof(Tile), // Выделение памяти под игровое поле
									Game.FieldSize * Game.FieldSize);
	if (!Game.Field)
		return ERR_MALLOC;

	/*Создание набора цветов*/
	if (!(Params.cols = CreateColourSet(FLAG_DARKMODE & Params.Flags)))
		return ERR_MALLOC;

	// Создание окна и рисовальщика
	if ((errCode = CreateWorkspace(&window, &rend, "Добро пожаловать", &Params.WinSize, Params.Flags)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params);

	// Вывод приветствия
	if ((errCode = Greeting(window, rend, &Events, &Params, &Game, MODE_ADD)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params);

	if(!(Params.textures = CreateTextureSet(rend, Params.cols, &Params.WinSize, &Game)))
		return PrintErrorAndLeaveWithCode(ERR_MALLOC, window, rend, &Game, &Params);

	// Игровой цикл
	while (SDL_TRUE)
	{
		SetMode(&Events, &Params); // Выбор режима работы в данный момент
		if (CheckForResize(window, &Params, &Events, WIN_MIN)) // Проверка на изменение размера
		{
			Params.textures = UpdateTextureSet(rend, Params.textures,
				Params.cols, &Params.WinSize, &Game);
			// Рисование поля со старыми элементами
			if ((errCode = DrawOldElements(rend, &Params, &Game) /*== ERR_SDL*/))
				PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params);
			SDL_RenderPresent(rend);
		}

		switch (Params.Mode)
		{
		case MODE_QUIT:
			return SilentLeaveWithCode(errCode, window, rend, &Game, &Params);

		case MODE_WAIT:
			continue;

		case MODE_ADD:
			NewElementIndex = AddElement(&Game);
			Game.Field[NewElementIndex].size = 0;
			/*Если было найдено место для нового элемента, оно хранится в NewElementIndex.
			В противном случае там -1, что приведёт к выходу из программы*/
			Params.Mode = (NewElementIndex < 0) ? MODE_QUIT : MODE_DRAW_NEW;
			dtCount();	//Сброс счётчика времени кадра перед отрисовкой
			break;

		case MODE_CHECK_RIGHT:
		case MODE_CHECK_LEFT:
		case MODE_CHECK_DOWN:
		case MODE_CHECK_UP:
			Params.Mode = CheckMove[Params.Mode](&Game);
			DoOffset(&Game,&Params);
			dtCount();
			continue;
			
		case MODE_MOVE_RIGHT:
		case MODE_MOVE_LEFT:
		case MODE_MOVE_UP:
		case MODE_MOVE_DOWN:
			if((errCode = DoMove[Params.Mode](rend, &Game, &Params)))
				return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params);
			SDL_RenderPresent(rend);
			break;

		case MODE_DRAW_NEW:
			if (NewElementIndex >= 0)
			{
				// Рисование поля со старыми элементами
				if ((errCode = DrawOldElements(rend, &Params, &Game) /*== ERR_SDL*/))
					return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params);

				if ((errCode = DrawNewElement(rend, &Params, &Game, NewElementIndex)))
					return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game, &Params);
				SDL_RenderPresent(rend);
				/*Если размер был сброшен, значит цикл отрисовки пора прервать,
					выставив соответстветствующие флаги */
				if (!Game.Field[NewElementIndex].size) 
				{
					NewElementIndex = -1;
					Params.Mode = MODE_WAIT;
				}				
			}
			break;

		default:
			break;
		}
	}
}
