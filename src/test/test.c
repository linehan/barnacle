
/*
 * FILENAME: test.c
 * Provides various panels and windows for printing and requesting diagnostic
 * or error output.
 */

#include "../com/arawak.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../eng/model/bytes.h"
#include "../gfx/gfx.h"
#include "../map/terrain.h"
#include "../lib/morton.h"
#include "../lib/ufo.h"

/* -------------------------------------------------------------------------- */

/* The console menu */
#define CONSOLE_W (COLS) 
#define CONSOLE_H (LINES/5)
WINDOW *CONSOLE_WIN;
PANEL  *CONSOLE_PAN;
WINDOW *CONSOLE_INPUT;

/* Dump print statements */
#define DIAGNOSTIC_W (COLS)
#define DIAGNOSTIC_H (LINES/2)
WINDOW *DIAGNOSTIC_WIN;
PANEL  *DIAGNOSTIC_PAN;

/* Cursor and message strip */
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


/* -------------------------------------------------------------------------- */

/* Create the diagnostic window */
void init_test(void)
{
	DIAGNOSTIC_WIN = newwin(DIAGNOSTIC_H, DIAGNOSTIC_W, 0, 0);
        DIAGNOSTIC_PAN = new_panel(DIAGNOSTIC_WIN);

        CONSOLE_WIN = newwin(CONSOLE_H, CONSOLE_W, 0, 0);
        CONSOLE_PAN = new_panel(CONSOLE_WIN);

        CONSOLE_INPUT = derwin(CONSOLE_WIN, 1, CONSOLE_W, CONSOLE_H-1, 0);

        INSPECTORWIN = newwin(1, 1, 0, 0);
        INSPECTORPAN = new_panel(INSPECTORWIN);
        wbkgrnd(INSPECTORWIN, &PLAIN);

        INSPECTORMSGWIN = newwin(1, COLS, 0, 0);
        INSPECTORMSGPAN = new_panel(INSPECTORMSGWIN);
        wbkgrnd(INSPECTORMSGWIN, &WARNBG);

        hide_panel(DIAGNOSTIC_PAN);
        hide_panel(CONSOLE_PAN);
        hide_panel(INSPECTORPAN);
        hide_panel(INSPECTORMSGPAN);
}

/* -------------------------------------------------------------------------- */

void test_request(void)
{
        char buf[81], *p;

        werase(CONSOLE_INPUT);
        wprintw(CONSOLE_INPUT, "> ");
        fflush(stdout);
        /*p = fgets(buf, 80, stdin);*/

        doupdate();
}

/* -------------------------------------------------------------------------- */

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

