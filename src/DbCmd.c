#include "DbCmd.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

static const char *sql_cmd_create_tables = 
"CREATE TABLE IF NOT EXISTS GameTable ("
"id INTEGER PRIMARY KEY,"
"Name TEXT UNIQUE NOT NULL COLLATE NOCASE,"
"Genre TEXT COLLATE NOCASE,"
"Platform TEXT COLLATE NOCASE,"
"Year INTEGER"
");"

"CREATE TABLE IF NOT EXISTS LogTable ("
"id INTEGER PRIMARY KEY,"
"GameId INTEGER NOT NULL,"
"StartDate DATE,"
"EndDate DATE,"
"Score INTEGER,"
"FOREIGN KEY (GameId) REFERENCES GameTable(id) "
"ON DELETE CASCADE "
"ON UPDATE CASCADE"
");"
;

static const char *sql_cmd_insert_game = 
"INSERT INTO GameTable (Name, Genre, Platform, Year) VALUES (%Q, %Q, %Q, %d);"
;

static const char *sql_cmd_delete_game =
"DELETE FROM GameTable WHERE Name=%Q;"
;

static const char *sql_cmd_update_game =
"UPDATE GameTable "
"SET Name=%Q, Genre=%Q, Platform=%Q, Year=%d "
"WHERE Name=%Q;"
;

static const char *sql_cmd_get_game_table = 
"SELECT Name, Genre, Platform, Year FROM GameTable ORDER BY Name ASC;"
;

static const char *sql_cmd_search_game = 
"SELECT Name, Genre, Platform, Year FROM GameTable "
"WHERE Name LIKE %Q OR Genre LIKE %Q OR Platform LIKE %Q OR Year Like %Q "
"ORDER BY Name ASC;"
;

static const char *sql_cmd_insert_log =
"INSERT INTO LogTable (GameId, StartDate, EndDate, Score) VALUES "
"((SELECT id FROM GameTable Where Name=%Q),"
"%Q, %Q, %d);"
;

static const char *sql_cmd_delete_log =
"DELETE FROM LogTable WHERE id=%s;"
;

static const char *sql_cmd_update_log =
"UPDATE LogTable "
"SET StartDate=%Q, EndDate=%Q, Score=%d "
"WHERE id=%s;"
;

static const char *sql_cmd_get_log_table =
"SELECT "
"l.id as id, "
"g.Name AS Name," 
"strftime('%%d/%%m/%%Y', l.StartDate) AS [Start Date],"
"strftime('%%d/%%m/%%Y', l.EndDate) AS [End Date],"
"l.Score "
"FROM GameTable g "
"INNER JOIN LogTable l on g.id = l.GameId "
"ORDER BY l.StartDate DESC;"
;

static const char *sql_cmd_search_log =
"SELECT "
"l.id as id, "
"g.Name AS Name," 
"strftime('%%d/%%m/%%Y', l.StartDate) AS [Start Date],"
"strftime('%%d/%%m/%%Y', l.EndDate) AS [End Date],"
"l.Score "
"FROM GameTable g "
"INNER JOIN LogTable l on g.id = l.GameId "
"WHERE g.Name LIKE %Q "
"ORDER BY l.StartDate DESC;"
;

static const char *sql_cmd_count_stats =
"SELECT %s as [CurrentStatCount], COUNT(*) as qt "
"FROM (SELECT DISTINCT GameId FROM %s) l "
"INNER JOIN GameTable g on g.id = l.GameId "
"WHERE CurrentStatCount IS NOT NULL "
"GROUP BY CurrentStatCount "
"ORDER BY qt DESC;"
;

static char * Util_ConvertDateToSqlFmt(const char *date);
static char * Util_GetLogTableYear(int year);
static void DB_Exec(sqlite3 *db, const char *fmt, ...);
static void DB_GetTable(sqlite3 *db, Table *table, const char *fmt, ...);

static char * Util_ConvertDateToSqlFmt(const char *date) {
	int day = 0, month = 0, year = 0;
	sscanf(date, "%d/%d/%d", &day, &month, &year);

	return sqlite3_mprintf("%04d-%02d-%02d", year, month, day);
}

static char * Util_GetLogTableYear(int year) {
	if(year <= 0) {
		return sqlite3_mprintf("LogTable WHERE date(EndDate) IS NOT NULL");
	}

	return sqlite3_mprintf(
			"LogTable WHERE StartDate >= '%04d-01-01' AND StartDate < '%04d-01-01' AND date(EndDate) IS NOT NULL",
			year,
			year + 1
			);
}

static void DB_Exec(sqlite3 *db, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	char *cmd = sqlite3_vmprintf(fmt, args);

	sqlite3_exec(db, cmd, NULL, NULL, NULL);

	sqlite3_free(cmd);

	va_end(args);
}

