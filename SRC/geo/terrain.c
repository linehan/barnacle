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

#include "terrain.h"
#include "weather.h"
#include "map.h"

#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../gen/dice.h"
#include "../lib/morton.h"
#include "../pan/test.h"
#include "../lib/ufo.h"
//##############################################################################
/* Move terrain highlights in a direction determined by the current wind
 * direction. */
/*void shift_highlights(PLATE *pl)*/
/*{*/
        /*int wind = get_wind(__dir__);*/

        /*GNODE *tmp;*/
        /*list_for_each(pl->gfx, tmp, node) {*/
                /*if (tmp->dim.layer != __hig__) continue;*/
                /*switch(wind) {*/
                /*case NORTH:     tmp->dim.yco++;*/
                                /*break;*/
                /*case NE:        tmp->dim.yco++;*/
                                /*tmp->dim.xco--;*/
                                /*break;*/
                /*case EAST:      tmp->dim.xco--;*/
                                /*break;*/
                /*case SE:        tmp->dim.yco--;*/
                                /*tmp->dim.xco--;*/
                                /*break;*/
                /*case SOUTH:     tmp->dim.yco--;*/
                                /*break;*/
                /*case SW:        tmp->dim.yco--;*/
                                /*tmp->dim.xco++;*/
                                /*break;*/
                /*case WEST:      tmp->dim.xco++;*/
                                /*break;*/
                /*case NW:        tmp->dim.yco++;*/
                                /*tmp->dim.xco++;*/
                                /*break;*/
                /*}*/
                /*tmp->dim.xco %= (COLS-tmp->dim.w);*/
                /*tmp->dim.xco =  (tmp->dim.xco<=0)?(COLS-tmp->dim.w):tmp->dim.xco;*/
                /*tmp->dim.yco %= LINES;*/
                /*move_panel(tmp->pan, tmp->dim.yco, tmp->dim.xco);*/
        /*}*/
        /*scr_refresh();*/
/*}*/
//##############################################################################
//# Where the magic happens - draw the graphics layers based on generated
//# Perlin simplex noise and some coin flips. 
//##############################################################################
void draw_layers(struct map_t *map, double **pmap)
{
        int i, j;
        uint32_t z;           // For computing Morton codes
        int imax, jmax;       // loop boundaries for ground box
        int ymax, xmax;       // window boundaries
        int x0, y0, tx0, ty0; // initial position of ground and tree boxes
        int w, h, tw, th;     // width and height of ground and tree boxes
        int chunk, chunkmin;  // unit of land to draw at a time
        int lastchunk;        // value of the previous chunk
        double seed;          // perlin noise (elevation) values

        seed     = 0.95; // threshold value to determine whether to draw
        chunk    = 6;
        chunkmin = 3;
        ymax     = ((map->h)-1)-chunkmin; // beware the fencepost error
        xmax     = ((map->w)-1)-chunkmin;

        cchar_t *bgtop;            // background sprite
        bgtop = get_tile(__Gra__);

        for (y0=0; y0<ymax; y0++) {
                for (x0=0; x0<xmax; x0++) {

                        if (pmap[y0][x0] > seed) {

                                lastchunk = chunk;

                                if (flip_biased(0.57))   
                                        chunk += 1;
                                else    
                                        chunk -= 1;

                                chunk = (chunk < chunkmin) ? lastchunk : chunk;

                                h = w = chunk; // square
                                imax = y0+h;
                                jmax = x0+w;
                                if (imax > map->h) h = imax-map->h;
                                if (jmax > map->w) w = jmax-map->w;

                                // Draw the ground box
                                for (i=y0; i<=imax; i++) {
                                        for (j=x0; j<jmax; j++) {
                                                MORT(i, j, &z);
                                                if (i == imax) {
                                                        mvwadd_wch(PEEK(map->L[DRP]), i, j, &MTN[2]);
                                                        set_cell(map->tree, z, LAY, DRP);
                                                        set_cell(map->tree, z, SED, LIME);
                                                        set_cell(map->tree, z, SOI, MOLL);
                                                        continue;
                                                }
                                                mvwadd_wch(PEEK(map->L[TOP]), i, j, bgtop); // top
                                                set_cell(map->tree, z, LAY, TOP);
                                                set_cell(map->tree, z, SED, LIME);
                                                set_cell(map->tree, z, SOI, MOLL);
                                        }
                                }
                                // Draw the tree box
                                if ((flip_biased(0.5))||(w < 4)) continue;
                                tx0  = x0+1;
                                ty0  = y0-1;
                                th   = h-1;
                                tw   = w-2;
                                imax = ty0+th; // recalculation
                                jmax = tx0+tw; // recalculation
                                for (i=ty0; i<=imax; i++) {
                                        for (j=tx0; j<jmax; j++) {
                                                MORT(i, j, &z);
                                                if (i == imax) {
                                                        set_cell(map->tree, z, LAY, VEG);
                                                        set_cell(map->tree, z, SED, LIME);
                                                        set_cell(map->tree, z, SOI, MOLL);
                                                        mvwadd_wch(PEEK(map->L[VEG]), i, j, &TREE[0]);
                                                        continue;
                                                }
                                                mvwadd_wch(PEEK(map->L[VEG]), i, j, &TREE[1]);
                                                set_cell(map->tree, z, LAY, VEG);
                                                set_cell(map->tree, z, SED, LIME);
                                                set_cell(map->tree, z, SOI, SPOD);
                                        }
                                }
                        }
                        mvwadd_wch(PEEK(map->L[BGR]), y0, x0, &OCEAN[0]);
                }
        }

}


