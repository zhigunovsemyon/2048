#include "fileio.h"

typedef struct
{
	Tile* Field;	//Поле ячеек
	Sint8 FieldSize;//Размер поля
	Sint64 Score;	//Текущий счёт
	Sint64 MaxScore;//Максимальный счёт
	Sint8 Mode;		//Текущий этап игры
}
_inGame;

int_fast8_t ReadFile(const char *filename, Game *game)
{	/* Попытка открыть существующий файл с сохранением,
	Если не удалось --
	 * игра начинается по новому*/
	SDL_RWops *fptr = SDL_RWFromFile(filename, "rb");
	if (!fptr)	
		return 1;

	_inGame localGame;

	//Сохранение старого размера поля для сравнения с прочитанным
	int_fast8_t BaseFieldSize = game->FieldSize;
	Tile *oldFieldPtr = game->Field;

	// Чтение прошлого режима
	SDL_RWread(fptr, &localGame, sizeof(localGame), 1);
	
	//Копирование в нужную структуру с быстрыми интами
	game->Field = localGame.Field;
	game->FieldSize = localGame.FieldSize;
	game->Mode = localGame.Mode;
	game->Score = localGame.Score;
	game->MaxScore = localGame.MaxScore;

	game->Field = oldFieldPtr;
	
	/* Если прочитанный размер не бьётся с заданным параметрами :
	файл закрывается, возвращается признак новой игры*/
	if (BaseFieldSize != game->FieldSize)
	{
		SDL_RWclose(fptr);
		return 1;		
	}
	
	/*Если в прочитанном файле сохранён режим MODE_GAMEOVER, 
	значит игровое поле нерелевантно, в противном случае необходимо
	скопировать поле в память*/
	if (game->Mode == MODE_GAMEOVER)
	{
		game->Score = 0;		
		game->Field[AddElement(game)].mode = TILE_OLD;
		game->Mode = MODE_ADD;
	}
	else
	{
		SDL_RWread(fptr, game->Field, sizeof(Tile), (size_t)_SQ(game->FieldSize));
		game->Mode = MODE_DRAW_NEW;
	}

	SDL_RWclose(fptr);
	return 0;
}

int_fast8_t SaveGame(Game *game, const char *filename)
{ // Открытие файла, в который будет записан прогресс
	SDL_RWops *fptr = SDL_RWFromFile(filename, "wb");
	if (!fptr)
	{
		SDL_SetError("Не удалось сохранить в файл!");
		return ERR_FILE;
	}

	//Очистка поля, если игра завершилась геймовером
	if(game->Mode == MODE_GAMEOVER)
		SDL_memset(game->Field, 0, (size_t)_SQ(game->FieldSize));

	_inGame localGame;

	//Копирование из структуру с быстрыми интами
	localGame.Field	= game->Field;
	localGame.FieldSize = game->FieldSize;
	localGame.Mode = game->Mode;
	localGame.Score = game->Score;
	localGame.MaxScore = game->MaxScore;

	// Сохранение игры
	SDL_RWwrite(fptr, &localGame, sizeof(_inGame), 1);
	SDL_RWwrite(fptr, game->Field, sizeof(Tile), (size_t)_SQ(game->FieldSize));

	// Закрытие файла
	SDL_RWclose(fptr);
	return ERR_NO;
}

