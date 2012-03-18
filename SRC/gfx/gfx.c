// vim:fdm=marker
/******************************************************************************
 * FILENAME:  gfx.c
 * This module makes extensive use of the "famous" doubly linked list 
 * routines written by Rusty Russell <rusty@rustcorp.com.au> for use in 
 * the Linux kernel. He later modified them to an even more general form, 
 * which is implemented here. For more information, see the CCAN entry 
 * at <http://ccodearchive.net/info/list.html>.
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
#include "../gen/wave.h"
#include "../lib/llist/list.h"
#include "../gen/perlin.h"
#include "palette.h"
#include "sprite.h"
#include "../geo/terrain.h"
#include "../lib/morton.h"
#include "../lib/ufo.h"

#define IN_GNODE(G, y, x) \
        ((x>=G->dim.x0)&&(x<=(G->dim.xmax))&&(y>=G->dim.y0)&&(y<=(G->dim.ymax))) ? 1 : 0

sem_t  *REFRESH_LOCK;
struct map_t *GLOBE;

//##############################################################################
//# Start the GeoJug engine                                                    #
//#----------------------------------------------------------------------------#
//# This routine should be called after ncurses has been initialized, but      #
//# before practically anything else.                                          #
//##############################################################################
void geojug_start(void)
{
        REFRESH_LOCK = (sem_t *)malloc(sizeof(sem_t)); // keeps things honest
        sem_init(REFRESH_LOCK, 0, 1);

        init_random();      // Mersenne twister
        init_simplex();     // Perlin simplex noise generator
        init_palette(0);    // Set color palette
        init_gfx_colors();  // Apply palette to character set
        init_boats();       // Initialize boat MOBs
        init_wave();
        init_weather();     // Start weather engine
        init_instruments(); // Various meters (compass)
        init_menus();       // psh menu and commands
        init_test();        // Start test structures

        GLOBE = malloc(sizeof(struct map_t)); // initialize the global map
}
/******************************************************************************
 * Allocate memory for a new WNODE, and initialize it with the arguments
 * supplied. This function is almost always called from inside new_gnode(), 
 * for (pretty good) reasons, as described in that function's description.
 ******************************************************************************/
struct wnode_t *new_wnode(int id, int h, int w, int y0, int x0)
{
        struct wnode_t *new = (struct wnode_t *)malloc(sizeof *new);

        // Detect if WINDOW should be a pad.
        if (h > LINES || w > COLS) new->window = newpad(h, w);
        else                       new->window = newwin(h, w, y0, x0);

        return new;
}

struct ring_t *new_winring(int h, int w, int y0, int x0, int nwindows)
{
        struct ring_t *new = (struct ring_t *)malloc(sizeof *new);

        MAKE_RING(new->ring); // Initialize the list head
        new->n = nwindows;

        while (nwindows-->0) {
                struct wnode_t *wnode = new_wnode(nwindows, h, w, y0, x0);
                list_add(new->ring, &wnode->node);
        }
        NEXT(new); // Make sure *peek points to something.
        return (new);
}
/*
  Returns 0 (false) if collision detected, otherwise returns 1.
*/
int hit_test(struct map_t *map, int y, int x)
{
        uint32_t z; // Stores Morton code.

        MORT(y, x, &z);

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

void build_gpkg_cbar(struct gpkg *g, struct cbar *c)
{
        int i, p;

        /*g->len[0] = (int)(wcslen(g->wch[0])+1);*/
        g->len[0] = (sizeof(wchar_t))*(wcslen(g->wch[0])+1);
        g->cch[0] = malloc(g->len[0] * sizeof(cchar_t));
        for (i=0, p=0; i<(g->len[0]); i++) {
                p = (c->bar[i] - '0');
                setcchar(&g->cch[0][i], &g->wch[0][i], 0, c->pair[p], NULL);
        }
}

void dopaint(const void *self)
{
        int str; // Incremented every string.
        int chr; // Incremented every character in a string.

        struct recipe *R = (struct recipe *)self;  // Cast the void pointer.

        if (R->win == NULL) return; // Let's not do anything we may regret...

        for (str=0; str<(R->n); str++) {
                mvwadd_wchstr(R->win, str, 0, R->cch[str]); // Print rest (advance cursor)
        }
        vrt_refresh();
}
                        

void illuminate(struct recipe *R)
{
        setlocale(LC_ALL,""); // UTF-8
        unsigned int chr; // Incremented every character in a string.
        int str; // Incremented every string.
        int pai; // Tracks the color pair.
        int wid; // We'll want the width (max str length) to make a WINDOW.

        // Compute the length of each string and allocate cchar_t's for it.
        for (str=0, wid=0; str<(R->n); str++) {
                R->len[str] = wcswidth(R->wch[str], 100);
                wid = (R->len[str] > wid) ? R->len[str] : wid; // Catch max len
        }

        pai = BOAT_WOOD;
        for (str=0; str<(R->n); str++) {
                // Compute the matching color pair and set the cchar_t's.
                for (chr=0, pai=0; chr<(R->len[str]); chr++) {
                        /*pai = (R->bar[str][chr] - '0');*/
                        setcchar(&R->cch[str][chr], &R->wch[str][chr], 0, R->pair[pai], NULL);
                }
        }
        // Initialize "paint" method and create an appropriately sized WINDOW.
        if (R->paint == NULL) R->paint = &dopaint;
        if (R->win   == NULL) R->win   = newwin(R->n, wid, R->yof, R->xof);
        vrt_refresh();
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

/******************************************************************************
 * DESPAIR, THE ROOST OF WRETCHED FUNCS DEPORTED TO THIS PENULT BARE
 * OF DOCUMENTS OR COMMENTS, YEA, OF HOPE, TO WASTE, UNTIL REPAIR'D.
 ******************************************************************************/

