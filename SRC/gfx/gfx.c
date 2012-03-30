// vim:fdm=marker
/******************************************************************************
 * FILENAME:  gfx.c
 ******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
#include <stdarg.h>
#include <ncurses.h>
#include <stdint.h>
#include <panel.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

#include "gfx.h"
#include "../geo/map.h"

#include "../pan/test.h"
#include "../gen/dice.h"
#include "../lib/llist/list.h"
#include "../gen/perlin.h"
#include "palette.h"
#include "sprite.h"
#include "../geo/terrain.h"
#include "../lib/morton.h"
#include "../lib/ufo.h"

#define IN_GNODE(G, y, x) \
        ((x>=G->dim.x0)&&(x<=(G->dim.xmax))&&(y>=G->dim.y0)&&(y<=(G->dim.ymax))) ? 1 : 0

struct map_t *GLOBE;

//##############################################################################
//# Start the GeoJug engine                                                    #
//#----------------------------------------------------------------------------#
//# This routine should be called after ncurses has been initialized, but      #
//# before practically anything else.                                          #
//##############################################################################
void geojug_start(void)
{
        GLOBE = malloc(sizeof(struct map_t)); // initialize the global map
}


/*
  Returns 0 (false) if collision detected, otherwise returns 1.
*/
int hit_test(struct map_t *map, int y, int x)
{
        uint32_t z; // Stores Morton code.

        z = MORT(y, x);

        if (is_state(map->tree, z, 1, LAY, TOP) || is_state(map->tree, z, 1, LAY, DRP))
                return (0);
        else    
                return (1);
}

void build_gpkg(struct gpkg *g)
{
        int n, i;
        n = g->n;

        while (n-->0) {
               g->len[n] = (sizeof(wchar_t))*(wcslen(g->wch[n])+1);
               g->cch[n] = malloc(g->len[n] * sizeof(cchar_t));
               for (i=0; i<(g->len[n]); i++) {
                       setcchar(&g->cch[n][i], &g->wch[n][i], 0, g->pair, NULL);
               }
        }
}


void build_gpack(struct gpack *g)
{
        setcchar(&g->cch, &g->wcs, 0, g->pair, NULL);
}

void center_text(WINDOW *win, int y0, int x0, int w, char *string)
{
	int len, x, y;
	float temp;

	getyx(win, y, x);
	if (x0 != 0) x = x0;
	if (y0 != 0) y = y0;
	if (w  == 0) w = 80;

	len = strlen(string);
	temp = (w - len)/2;
	x = x0 + (int)temp;
	mvwprintw(win, y, x, "%s", string);
}

void wwrapstr(WINDOW *win, const char *string)
{
        int width = getmaxx(win);
        int ch;
        int i;
        int wrap; // The index at which to wrap the string.

        i = wrap = 0;

        while (ch = string[i++]) {
                if ((ch==' ')&&(i<width)) wrap = i;
        }
        /* If the line will fit, simply print it. */
        if (i < width) {
                waddstr(win, string);
                return;
        }
        /* Else wrap it. */
        else {
                waddnstr(win, string, wrap);
                wprintw(win, "\n");
                waddstr(win, &string[wrap]);
        }
}

short cuco(WINDOW *win)
{
        attr_t attrs;
        short color_pair;

        wattr_get(win, &attrs, &color_pair, NULL);

        return (color_pair);
}


/******************************************************************************
 * DESPAIR, THE ROOST OF WRETCHED FUNCS DEPORTED TO THIS PENULT BARE
 * OF DOCUMENTS OR COMMENTS, YEA, OF HOPE, TO WASTE, UNTIL REPAIR'D.
 ******************************************************************************/