void draw_water_rim(struct map_t *map)
{
        int i, j;             // Incrementors
        int iu, jl, jr;       // Incrementors for up, left, and right.
        uint32_t z;           // For computing current Morton code
        uint32_t zu, zl, zr;  // Morton code for cells above, left, and right
        uint32_t zul, zur;    // Check if at top corners
        WINDOW *rim1, *rim2;  // Where to draw seashore animation.

        rim1 = PEEK(map->L[RIM]);
               NEXT(map->L[RIM]);
        rim2 = PEEK(map->L[RIM]);

        for (i=1; i<(map->h); i++) {
                for (j=1; j<(map->w); j++) {

                        MORT(i, j, &z);

                        // Draw nothing is the cursor is on land.
                        if ((is_cell(map->tree, z, LAY, TOP))) continue;
                        if ((is_cell(map->tree, z, LAY, DRP))) continue;

                        // Compute offsets
                        iu = (i-1);
                        jl = (j-1);
                        jr = (j+1);

                        // Compute Morton codes
                        MORT(iu, j, &zu);
                        MORT(i, jr, &zr);
                        MORT(i, jl, &zl);
                        MORT(iu, jl, &zul);
                        MORT(iu, jr, &zur);

                        // Draw nothing if the cursor is at a top corner.
                        if (  (is_cell(map->tree, zul, LAY, XXX))
                            &&(is_cell(map->tree, zl,  LAY, TOP))) continue;
                        if (  (is_cell(map->tree, zur, LAY, XXX))
                            &&(is_cell(map->tree, zr,  LAY, TOP))) continue;

                        // Draw an edge if there is an edge in the directions.
                        if (  (is_cell(map->tree, zu, LAY, TOP))
                            ||(is_cell(map->tree, zu, LAY, DRP))
                            ||(is_cell(map->tree, zr, LAY, TOP))
                            ||(is_cell(map->tree, zr, LAY, DRP))
                            ||(is_cell(map->tree, zl, LAY, TOP))
                            ||(is_cell(map->tree, zl, LAY, DRP)))
                        {
                                mvwadd_wch(rim1, i, j, &OCEAN[3]);
                                mvwadd_wch(rim2, i, j, &OCEAN[2]);
                                set_cell(map->tree, z, LAY, RIM);
                        }
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
