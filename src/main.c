#include "main.h"

int main(int argc, const char **args)
{
	Game Game;
	Settings Settings;
	SDL_Event Events;
	char title[40];// = "2048 | Очков: 00000000000000000000";
	SDL_Point WinSize = {WIN_WIDTH, WIN_HEIGHT};
	SDL_Window *window = NULL;
	SDL_Renderer *rend = NULL;
	Game.Score = 0;
	Uint8 GameOver = 0;

	Game.FieldSize = LaunchOptions(argc, args, &Settings);

	// Возможность отключения вывода информации
	if (SDL_TRUE)
	{
		if (Settings.Flags &= GLOB_FLAG_DARKMODE)
			SDL_Log("Включён тёмный режим\n");
		else
			SDL_Log("Включён светлый режим\n");

		if (Settings.mouse)
			SDL_Log("Включено управление мышью\n");
		else
			SDL_Log("Управление мышью отключено\n");

		if (Settings.up == SDL_SCANCODE_W)
			SDL_Log("Используется управление WASD\n");
		if (Settings.up == SDL_SCANCODE_K)
			SDL_Log("Используется управление vi\n");
		if (Settings.up == SDL_SCANCODE_UP)
			SDL_Log("Используется управление стрелками\n");

		SDL_Log("Используется размер поля: %u\n", Game.FieldSize);
	}

	if(CreateWorkspace(&window, &rend, title, &WinSize))
		return PrintErrorAndLeaveWithCode(SDLERR, window, rend);

	while (!GameOver)
	{
		while(!SDL_PollEvent(&Events))
			continue;
		SDL_Log("Что-то случилось %lu\n", Game.Score++);


		sprintf(title, "2048 | Очков: %lu", Game.Score);
		SDL_SetWindowTitle(window, title);
		
		DrawBackground(rend, &WinSize, Settings.Flags);

		SDL_RenderPresent(rend);
	}


	return SilentLeaveWithCode(NOERR, window, rend);
}
