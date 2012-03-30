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
#include <pthread.h>
#include <semaphore.h>
#include "sprite.h"
#include "palette.h"
#include "multiwin.h"
#include "../lib/llist/list.h"
#include "../pan/test.h"
#include "../geo/map.h"
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





#define vrt_refresh()    \
        update_panels(); \

#define scr_refresh()    \
        update_panels(); \
        doupdate();      \

#define win_refresh(win) \
        wrefresh(win);   \

#define map_refresh(map) \
        copywin(PEEK(map->W), map->win, ufo_y(map, ufo), ufo_x(map, ufo), 0, 0, LINES-1, COLS-1, 0); \
        update_panels(); \
        doupdate(); 

#define TOGPAN(pan)              \
        if (panel_hidden(pan))   \
                show_panel(pan); \
        else    hide_panel(pan)  

#endif
