#include "logic.h"

int MinOfTwo(int a, int b)
{
	return (a > b) ? b : a; 
}

int MaxOfTwo(int a, int b)
{
	return (a < b) ? b : a; 
}

Uint8 LaunchOptions(int argc, const char **argv, Settings *Settings)
{
	//Базовые параметры работы игры
	Uint8 FieldSize = 4;
	Settings->left	= SDL_SCANCODE_LEFT;
	Settings->right = SDL_SCANCODE_RIGHT;
	Settings->up	= SDL_SCANCODE_UP;
	Settings->down	= SDL_SCANCODE_DOWN;
	Settings->quit	= SDL_SCANCODE_ESCAPE;
	Settings->mouse = SDL_TRUE;
	Settings->Flags	= 1;

	//Если игра была запущена без флагов, 
	//то используется стандартная раскладка
	Uint8 Setters = (argc != 1) ? 15 : 0;

	/*Перебор аргументов, с которыми была запущена игра. Если их не было,
	 * цикл ниже будет пропущен*/
	for (Uint8 i = 1; Setters && (i < argc); ++i)
	{
		if (!SDL_strcmp(argv[i],"--size=3") && (Setters & SIZE_UNSET))
		{
			Setters &= ~SIZE_UNSET;
			continue;
		}

		if (!SDL_strcmp(argv[i],"--size=4") && (Setters & SIZE_UNSET))
		{
			Setters &= ~SIZE_UNSET;
			FieldSize = 4;
			continue;
		}

		if (!SDL_strcmp(argv[i],"--size=5") && (Setters & SIZE_UNSET))
		{
			Setters &= ~SIZE_UNSET;
			FieldSize = 5;
			continue;
		}

		if (!SDL_strcmp(argv[i],"--nomouse") && (Setters & MOUSE_UNSET))
		{
			Settings->mouse = SDL_FALSE;
			Setters &= ~MOUSE_UNSET;
			continue;
		}

		if (!SDL_strcmp(argv[i],"--mouse") && (Setters & MOUSE_UNSET)) 
		{
			Setters &= ~MOUSE_UNSET;
			continue;
		}

		if(!SDL_strcmp(argv[i], "--light") && (Setters & COL_UNSET))
		{
			Setters &= ~COL_UNSET;
			Settings->Flags &= ~1;
			continue;
		}


		if(!SDL_strcmp(argv[i], "--dark") && (Setters & COL_UNSET))
		{
			Setters &= ~COL_UNSET;
			continue;
		}

		if(!SDL_strcmp(argv[i], "--arrows") && (Setters & KEY_UNSET))
		{
			Setters &= ~KEY_UNSET;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--wasd") && (Setters & KEY_UNSET))
		{
			Setters &= ~KEY_UNSET;
			Settings->left	= SDL_SCANCODE_A;
			Settings->right = SDL_SCANCODE_D;
			Settings->up	= SDL_SCANCODE_W;
			Settings->down	= SDL_SCANCODE_S;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--vi") && (Setters & KEY_UNSET))
		{
			Setters &= ~KEY_UNSET;
			Settings->left	= SDL_SCANCODE_H;
			Settings->right = SDL_SCANCODE_L;
			Settings->up	= SDL_SCANCODE_K;
			Settings->down	= SDL_SCANCODE_J;
			continue;
		}
	}
	return FieldSize;
}
