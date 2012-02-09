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

        GNODE *tree = new_gnode(__sec__, d->w, d->w-2, d->y0-1, d->x0+1, 1);
        WNODE *treew = tree->W;

        wbkgrnd(treew->window, &TREE[1]);

        mvwhline_set(treew->window, d->w-1, 0, &TREE[0], d->w-2);
        LINCREMENT(pl, gfx, &tree->node);
}
        

void draw_chunk(PLATE *pl, DIMS *d, int terrain)
{
        cchar_t *bgtop = get_tile(terrain);
        /*cchar_t *bgdrp = get_tile(terrain);*/

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
                        mvwadd_wch(topw->window, j, i, bgtop);
                } 
        }
        /* Shore effects on the bottom of the terrane */
        mvwhline_set(se1->window, yPLUSj+1, d->x0-1, &OCEAN[3], d->w+2);
        mvwhline_set(se2->window, yPLUSj+1, d->x0-1, &OCEAN[2], d->w+2);

        list_add(pl->gfx, &top->node);
        list_add(pl->gfx, &drp->node);

        if (flip_biased(0.5)) {
                draw_trees(pl, d);
                /*wbkgrnd(topw->window, &TREE[2]);*/
        }
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
                        
                        draw_chunk(pl, d, __Gra__);
                }
        }
        }
        restack(pl);
}

