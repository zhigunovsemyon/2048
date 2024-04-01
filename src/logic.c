#ifndef _LOGIC_H_
#define _LOGIC_H_

#include "logic.h"

void GameInit(int argc, const char **argv, Controls *Controls)
{
	//Если игра была запущена без флагов, 
	//то используется стандартная раскладка
	if (argc == 1)
	{
		Controls->left	= SDL_SCANCODE_LEFT;
		Controls->right = SDL_SCANCODE_RIGHT;
		Controls->up	= SDL_SCANCODE_UP;
		Controls->down	= SDL_SCANCODE_DOWN;
		Controls->quit	= SDL_SCANCODE_ESCAPE;
		Controls->mouse = SDL_TRUE;

		SDL_Log("Для управления используются стрелки, \n%s%s",
		  "либо при помощи нажатий по соответствующим областям поля\n",
		  "для выхода нажмите Escape\n");
		return;
	}
	
}
#endif /* ifndef _LOGIC_H_ */
