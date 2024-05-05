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

SDL_Texture *CreateGreetingTexture (SDL_Event *ev, SDL_Renderer *rend, Params *Params, Game *Game, SDL_Rect *txt_size)
{
	TTF_Font *font = TTF_OpenFont("monospace", Params->WinSize.y / 10);
	if(!font)
		return NULL;

	SDL_Colour txt_col;
	txt_col.a = 0xFF;
	if (FLAG_DARKMODE & Params->Flags)
	{
		txt_col.r = BG_LIGHT_BRIGHTNESS;
		txt_col.g = BG_LIGHT_BRIGHTNESS;
		txt_col.b = BG_LIGHT_BRIGHTNESS;
	}
	else
	{
		txt_col.r = BG_DARK_BRIGHTNESS;
		txt_col.b = BG_DARK_BRIGHTNESS;
		txt_col.g = BG_DARK_BRIGHTNESS;
	}

	char message[260] = "Добро пожаловать в игру 2048!\n";
	if (Params->Flags & FLAG_DARKMODE)
		SDL_strlcat(message, "Включён тёмный режим\n", 256);
	else
		SDL_strlcat(message, "Включён светлый режим\n", 256);

	if (Params->Flags & FLAG_MOUSEOFF)
		SDL_strlcat(message, "Управление мышью отключено\n", 256);
	else
		SDL_strlcat(message, "Включено управление мышью\n", 256);

	if (Params->Flags & FLAG_WASDKEY)
		SDL_strlcat(message, "Используется управление WASD\n", 256);
	if (Params->Flags & FLAG_VIMKEY)
		SDL_strlcat(message, "Используется управление vi\n", 256);
	if (Params->Flags & FLAG_ARROWKEY)
		SDL_strlcat(message, "Используется управление стрелками\n", 256);
	
	sprintf(message + SDL_strlen(message), "Используется размер поля: %u\n", Game->FieldSize);
	SDL_Surface *txt_surf = TTF_RenderUTF8_Solid(font, message, txt_col);
	if(!txt_surf)
	{
		TTF_CloseFont(font);
		return NULL;
	}
	SDL_Texture *ret = SDL_CreateTextureFromSurface(rend, txt_surf);
	TTF_CloseFont(font);
	txt_size->w = txt_surf->w;
	txt_size->h = txt_surf->h;
	SDL_FreeSurface(txt_surf);
	return ret;
}

int main(int argc, const char **args)
{
	srand(time(NULL));
	Uint8 errCode;
	Sint8 NewElementIndex;
	Game Game;
	Params Params;
	SDL_Event Events;
	char title[40]; // = "2048 | Очков: 00000000000000000000";
	Params.Mode = MODE_START;
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

	// Возможность отключения вывода информации
	// SDL_Log("Используется размер поля: %u\n", Game.FieldSize);
	// PrintSetup(&Params);
	

	// Создание окна и рисовальщика
	if ((errCode = CreateWorkspace(&window, &rend, title, &Params.WinSize)))
		return PrintErrorAndLeaveWithCode(errCode, window, rend, &Game);

	//Вывод приветствия
	SDL_Rect txt_size;
	txt_size.x = 0, txt_size.y = 0;
	SDL_Texture *greet = CreateGreetingTexture(&Events, rend, &Params, &Game, &txt_size);
	if(!greet)
		return PrintErrorAndLeaveWithCode(ERR_SDL, window, rend, &Game);
	while(Params.Mode == MODE_START)
	{
		SDL_SetWindowTitle(window, "Добро пожаловать");

		DrawBackground(rend, Game.FieldSize, &Params);
		SDL_RenderCopy(rend, greet, NULL, &txt_size);

		SDL_RenderPresent(rend);

	}
	SDL_DestroyTexture(greet);
	Params.Mode = MODE_ADD;


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
			SDL_Log("Проверка поля\n");
			continue;

		case MODE_DRAW:
			// break;

		default:
			sprintf(title, "2048 | Очков: %lu", Game.Score);
			SDL_SetWindowTitle(window, title);

			DrawBackground(rend, Game.FieldSize, &Params);

			SDL_RenderPresent(rend);
		}
	}
}

