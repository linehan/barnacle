// vim:fdm=marker
#ifndef __GFX_TYPES
#define __GFX_TYPES
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <wchar.h>
#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <locale.h>
#include <string.h>

#include "sprite.h"
#include "palette.h"
#include "multiwin.h"

#include "../test/test.h"
#include "../map/map.h"
#include "../lib/ufo.h"


struct gpkg {
        int n;
        int ofs[16];
        short pair;
        const wchar_t *wch[16];
        int len[16];
        cchar_t *cch[16];
};

struct gpack {
        wchar_t *wcs;
        short pair;
        cchar_t cch;
};


void geojug_start(void);

void build_gpack(struct gpack *g);
void build_gpkg(struct gpkg *g);

void center_text(WINDOW *win, int y0, int x0, int w, char *string);
void wwrapstr(WINDOW *win, const char *string);
short cuco(WINDOW *win);

cchar_t *mkcch(wchar_t *wch, attr_t attr, short co);



#define vrt_refresh()    \
        update_panels(); \

#define scr_refresh()    \
        update_panels(); \
        doupdate();      \

#define win_refresh(win) \
        wrefresh(win);   \

#define map_refresh(map) \
        copywin(PEEK(map->W), map->win, ufo_y(map, ufo), ufo_x(map, ufo), 0, 0, LINES-1, COLS-1, 0); \
        doupdate();\

#define TOGPAN(pan)              \
        if (panel_hidden(pan))   \
                show_panel(pan); \
        else    hide_panel(pan)  


#define mvwp mvwpaint

void mvwpaint(WINDOW *win, int y, int x, wchar_t *wch, short pair);
void mvwnpaint(WINDOW *win, int y, int x, wchar_t *wcs, short pair, int n);






#endif
