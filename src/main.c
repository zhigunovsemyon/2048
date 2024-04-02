#include "main.h"

int main(int argc, const char **args)
{
	Game Game;
	Controls Controls;
	Game.FieldSize = LaunchOptions(argc, args, &Controls);
	//Возможность отключения вывода информации
	if (SDL_TRUE)
	{
		if(Controls.mouse)
			SDL_Log("Включено управление мышью\n");
		else
			SDL_Log("Управление мышью отключено\n");
	
	
		if (Controls.up == SDL_SCANCODE_W)
			SDL_Log("Используется управление WASD\n");
		if (Controls.up == SDL_SCANCODE_K)
			SDL_Log("Используется управление vi\n");
		if (Controls.up == SDL_SCANCODE_UP)
			SDL_Log("Используется управление стрелками\n");
		
		SDL_Log("Используется размер поля: %u\n", Game.FieldSize);
	}

	return NOERR;
}
