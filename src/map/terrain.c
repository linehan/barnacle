/* 
 * terrain.c -- Generalized "light source" objects.
 *
 * These routines perform "terrain generation" by traversing a matrix
 * structure and labeling components based on a comparison criteria.
 * When labeling is complete, future traversals can apply an arbitrary
 * action based on the value of the label, including the alteration of
 * the label. 
 *
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <stdlib.h>
#include "../lib/matrix.h"
#include "../lib/stoc/stoc.h"
#include "../eng/bytes.h"
#include "../eng/error.h"
#include "../gfx/ui/titlecard.h"
#include "../test/test.h"
#include "terrain.h"
#include "tile.h"


/*
 * Perform a series of low-pass filters on the perlin noise matrix
 * contained in 'map'. The goal is to reduce jitter and consolodate
 * areas of similar noise value into "blob enough for land". 
 */
void smooth_layers(struct map_t *map)
{
        int ymax;
        int xmax; 

        /* Set the loop (pmap) boundaries */
        ymax = pos_boxh(map->pos) - 1;         
        xmax = pos_boxw(map->pos) - 1;

        /* Smooth the Perlin noise map over a given number of octaves */
        perlin_smooth(map->pmap, ymax, xmax, PERSIST, OCTAVES);

        /* Now, target specific threshold values for smoothing. */
        smooth_cycle(map->pmap, ymax, xmax, TERRA, SMOOTH_BO, 3);
        smooth_cycle(map->pmap, ymax, xmax, BEACH, SMOOTH_BO, 3);
        smooth_cycle(map->pmap, ymax, xmax, SHOAL, SMOOTH_BO, 3);
        smooth_cycle(map->pmap, ymax, xmax, BEACH, SMOOTH_BO, 1);
        smooth_cycle(map->pmap, ymax, xmax, TERRA, SMOOTH_BO, 3);
}


/*
 * Traverse the screen, laying the appropriate tiles based on the 
 * corresponding values in the Perlin map. Each coordinate pair 
 * receives a graphical tile and a label which is set in the state 
 * word of the map tree.
 */
void label_regions(struct map_t *map)
{
        struct seed_t seed = {{0},0};
        int i;
        int j;

        for (i=0; i<pos_h(map->pos); i++) {
        for (j=0; j<pos_w(map->pos); j++) {

                mx_seed(map->tile, i, j, &seed);

                if (map->pmap[i][j] < BEACH) place_ocean_label(seed.cur); else
                if (map->pmap[i][j] < TERRA) place_beach_label(seed.cur); else
                                             place_terra_label(seed.cur);
        }
        }
}


/*
 * Now that the initial pass is complete, identify edges which need 
 * to be changed to carry a perspective label. These are usually the 
 * "bottom" edges of certain layers, in this case, TOP. 
 */
void label_cliffs(struct map_t *map)
{
        struct seed_t seed;
        int i;
        int j;

        for (i=0; i<pos_h(map->pos); i++) {
        for (j=0; j<pos_w(map->pos); j++) {

                mx_seed(map->tile, i, j, &seed);

                if (LABEL(seed.cur, TOP) && !LABEL(seed.s, TOP))
                        place_cliff_label(seed.cur);
        }
        }
}


/*
 * Overwrite TOP labels with TTO labels, based on a size heuristic 
 * (the TOP region must meet a minimum area requirement)
 */
void label_treetops(struct map_t *map)
{
        struct seed_t seed;
        int i;
        int j;

        for (i=0; i<pos_h(map->pos); i++) {
        for (j=0; j<pos_w(map->pos); j++) {

                mx_seed(map->tile, i, j, &seed);

                if (!LABEL(seed.cur, TOP)
                ||  !LABEL(seed.n,   TOP)
                ||  !LABEL(seed.s,   TOP) 
                ||  !LABEL(seed.e,   TOP)
                ||  !LABEL(seed.w,   TOP)
                ||  !LABEL(seed.nw,  TOP)
                ||  !LABEL(seed.ne,  TOP)
                ||  !LABEL(seed.sw,  TOP)
                ||  !LABEL(seed.se,  TOP)) {
                        continue;
                } else {
                        place_treetop_label(seed.cur);
                        place_treetop_label(seed.n);
                        place_treetop_label(seed.s);
                        place_treetop_label(seed.e);
                        place_treetop_label(seed.w);
                        place_treetop_label(seed.nw);
                        place_treetop_label(seed.ne);
                        place_treetop_label(seed.sw);
                        place_treetop_label(seed.se);
                }
        }
        }
}


/*
 * Remove TTO tiles which have been placed at the edges of a TOP 
 * region, so that they don't look like shit.
 */
void label_treetops_trim(struct map_t *map)
{
        struct seed_t seed;
        int i;
        int j;

        for (i=0; i<pos_h(map->pos); i++) {
        for (j=0; j<pos_w(map->pos); j++) {

                mx_seed(map->tile, i, j, &seed);

                if (LABEL(seed.cur, TTO) 
                && (!LABEL(seed.w,  TTO, TOP) 
                ||  !LABEL(seed.e,  TTO, TOP)
                ||  !LABEL(seed.n,  TTO, TOP)
                ||  !LABEL(seed.s,  TTO, TOP)
                ||  !LABEL(seed.sw, TTO, TOP)
                ||  !LABEL(seed.se, TTO, TOP)
                ||  !LABEL(seed.nw, TTO, TOP) 
                ||  !LABEL(seed.ne, TTO, TOP)))
                {
                        wipe_label(seed.cur);
                        place_terra_label(seed.cur);
                }
        }
        }
}


