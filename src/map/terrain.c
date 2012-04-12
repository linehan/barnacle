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


/* Holdunter yer buttz... */
void smooth_layers(struct map_t *map);
void label_regions(struct map_t *map);
void label_cliffs(struct map_t *map);
void label_treetops(struct map_t *map);
void label_treetops_trim(struct map_t *map);
void label_treetrunks(struct map_t *map);
void label_shorelines(struct map_t *map);


/*
 * Perform a detailed series of passes over the map's matrix, and apply a
 * variety of strategies to label grid cells so that they can later be
 * detected by the map_render routine.
 * 
 * The rules that determine a certain labeling are somewhat tedious, and 
 * some minimum amount of fussiness should be expected. Refer to the
 * documentation of the individual subroutines.
 */
void map_label(struct map_t *map)
{
        print_status("Smoothing noise...");
        smooth_layers(map);
        print_status(SUCCESS);

        print_status("Laying tiles...");
        label_regions(map);
        print_status(SUCCESS);

        print_status("Drawing cliffs...");
        label_cliffs(map);
        print_status(SUCCESS);

        print_status("Planting trees...");
        label_treetops(map);
        print_status(SUCCESS);

        print_status("Trimming trees...");
        label_treetops_trim(map);
        print_status(SUCCESS);

        print_status("Eating breakfast...");
        label_treetrunks(map);
        print_status(SUCCESS);

        print_status("Going swimming...");
        label_shorelines(map);
        print_status(SUCCESS);
}

/*
 * Traverse the map's matrix and simply pass every label encountered
 * to the place_tile routine. Any labels associated with a tile rendering
 * will have that rendering applied to the grid cell at (i,j).
 */
void map_render(struct map_t *map)
{
        int i;
        int j;

        for (i=0; i<map->mx->rows; i++) {
        for (j=0; j<map->mx->cols; j++) {
                place_tile(map, i, j, get_nibble(mx_val(map->mx, i, j), LAY));
        }
        }
}



/*
 * Simplified accessors for comparing the value of multiple labels in
 * the same block. See the documentation for or_nibble in "../eng/bytes.h"
 */
#define LAYER(val,n,...) or_nibble(val, LAY, n, __VA_ARGS__)
#define  ELEV(val,n,...) or_nibble(val, ALT, n, __VA_ARGS__)



/* Constants */
static const int   OCTAVES =  6;      /* Number of octaves to smooth */
static const float PERSIST =  0.99;   /* Persistence factor */
static const float SHOAL   = -0.003;  /* Shoal isoline */
static const float BEACH   =  0.00;   /* Beach isoline */
static const float TERRA   =  0.45;   /* Ground isoline */



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

                seed.cur = mx_get(map->mx, y, x);

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

                seed.cur = mx_get(map->mx, y, x);
                seed.s   = mx_s(map->mx, y, x);

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

                mx_seed(map->mx, y, x, &seed);

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

                mx_seed(map->mx, y, x, &seed);

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
 * One more pass, place TTR labels at the "bottom" edges of every 
 * TTO label (analagous to label_cliffs, see above)
 */
void label_treetrunks(struct map_t *map)
{
        struct seed_t seed;
        int y; 
        int x;     

        for (y=0; y<map->ufo.box.h-1; y++) {
        for (x=0; x<map->ufo.box.w-1; x++) {

                mx_seed(map->mx, y, x, &seed);

                if (LAYER(*seed.cur, 1, TTO) &&
                    LAYER(*seed.s,   1, TOP) &&
                    LAYER(*seed.n,   1, TTO)) {
                        wipe_label(seed.cur);
                        place_treetrunk_label(seed.cur);
                }
                else if (LAYER(*seed.n,   1, TOP) && 
                         LAYER(*seed.cur, 1, TTR) &&
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
        static wchar_t shore[4][24]={L"⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘", 
                                     L"⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠", 
                                     L"⠁⠁⠂⠂⠁⠁⠂⠂⠄⡀⠃⠅⠆⠄⡀⠃⡂⠄⡀⠃⠅⠆⡁⡂",
                                     L"⠈⠐⠠⢀⠘⠈⠐⠠⢀⠘⠨⠰⠰⢈⢈⢐⢠⢠⠨⠰⠰⢈⢐⢠"};
        
        for (i=0; i<map->ufo.box.h-1; i++) {
        for (j=0; j<map->ufo.box.w-1; j++) {

                mx_seed(map->mx, i, j, &seed);

                /* Draw nothing if the cursor is on land */
                if ((LAYER(*seed.cur, 4, TOP, DRP, BEA, SHO)) 
                ||  (LAYER(*seed.nw,  1, XXX) && LAYER(*seed.w, 1, TOP))
                ||  (LAYER(*seed.ne,  1, XXX) && LAYER(*seed.e, 1, TOP)))
                {
                        continue;
                }
                /* Draw an edge if the following conditions apply... */
                if (LAYER(*seed.n,  2, BEA, SHO) 
                ||  LAYER(*seed.nw, 2, BEA, SHO)
                ||  LAYER(*seed.ne, 2, BEA, SHO))
                {
                        wch = shore[0];
                        color = D_SEA_LAGOON;
                }
                else if (LAYER(*seed.s,  2, BEA, SHO)
                ||       LAYER(*seed.sw, 2, BEA, SHO)
                ||       LAYER(*seed.se, 2, BEA, SHO))
                {
                        wch = shore[1];
                        color = D_SEA_LAGOON;
                }
                else if (LAYER(*seed.w, 2, BEA, SHO)) 
                {
                        wch = shore[2];
                        color = D_SEA_LAGOON;
                }
                else if (LAYER(*seed.e, 2, BEA, SHO)) 
                {
                        wch = shore[3];
                        color = D_SEA_LAGOON;
                }
                else if (LAYER(*seed.n, 1, DRP)) 
                {
                        wch = shore[0];
                        color = _SEA_SHALLOW;
                }
                else {
                        continue; /* Arrest any wild ones */
                }

                for (k=0; k<SURF_FRAMES; k++) {
                        mvwp(PEEK(map->L[RIM]), i, j, &wch[roll1d(10)], color, 0);
                        NEXT(map->L[RIM]);
                }
                set_nibble(seed.cur, LAY, RIM);
        }
        }
}


