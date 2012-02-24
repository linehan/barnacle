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
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <ncurses.h>
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
        init_boat();        // Initialize boat MOBs
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

        if (is_sw(map->sw, z, LAY, TOP) || is_sw(map->sw, z, LAY, DRP))
                return (0);
        else    
                return (1);

        /*MORT(yu, x, &zu);*/
        /*MORT(yd, x, &zd);*/
        /*MORT(y, xl, &zl);*/
        /*MORT(y, xr, &zr);*/

        /*if (  (is_cell(map->sw, zu, LAY, TOP))*/
            /*||(is_cell(map->sw, zu, LAY, DRP))*/
            /*||(is_cell(map->sw, zd, LAY, TOP))*/
            /*||(is_cell(map->sw, zd, LAY, DRP))*/
            /*||(is_cell(map->sw, zr, LAY, TOP))*/
            /*||(is_cell(map->sw, zr, LAY, DRP))*/
            /*||(is_cell(map->sw, zl, LAY, TOP))*/
            /*||(is_cell(map->sw, zl, LAY, DRP)))*/
        /*{*/

}

/*struct gpkg *new_gpkg(const wchar_t **wch, int len, short pair, int ofs0, int ofs1, int ofs2)*/
/*{*/
        /*struct gpkg *new = malloc(sizeof *new);*/

        /*new->ofs[0] = ofs0;*/
        /*new->ofs[1] = ofs1;*/
        /*new->ofs[2] = ofs2;*/

        /*[>new->len = (wcslen(wch))+1;<]*/
        /*wprintw(DIAGNOSTIC_WIN, "%u\n", new->len);*/
        /*new->pair = pair;*/
        /*[>new->_wch = malloc(new->len * sizeof(wchar_t));<]*/
        /*[>new->_wch = wch;<]*/
        /*new->len = len;*/
        /*new->_cch = malloc(new->len * sizeof(cchar_t));*/

        /*[>new->_wch = wcsncpy(new->_wch, wch, new->len);<]*/

        /*int i;*/
        /*for (i=0; i<(new->len); i++) {*/
                /*setcchar(&new->_cch[i], wch[i], 0, new->pair, NULL); // Generate cchar*/
        /*}*/

        /*return (new);*/
/*}*/
struct gpkg *
new_gpkg(const wchar_t *wch, short pair, int yo, int xo, int yr, int xr)
{
        struct gpkg *new = malloc(sizeof *new);
        int i;

        new->len = (unsigned)(wcslen(wch)+1);
        new->cch = malloc(new->len * sizeof(cchar_t));
        for (i=0; i<(new->len); i++) {
                setcchar(&new->cch[i], &wch[i], 0, pair, NULL);
        }
        new->xofs = xo;
        new->yofs = yo;
        new->xrel = xr;
        new->yrel = yr;

        return (new);
}




/******************************************************************************
 * DESPAIR, THE ROOST OF WRETCHED FUNCS DEPORTED TO THIS PENULT BARE
 * OF DOCUMENTS OR COMMENTS, YEA, OF HOPE, TO WASTE, UNTIL REPAIR'D.
 ******************************************************************************/

