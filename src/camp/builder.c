#include <stdlib.h>
#include "../com/arawak.h"
#include "../gfx/gfx.h"
#include "../gfx/ui/stdpan.h"
#include "../gfx/ui/stdmenu.h"
#include "../gfx/ui/dock.h"
#include "../test/test.h"

WINDOW *build_curs_win;
PANEL  *build_curs_pan;

static int cur_x;
static int cur_y;


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

        move_panel(build_curs_pan, cur_y, cur_x);
        top_panel(build_curs_pan);
        update_panels();
}

int build_control(int dir)
{
        if (!build_curs_win) {
                build_curs_win = newwin(1, 1, 0, 0);
                build_curs_pan = new_panel(build_curs_win);
                wbkgrnd(build_curs_win, mkcch(L"█", A_REVERSE, WARNING));
        }

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

        return MODE_PERSIST;
}


