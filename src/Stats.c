#include "App.h"


enum StatsType {
	STATS_PLATFORM,
	STATS_GENRE,
	STATS_DECADE,
	NUM_TYPE_STATS
};

enum StatsInputType {
	STATS_INPUT_NAV,
	STATS_INPUT_SEARCH,
};

static void Stats_BuildTables(App *app, StatsState *state);
static void Stats_DrawSearchBar(App *app, StatsState *state);
static void Stats_DrawGraph(App *app, StatsState *state);
static void Stats_HandleInputNav(App *app, StatsState *state);
static void Stats_HandleInputSearch(App *app, StatsState *state);
static void Stats_HandleInput(App *app, StatsState *state);

static void Stats_BuildTables(App *app, StatsState *state) {
	DB_GetStats(app->db, &state->genre_data, "g.Genre", state->current_year);
	DB_GetStats(app->db, &state->platform_data, "g.Platform", state->current_year);
	DB_GetStats(app->db, &state->decade_data, "((g.Year / 10) * 10) || 's'", state->current_year);
}

static void Stats_DrawSearchBar(App *app, StatsState *state) {
	WINDOW *win = app->bottom_window;

	if(state->input_state != STATS_INPUT_SEARCH) {
		App_ClearBottomWindow(app);
		return;
	}

	Style_SetAttr(win, STYLE_SEARCH_BG);
	wmove(win, 0, 0);
	Widget_Label(win, " Year: ", -1);

	state->search_data.width = getmaxx(win) - getcurx(win);
	Widget_InputLine(win, &state->search_data);
}

static void Stats_DrawGraph(App *app, StatsState *state) {
	if(!state->redraw_graph) return;

	Table *current_table = NULL;
	const char *label_desc = "";
	static const int label_width = 255;
	char current_label[label_width + 1];
	WINDOW *win = app->content_window;

	current_label[label_width] = '\0';

	wclear(win);
	state->redraw_graph = false;

	switch(state->current_graph) {
		case STATS_PLATFORM:
			current_table = &state->platform_data;
			label_desc = "Most played platforms";
			break;

		case STATS_GENRE:
			current_table = &state->genre_data;
			label_desc = "Most played genres";
			break;

		case STATS_DECADE:
			current_table = &state->decade_data;
			label_desc = "Most played decades";
			break;
	}

	if(state->current_year <= 0) {
		snprintf(current_label, label_width, "%s (All Time)", label_desc);
	} else {
		snprintf(current_label, label_width, "%s (%04d)", label_desc, state->current_year);
	}

	Style_SetAttr(win, STYLE_STATS_HEADER);
	wmove(win, 0, 0);
	Widget_Space(win, -1);
	wmove(win, 0, 2);
	Widget_Label(win, current_label, -1);
	wmove(win, 0, getmaxx(win) - 5);
	wprintw(win, "%01d/%01d", state->current_graph + 1, NUM_TYPE_STATS);

	wmove(win, 2, 0);

	Widget_HorizontalBarGraph(
			win,
			current_table,
			8
			);
}

static void Stats_HandleInputNav(App *app, StatsState *state) {
	switch(app->ch) {
		case 'h':
		case 'j':
			if(state->current_graph <= 0) break;

			state->redraw_graph = true;
			state->current_graph--;
			break;

		case 'l':
		case 'k':
			if(state->current_graph >= NUM_TYPE_STATS - 1) break;

			state->redraw_graph = true;
			state->current_graph++;
			break;

		case 'f':
			LineWidgetData_Init(&state->search_data);
			state->input_state = STATS_INPUT_SEARCH;
			break;

		case KEY_ESCAPE:
			LineWidgetData_Init(&state->search_data);
			state->current_year = 0;
			Stats_BuildTables(app, state);
			state->redraw_graph = true;
			break;
	}
}

static void Stats_HandleInputSearch(App *app, StatsState *state) {
	app->block_menubar = true;

	LineWidgetData_HandleInput(&state->search_data, app->ch);

	switch(app->ch) {
		case KEY_ESCAPE:
			LineWidgetData_Init(&state->search_data);
			state->input_state = STATS_INPUT_NAV;
			break;

		case '\n':
			state->current_year = atoi(state->search_data.str);
			state->redraw_graph = true;
			state->input_state = STATS_INPUT_NAV;

			Stats_BuildTables(app, state);
			state->redraw_graph = true;
			break;
	}
}

static void Stats_HandleInput(App *app, StatsState *state) {
	switch(state->input_state) {
		case STATS_INPUT_NAV:
			Stats_HandleInputNav(app, state);
			break;

		case STATS_INPUT_SEARCH:
			Stats_HandleInputSearch(app, state);
			break;
	}
}

void Stats_Update(App *app) {
	StatsState *state = app->states[STATE_STATS];

	if(App_IsNewState(app)) {
		Stats_BuildTables(app, state);
		state->redraw_graph = true;
		state->search_data.bg_color = STYLE_SEARCH_BG;
		state->search_data.cursor_color = STYLE_SEARCH_CURSOR;
		state->search_data.input_type = INPUT_TYPE_NUMBER;
	}
	
	if(app->ch == KEY_RESIZE) {
		state->redraw_graph = true;
	}

	Stats_HandleInput(app, state);

	Stats_DrawGraph(app, state);
	Stats_DrawSearchBar(app, state);
}