static void DB_GetTable(sqlite3 *db, Table *table, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	char *cmd = sqlite3_vmprintf(fmt, args);

	if(table->str != NULL) DB_DestroyTable(table);

	sqlite3_get_table(
			db,
			cmd,
			&table->str,
			&table->nrow,
			&table->ncol,
			NULL
			);

	table->nrow++;
}

void DB_InsertTestGames(sqlite3 *db, int num_games) {
	for(int i = 0; i < num_games; i++) {
		char *name = sqlite3_mprintf("game%02d", i);
		char *plat = sqlite3_mprintf("plat%02d", i / 5);
		
		DB_InsertGame(db, name, NULL, plat, i + 2000);
		
		sqlite3_free(name);
		sqlite3_free(plat);
	}

	for(int i = 0; i < 5; i++) {
		//DB_InsertLog(db, "game00", "20/10/2023", NULL, 10);
		DB_InsertLog(db, "game00", "20/10/2023", "", 10);
	}
}

sqlite3 * DB_Connect(const char *filename, const char *backup) {
	sqlite3 *db;

	sqlite3_open(filename, &db);
	DB_Exec(db, "VACUUM INTO %Q", backup);
	DB_Exec(db, "PRAGMA foreign_keys = ON;");
	DB_Exec(db, sql_cmd_create_tables);

	return db;
}

void DB_Quit(sqlite3 *db) {
	sqlite3_close(db);
}

void DB_DestroyTable(Table *table) {
	if(table->str != NULL) {
		sqlite3_free_table(table->str);
	}
}

void DB_InsertGame(sqlite3 *db, const char *name, const char *genre, const char *platform, int year) {
	DB_Exec(
			db,
			sql_cmd_insert_game,
			name,
			genre,
			platform,
			year
		   );
}

void DB_DeleteGame(sqlite3 *db, const char *name) {
	DB_Exec(
			db,
			sql_cmd_delete_game,
			name
		   );
}

void DB_UpdateGame(sqlite3 *db, const char *old_name, const char *name, const char *genre, const char *platform, int year) {
	DB_Exec(
			db,
			sql_cmd_update_game,
			name,
			genre,
			platform,
			year,
			old_name
		   );
}

void DB_GetGameTable(sqlite3 *db, Table *table) {
	DB_GetTable(
			db,
			table,
			sql_cmd_get_game_table
			);
}

void DB_SearchGame(sqlite3 *db, Table *table, const char *name) {
	if(name == NULL || name[0] == '\0') {
		DB_GetGameTable(db, table);
		return;
	}

	char *like_str = sqlite3_mprintf("%%%s%%", name);

	DB_GetTable(
			db,
			table,
			sql_cmd_search_game,
			like_str,
			like_str,
			like_str,
			like_str
			);

	sqlite3_free(like_str);
}

void DB_InsertLog(sqlite3 *db, const char *name, const char *start_date, const char *end_date, int score) {
	char *conv_start_date = Util_ConvertDateToSqlFmt(start_date);
	char *conv_end_date = Util_ConvertDateToSqlFmt(end_date);

	DB_Exec(
			db,
			sql_cmd_insert_log,
			name,
			conv_start_date,
			conv_end_date,
			score
		   );
	
	sqlite3_free(conv_start_date);
	sqlite3_free(conv_end_date);
}

void DB_DeleteLog(sqlite3 *db, const char *id) {
	DB_Exec(
			db,
			sql_cmd_delete_log,
			id
		   );
}

void DB_UpdateLog(sqlite3 *db, const char *start_date, const char *end_date, int score, const char *id) {
	char *conv_start_date = Util_ConvertDateToSqlFmt(start_date);
	char *conv_end_date = Util_ConvertDateToSqlFmt(end_date);

	DB_Exec(
			db,
			sql_cmd_update_log,
			conv_start_date,
			conv_end_date,
			score,
			id
		   );

	sqlite3_free(conv_start_date);
	sqlite3_free(conv_end_date);
}

void DB_GetLogTable(sqlite3 *db, Table *table) {
	DB_GetTable(
			db,
			table,
			sql_cmd_get_log_table
			);
}

void DB_SearchLog(sqlite3 *db, Table *table, const char *name) {
	if(name == NULL || name[0] == '\0') {
		DB_GetLogTable(db, table);
		return;
	}

	char *tmp = sqlite3_mprintf("%%%s%%", name);

	DB_GetTable(
			db,
			table,
			sql_cmd_search_log,
			tmp
			);

	sqlite3_free(tmp);
}

void DB_GetStats(sqlite3 *db, Table *table, const char *column, int year) {
	char *tmp = Util_GetLogTableYear(year);

	DB_GetTable(
			db,
			table,
			sql_cmd_count_stats,
			column,
			tmp
			);

	sqlite3_free(tmp);
}
