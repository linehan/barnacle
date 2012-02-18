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
#include "zbox.h"

#include "../pan/test.h"
#include "../gen/dice.h"
#include "../lib/llist/list.h"
#include "../gen/perlin.h"
#include "palette.h"
#include "sprite.h"
#include "../geo/terrain.h"

#define IN_GNODE(G, y, x) \
        ((x>=G->dim.x0)&&(x<=(G->dim.xmax))&&(y>=G->dim.y0)&&(y<=(G->dim.ymax))) ? 1 : 0

sem_t  *REFRESH_LOCK;
MAP    *GLOBE;

// Start the GeoJug engine
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

        GLOBE = malloc(sizeof(MAP)); // initialize the global map
}

/******************************************************************************
 * Allocate memory for a new WNODE, and initialize it with the arguments
 * supplied. This function is almost always called from inside new_gnode(), 
 * for (pretty good) reasons, as described in that function's description.
 ******************************************************************************/
WNODE *new_wnode(int id, int h, int w, int y0, int x0)
{
        WNODE *new = (WNODE *)malloc(sizeof(WNODE));
        new->window = newwin(h, w, y0, x0);
        new->id = id;

        return new;
}
/*void next_wnode(const void *gnode)*/
/*{*/
        /*GNODE *gfx = (GNODE *)gnode;*/
        /*CYCLE(gfx->wins, gfx->W, WNODE);*/
/*}*/
/*void step_wnode_forward(const void *gnode)*/
/*{*/
        /*GNODE *gfx = (GNODE *)gnode;*/
        /*if (gfx->dim.n <= 1) return;*/

        /*gfx->next(gfx);*/
        /*replace_panel(gfx->pan, gfx->W->window);*/
        /*scr_refresh();*/
/*}*/
/******************************************************************************
 * DESPAIR, THE ROOST OF WRETCHED FUNCS DEPORTED TO THIS PENULT BARE
 * OF DOCUMENTS OR COMMENTS, YEA, OF HOPE, TO WASTE, UNTIL REPAIR'D.
 ******************************************************************************/
/*int hit_test(PLATE *pl, int y, int x)*/
/*{*/
        /*GNODE *tmp;*/
        /*int i = 0;*/

        /*list_for_each(pl->gfx, tmp, node) {*/
                /*if ((tmp->dim.layer == __grt__)||(tmp->dim.layer == __drd__)) {*/
                        /*if (((x >= tmp->dim.x0)                  && */
                             /*(x <= (tmp->dim.x0 + tmp->dim.w))   &&*/
                             /*(y >= tmp->dim.y0)                  &&*/
                             /*(y <= (tmp->dim.y0 + tmp->dim.h))))*/
                                /*i++;*/
                /*}*/
        /*}*/
        /*return i;*/
/*}*/

void set_nyb(ZBOX *Z, int y, int x, int n, ...)
{
        if (n == 0) return;
        va_list ap; // accepts a variable number of nibble-state pairs
        int nyb, set; // will hold the next nibble-state pair

        va_start (ap, n); // initialize argument list

        uint32_t *data;
       
        mort(y, x, &(Z->z));
        data = bst_get(Z->bst, Z->z);
       
        while (n--) {
                nyb = va_arg(ap, int);
                set = va_arg(ap, int);
                _set_nyb(data, nyb, set);
        }
        va_end(ap); // tidy up
}

void stat_nyb(ZBOX *Z, uint32_t y, uint32_t x)
{
        if (Z == NULL) return;

        uint32_t *data;
        mort(y, x, &(Z->z));
        data = bst_get(Z->bst, Z->z);

        werase(INSPECTORMSGWIN);   \
        wprintw(INSPECTORMSGWIN, "Z-Code: %5u (%3u,%3u) | ", Z->z, y, x); \
        _stat_nyb(*data);
};

inline int is_nyb(ZBOX *Z, uint32_t y, uint32_t x, int n, int s)
{
        mort(y, x, &(Z->z));
        Z->v = bst_get(Z->bst, Z->z);
        return (_is_nyb(*(Z->v), n, s));
}
