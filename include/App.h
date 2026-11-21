#ifndef APP_H
#define APP_H

#include "Widgets.h"
#include "Style.h"
#include <sqlite3.h>

#include <stdlib.h>
#include <string.h>

#define APP_CFG_DIR "%s/.savestate"
#define APP_CFG_FILENAME APP_CFG_DIR "/config.ini"
#define APP_DATABASE_FILENAME APP_CFG_DIR "/data.db"
#define APP_BACKUP_FILENAME APP_CFG_DIR "/backup.db"

#define KEY_ESCAPE 27

#define FOR_STATE_LIST(DO) \
	DO(STATE_CATALOG,    TableManagerState, Catalog_Init,      Catalog_Update ) \
	DO(STATE_LOGVIEW,    TableManagerState, LogView_Init,      LogView_Update ) \
	DO(STATE_WISHLIST,   TableManagerState, WishList_Init,     WishList_Update) \
	DO(STATE_ADDGAME,    AddGameState,      AddGame_Init,      AddGame_Update ) \
	DO(STATE_UPDATEGAME, int,               AddGame_Init,      AddGame_Update ) \
	DO(STATE_ADDLOG,     AddLogState,       AddLog_Init,       AddLog_Update  ) \
	DO(STATE_UPDATELOG,  int,               AddLog_Init,       AddLog_Update  ) \
	DO(STATE_STATS,      StatsState,        State_NullInit,    Stats_Update   ) \
	DO(STATE_MENUBAR,    MenubarState,      State_NullInit,    Menubar_Update ) \
	DO(STATE_ABOUT,      int,               State_NullInit,    About_Update   ) \
	DO(STATE_LOGO,       int,               State_NullInit,    Logo_Update    )

#define LIMIT_VAR(x, min_x, max_x) (x = (x < (min_x)) ? (min_x) : ((x >= (max_x)) ? (max_x - 1) : x))

typedef struct App App;

typedef struct TableManagerState {
	int input_state;

	TableWidgetData table_data;
	LineWidgetData search_data;

	void (*update_table_callback)(App *, struct TableManagerState *);
	void (*delete_item_callback)(App *, struct TableManagerState *);
	void (*input_nav_callback)(App *, struct TableManagerState *);
} TableManagerState;

typedef struct MenubarState {
	int button_selected;
} MenubarState;

typedef struct AddGameState {
	FormWidgetData form_data;

	char update_game[MAX_LINE_SIZE + 1];

	const char *error_msg;
} AddGameState;

#define MAX_FORM_TITLE_LEN (300)
typedef struct AddLogState {
	FormWidgetData form_data;

	char form_title_data[MAX_FORM_TITLE_LEN + 1];
	char chosen_game[MAX_LINE_SIZE + 1];

	const char *error_msg;
} AddLogState;

typedef struct StatsState {
	int current_graph;
	int current_year;
	int input_state;

	Table platform_data;
	Table genre_data;
	Table decade_data;

	LineWidgetData search_data;

	bool redraw_graph;
} StatsState;

void App_Config(App *app);

void App_Init(App *app);

void App_Run(App *app);

void App_End(App *app);

void App_ClearBottomWindow(App *app);

bool App_IsNewState(App *app);

void Catalog_SetGameSelected(App *app, const char *game);

void AddGame_SetUpdateData(App *app, char **data);

void AddLog_SetUpdateData(App *app, char **data);

void AddLog_SetGame(App *app, const char *str);

void TableManager_Init(App *app, TableManagerState *state);

void TableManager_Update(App *app, TableManagerState *state);

/* ============= MACROS EXPANDING ============= */

#define EXPAND_AS_FUNCTION(type, state, init, update) void init(App *app); void update(App *app);
FOR_STATE_LIST(EXPAND_AS_FUNCTION)
#undef EXPAND_AS_FUNCTION

#define EXPAND_AS_ENUM(type, ...) type,
typedef enum {
	FOR_STATE_LIST(EXPAND_AS_ENUM)
	NUM_STATES,
	STATE_NONE
} StateType;
#undef EXPAND_AS_ENUM

struct App {
	WINDOW *content_window;
	WINDOW *top_window;
	WINDOW *bottom_window;
	WINDOW *logo_window;

	void *states[NUM_STATES];

	sqlite3 *db;

	int prev_state;
	int current_state;
	int next_state;

	int quit;
	int prev_ch;
	int ch;

	bool block_menubar;
};

#endif
