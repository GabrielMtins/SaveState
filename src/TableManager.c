#include "App.h"

typedef enum {
	MANAGER_INPUT_NAV,
	MANAGER_INPUT_SEARCH,
	MANAGER_INPUT_DELETE,
} TableManagerInputState;

static void TableManager_UpdateTable(App *app, TableManagerState *state);
static void TableManager_DeleteItem(App *app, TableManagerState *state);
static void TableManager_HandleInputNav(App *app, TableManagerState *state);
static void TableManager_HandleInputSearch(App *app, TableManagerState *state);
static void TableManager_HandleInputDeleteConfirm(App *app, TableManagerState *state);
static void TableManager_HandleInput(App *app, TableManagerState *state);
static void TableManager_DrawFilterBar(App *app, TableManagerState *state);
static void TableManager_DrawDeleteConfirmation(App *app);
static void TableManager_DrawSearch(App *app, TableManagerState *state);
static void TableManager_DrawTable(App *app, TableManagerState *state);

static void TableManager_UpdateTable(App *app, TableManagerState *state) {
	if(state->update_table_callback != NULL) {
		state->update_table_callback(app, state);
	}
}

static void TableManager_DeleteItem(App *app, TableManagerState *state) {
	if(state->delete_item_callback != NULL) {
		state->delete_item_callback(app, state);
	}
}

static void TableManager_HandleInputNav(App *app, TableManagerState *state) {
	Table *table = &state->table_data.working_table;
	bool is_table_valid = table->nrow > 1;

	if(state->input_nav_callback != NULL) {
		state->input_nav_callback(app, state);
	}

	switch(app->ch) {
		case 'd':
			if(!is_table_valid) break;

			state->input_state = MANAGER_INPUT_DELETE;
			TableManager_DrawDeleteConfirmation(app);
			break;

		case 'f':
			LineWidgetData_Init(&state->search_data);
			state->input_state = MANAGER_INPUT_SEARCH;
			TableManager_DrawSearch(app, state);
			break;

		case KEY_ESCAPE:
			App_ClearBottomWindow(app);
			LineWidgetData_Init(&state->search_data);
			TableManager_UpdateTable(app, state);
			break;
	}

	TableWidgetData_HandleInput(&state->table_data, app->ch, app->prev_ch);
}

static void TableManager_HandleInputSearch(App *app, TableManagerState *state) {
	app->block_menubar = true;
	LineWidgetData_HandleInput(&state->search_data, app->ch);
	TableManager_DrawSearch(app, state);

	switch(app->ch) {
		case KEY_ESCAPE:
			App_ClearBottomWindow(app);
			LineWidgetData_Init(&state->search_data);
			state->input_state = MANAGER_INPUT_NAV;
			break;

		case '\n':
			App_ClearBottomWindow(app);
			TableManager_DrawFilterBar(app, state);
			state->input_state = MANAGER_INPUT_NAV;
			TableManager_UpdateTable(app, state);
			break;
	}
}

static void TableManager_HandleInputDeleteConfirm(App *app, TableManagerState *state) {
	switch(app->ch) {
		case 'n':
		case KEY_ESCAPE:
			App_ClearBottomWindow(app);
			state->input_state = MANAGER_INPUT_NAV;
			break;

		case 'y':
		case '\n':
			App_ClearBottomWindow(app);
			state->input_state = MANAGER_INPUT_NAV;
			state->input_state = MANAGER_INPUT_NAV;

			TableManager_DeleteItem(app, state);
			TableManager_UpdateTable(app, state);
			break;
	}
}

static void TableManager_HandleInput(App *app, TableManagerState *state) {
	state->table_data.height = getmaxy(app->content_window);

	switch(state->input_state) {
		case MANAGER_INPUT_NAV:
			TableManager_HandleInputNav(app, state);
			break;

		case MANAGER_INPUT_DELETE:
			TableManager_HandleInputDeleteConfirm(app, state);
			break;

		case MANAGER_INPUT_SEARCH:
			TableManager_HandleInputSearch(app, state);
			break;
	}

	TableWidgetData_ApplyLimits(&state->table_data);
}

static void TableManager_DrawFilterBar(App *app, TableManagerState *state) {
	WINDOW *win = app->bottom_window;

	if(state->search_data.len == 0) {
		return;
	}

	wmove(win, 0, 0);
	Style_SetAttr(win, STYLE_FILTER_MSG);
	Widget_Label(win, " Filtering by: ", -1);
	Widget_Label(win, state->search_data.str, -1);
	Widget_Space(win, -1);
}

static void TableManager_DrawDeleteConfirmation(App *app) {
	WINDOW *win = app->bottom_window;
	
	wmove(win, 0, 0);
	Style_SetAttr(win, STYLE_CONFIRM_MSG);
	Widget_Label(win, " Delete item? Y/n", -1);
	Widget_Space(win, -1);
}

static void TableManager_DrawSearch(App *app, TableManagerState *state) {
	WINDOW *win = app->bottom_window;
	
	wmove(win, 0, 0);
	Style_SetAttr(win, STYLE_SEARCH_BG);
	Widget_Label(win, " Search: ", -1);

	state->search_data.width = getmaxx(win) - getcurx(win);
	Widget_InputLine(win, &state->search_data);
}

static void TableManager_DrawTable(App *app, TableManagerState *state) {
	WINDOW *win = app->content_window;
	
	wmove(win, 0, 0);
	Widget_Table(win, &state->table_data);
}

void TableManager_Init(App *app, TableManagerState *state) {
	state->input_state = MANAGER_INPUT_NAV;
	
	state->search_data.bg_color = STYLE_SEARCH_BG;
	state->search_data.cursor_color = STYLE_SEARCH_CURSOR;

	TableManager_UpdateTable(app, state);
}

void TableManager_Update(App *app, TableManagerState *state) {
	TableManager_HandleInput(app, state);

	TableManager_DrawTable(app, state);
}
