#ifndef DBCMD_H
#define DBCMD_H

#include <sqlite3.h>

typedef struct Table {
	char **str;
	int ncol, nrow;
} Table;

void DB_InsertTestGames(sqlite3 *db, int num_games);

sqlite3 * DB_Connect(const char *filename, const char *backup);

void DB_Quit(sqlite3 *db);

void DB_DestroyTable(Table *table);

void DB_InsertGame(sqlite3 *db, const char *name, const char *genre, const char *platform, int year);

void DB_DeleteGame(sqlite3 *db, const char *name);

void DB_UpdateGame(sqlite3 *db, const char *old_name, const char *name, const char *genre, const char *platform, int year);

void DB_GetGameTable(sqlite3 *db, Table *table);

void DB_SearchGame(sqlite3 *db, Table *table, const char *name);

void DB_InsertLog(sqlite3 *db, const char *name, const char *start_date, const char *end_date, int score);

void DB_DeleteLog(sqlite3 *db, const char *id);

void DB_UpdateLog(sqlite3 *db, const char *start_date, const char *end_date, int score, const char *id);

void DB_GetLogTable(sqlite3 *db, Table *table);

void DB_SearchLog(sqlite3 *db, Table *table, const char *name);

void DB_InsertWish(sqlite3 *db, const char *name);

void DB_DeleteWish(sqlite3 *db, const char *id);

void DB_GetWishTable(sqlite3 *db, Table *table);

void DB_SearchWish(sqlite3 *db, Table *table, const char *name);

void DB_GetStats(sqlite3 *db, Table *table, const char *column, int year);

#endif
