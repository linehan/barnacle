
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

#include "../lib/matrix.h"
#include "../eng/bytes.h"
#include "../gfx/gfx.h"
#include "../map/terrain.h"
#include "../lib/morton.h"
#include "../lib/pos.h"
#include "../lib/matrix.h"

/* -------------------------------------------------------------------------- */

/* The console menu */
#define CONSOLE_W (COLS) 
#define CONSOLE_H (LINES/5)
WINDOW *CONSOLE_WIN;
PANEL  *CONSOLE_PAN;
WINDOW *CONSOLE_INPUT;

/* Dump print statements */
#define DIAGNOSTIC_W (COLS)
#define DIAGNOSTIC_H (LINES)
WINDOW *DIAGNOSTIC_WIN;
PANEL  *DIAGNOSTIC_PAN;


/* Screen overlay */
#define HUD_W (COLS)
#define HUD_H (LINES)
WINDOW *HUD_WIN;
PANEL  *HUD_PAN;

/* Cursor and message strip */
WINDOW *INSPECTORWIN;
PANEL  *INSPECTORPAN;
WINDOW *INSPECTORMSGWIN;
PANEL  *INSPECTORMSGPAN;

static bool diagnostic_lock;

static int inspector_y = 0;
static int inspector_x = 0;

static int cur_y;
static int cur_x;


/* -------------------------------------------------------------------------- */

/* Create the diagnostic window */
void init_test(void)
{
	DIAGNOSTIC_WIN = newwin(DIAGNOSTIC_H, DIAGNOSTIC_W, 0, 0);
        DIAGNOSTIC_PAN = new_panel(DIAGNOSTIC_WIN);

	HUD_WIN = newwin(HUD_H, HUD_W, 0, 0);
        HUD_PAN = new_panel(HUD_WIN);

        CONSOLE_WIN = newwin(CONSOLE_H, CONSOLE_W, 0, 0);
        CONSOLE_PAN = new_panel(CONSOLE_WIN);
        scrollok(CONSOLE_WIN, true);

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
        werase(CONSOLE_INPUT);
        wprintw(CONSOLE_INPUT, "> ");
        fflush(stdout);
        /*p = fgets(buf, 80, stdin);*/

        doupdate();
}

void hud_win(WINDOW *win)
{
        overlay(HUD_WIN, win);
}

/* -------------------------------------------------------------------------- */
static inline void move_cursor(int dir, int step)
{
        switch (dir) {
        case 'l':
                while (step-->0) DEC(cur_x,0);
                break;
        case 'r':
                while (step-->0) INC(cur_x, COLS);
                break;
        case 'u':
                while (step-->0) DEC(cur_y,0);
                break;
        case 'd':
                while (step-->0) INC(cur_y, LINES);
                break;
        }

        move_panel(INSPECTORPAN, cur_y, cur_x);
        top_panel(INSPECTORPAN);
        vrt_refresh();
}


int inspect_control(int dir)
{
        uint32_t z;
        switch (dir) {
        case 'h':
                move_cursor('l', 1);
                break;
        case 'H':
                move_cursor('l', 4);
                break;
        case 'l':
                move_cursor('r', 1);
                break;
        case 'L':
                move_cursor('r', 4);
                break;
        case 'k':
                move_cursor('u', 1);
                break;
        case 'K':
                move_cursor('u', 4);
                break;
        case 'j':
                move_cursor('d', 1);
                break;
        case 'J':
                move_cursor('d', 4);
                break;
        case '?':
                return MODE_RELEASE;
        }

        cur_y += pos_y(ACTIVE->pos);
        cur_x += pos_x(ACTIVE->pos);

        z = MORT(cur_y, cur_x);

        top_panel(INSPECTORMSGPAN);
        vrt_refresh();

        werase(INSPECTORMSGWIN);
        wprintw(INSPECTORMSGWIN, "LINES: %d COLS: %d\tY: %2u X: %2u M: %2u\t"
                                 "ELEV: %2u "
                                 "PERLIN: %+4f\t" 
                                 "DOOR: %u\t"
                                 "MOBS: %u\t"
                                 "ITEM: %u",
                        LINES,
                        COLS,
                        cur_y, 
                        cur_x, 
                        z,
                        ELEV(ACTIVE, cur_y, cur_x),
                        ACTIVE->pmap[cur_y][cur_x],
                        mx_val(ACTIVE->door, cur_y, cur_x),
                        mx_val(ACTIVE->mobs, cur_y, cur_x),
                        mx_val(ACTIVE->item, cur_y, cur_x));
        scr_refresh();
        return MODE_PERSIST;
}


void print_matrix(WINDOW *win, void *matrix)
{
        struct matrix_t *mx = (struct matrix_t *)matrix;
        int i;
        int j;

        for (i=0; i<mx->itr.rows; i++) {
        for (j=0; j<mx->itr.cols; j++) {
                if (mx->mx[i][j] != 0)
                        mvwprintw(win, i, j, "#");
                else
                        mvwprintw(win, i, j, ".");
        }
        }
        scr_refresh();
}


