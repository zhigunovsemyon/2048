#include "main.h"

void PrintSetup(Params *Params)
{
	if (Params->Flags & FLAG_DARKMODE)
		SDL_Log("Включён тёмный режим\n");
	else
		SDL_Log("Включён светлый режим\n");

	if (Params->Flags & FLAG_MOUSEOFF)
		SDL_Log("Управление мышью отключено\n");
	else
		SDL_Log("Включено управление мышью\n");

	if (Params->Flags & FLAG_WASDKEY)
		SDL_Log("Используется управление WASD\n");
	if (Params->Flags & FLAG_VIMKEY)
		SDL_Log("Используется управление vi\n");
	if (Params->Flags & FLAG_ARROWKEY)
		SDL_Log("Используется управление стрелками\n");

}

int main(int argc, const char **args)
{
	srand(time(NULL));
	Uint8 errCode;
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
	Game.Field = (Uint64*)SDL_calloc(sizeof(Uint64), //Выделение памяти под игровое поле
		Game.FieldSize * Game.FieldSize);
	if (!Game.Field)
		return ERR_MALLOC;

	// Возможность отключения вывода информации
	SDL_Log("Используется размер поля: %u\n", Game.FieldSize);
	PrintSetup(&Params);

	// Создание окна и рисовальщика
	if ((errCode = CreateWorkspace(&window, &rend, title, &Params.WinSize)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game);

	Params.Mode = UINT8_MAX;
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

		case MODE_CHECK_UP:
		case MODE_CHECK_DOWN:
		case MODE_CHECK_RIGHT:
		case MODE_CHECK_LEFT:
			SDL_Log("Проверка поля\n");
			continue;

		default:
			sprintf(title, "2048 | Очков: %lu", Game.Score);
			SDL_SetWindowTitle(window, title);

			DrawBackground(rend, Game.FieldSize, &Params);

			SDL_RenderPresent(rend);
		}
	}
}