void testworld(int size_factor)
{
        /* DejaVu Mono, the font this program is designed to use, has an
         * aspect ratio of 0.55, with an x-width of 0.54, at 100pt.
         * That is, a given character -- or more precisely, the uniform
         * rectangle that holds each individual glyph -- is a little over
         * half as wide as it is tall.
         *
         * The plate carree or equirectangular projection that we use as
         * the basis for our world map is bounded by a rectangle with an
         * aspect ratio of 2:1, that is, twice as wide as it is tall.
         * 
         * We must determine, then, the appropriate aspect ratio of a 2:1
         * equirectangular figure, with the stipulation that each of its
         * component rectangles has an individual aspect ratio of .55:1.
         *
         * The math is straightforward:  2/.55 = ~3.6
         *
         * In other words, for a given map, we need 3.6 units of width for 
         * every unit of height, as long as we are using DejaVu Mono and 
         * the venerable plate carree. 
         *
         * In addition, the height value should be a multiple of 6, because
         * there are six major climate zones, or rather three, symmetric
         * across the equator: polar, subtropical, and tropical. Ideally,
         * each of these should be represented by at least one PLATE each. 
         *
         * So we find the LCM of 3.6 and 6, which is 18. */
        int w = size_factor * 18;
        int h = size_factor * 6;

        /* Then we get the hemisphere's dimensions */
        int hemiw = w/2;
        int hemih = h/2;
        int tal; /* antipodal latitude */

        int cellspan = h/6; /* How many PLATEs per hadley cell */
        int cellgrade = 0;
        int cellcount = 0;

        int cardpts = 16; /* remember */

        CLIM *row[20];

        /* We only need to really calculate one hemisphere for certain climate
         * variables. Once we have one hemisphere, we can assign their mirror
         * image in the opposite hemisphere. */
        int lat, lon;
        for (lat=0; lat<hemih; lat++) {
                tal = (h-1)-lat; /* fencepost magic numbers */
                row[lat] = malloc(w * sizeof(CLIM));
                row[tal] = malloc(w * sizeof(CLIM));
                cellgrade = lat%cellspan;
                cellcount += ((lat != 0)&&(cellgrade == 0)) ? 1 : 0;
                for (lon=0; lon<w; lon++) {
                        /* Assign latitude value */
                        row[lat][lon].lat = lat;
                        row[tal][lon].lat = tal;
                        /* Number of PLATEs in a circulation cell */
                        row[lat][lon].cellspan = cellspan;
                        row[tal][lon].cellspan = cellspan;
                        /* Temperature (depends on latitude and is symmetric */
                        row[lat][lon].temp = lat*3;
                        row[tal][lon].temp = row[lat][lon].temp;
                        /* The type of circulation cell (depends on lat.) */
                        switch (cellcount) {
                        case 0: row[lat][lon].celltype = 'p';/*polar cell*/
                                row[tal][lon].celltype = 'p';
                                row[lat][lon].cellgrade = (cellspan-cellgrade)-1;
                                row[tal][lon].cellgrade = (cellspan-cellgrade)-1;
                                break;
                        case 1: row[lat][lon].celltype = 'f';/*ferrel cell*/
                                row[tal][lon].celltype = 'f';
                                /* circulation is reversed, look it up */
                                row[lat][lon].cellgrade = cellgrade;
                                row[tal][lon].cellgrade = cellgrade;
                                break;
                        case 2: row[lat][lon].celltype = 'h';/*hadley cell*/
                                row[tal][lon].celltype = 'h';
                                row[lat][lon].cellgrade = (cellspan-cellgrade)-1;
                                row[tal][lon].cellgrade = (cellspan-cellgrade)-1;
                                break;
                        }
                }
        }
        int lat_grade, tal_grade; /* The cell gradient values */
        /*int tal;                  [> The antipodal latitude <]*/
        for (lat=0; lat<hemih; lat++) {
                tal = (h-1)-lat; /* fencepost magic numbers */
                for (lon=0; lon<w; lon++) {
                        lat_grade = row[lat][lon].cellgrade;
                        tal_grade = row[tal][lon].cellgrade;
                        switch (row[lat][lon].celltype) {
                        case 'h': /* In a Hadley cell, surface flow is directed
                                   * toward the equator and deflected west due
                                   * to the Coriolis effect. A polar cell is a
                                   * type of Hadley cell. */
                        case 'p': /* In a polar cell, surface flow is directed
                                   * toward the equator and deflected west due
                                   * to the Coriolis effect. */
                                row[lat][lon].wind = (WEST-lat_grade)%cardpts;
                                row[tal][lon].wind = (WEST+tal_grade)%cardpts;
                                break;
                        case 'f': /* In a Ferrel cell, surface flow is directed
                                   * toward the hemispheric pole, drawn by the
                                   * low of the polar vortex, and deflected 
                                   * east due to the Coriolis effect.*/
                                row[lat][lon].wind = (EAST+lat_grade)%cardpts;
                                row[tal][lon].wind = (EAST-tal_grade)%cardpts;
                                break;
                        }
                }
        }
        /*for (lat=0; lat<h; lat++) {*/
                /*wprintw(BIGWIN, "\n");*/
                /*for (lon=0; lon<w; lon++) {*/
                        /*wprintw(BIGWIN, "%02d", row[lat][lon].lat);*/
                /*}*/
        /*}*/
        wprintw(BIGWIN, "\n\n\n");
        for (lat=0; lat<h; lat++) {
                wprintw(BIGWIN, "\n");
                for (lon=0; lon<w; lon++) {
                        wprintw(BIGWIN, "%02d", row[lat][lon].temp);
                }
        }
        wprintw(BIGWIN, "\n\n\n");
        for (lat=0; lat<h; lat++) {
                wprintw(BIGWIN, "\n");
                for (lon=0; lon<w; lon++) {
                        wprintw(BIGWIN, "%02d", row[lat][lon].wind);
                }
        }
        wprintw(BIGWIN, "\n\n\n");
        for (lat=0; lat<h; lat++) {
                wprintw(BIGWIN, "\n");
                for (lon=0; lon<w; lon++) {
                        wprintw(BIGWIN, "%c", row[lat][lon].celltype);
                }
        }
        /*wprintw(BIGWIN, "\n\n\n");*/
        /*for (lat=0; lat<h; lat++) {*/
                /*wprintw(BIGWIN, "\n");*/
                /*for (lon=0; lon<w; lon++) {*/
                        /*wprintw(BIGWIN, "%d", row[lat][lon].cellgrade);*/
                /*}*/
        /*}*/
}




