#ifndef _LOGIC_H_
#define _LOGIC_H_

#include "logic.h"

Uint8 LaunchOptions(int argc, const char **argv, Controls *Controls)
{
	//Базовые параметры работы игры
	Uint8 FieldSize = 4;
	Controls->left	= SDL_SCANCODE_LEFT;
	Controls->right = SDL_SCANCODE_RIGHT;
	Controls->up	= SDL_SCANCODE_UP;
	Controls->down	= SDL_SCANCODE_DOWN;
	Controls->quit	= SDL_SCANCODE_ESCAPE;
	Controls->mouse = SDL_TRUE;

	//Если игра была запущена без флагов, 
	//то используется стандартная раскладка
	Uint8 key_unset = 1, mouse_unset = 1, size_unset = 1;
	if (argc == 1)
	{
		key_unset	= 0; 
		mouse_unset = 0;
		size_unset	= 0;
	}

	/*Перебор аргументов, с которыми была запущена игра. Если их не было,
	 * цикл ниже будет пропущен*/
	for (Uint8 i = 1; (key_unset || mouse_unset || size_unset) && (i < argc); ++i)
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
			Controls->mouse = SDL_FALSE;
			mouse_unset = 0;
			continue;
		}

		if (!SDL_strcmp(argv[i],"--mouse") && mouse_unset) 
		{
			mouse_unset = 0;
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
			Controls->left	= SDL_SCANCODE_A;
			Controls->right = SDL_SCANCODE_D;
			Controls->up	= SDL_SCANCODE_W;
			Controls->down	= SDL_SCANCODE_S;
			continue;
		}

		if (!SDL_strcmp(argv[i], "--vi") && key_unset)
		{
			key_unset = 0;
			Controls->left	= SDL_SCANCODE_H;
			Controls->right = SDL_SCANCODE_L;
			Controls->up	= SDL_SCANCODE_K;
			Controls->down	= SDL_SCANCODE_J;
			continue;
		}
	}
	return FieldSize;
}
#endif /* ifndef _LOGIC_H_ */
