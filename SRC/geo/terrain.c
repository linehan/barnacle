// vim:fdm=marker
/*******************************************************************************
    FILENAME:  terrain.c
    Functions for creating landscapes.
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

#include "../lib/llist/list.h"
#include "../lib/memmacro.h"
#include "../lib/hash/hash.h"

#include "../cmd/control.h"
#include "../gen/dice.h"

#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../pan/test.h"
#include "../gen/perlin.h"

#include "weather.h"
/******************************************************************************/
PLATE *hi; /* Temporary highlight environment list_head */
/******************************************************************************/
/******************************************************************************
 *  +----------------+ 
 *  | +------------+ <---- edge       These are, essentially, a stacked
 *  | | +--------+ | |                set of panels.
 *  | | |        | <------ mantle
 *  | | |        | | |
 *  | | |        <-------- core
 *  | | +--------+ | |
 *  | +------------+ |
 *  +----------------+
 *
 * Generate terrain body; conforms to a three-tier pattern, typically with the 
 * 'edge' panel being animated with two frames. This was an early attempt to do
 * something that might have been superceded by gen_terrain().
 ******************************************************************************/
/* Move terrain highlights in a direction determined by the current wind
 * direction. */
void shift_highlights(PLATE *pl)
{
        int wind = get_wind(__dir__);

        GNODE *tmp;
        list_for_each(pl->gfx, tmp, node) {
                if (tmp->dim.layer != __hig__) continue;
                switch(wind) {
                case NORTH:     tmp->dim.yco++;
                                break;
                case NE:        tmp->dim.yco++;
                                tmp->dim.xco--;
                                break;
                case EAST:      tmp->dim.xco--;
                                break;
                case SE:        tmp->dim.yco--;
                                tmp->dim.xco--;
                                break;
                case SOUTH:     tmp->dim.yco--;
                                break;
                case SW:        tmp->dim.yco--;
                                tmp->dim.xco++;
                                break;
                case WEST:      tmp->dim.xco++;
                                break;
                case NW:        tmp->dim.yco++;
                                tmp->dim.xco++;
                                break;
                }
                tmp->dim.xco %= (COLS-tmp->dim.w);
                tmp->dim.xco =  (tmp->dim.xco<=0)?(COLS-tmp->dim.w):tmp->dim.xco;
                tmp->dim.yco %= LINES;
                move_panel(tmp->pan, tmp->dim.yco, tmp->dim.xco);
        }
        scr_refresh();
}
void draw_trees(PLATE *pl, DIMS *d)
{
        if (d->w < 3) return;

        GNODE *tree = new_gnode(__sec__, d->w-1, d->w, d->y0, d->x0, 1);
        WNODE *treew = tree->W;

        wbkgrnd(treew->window, &TREE[1]);

        mvwhline_set(treew->window, d->w-2, 0, &TREE[0], d->w);
        LINCREMENT(pl, gfx, &tree->node);
}
        

void draw_chunk(PLATE *pl, DIMS *d)
{
        /* The new terrane gets its own GNODE that will be strung
         * on to the GNODE ring of the PLATE. */
        GNODE *top  = new_gnode(__top__, d->w, d->w, d->y0,   d->x0, 1);
        GNODE *drp  = new_gnode(__drp__, 1,    d->w, d->ymax,  d->x0, 1);
        WNODE *topw = top->W;
        WNODE *drpw = drp->W;

        /* The shore effects will be drawn on the 'bgr' GNODE, ensuring
         * that it always stays at a z-level below that of the new terrane. */
        GNODE *bgr; 
        MATCH_FIELD(pl->gfx, bgr, dim.layer, __bgr__);
        WNODE *se1 = bgr->W;
        bgr->next(bgr);
        WNODE *se2 = bgr->W;

        /* Supply a background */
        wbkgrnd(topw->window, &OCEAN[0]);
        wbkgrnd(drpw->window, &MTN[2]);

        int shoreWEST = d->x0-1;
        int shoreBOTW = d->w+2;
        int shoreEAST = d->xmax;

        int i, j, yPLUSj;

        for (j=0; j<(d->w); j++) {
                /* Shore effects on the left side of the terrane */
                yPLUSj = (d->y0+j+1);
                mvwadd_wch(se1->window, yPLUSj, shoreWEST, &OCEAN[3]);
                mvwadd_wch(se2->window, yPLUSj, shoreWEST, &OCEAN[2]);
                /* Shore effects on the left side of the terrane */
                mvwadd_wch(se1->window, yPLUSj, shoreEAST, &OCEAN[3]);
                mvwadd_wch(se2->window, yPLUSj, shoreEAST, &OCEAN[2]);
                for (i=0; i<(d->w); i++) {
                        /* The body of the terrane */
                        mvwadd_wch(topw->window, j, i, &MTN[1]);
                } 
        }
        /* Shore effects on the bottom of the terrane */
        mvwhline_set(se1->window, yPLUSj+1, d->x0-1, &OCEAN[3], d->w+2);
        mvwhline_set(se2->window, yPLUSj+1, d->x0-1, &OCEAN[2], d->w+2);

        list_add(pl->gfx, &top->node);
        list_add(pl->gfx, &drp->node);

        if (flip_biased(0.5)) draw_trees(pl, d);
}

void simpledraw(PLATE *pl)
{
        int x, y, i, j;
        int h = pl->dim.h;
        int w = pl->dim.w;

        /* The PLATE's Perlin map */
        double **pmap = pl->pmap;
        /* A seed cell scans the surrounding chunk^2 cells */
        int chunk = 6; 
        /* Threshhold value for the Perlin map */
        double seed = 0.95;

        DIMS *d = malloc(sizeof(DIMS));

        /* Scan the character array in row-major order */
        for (y=0; y<h; y++) {
        for (x=0; x<w; x++) {

                /* If the cell's noise value exceeds seed thresh. */
                if (pmap[y][x] > seed) {

                        d->y0 = y;
                        d->x0 = x;

                        /* Jiggle that chunk */
                        if (flip_biased(0.56))   chunk += chunk/3;
                        else                     chunk -= chunk/3;

                        if ((chunk < 1)) continue;

                        d->w = chunk;
                        d->h = chunk;
                        d->xmax = d->x0 + d->w;
                        d->ymax = d->y0 + d->w;
                        
                        draw_chunk(pl, d);
                }
        }
        }
        restack(pl);
}