/*
 * One more pass, place TTR labels at the "bottom" edges of every TTO 
 * label (analagous to label_cliffs, see above). If the normal condition
 * isn't met, the loop suspects a single lonely TTO label, and if confirmed,
 * wipes it. 
 */
void label_treetrunks(struct map_t *map)
{
        struct seed_t seed;
        int i;
        int j;

        for (i=0; i<pos_h(map->pos); i++) {
        for (j=0; j<pos_w(map->pos); j++) {

                mx_seed(map->tile, i, j, &seed);

                if (LABEL(seed.cur, TTO) 
                &&  LABEL(seed.s,   TOP) 
                &&  LABEL(seed.n,   TTO)) {
                        wipe_label(seed.cur);
                        place_treetrunk_label(seed.cur);
                }
                else if (LABEL(seed.n,   TOP) 
                     &&  LABEL(seed.cur, TTO)
                     &&  LABEL(seed.s,   TOP)) {
                                wipe_label(seed.cur);
                                place_terra_label(seed.cur);
                }
        }
        }
}


/* 
 * Now we focus our attention on the edges of the SHOAL tiles,
 * that is, the threshold between the background "sea level" 
 * and any sort of tile at all. We want to make them look nice,
 * so we draw a special animated tile that can be stepped through
 * by the event loop.
 */
void label_shorelines(struct map_t *map)
{
        #define SURF_FRAMES 4

        struct seed_t seed;
        wchar_t *wch = NULL;
        short color;
        attr_t attr;
        int i;
        int j;
        int k; 
        int which;
        static wchar_t *shore[6]={L"⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘⠉⠑⠃⠘⠘", 
                                  L"⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠⡀⢀⢀⣀⢄⡄⢠⣀⢄⡄⢠", 
                                  L"⠁⠁⠂⠂⠁⠁⠂⠂⠄⡀⠃⠅⠆⠄⡀⠃⡂⠄⠃⠅⠆⡁⡂⡀⠃⠅⠆⡁⡂",
                                  L"⠈⠐⠠⢀⠘⠈⠐⠠⢀⠘⠨⠰⠰⢈⢈⢐⢠⢠⠨⠨⠰⠰⢈⢐⢠⠰⠰⢐⢠",
                                  L"⠁⠁⠂⠂⠁⠁⠂⠂⠁⠁⠂⠂⠁⠁⠂⠂⠁⠁⠂⠂⠂⠁⠁⠂⠂⠁⠁⠂⠂", 
                                  L"⠈⠐⠐⠘⠈⠘⠈⠑⠐⠐⠘⠈⠐⠑⠑⠈⠐⠘⠈⠐⠐⠑⠑⠈⠐⠑⠑⠈⠐" 
        };

        attr = 0;
        
        /*mx_foreach_seed(&seed, map->tile) {*/
        for (i=0; i<pos_h(map->pos); i++) {
        for (j=0; j<pos_w(map->pos); j++) {

                mx_seed(map->tile, i, j, &seed);

                /* Draw nothing if the cursor is on land */
                if ((LAYER(*seed.cur, 1, TOP))
                ||  (LAYER(*seed.cur, 1, DRP))
                ||  (LAYER(*seed.cur, 1, BEA))
                ||  (LAYER(*seed.cur, 1, TTO))
                ||  (LAYER(*seed.cur, 1, TTR)))
                        continue;

                if ((LAYER(*seed.nw, 1, OCN))
                &&  (LAYER(*seed.w, 1, TOP)))
                        continue;

                if ((LAYER(*seed.ne, 1, OCN))
                &&  (LAYER(*seed.e, 1, TOP)))
                        continue;

                /* Draw an edge if the following conditions apply... */

                if (LAYER(*seed.n, 1, DRP)) {
                        wch = shore[0];
                        color = _SEA_SHALLOW;
                }
                else if ((LAYER(*seed.e, 1, TOP))
                     ||  (LAYER(*seed.e, 1, DRP)))
                {
                        wch = shore[3];
                        color = SEA_MED;
                }
                else if ((LAYER(*seed.w, 1, TOP))
                     ||  (LAYER(*seed.w, 1, DRP)))
                {
                        wch = shore[2];
                        color = SEA_MED;
                }
                else if ((LAYER(*seed.nw, 1, TOP))
                     ||  (LAYER(*seed.nw, 1, DRP)))
                {
                        wch = shore[4];
                        color = SEA_MED;
                }
                else if ((LAYER(*seed.ne, 1, TOP))
                     ||  (LAYER(*seed.ne, 1, DRP)))
                {
                        wch = shore[5];
                        color = SEA_MED;
                }
                else {
                        continue; /* Arrest any wild ones */
                }

                for (k=0; k<SURF_FRAMES; k++) {
                        which = roll1d(10);
                        mvwcch(PEEK(map->L[RIM]), i, j, &wch[which], attr, color);
                        NEXT(map->L[RIM]);
                }
                set_byte(seed.cur, LAB, RIM);
        }
        }
}


