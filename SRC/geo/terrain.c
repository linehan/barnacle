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
        int i, j;
        int tx0, ty0, th, tw, imax, jmax;

        static signed int xoffset = 1;
        static signed int yoffset = -1;
        static signed int htrim = -1;
        static signed int wtrim = -2;

        if (d->w < 4) return;

        /* The width of the trees should be at least two less than the
         * width of the land they stand on, i.e., leaving at least one
         * char of padding on each side. */
        tx0 = d->x0+xoffset;
        ty0 = d->y0+yoffset;
        th = d->h+htrim;
        tw = d->w+wtrim;
        imax = ty0+th;
        jmax = tx0+tw;

        for (i=ty0; i<imax; i++) {
        for (j=tx0; j<jmax; j++) {
                mvwadd_wch(pl->env[__trt__]->W->window, i, j, &TREE[1]);
                SET_BIT(pl->env[__trt__]->bb, i, j);
        }
        }
        SET_BITLINE(pl->env[__trd__]->bb, imax, tx0, tw);
        mvwhline_set(pl->env[__trd__]->W->window, imax, tx0, &TREE[0], tw);
}

void draw_chunk(PLATE *pl, DIMS *d, int terrain)
{
        int i, j, imax, jmax;
        GNODE *top, *drp;

        cchar_t *bgtop = get_tile(terrain); /* Get background tiles */

        top = pl->env[__grt__];
        drp = pl->env[__drd__];
        imax = d->y0+d->h;
        jmax = d->x0+d->w;
        /* Draw the top of terrane */
        for (i=d->y0; i<imax; i++) {
        for (j=d->x0; j<jmax; j++) {
                mvwadd_wch(top->W->window, i, j, bgtop);
                SET_BIT(top->bb, i, j);
        } 
        } /* Draw the drop */
        SET_BITLINE(drp->bb, imax, d->x0, d->w);
        mvwhline_set(drp->W->window, imax, d->x0, &MTN[2], d->w);

        if (flip_biased(0.5))
                draw_trees(pl, d);
}

void simpledraw(PLATE *pl)
{
        int i, j, imax, jmax; /* Loop coordinate variables */
        int chunk, chunkmin;  /* Chunk!! CHUNK! */
        double **pmap, seed;  /* Perlin map stuff */

        pmap = pl->pmap; /* The PLATE's Perlin map */
        seed = 0.95;     /* Threshhold value for the Perlin map */
        chunk = 6;
        chunkmin = 3;
        imax = LINES-chunkmin;
        jmax = COLS-chunkmin;

        DIMS d;

        /* Uses the starting coordinates as the increment values to save
         * assignment time and space. */
        for (d.y0=0; d.y0<imax; d.y0++) {
        for (d.x0=0; d.x0<jmax; d.x0++) {

                if (pmap[d.y0][d.x0] > seed) {

                        /* Jiggle that chunk */
                        if (flip_biased(0.56))   chunk += chunk/3;
                        else                     chunk -= chunk/3;

                        if ((chunk < 2)) continue; /* this tall to ride */

                        d.w = chunk;
                        d.h = chunk;
                        d.ymax = d.y0+d.h;
                        d.xmax = d.x0+d.w;
                        /* Trim edges */
                        if (d.xmax > COLS) d.w = d.xmax-COLS;
                        if (d.ymax > LINES) d.h = d.ymax-LINES;

                        draw_chunk(pl, &d, __Gra__);
                }
        }
        }
}

/*
 *     111     100   011
 *     111 XOR 100 = 011 
 *     111     100   011
 */
