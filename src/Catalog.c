#include "App.h"
#include <string.h>

static const int cols_width[] = {
	37,
	20,
	15,
	8,
};

static void Catalog_AddGameToWishList(App *app, TableManagerState *state);
static void Catalog_DrawAddedGame(App *app, TableManagerState *state);
static void Catalog_UpdateTable(App *app, TableManagerState *state);
static void Catalog_DeleteItem(App *app, TableManagerState *state);
static void Catalog_HandleInputNav(App *app, TableManagerState *state);

static void Catalog_AddGameToWishList(App *app, TableManagerState *state) {
	DB_InsertWish(
			app->db,
			TableWidgetData_GetSelectedRow(&state->table_data)[0]
			);

	Catalog_DrawAddedGame(app, state);
}

static void Catalog_DrawAddedGame(App *app, TableManagerState *state) {
	WINDOW *win = app->bottom_window;

	wmove(win, 0, 0);
	Style_SetAttr(win, STYLE_SUCCESS_MSG);
	Widget_Label(win, " Added ", -1);
	Widget_Label(
			win,
			TableWidgetData_GetSelectedRow(&state->table_data)[0],
			-1
			);
	Widget_Label(win, " to WishList!", -1);
	Widget_Space(win, -1);
}

static void Catalog_UpdateTable(App *app, TableManagerState *state) {
	DB_SearchGame(
			app->db,
			&state->table_data.working_table,
			state->search_data.str
			);
}

static void Catalog_DeleteItem(App *app, TableManagerState *state) {
	DB_DeleteGame(
			app->db,
			TableWidgetData_GetSelectedRow(&state->table_data)[0]
			);
}

static void Catalog_HandleInputNav(App *app, TableManagerState *state) {
	Table *table = &state->table_data.working_table;
	bool is_table_valid = table->nrow > 1;

	switch(app->ch) {
		case 'a':
			app->next_state = STATE_ADDGAME;
			break;

		case 'i':
			if(!is_table_valid) break;

			AddLog_SetGame(app, TableWidgetData_GetSelectedRow(&state->table_data)[0]);
			app->next_state = STATE_ADDLOG;
			break;

		case 'W':
			if(!is_table_valid) break;

			Catalog_AddGameToWishList(app, state);
			break;

		case 'c':
		case 'r':
			if(!is_table_valid) break;

			app->next_state = STATE_UPDATEGAME;

			AddGame_SetUpdateData(
					app,
					TableWidgetData_GetSelectedRow(&state->table_data)
					);
			break;
	}
}

void Catalog_SetGameSelected(App *app, const char *game) {
	TableManagerState *state = app->states[STATE_CATALOG];
	Catalog_UpdateTable(app, state);

	Table *table = &state->table_data.working_table;

	for(int i = 0; i < table->nrow; i++) {
		const char *name = table->str[i * table->ncol];

		if(name == NULL) continue;

		if(strcmp(name, game) == 0) {
			state->table_data.selected_row = i;
			break;
		}
	}
}

void Catalog_Init(App *app) {
	TableManagerState *state = app->states[STATE_CATALOG];

	state->table_data.msg_no_entries = "  There are no items under this category.";
	state->table_data.col_width_values = cols_width;

	state->input_nav_callback = Catalog_HandleInputNav;
	state->delete_item_callback = Catalog_DeleteItem;
	state->update_table_callback = Catalog_UpdateTable;

	TableManager_Init(app, state);
}

void Catalog_Update(App *app) {
	TableManager_Update(app, app->states[STATE_CATALOG]);
}
