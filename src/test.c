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
/******************************************************************************/
WINDOW *DIAGNOSTIC_WIN;
PANEL *DIAGNOSTIC_PAN;

static int counter = 0;
/******************************************************************************/
/* Create the diagnostic window */
void test_init(void)
{
	DIAGNOSTIC_WIN = newwin((LINES-30), (COLS-6), 30, 3);
        DIAGNOSTIC_PAN = new_panel(DIAGNOSTIC_WIN);
}
/* Toggle the diagnostic panel's visibility */
void toggle_dpanel(int opt)
{
        if ((panel_hidden(DIAGNOSTIC_PAN)))                 
                show_panel(DIAGNOSTIC_PAN);
        else 
                hide_panel(DIAGNOSTIC_PAN);
        update_panels();
        doupdate();
}