void draw_water_rim(PLATE *pl)
{
        int i, j, n;
        int pad, padl, padr, padu, padd;

        pad = 1;
        n = 5;

        GNODE *top, *drp;
        top = pl->env[__grt__];
        drp = pl->env[__drd__];

        WINDOW *rimwin1, *rimwin2;
        rimwin1 = pl->env[__rim__]->W->window;
                  pl->env[__rim__]->next(pl->env[__rim__]);
        rimwin2 = pl->env[__rim__]->W->window;

        for (i=1; i<LINES; i++) {
                padu = i-1;
                padd = i+1;
        for (j=1; j<COLS-1; j++) {
                padl=j-1;
                padr=j+1;
                        /* If I am part of layer 'top' or layer 'drp', loop */
                        if (BIT_SET(top->bb, (i), (j))) continue;
                        if (BIT_SET(drp->bb, (i), (j))) continue;

                        n = 1;
                        do {
                                switch (n) {
                                case 0:
                                if (BIT_SET(top->bb, (i), (padl))) break;
                                if (BIT_SET(top->bb, (padu), (padr))) break;
                                if (BIT_SET(top->bb, (padu), (padl))) break;
                                if (BIT_SET(drp->bb, (padu), (j))) break;
                                if (BIT_SET(drp->bb, (i), (padr))) break;
                                if (BIT_SET(drp->bb, (i), (padl))) break;
                                n = 2;
                                case 1:
                                mvwadd_wch(rimwin1, i, j, &OCEAN[3]);
                                mvwadd_wch(rimwin2, i, j, &OCEAN[2]);
                                break;
                        
                        /*}*/
                        /*[> If the bit in the row above me is set <]*/
                        /*if (BIT_SET(drp->bb, (i-1), (j))) {*/
                                /*mvwadd_wch(rimwin1, i, j, &OCEAN[3]);*/
                                /*mvwadd_wch(rimwin2, i, j, &OCEAN[2]);*/
                                /*continue;*/
                        /*}*/
                        /*[> If the bit in the row above me is set <]*/
                        /*if (BIT_SET(drp->bb, (i-1), (j))) {*/
                                /*mvwadd_wch(rimwin1, i, j, &OCEAN[3]);*/
                                /*mvwadd_wch(rimwin2, i, j, &OCEAN[2]);*/
                                /*continue;*/
                        /*}*/
                        /*[> If the bit in the row above me is set <]*/
                        /*if (BIT_SET(drp->bb, (i-1), (j))) {*/
                                /*mvwadd_wch(rimwin1, i, j, &OCEAN[3]);*/
                                /*mvwadd_wch(rimwin2, i, j, &OCEAN[2]);*/
                                /*continue;*/
                        /*}*/
        }
        }
}

void testworld(int size_factor, int testtype)
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
        switch(testtype) {
        case 0: /* Print latitude */
                wprintw(DIAGNOSTIC_WIN, "\n%dhx%dw world latitude\n", h, w);
                for (lat=0; lat<h; lat++) {
                        wprintw(DIAGNOSTIC_WIN, "\n");
                        for (lon=0; lon<w; lon++) {
                                wprintw(DIAGNOSTIC_WIN, "%02d", row[lat][lon].lat);
                        }
                }
                break;
        case 1: /* Print temperature */
                wprintw(DIAGNOSTIC_WIN, "\n%dhx%dw world temperature\n", h, w);
                for (lat=0; lat<h; lat++) {
                        wprintw(DIAGNOSTIC_WIN, "\n");
                        for (lon=0; lon<w; lon++) {
                                wprintw(DIAGNOSTIC_WIN, "%02d", row[lat][lon].temp);
                        }
                }
                break;
        case 2: /* Print wind direction */
                wprintw(DIAGNOSTIC_WIN, "\n%dhx%dw world wind direction\n", h, w);
                for (lat=0; lat<h; lat++) {
                        wprintw(DIAGNOSTIC_WIN, "\n");
                        for (lon=0; lon<w; lon++) {
                                wprintw(DIAGNOSTIC_WIN, "%02d", row[lat][lon].wind);
                        }
                }
                break;
        case 3: /* Print convection cell species */
                wprintw(DIAGNOSTIC_WIN, "\n%dhx%dw world convection cell species\n", h, w);
                for (lat=0; lat<h; lat++) {
                        wprintw(DIAGNOSTIC_WIN, "\n");
                        for (lon=0; lon<w; lon++) {
                                wprintw(DIAGNOSTIC_WIN, "%c", row[lat][lon].celltype);
                        }
                }
                break;
        case 4: /* Print convection cell gradients */
                wprintw(DIAGNOSTIC_WIN, "\n%dhx%dw world convection cell gradients\n", h, w);
                for (lat=0; lat<h; lat++) {
                        wprintw(DIAGNOSTIC_WIN, "\n");
                        for (lon=0; lon<w; lon++) {
                                wprintw(DIAGNOSTIC_WIN, "%d", row[lat][lon].cellgrade);
                        }
                }
                break;
        }
}
