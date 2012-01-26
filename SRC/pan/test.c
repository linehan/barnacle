// vim:fdm=marker
/*******************************************************************************
 FILENAME: test.c
 Provides a diagnostic window for diagnostic output, and connects it to
 a panel which can be toggled.
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>

#include "../gfx/sprite.h"
/******************************************************************************/
WINDOW *DIAGNOSTIC_WIN;
PANEL *DIAGNOSTIC_PAN;

WINDOW *ERROR_WIN;
WINDOW *ERRSH_WIN;
PANEL *ERROR_PAN;
PANEL *ERRSH_PAN;

static int counter = 0;
/******************************************************************************/
/* Create the diagnostic window */
void test_init(void)
{
	DIAGNOSTIC_WIN = newwin((LINES-30), (COLS/3), 15, 0);
        DIAGNOSTIC_PAN = new_panel(DIAGNOSTIC_WIN);

	ERROR_WIN = newwin(10, COLS-(COLS/2), ((LINES/2)-5), (COLS/2)/2);
	/*ERRSH_WIN = newwin(10, COLS-(COLS/2), ((LINES/2)-5)+1, ((COLS/2)/2)+1);*/
        ERROR_PAN = new_panel(ERROR_WIN);
        /*ERRSH_PAN = new_panel(ERRSH_WIN);*/
        wbkgrnd(ERROR_WIN, &WARNBG);
        /*wbkgrnd(ERRSH_WIN, &WARNSH);*/

        hide_panel(DIAGNOSTIC_PAN);
        hide_panel(ERROR_PAN);
        /*hide_panel(ERRSH_PAN);*/
}
/* Toggle the diagnostic panel's visibility */
void toggle_dpanel(void)
{
        if ((panel_hidden(DIAGNOSTIC_PAN)))                 
                show_panel(DIAGNOSTIC_PAN);
        else 
                hide_panel(DIAGNOSTIC_PAN);
}

void speak_error(const char *error)
{
        mvwprintw(ERROR_WIN, 1, 2, "%s\n", error);
        /*show_panel(ERRSH_PAN);*/
        show_panel(ERROR_PAN);
        noecho();
        while (wgetch(ERROR_WIN) != '\n');
        echo();
        /*hide_panel(ERRSH_PAN);*/
        hide_panel(ERROR_PAN);
        werase(ERROR_WIN);
}
