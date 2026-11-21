#include "App.h"

static const int cols_width[] = {
	0,
	37,
	20,
	15,
	8,
};

static void WishList_UpdateTable(App *app, TableManagerState *state);
static void WishList_DeleteItem(App *app, TableManagerState *state);

static void WishList_UpdateTable(App *app, TableManagerState *state) {
	DB_SearchWish(
			app->db,
			&state->table_data.working_table,
			state->search_data.str
			);
}

static void WishList_DeleteItem(App *app, TableManagerState *state) {
	DB_DeleteWish(
			app->db,
			TableWidgetData_GetSelectedRow(&state->table_data)[0]
			);
}

void WishList_Init(App *app) {
	TableManagerState *state = app->states[STATE_WISHLIST];

	state->table_data.msg_no_entries = "  There are no items under this category.";
	state->table_data.col_width_values = cols_width;

	state->input_nav_callback = NULL;
	state->update_table_callback = WishList_UpdateTable;
	state->delete_item_callback = WishList_DeleteItem;

	TableManager_Init(app, state);
}

void WishList_Update(App *app) {
	TableManager_Update(app, app->states[STATE_WISHLIST]);
}
