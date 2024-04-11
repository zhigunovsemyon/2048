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
	Settings->Dark	= SDL_TRUE;

	//Если игра была запущена без флагов, 
	//то используется стандартная раскладка
	Uint8 key_unset = 1, mouse_unset = 1, size_unset = 1, col_unset = 1;
	if (argc == 1)
	{
		key_unset	= 0; 
		mouse_unset = 0;
		col_unset	= 0;
		size_unset	= 0;
	}

	/*Перебор аргументов, с которыми была запущена игра. Если их не было,
	 * цикл ниже будет пропущен*/
	for (Uint8 i = 1; (col_unset || key_unset || mouse_unset || size_unset) && (i < argc); ++i)
	{
		if (!SDL_strcmp(argv[i],"--size=3") && size_unset)
		{
			size_unset = 0;
			continue;
		}

		if (!SDL_strcmp(argv[i],"--size=4") && size_unset)
		{
			size_unset = 0;
			FieldSize = 4;
			continue;
		}

		if (!SDL_strcmp(argv[i],"--size=5") && size_unset)
		{
			size_unset = 0;
			FieldSize = 5;
			continue;
		}

		if (!SDL_strcmp(argv[i],"--nomouse") && mouse_unset)
		{
			Settings->mouse = SDL_FALSE;
			mouse_unset = 0;
			continue;
		}

		if (!SDL_strcmp(argv[i],"--mouse") && mouse_unset) 
		{
			mouse_unset = 0;
			continue;
		}

		if(!SDL_strcmp(argv[i], "--light") && col_unset)
		{
			col_unset = 0;
			Settings->Dark = SDL_FALSE;
			continue;
		}


		if(!SDL_strcmp(argv[i], "--dark") && col_unset)
		{
			col_unset = 0;
			continue;
		}

		if(!SDL_strcmp(argv[i], "--arrows") && key_unset)
		{
			key_unset = 0;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--wasd") && key_unset)
		{
			key_unset = 0;
			Settings->left	= SDL_SCANCODE_A;
			Settings->right = SDL_SCANCODE_D;
			Settings->up	= SDL_SCANCODE_W;
			Settings->down	= SDL_SCANCODE_S;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--vi") && key_unset)
		{
			key_unset = 0;
			Settings->left	= SDL_SCANCODE_H;
			Settings->right = SDL_SCANCODE_L;
			Settings->up	= SDL_SCANCODE_K;
			Settings->down	= SDL_SCANCODE_J;
			continue;
		}
	}
	return FieldSize;
}
