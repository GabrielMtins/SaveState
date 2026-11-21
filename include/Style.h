#ifndef STYLE_H
#define STYLE_H

#include <ncurses.h>

#define FOR_STYLE_LIST(DO) \
	DO(STYLE_WINDOW_BG,            A_NORMAL          ) \
	DO(STYLE_TABLE_HEADER,         A_REVERSE         ) \
	DO(STYLE_TABLE_ROW,            A_NORMAL          ) \
	DO(STYLE_TABLE_SELECTED,       A_REVERSE | A_BOLD) \
	DO(STYLE_FORM_HEADER,		   A_REVERSE         ) \
	DO(STYLE_FORM_LABEL,       	   A_REVERSE         ) \
	DO(STYLE_FORM_INPUT_BG,    	   A_REVERSE | A_DIM ) \
	DO(STYLE_FORM_INPUT_CURSOR,	   A_REVERSE | A_BOLD) \
	DO(STYLE_ERROR_MSG,        	   A_NORMAL          ) \
	DO(STYLE_CONFIRM_MSG,      	   A_NORMAL          ) \
	DO(STYLE_FILTER_MSG,       	   A_NORMAL          ) \
	DO(STYLE_SUCCESS_MSG,          A_NORMAL          ) \
	DO(STYLE_MENUBAR_BG,           A_NORMAL          ) \
	DO(STYLE_MENUBAR_SELECTED, 	   A_REVERSE         ) \
	DO(STYLE_MENUBAR_OFF,      	   A_REVERSE | A_DIM ) \
	DO(STYLE_STATS_HEADER,     	   A_REVERSE         ) \
	DO(STYLE_BUTTON_OFF,       	   A_REVERSE | A_DIM ) \
	DO(STYLE_BUTTON_SELECTED,  	   A_REVERSE         ) \
	DO(STYLE_HGRAPH_BAR,       	   A_REVERSE         ) \
	DO(STYLE_HGRAPH_LABEL,     	   A_NORMAL          ) \
	DO(STYLE_HGRAPH_VLINE,     	   A_NORMAL          ) \
	DO(STYLE_HGRAPH_NUMBER,    	   A_NORMAL          ) \
	DO(STYLE_BOTTOM_BAR,       	   A_REVERSE         ) \
	DO(STYLE_SEARCH_BG,        	   A_NORMAL          ) \
	DO(STYLE_SEARCH_CURSOR,    	   A_REVERSE         ) \
	DO(STYLE_ABOUT_TEXT,       	   A_NORMAL          ) \
	DO(STYLE_END, 0)

#define EXPAND_AS_ENUM(id, attr1) id,
typedef enum {
	FOR_STYLE_LIST(EXPAND_AS_ENUM)
	NUM_STYLES
} StyleColor;
#undef EXPAND_AS_ENUM

void Style_SetDefaultTheme(void);

int Style_GetAttr(StyleColor color);

void Style_SetAttr(WINDOW *win, StyleColor color);

void Style_SetBasicColorTheme(void);

void Style_SetCatppuccinTheme(void);

#endif
