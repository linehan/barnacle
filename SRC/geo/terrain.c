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

void draw_atw(WINDOW *win, int x, int y, char tile)
{
        mvwadd_wch(win, x, y, get_tile(tile));
}

void new_terrain(struct list_head *head, char type, int h, int w, int y0, int x0)
{
        /* Apply landscape style */
        if ((type=='s')) {
                /* Create nested GNODEs */
                GNODE *edge = new_gnode(__an__, h, w, y0, x0, 2);
                GNODE *mant = new_gnode(__an__, (h-2), (w-2), (y0+1), (x0+1), 1);
                GNODE *core = new_gnode(__an__, (h-4), (w-4), (y0+2), (x0+2), 1);

                wbkgrnd(core->W->window, &SAND);
                wbkgrnd(mant->W->window, &SHORE);

                wbkgrnd(edge->W->window, &OCEAN[2]);
                edge->nextw(edge);
                wbkgrnd(edge->W->window, &OCEAN[1]);

                /* Add the GNODEs to the wad */
                list_add(head, &edge->node);
                list_add(head, &mant->node);
                list_add(head, &core->node);
        }
}
/* Allocate a new environment to store the terrain highlights */
void highlights_init(PLATE *pl)
{
        int i, w, wnoise, x0, xnoise, y0, ynoise;
        wnoise = 50;
        xnoise = 50;
        ynoise = 80;

        for (i=0; i<5; i++) {

                 w = roll_fair(wnoise);
                x0 = roll_fair(xnoise);
                y0 = roll_fair(ynoise);

                GNODE *new = new_gnode(__hi__, 1, w, y0, x0, 1);

                wbkgrnd(new->W->window, &OCEAN[2]);
                list_add(pl->hi, &new->node);
                vrt_refresh();
        }
}
/* Move terrain highlights in a direction determined by the current wind
 * direction. */
void shift_highlights(PLATE *pl)
{
        int wind = get_wind(__dir__);

        GNODE *tmp;
        list_for_each(pl->hi, tmp, node) {
                if ((((tmp->dim.x0)+(tmp->dim.w)) > COLS))
                        tmp->dim.x0 = 0;
                if (((tmp->dim.x0) < 0))
                        tmp->dim.x0 = COLS-(tmp->dim.w);
                if ((((tmp->dim.y0)+(tmp->dim.h)) > LINES))
                        tmp->dim.y0 = 0;
                if (((tmp->dim.y0) < 0))
                        tmp->dim.y0 = LINES;

                switch(wind) {
                case NORTH:
                        tmp->dim.y0 = (tmp->dim.y0+1);
                        break;
                case NE:
                        tmp->dim.y0 = (tmp->dim.y0+1);
                        tmp->dim.x0 = (tmp->dim.x0-1);
                        break;
                case EAST:
                        tmp->dim.x0 = (tmp->dim.x0-1);
                        break;
                case SE:
                        tmp->dim.y0 = (tmp->dim.y0-1);
                        tmp->dim.x0 = (tmp->dim.x0-1);
                        break;
                case SOUTH:
                        tmp->dim.y0 = (tmp->dim.y0-1);
                        break;
                case SW:
                        tmp->dim.y0 = (tmp->dim.y0-1);
                        tmp->dim.x0 = (tmp->dim.x0+1);
                        break;
                case WEST:
                        tmp->dim.x0 = (tmp->dim.x0+1);
                        break;
                case NW:
                        tmp->dim.y0 = (tmp->dim.y0+1);
                        tmp->dim.x0 = (tmp->dim.x0+1);
                        break;
                }
                move_panel(tmp->pan, tmp->dim.y0, tmp->dim.x0);
                vrt_refresh();
        }
        scr_refresh();
}

void draw_chunk(PLATE *pl, DIMS *d)
{
        /* The new terrane gets its own GNODE that will be strung
         * on to the GNODE ring of the PLATE. */
        GNODE *top  = new_gnode(__top__, d->w, d->w, d->y0, d->x0, 1);
        GNODE *drp  = new_gnode(__drp__, 1,    d->w, d->y,  d->x0, 1);
        WNODE *topw = top->W;
        WNODE *drpw = drp->W;

        /* The shore effects will be drawn on the 'trim' GNODE, ensuring
         * that it always stays at a z-level below that of the new terrane. */
        WNODE *se1 = pl->trim->W;
        pl->trim->nextw(pl->trim);
        WNODE *se2 = pl->trim->W;

        /* Supply a background */
        wbkgrnd(topw->window, &OCEAN[0]);
        wbkgrnd(drpw->window, &MTN[2]);
        vrt_refresh();

        int shoreWEST = d->x0 - 1;
        int shoreBOTW = d->w + 2;
        int shoreEAST = d->x;

        int i, j, yPLUSj;

        for (j=0; j<(d->w); j++) {
                /* Shore effects on the left side of the terrane */
                yPLUSj = (d->y0+j+1);
                mvwadd_wch(se1->window, yPLUSj, shoreWEST, &OCEAN[3]);
                mvwadd_wch(se2->window, yPLUSj, shoreWEST, &OCEAN[2]);
                /* Shore effects on the left side of the terrane */
                mvwadd_wch(se1->window, yPLUSj, shoreEAST, &OCEAN[3]);
                mvwadd_wch(se2->window, yPLUSj, shoreEAST, &OCEAN[2]);
                vrt_refresh();
                for (i=0; i<(d->w); i++) {
                        /* The body of the terrane */
                        mvwadd_wch(topw->window, j, i, &MTN[1]);
                        vrt_refresh();
                } 
        }
        /* Shore effects on the bottom of the terrane */
        mvwhline_set(se1->window, yPLUSj+1, d->x0-1, &OCEAN[3], d->w+2);
        mvwhline_set(se2->window, yPLUSj+1, d->x0-1, &OCEAN[2], d->w+2);
        vrt_refresh();

        list_add(pl->gfx, &top->node);
        list_add(pl->gfx, &drp->node);
}


void unfuck_perspective(PLATE *pl)
{
        GNODE *tmp;
        list_for_each(pl->gfx, tmp, node) {
                if (tmp->id == __top__)
                        top_panel(tmp->pan);
        }
}
                
void simpledraw(PLATE *pl)
{
        int x, y, i, j;
        int h = pl->noise->h;
        int w = pl->noise->w;
        pl->setme = 1;

        /* The PLATE's Perlin map */
        double **pmap = pl->noise->box;
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
                        d->x = d->x0 + d->w;
                        d->y = d->y0 + d->w;
                        
                        draw_chunk(pl, d);
                }
        }
        }
        unfuck_perspective(pl);
}
