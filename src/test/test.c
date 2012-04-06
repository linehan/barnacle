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

#include "../eng/model/bytes.h"
#include "../gfx/gfx.h"
#include "../map/terrain.h"
#include "../lib/morton.h"
#include "../lib/ufo.h"
#include "../eng/fsm.h"
/******************************************************************************/
WINDOW *DIAGNOSTIC_WIN;
PANEL  *DIAGNOSTIC_PAN;

WINDOW *MARQUEEW;
PANEL  *MARQUEEP;

WINDOW *BIGWIN;
PANEL  *BIGPAN;

WINDOW *ERROR_WIN;
PANEL  *ERROR_PAN;

WINDOW *SYSWIN;
PANEL  *SYSPAN;

WINDOW *INSPECTORWIN;
PANEL  *INSPECTORPAN;
WINDOW *INSPECTORMSGWIN;
PANEL  *INSPECTORMSGPAN;

#define i_yy inspector_yy
#define i_xx inspector_xx

static bool diagnostic_lock;

static int inspector_y = 0;
static int inspector_x = 0;
static int inspector_yy = 0;
static int inspector_xx = 0;

static int counter = 0;
/******************************************************************************/
/* Create the diagnostic window */
void init_test(void)
{
	DIAGNOSTIC_WIN = newwin((LINES-30), COLS, 0, 0);
        DIAGNOSTIC_PAN = new_panel(DIAGNOSTIC_WIN);

        diagnostic_lock = false;

        BIGWIN = newwin(LINES, COLS, 0, 0);
        BIGPAN = new_panel(BIGWIN);

        MARQUEEW = newwin(1, COLS, LINES-1, 0);
        MARQUEEP = new_panel(MARQUEEW);

        INSPECTORWIN = newwin(1, 1, 0, 0);
        INSPECTORPAN = new_panel(INSPECTORWIN);
        wbkgrnd(INSPECTORWIN, &PLAIN);

        INSPECTORMSGWIN = newwin(1, COLS, 0, 0);
        INSPECTORMSGPAN = new_panel(INSPECTORMSGWIN);
        wbkgrnd(INSPECTORMSGWIN, &WARNBG);

	SYSWIN = newwin(30, COLS-(COLS/2), ((LINES/2)-15), (COLS/2)/2);
        SYSPAN = new_panel(SYSWIN);

	ERROR_WIN = newwin(10, COLS-(COLS/2), ((LINES/2)-5), (COLS/2)/2);
        ERROR_PAN = new_panel(ERROR_WIN);
        wbkgrnd(ERROR_WIN, &WARNBG);

        hide_panel(DIAGNOSTIC_PAN);
        hide_panel(ERROR_PAN);
        hide_panel(SYSPAN);
        hide_panel(BIGPAN);
        hide_panel(INSPECTORPAN);
        hide_panel(INSPECTORMSGPAN);
}
/* Toggle the diagnostic panel's visibility */
void toggle_dpanel(void)
{
        if ((panel_hidden(DIAGNOSTIC_PAN)))                 
                show_panel(DIAGNOSTIC_PAN);
        else 
                hide_panel(DIAGNOSTIC_PAN);
}

void toggle_syspan(void)
{
        if ((panel_hidden(SYSPAN)))                 
                show_panel(SYSPAN);
        else 
                hide_panel(SYSPAN);
}

void toggle_bigpan(void)
{
        if ((panel_hidden(BIGPAN)))                 
                show_panel(BIGPAN);
        else 
                hide_panel(BIGPAN);
}

void speak_error(const char *error)
{
        mvwprintw(ERROR_WIN, 1, 2, "%s\n", error);
        show_panel(ERROR_PAN);
        noecho();
        while (wgetch(ERROR_WIN) != '\n');
        echo();
        hide_panel(ERROR_PAN);
        werase(ERROR_WIN);
}

void speak_error_mono(const char *error)
{
        wcolor_set(ERROR_WIN, COLOR_BLACK, NULL);
        wbkgrnd(ERROR_WIN, &PLAIN);
        mvwprintw(ERROR_WIN, 1, 2, "%s\n", error);
        show_panel(ERROR_PAN);
        while (wgetch(ERROR_WIN) != '\n');
        hide_panel(ERROR_PAN);
        werase(ERROR_WIN);
}

int inspect_control(int dir)
{
        uint32_t z;
        switch (dir) {
        case KEY_LEFT:
                inspector_x -= (inspector_x > 0) ? 1 : 0;
                break;
        case KEY_RIGHT:
                inspector_x += 1;
                break;
        case KEY_UP:
                inspector_y -= (inspector_y > 0) ? 1: 0;
                break;
        case KEY_DOWN:
                inspector_y += 1;
                break;
        case '?':
                return MODE_RELEASE;
        }

        i_yy = ufo_y(GLOBE, ufo)+(inspector_y);
        i_xx = ufo_x(GLOBE, ufo)+(inspector_x);

        z = MORT(i_yy, i_xx);
        move_panel(INSPECTORPAN, inspector_y, inspector_x);
        top_panel(INSPECTORPAN);
        top_panel(INSPECTORMSGPAN);
        vrt_refresh();

        werase(INSPECTORMSGWIN);
        wprintw(INSPECTORMSGWIN, "Y: %2u X: %2u M: %2u\t"
                                 "LAYER: %s\t"
                                 "ELEV: %2u\t"
                                 "PERLIN: %+5f", 
                        i_yy, 
                        i_xx, 
                        z,
                        lay_tag[get_nibble(rb_data(GLOBE->tree, z), LAY)], 
                        get_nibble(rb_data(GLOBE->tree, z), ALT), 
                        GLOBE->pmap[i_yy][i_xx]);
        scr_refresh();
        return MODE_PERSIST;
}
