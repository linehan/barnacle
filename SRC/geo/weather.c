// vim:fdm=marker
/*******************************************************************************
 FILENAME:  weather.c
 Weather and environmental (not those kind) variables that determine, among 
 other things, the state and rate of a ship at sail.
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
#include <pthread.h>
#include <semaphore.h>

#include "../lib/llist/list.h"
#include "../lib/memmacro.h"
#include "../gen/dice.h"
#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../mob/mob.h"
#include "../mob/boat.h"
#include "../pan/test.h"
/******************************************************************************/
enum wind_options{
        __dir__ = 0, 
        __str__ = 1, 
        __pre__ = 2 
};
struct windpackage {
        int dir;
        int str;
        int pre;
};
static struct windpackage g_windstate; /* the global wind state */
static sem_t *wind_IO;
WINDOW *CLOUDW;
PANEL *CLOUDP;
int cloudx;
int cloudy;
int cloudfresh;
/******************************************************************************/
/* Initialize weather globals */
void init_weather(void)
{
        CLOUDW = newwin(10, 10, 10, 10);
        CLOUDP = new_panel(CLOUDW);
        cloudx = 10;
        cloudy = 10;
        cloudfresh = 0;
        wind_IO = MALLOC(sem_t);
        sem_init(wind_IO, 0, 1);

        g_windstate.dir = 0;
        g_windstate.str = 2; 
        g_windstate.pre = 0;

}
void movecloud(int dir)
{
        switch (dir) {
        case 'l':
                move_panel(CLOUDP, cloudy, --cloudx);
                break;
        case 'r':
                move_panel(CLOUDP, cloudy, ++cloudx);
                break;
        case 'u':
                move_panel(CLOUDP, --cloudy, cloudx);
                break;
        case 'd':
                move_panel(CLOUDP, ++cloudy, cloudx);
                break;
        }
        cloudfresh = 0;
        vrt_refresh();
}

void render_clouds(PLATE *pl)
{
        if (cloudfresh == 1) return;

        GNODE *tmp, *under;
        WINDOW *pwin;

        int py, px;     /* coordinates in target window */
        int wmax, hmax;
        int x, y;

        cchar_t cch;    /* cchar_t to be decomposed */
        wchar_t wch;    /* wchar_t of extracted cchar */
        short pair;     /* color pair of extracted cchar */
        attr_t attr;    /* attributes of extracted cchar */

        list_for_each(pl->gfx, tmp, node) {
        if (tmp->dim.layer != __wea__) continue;

        wmax = tmp->dim.xmax;  /* width of cloud window */
        hmax = tmp->dim.ymax;  /* height of cloud window */

        for (y=0; y<tmp->dim.h; y++) {
                for (x=0; x<tmp->dim.w; x++) {
                        /* Get the address of the GNODE which contains the
                         * tile directly below the indicated coordinates.
                         * (respects the stack order specified in "gfx.h" so
                         * that hidden panels or empty spaces don't count) */
                        under = gnode_below(GLOBE->P, tmp->dim.y0+y, tmp->dim.x0+x);

                        if (under == NULL) continue;

                        pwin = under->W->window;
                        /* Map the cloud's coordinates to the coordinates of
                         * the underlying GNODE, so we can extract a character
                         * from it at the appropriate point. */
                        py = (tmp->dim.y0-under->dim.y0)+y;
                        px = (tmp->dim.x0-under->dim.x0)+x;

                        /* Position the cursor in the underlying window and 
                         * extract the char_t at that location, placing it in
                         * our variable 'cch'. */
                        mvwin_wch(pwin, py, px, &cch);
                        mvwadd_wch(pwin, py, px, &cch);

                        /* Get the wchar_t and the color pair that make up the
                         * cchar_t we've designated. */
                        getcchar(&cch, &wch, &attr, &pair, NULL);
                        /* If the color pair isn't one of ours, then the char
                         * is probably invalid, or blank */
                        /*if (pair <= 17) continue;*/

                        /* Modify our copy of the cchar_t to use the same
                         * wchar_t as the one underneath, but render it with
                         * a color pair from shade group 1. */
                        setcchar(&cch, &wch, 0, to_shade(pair, 1), NULL);

                        /* Position the cursor in our cloud window, and write
                         * the modified cchar_t at the specified location. */
                        mvwadd_wch(tmp->W->window, y, x, &cch);
                        wrefresh(tmp->W->window);
                        vrt_refresh();
                }
                scr_refresh();
        }
        }
        cloudfresh = 1;
        /*restack(pl);*/
}
        
/* Access the global wind state */
int get_wind(int a)
{
        int buf = 0;
        sem_wait(wind_IO);
                switch (a) {
                case __dir__:
                        buf = g_windstate.dir;
                        break;
                case __str__:
                        buf = g_windstate.str;
                        break;
                case __pre__:
                        buf = g_windstate.pre;
                        break;
                }
        sem_post(wind_IO);
        return buf;
}
/* Set the global wind state */
void set_wind(int a, int val)
{
        sem_wait(wind_IO);
                switch (a) {
                case __dir__:
                        g_windstate.dir = val;
                        break;
                case __str__:
                        g_windstate.str = val;
                        break;
                case __pre__:
                        g_windstate.pre = val;
                        break;
                }
        sem_post(wind_IO);
}
