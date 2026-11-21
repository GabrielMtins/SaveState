#include "App.h"
#include "Widgets.h"

#define NUM_BUTTONS ((int) (sizeof(button_labels) / sizeof(const char *)))

static const char *button_labels[] = {
	" Games",
	" Diary",
	" Stats",
	" WishList",
	" About"
};

static const int button_to_state[] = {
	STATE_CATALOG,
	STATE_LOGVIEW,
	STATE_STATS,
	STATE_WISHLIST,
	STATE_ABOUT,
};

static bool Menubar_CheckIfStateIsOfMenubar(App *app);
static void Menubar_HandleInput(App *app, MenubarState *state);
static void Menubar_Draw(App *app, MenubarState *state);

static bool Menubar_CheckIfStateIsOfMenubar(App *app) {
	for(int i = 0; i < NUM_BUTTONS; i++) {
		if(app->current_state == button_to_state[i]) {
			return true;
		}
	}

	return false;
}

static void Menubar_HandleInput(App *app, MenubarState *state) {
	if(!Menubar_CheckIfStateIsOfMenubar(app)) {
		return;
	}

	if(app->block_menubar) {
		app->block_menubar = false;
		return;
	}

	switch(app->ch) {
		case 't':
			if(app->prev_ch != 'g') break;
			state->button_selected++;
			break;

		case '\t':
		case 'K':
			state->button_selected++;
			break;

		case 'T':
			if(app->prev_ch != 'g') break;
			state->button_selected--;
			break;

		case KEY_BTAB:
		case 'J':
			state->button_selected--;
			break;

		case 'q':
			app->quit = 1;
			break;

		default:
			return;
			break;
	}

	if(state->button_selected < 0) state->button_selected = NUM_BUTTONS - 1;
	if(state->button_selected >= NUM_BUTTONS) state->button_selected = 0;

	app->next_state = button_to_state[state->button_selected];
}

static void Menubar_Draw(App *app, MenubarState *state) {
	WINDOW *win = app->top_window;

	wmove(win, 0, 2);

	for(int i = 0; i < NUM_BUTTONS; i++) {
		if(i == state->button_selected) {
			Style_SetAttr(win, STYLE_MENUBAR_SELECTED);
		} else {
			Style_SetAttr(win, STYLE_MENUBAR_OFF);
		}

		Widget_Label(
				win,
				button_labels[i],
				10
				);

		wmove(win, 0, 2 + getcurx(win));
	}
}

void Menubar_Update(App *app) {
	MenubarState *state = (MenubarState *) app->states[STATE_MENUBAR];

	Menubar_HandleInput(app, state);
	Menubar_Draw(app, state);
}
