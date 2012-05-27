#ifndef __GFX_TYPES
#define __GFX_TYPES
#include "../com/arawak.h"
#include <stdlib.h>
#include <string.h>

#include "sprite.h"
#include "palette.h"
#include "multiwin.h"

#include "../test/test.h"
#include "../map/map.h"
#include "../lib/pos.h"

#include "ui/dock.h"
#include "ui/stdpan.h"
#include "ui/titlecard.h"


struct gpkg {
        int n;
        int ofs[16];
        short pair;
        const wchar_t *wch[16];
        int len[16];
        cchar_t *cch[16];
};


void      wcch(WINDOW *win, const wchar_t *wch, attr_t attr, short co);
void    mvwcch(WINDOW *win, int y, int x, const wchar_t *wch, attr_t attr, short co);
cchar_t *mkcch(const wchar_t *wch, attr_t attr, short co);

bool is_blank(WINDOW *win, int y, int x);

short winpair_yx(WINDOW *win, int y, int x);
short bgcolor_yx(WINDOW *win, int y, int x);
short fgcolor_yx(WINDOW *win, int y, int x);
void take_bkgrnd(WINDOW *dst, WINDOW *src, short pair);


/**
 * wrapper for mvwaddnwstr with n == 1 */
static inline void mvsayw(WINDOW *win, int y, int x, const wchar_t *wch)
{
        mvwaddnwstr(win, y, x, wch, 1);
}

/**
 * wrapper for waddnwstr with n == 1 */
static inline void sayw(WINDOW *win, const wchar_t *wch)
{
        waddnwstr(win, wch, 1);
}


#define vrt_refresh()    \
        update_panels(); \

#define scr_refresh()    \
        update_panels(); \
        doupdate();      \

#define win_refresh(win) \
        wrefresh(win);   \

#define map_refresh(map) \
        copywin(PEEK(map->W), map->win, pos_y((map)->pos), pos_x((map)->pos), 0, 0, LINES-1, COLS-1, 0); \
        doupdate();\


void geojug_start(void);
void build_gpkg(struct gpkg *g);

#endif


