/* 
 * terrain.c
 *
 * These routines perform "terrain generation" by traversing a matrix
 * structure and labeling components based on a comparison criteria.
 * When labeling is complete, future traversals can apply an arbitrary
 * action based on the value of the label, including the alteration of
 * the label. 
 */

#include <stdlib.h>
#include "../lib/matrix.h"
#include "../lib/stoc/stoc.h"
#include "../eng/bytes.h"
#include "../gfx/ui/titlecard.h"
#include "../test/test.h"
#include "terrain.h"
#include "tile.h"

/* -------------------------------------------------------------------------- */

/* Holdunter yer buttz... */
void smooth_layers(struct map_t *map);
void label_regions(struct map_t *map);
void label_cliffs(struct map_t *map);
void label_treetops(struct map_t *map);
void label_treetops_trim(struct map_t *map);
void label_treetrunks(struct map_t *map);
void label_shorelines(struct map_t *map);

/* -------------------------------------------------------------------------- */



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
        ymax = map->ufo.box.h - 1;         
        xmax = map->ufo.box.w - 1;

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
        struct seed_t seed;
        int y;
        int x;

        for (y=0; y<map->ufo.box.h-1; y++) {
        for (x=0; x<map->ufo.box.w-1; x++) {

                seed.cur = mx_get(map->tile, y, x);

                if (map->pmap[y][x] < SHOAL) place_ocean_label(seed.cur); else
                if (map->pmap[y][x] < BEACH) place_shoal_label(seed.cur); else
                if (map->pmap[y][x] < TERRA) place_beach_label(seed.cur); else
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
        int y; 
        int x;     

        for (y=0; y<map->ufo.box.h-1; y++) {
        for (x=0; x<map->ufo.box.w-1; x++) {

                seed.cur = mx_get(map->tile, y, x);
                seed.s   = mx_s(map->tile, y, x);

                if ((LAYER(*seed.cur, 1, TOP)) && !(LAYER(*seed.s, 1, TOP))) 
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
        int y; 
        int x;     

        for (y=0; y<map->ufo.box.h-1; y++) {
        for (x=0; x<map->ufo.box.w-1; x++) {

                mx_seed(map->tile, y, x, &seed);

                if (!LAYER(*seed.cur, 1, TOP) ||
                    !LAYER(*seed.n,   1, TOP) ||
                    !LAYER(*seed.s,   1, TOP) ||
                    !LAYER(*seed.e,   1, TOP) ||
                    !LAYER(*seed.w,   1, TOP) ||
                    !LAYER(*seed.nw,  1, TOP) ||
                    !LAYER(*seed.ne,  1, TOP) ||
                    !LAYER(*seed.sw,  1, TOP) ||
                    !LAYER(*seed.se,  1, TOP)) {
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
        int y; 
        int x;     

        for (y=0; y<map->ufo.box.h-1; y++) {
        for (x=0; x<map->ufo.box.w-1; x++) {

                mx_seed(map->tile, y, x, &seed);

                if (LAYER(*seed.cur, 1, TTO) &&
                   (!LAYER(*seed.w,  2, TTO, TOP) ||
                    !LAYER(*seed.e,  2, TTO, TOP) ||
                    !LAYER(*seed.n,  2, TTO, TOP) ||
                    !LAYER(*seed.s,  2, TTO, TOP) ||
                    !LAYER(*seed.sw, 2, TTO, TOP) ||
                    !LAYER(*seed.se, 2, TTO, TOP) ||
                    !LAYER(*seed.nw, 2, TTO, TOP) ||
                    !LAYER(*seed.ne, 2, TTO, TOP)))
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
        int y; 
        int x;     

        for (y=0; y<map->ufo.box.h-1; y++) {
        for (x=0; x<map->ufo.box.w-1; x++) {

                mx_seed(map->tile, y, x, &seed);

                if (LAYER(*seed.cur, 1, TTO) &&
                    LAYER(*seed.s,   1, TOP) &&
                    LAYER(*seed.n,   1, TTO)) {
                        wipe_label(seed.cur);
                        place_treetrunk_label(seed.cur);
                }
                else if (LAYER(*seed.n,   1, TOP) && 
                         LAYER(*seed.cur, 1, TTO) &&
                         LAYER(*seed.s,   1, TOP)) {
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
        wchar_t *wch;
        short color;
        int i;
        int j;
        int k; 
        static wchar_t shore[6][24]={L"⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘", 
                                     L"⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠", 
                                     L"⠁⠁⠂⠂⠁⠁⠂⠂⠄⡀⠃⠅⠆⠄⡀⠃⡂⠄⡀⠃⠅⠆⡁⡂",
                                     L"⠈⠐⠠⢀⠘⠈⠐⠠⢀⠘⠨⠰⠰⢈⢈⢐⢠⢠⠨⠰⠰⢈⢐⢠",
                                     L"⠁⠁⠂⠂⠁⠁⠂⠂⠁⠁⠂⠂⠁⠁⠂⠂⠁⠁⠂⠂⠁⠁⠂⠂", /* 4 NE */
                                     L"⠈⠐⠐⠘⠈⠘⠈⠑⠐⠐⠘⠈⠐⠑⠑⠈⠐⠘⠈⠐⠑⠑⠈⠐"  /* 5 NW */
        };
        
        for (i=0; i<map->ufo.box.h-1; i++) {
        for (j=0; j<map->ufo.box.w-1; j++) {

                mx_seed(map->tile, i, j, &seed);

                /* Draw nothing if the cursor is on land */
                if ((LAYER(*seed.cur, 6, TOP, DRP, BEA, SHO, TTO, TTR)) 
                ||  (LAYER(*seed.nw,  1, OCN) && LAYER(*seed.w, 1, TOP))
                ||  (LAYER(*seed.ne,  1, OCN) && LAYER(*seed.e, 1, TOP)))
                {
                        continue;
                }

                /* Draw an edge if the following conditions apply... */

                /*if (LAYER(*seed.n,  2, BEA, SHO) */
                /*||  LAYER(*seed.nw, 2, BEA, SHO)*/
                /*||  LAYER(*seed.ne, 2, BEA, SHO))*/
                /*{*/
                        /*wch = shore[0];*/
                        /*[>color = D_SEA_LAGOON;<]*/
                        /*color = SEA__MED;*/
                /*}*/
                /*else if (LAYER(*seed.s,  2, BEA, SHO)*/
                /*||       LAYER(*seed.sw, 2, BEA, SHO)*/
                /*||       LAYER(*seed.se, 2, BEA, SHO))*/
                /*{*/
                        /*wch = shore[1];*/
                        /*[>color = D_SEA_LAGOON;<]*/
                        /*color = SEA__MED;*/
                /*}*/
                /*else if (LAYER(*seed.w, 2, BEA, SHO)) */
                /*{*/
                        /*wch = shore[2];*/
                        /*[>color = D_SEA_LAGOON;<]*/
                        /*color = SEA__MED;*/
                /*}*/
                /*else if (LAYER(*seed.e, 2, BEA, SHO)) */
                /*{*/
                        /*wch = shore[3];*/
                        /*[>color = D_SEA_LAGOON;<]*/
                        /*color = SEA__MED;*/
                /*}*/
                else if (LAYER(*seed.n, 1, DRP)) 
                {
                        wch = shore[0];
                        color = _SEA_SHALLOW;
                }
                else if (LAYER(*seed.e, 2, TOP, DRP))
                {
                        wch = shore[3];
                        color = SEA_MED;
                }
                else if (LAYER(*seed.w, 2, TOP, DRP))
                {
                        wch = shore[2];
                        color = SEA_MED;
                }
                else if (LAYER(*seed.nw, 2, TOP, DRP))
                {
                        wch = shore[4];
                        color = SEA_MED;
                }
                else if (LAYER(*seed.ne, 2, TOP, DRP))
                {
                        wch = shore[5];
                        color = SEA_MED;
                }
                else {
                        continue; /* Arrest any wild ones */
                }

                for (k=0; k<SURF_FRAMES; k++) {
                        mvwp(PEEK(map->L[RIM]), i, j, &wch[roll1d(10)], color, 0);
                        NEXT(map->L[RIM]);
                }
                set_byte(seed.cur, LAB, RIM);
        }
        }
}


