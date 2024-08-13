#ifndef FILEIO_H_
#define FILEIO_H_

#include "main.h"

/*Сохранение игрового процесса game в файл finename*/
int_fast8_t SaveGame(Game *game, const char *filename);

int_fast8_t ReadFile(const char *filename, Game *game);

#endif  // INCLUDE/home/tuzgr/2048/srcfileiofileio.h_
