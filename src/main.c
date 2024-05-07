#include "main.h"

int main(int argc, const char **args)
{
	srand(time(NULL));
	Uint8 errCode;
	Sint8 NewElementIndex = -1;
	Game Game;
	Params Params;
	SDL_Event Events;
	char title[40]; // = "2048 | Очков: 00000000000000000000";
	Params.WinSize.x = WIN_WIDTH;
	Params.WinSize.y = WIN_HEIGHT;
	SDL_Window *window = NULL;
	SDL_Renderer *rend = NULL;
	Game.Score = 0;

	Game.FieldSize = LaunchOptions(argc, args, &Params);
	Game.Field = (Tile*)SDL_calloc(sizeof(Tile), //Выделение памяти под игровое поле
		Game.FieldSize * Game.FieldSize);
	if (!Game.Field)
		return ERR_MALLOC;

	// Создание окна и рисовальщика
	if ((errCode = CreateWorkspace(&window, &rend, title, &Params.WinSize, Params.Flags)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game);

	//Вывод приветствия
	if ((errCode = Greeting(window, rend, &Events, &Params, &Game, MODE_ADD)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game);

	// Игровой цикл
	while (SDL_TRUE)
	{
		SetMode(&Events, &Params);	//Выбор режима работы в данный момент
		CheckForResize(window, &Params, &Events, WIN_MIN);	//Проверка на изменение размера

		switch (Params.Mode)
		{
		case MODE_QUIT:
			return SilentLeaveWithCode(errCode, window, rend, &Game);

		case MODE_WAIT:
			continue;

		case MODE_ADD:
			NewElementIndex = AddElement(&Game);
			Params.Mode = (NewElementIndex < 0) ? 
				MODE_QUIT : MODE_DRAW;
			break;

		case MODE_CHECK_UP:
		case MODE_CHECK_DOWN:
		case MODE_CHECK_RIGHT:
		case MODE_CHECK_LEFT:
			Params.Mode = MODE_ADD;
			SDL_Log("Проверка поля\n");
			continue;

		case MODE_DRAW:
			if (NewElementIndex >= 0)
			{
				DrawNewElement(rend, &Params, &Game, NewElementIndex);
				NewElementIndex = -1;
			}
			else 
			{
				Params.Mode = MODE_WAIT;
			}
			break;

		default:
			break;
			// sprintf(title, "2048 | Очков: %lu", Game.Score);
			// SDL_SetWindowTitle(window, title);
			//
			// DrawBackground(rend, Game.FieldSize, &Params);
			//
			// SDL_RenderPresent(rend);
		}
	}
}

