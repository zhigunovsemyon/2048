#include "main.h"

SDL_Colour *CreateColourSet(Uint8 DarkModeFlag)
{
	SDL_Colour *set = (SDL_Colour *)SDL_malloc(COLOURS_COUNT * sizeof(SDL_Colour));
	if (!set)
		return NULL;
	for (Uint8 i = 0; i < COLOURS_COUNT; ++i)
		set[i].a = 0xFF;

	if (DarkModeFlag) //Если включена тёмная тема, в цвета фона передаются соответствующие значения
	{
		set[COL_BG].r = BG_DARK_BRIGHTNESS, set[COL_BG].g = BG_DARK_BRIGHTNESS;
		set[COL_BG].b = BG_DARK_BRIGHTNESS, set[COL_FG].r = BG_LIGHT_BRIGHTNESS;
		set[COL_FG].g = BG_LIGHT_BRIGHTNESS, set[COL_FG].b = BG_LIGHT_BRIGHTNESS;
	}
	else
	{
		set[COL_BG].r = BG_LIGHT_BRIGHTNESS, set[COL_BG].g = BG_LIGHT_BRIGHTNESS;
		set[COL_BG].b = BG_LIGHT_BRIGHTNESS, set[COL_FG].r = BG_DARK_BRIGHTNESS;
		set[COL_FG].g = BG_DARK_BRIGHTNESS, set[COL_FG].b = BG_DARK_BRIGHTNESS;
	}
	for (Uint8 i = 2; i < COLOURS_COUNT; ++i)
	{
		set[i].r = 0xFF;
		set[i].g = 0;
		set[i].b = 0;
	}
	return set;
}

SDL_Texture *GetScoreTexture(SDL_Renderer *rend, SDL_Texture *OldTexture, 
								SDL_Colour *ColourSet, SDL_Rect *Tile, Game *Game)
{
	if(!OldTexture)
		SDL_DestroyTexture(OldTexture);

	char text[42];
	sprintf(text, "Число очков:\n%lu\nРекорд:\n%lu", Game->Score, Game->MaxScore);
	return CreateMessageTexture(rend, ColourSet + COL_BG, 
							 ColourSet + COL_FG, Tile, FONT, text, SDL_FALSE);
}

SDL_Texture **CreateTextureSet(SDL_Renderer *rend, SDL_Colour *ColourSet, SDL_Point *WinSize, Game *Game)
{
	char text[40];
	Uint64 TileValue = 1;
	SDL_Colour txt_col = {0xFF, 0xFF, 0xFF, 0xFF};
	SDL_Texture **set = (SDL_Texture **)SDL_malloc(TEXTURES_COUNT * sizeof(SDL_Texture *));
	if (!set)
		return NULL;

	// Размер поля
	float FieldSize = FIELD_SIZE_COEFFICIENT * // Отношение размера поля к размеру экрана
					  MinOfTwo(WinSize->x, WinSize->y); // Меньший и размеров окон

	SDL_Rect Tile;
	Tile.w = Tile.h = TILE_SIZE_COEFFICIENT * FieldSize / Game->FieldSize;

	for (Uint8 i = TEX_SQ2; i < TEXTURES_COUNT; ++i)
	{
		TileValue <<= 1;
		sprintf(text, "%lu", TileValue);
		set[i] = CreateMessageTexture(rend, &txt_col, ColourSet + i + 1, &Tile, FONT, text, SDL_TRUE);
		if(!(set[i]))
		{	//Очистка всех остальных текстур
			SDL_DestroyTexture(set[i]);
			for(;i;i--)
				SDL_DestroyTexture(set[i - 1]);
			SDL_free(set);
			return NULL;
		}
	}

	//Создание текстуры числа очков
	Tile.w *= 2;
	if(!(set[TEX_SCORE] = GetScoreTexture(rend, NULL, ColourSet, &Tile, Game)))
	{	//Очистка всех остальных текстур
		for(Uint8 i = TEX_SCORE;i;i--)
			SDL_DestroyTexture(set[i - 1]);
		SDL_free(set);
		return NULL;
	}
	return set;
}

SDL_Texture **UpdateTextureSet(SDL_Renderer *rend, SDL_Texture **OldSet, 
							   SDL_Colour *ColourSet, SDL_Point *WinSize, Game *Game)
{
	/*Освобождение всех текстур*/
	for (Uint8 i = 0; i < TEXTURES_COUNT; ++i)
		SDL_DestroyTexture(OldSet[i]);

	//Освобождать сам массив из памяти на данном этапе не нужно,
	//так как он заменится новыми текстурами.
	//Его освобождение должно происходить только на выходе из программы
	return CreateTextureSet(rend, ColourSet, WinSize, Game);
}

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
	Params.textures = NULL;
	SDL_Window *window = NULL;
	SDL_Renderer *rend = NULL;
	Game.Score = 0;

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
	if ((errCode = CreateWorkspace(&window, &rend, title, &Params.WinSize, Params.Flags)))
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
		if(CheckForResize(window, &Params, &Events, WIN_MIN)) // Проверка на изменение размера
			Params.textures = UpdateTextureSet(rend, Params.textures, 
									  Params.cols, &Params.WinSize, &Game);

		switch (Params.Mode)
		{
		case MODE_QUIT:
			return SilentLeaveWithCode(errCode, window, rend, &Game, &Params);

		case MODE_WAIT:
			continue;

		case MODE_ADD:
			NewElementIndex = AddElement(&Game);
			Params.Mode = (NewElementIndex < 0) ? MODE_QUIT : MODE_DRAW;
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
				// DrawOldElements(rend, &Params, &Game);
				Params.Mode = MODE_WAIT;
			}
			break;

		default:
			break;
		}
	}
}
