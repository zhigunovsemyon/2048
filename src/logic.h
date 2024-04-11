#ifndef LOGIC_H
#define LOGIC_H
#include "main.h"

enum SetFlags
{
	KEY_UNSET	= 1,
	COL_UNSET	= 2,
	MOUSE_UNSET = 4,	
	SIZE_UNSET	= 8
};

/* На основе параметров запуска игры argv, в количестве argc, подбирается схема управления
 * и размер игрового поля. По умолчанию используются стрелки с мышью, поле имеет  размер 4x4*/
Uint8 LaunchOptions(int argc, const char **argv, Settings *Settings);

//Вычисление меньшего из двух чисел
int MinOfTwo(int,int);

//Вычисление большего из двух чисел
int MaxOfTwo(int,int);

#endif // !LOGIC_H
