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

#include "../gen/smooth.h"

#include "../lib/common.h"
#include "../eng/model/bytes.h"
#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../gen/dice.h"
#include "../lib/morton.h"
#include "../pan/test.h"
#include "../lib/ufo.h"
#include "../gfx/brnoise.h"

#include "../eng/state.h"
#include "../pan/titlecard.h"
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
        /*for (int octave = octaveCount - 1; octave >= 0; octave--) {*/
        /*for (i = i - 1; i >= 0; i--) {*/ //him
        /*while (i-->0)*/                  // me
#define __y__(z) trom_y((z))
#define __x__(z) trom_x((z))
#define putwch mvwadd_wch

#define PLACE_OCEAN_TILE(map, z)                                           \
do {                                                                       \
        putwch(PLATE((map), BGR), __y__(z), __x__(z), &OCEAN[0]);    \
        set_state(map->tree, (z), 0, LAY, BGR);                            \
        set_state(map->tree, (z), 0, ALT, 0);                              \
} while (0)

#define PLACE_SHOAL_TILE(map, z)                                           \
do {                                                                       \
        if (flip_biased(0.7))                                              \
                putwch(PLATE((map), GRO), __y__(z), __x__(z), BRCCH(2, D_SEA_LAGOON)); \
        else    putwch(PLATE((map), GRO), __y__(z), __x__(z), &__LAGOON[0]); \
                                                                           \
        set_state(map->tree, (z), 0, LAY, GRO);                            \
        set_state(map->tree, (z), 0, ALT, 1);                              \
} while (0)

#define PLACE_BEACH_TILE(map, z)                                           \
do {                                                                       \
        putwch(PLATE((map), GRO), __y__(z), __x__(z), &SAND);        \
        set_state(map->tree, (z), 0, LAY, GRO);                            \
        set_state(map->tree, (z), 0, ALT, 2);                              \
} while (0)

#define PLACE_TERRA_TILE(map, z)                                           \
do {                                                                       \
        putwch(PLATE((map), TOP), __y__(z), __x__(z), &GRASS[1]);    \
        set_state(map->tree, (z), 0, LAY, TOP);                            \
        set_state(map->tree, (z), 0, ALT, 4);                              \
} while (0)

#define PLACE_CLIFF_TILE(map, z)                                           \
do {                                                                       \
        putwch(PLATE((map), TOP), __y__(z), __x__(z), &BLANK);      \
        putwch(PLATE((map), DRP), __y__(z), __x__(z), &MTN[2]);      \
        set_state(map->tree, (z), 0, LAY, DRP);                            \
        set_state(map->tree, (z), 0, ALT, 4);                              \
} while (0)


#define PLACE_TREETOP_TILE(map, z)                                           \
do {                                                                       \
        putwch(PLATE((map), TOP), __y__(z), __x__(z), &BLANK);      \
        putwch(PLATE((map), VEG), __y__(z), __x__(z), &TREE[1]);      \
        set_state(map->tree, (z), 0, LAY, VEG);                         \
        set_state(map->tree, (z), 0, ALT, 4);                              \
} while (0)

#define PLACE_TREETRUNK_TILE(map, z)                                           \
do {                                                                       \
        putwch(PLATE((map), TOP), __y__(z), __x__(z), &BLANK);      \
        putwch(PLATE((map), VEG), __y__(z), __x__(z), &TREE[0]);      \
        set_state(map->tree, (z), 0, LAY, VEG);                         \
        set_state(map->tree, (z), 0, ALT, 4);                              \
} while (0)

#define WIPE_TILE(map, z, layer)\
do {                                                            \
        putwch(PLATE((map), layer), __y__(z), __x__(z), &BLANK);      \
} while (0)




/*enum zcodes { CUR=0, U=1, D=2, L=3, R=4, UL=5, UR=6, BL=7, BR=8 };*/
enum zcodes { UL=0, U=1, UR=2, L=3, CUR=4, R=5, BL=6, D=7, BR=8};
uint32_t z[9];

inline void fill_codes(int I, int J, int i, int j)
{
        int _i, i_;
        int _j, j_;

        _i = (i > 0) ? (i-1) : i;
        _j = (j > 0) ? (j-1) : j;

        i_ = (i < (I-1)) ? (i+1) : i;
        j_ = (j < (J-1)) ? (j+1) : j;

        // Compute Morton codes
        z[CUR] = MORT( i  ,  j );
        z[U]   = MORT(_i  ,  j );
        z[D]   = MORT( i_ ,  j );
        z[L]   = MORT( i  , _j );
        z[R]   = MORT( i  ,  j_);
        z[UL]  = MORT(_i  , _j );
        z[UR]  = MORT(_i  ,  j_);
        z[BL]  = MORT( i_ , _j );
        z[BR]  = MORT( i_ ,  j_);
}


#define ZMAX 5
uint32_t Z[ZMAX][ZMAX];
int __zi__;
int __zj__;
#define _Z_ Z[__zi__][__zj__]
#define _I_ __zi__
#define _J_ __zj__

#define FOR_EACH_Z \
        for (__zi__=0; __zi__<ZMAX; __zi__++) { \
        for (__zj__=0; __zj__<ZMAX; __zj__++)  \

#define END_EACH_Z }

inline void FILL_Z(int y, int x, int ymax, int xmax)
{
        int i, j;
        int yy, xx;

        yy = xx = 0;

        for (i=0; i<ZMAX; i++) {
        for (j=0; j<ZMAX; j++) {

                yy = (y+i < ymax) ? y+i : yy;
                xx = (x+j < xmax) ? x+j : xx;
                
                Z[i][j] = MORT(yy, xx);
        }
        }
}


#define LAYER(mort, n, ...) or_nibble(rb_data(map->tree, (mort)), LAY, n, __VA_ARGS__)
#define ELEV(mort, n, ...) or_nibble(rb_data(map->tree, (mort)), ALT, n, __VA_ARGS__)

#define DEC(x, min) x = ((x) > (min)) ? ((x)-1) : (x)
#define INC(x, max) x = ((x) < (max)) ? ((x)+1) : (x)


void sss(double **pmap, int h, int w, float lim, int span)
{
        bool possible_hole;
        int count;
        int y, x, n;

        count = 0;
        possible_hole = true;

        for (y=0; y<h; y++) {
        for (x=0; x<w; x++) {

                if (pmap[y][x] >= lim) {
                        possible_hole = true;
                        /* If count isn't at zero, fill in the previous
                           squares with the current value. (Also, set count
                           to zero, as a side effect.) */
                        while (count-->0)
                                pmap[y][x-(count+1)] = pmap[y][x];
                }

                if (pmap[y][x] < lim) {
                        if (possible_hole) { /* This is the first non-top tile */
                                if (count++ > span) { /* If hole outspans the span */
                                        possible_hole = false; /* not a hole */
                                        count = 0;             /* reset the count */
                                }
                        }
                }

        }
        }
}



/*
 * Holdunter yer buttz... 
 *
 * Using the Perlin simplex noise at 'pmap', generate and then draw the 
 * terrain layers for the struct map_t pointed to by the 'map' argument.
 */
void draw_layers(struct map_t *map, double **pmap)
{
        #define OCTAVES 6
        #define SMOOTH 0.99

        /*#define SHOAL -0.10*/
        #define SHOAL -0.003
        #define BEACH 0.00
        #define TERRA 0.45

        int ymax;
        int xmax; 
        int y; 
        int x;     

        ymax = map->ufo.box.h - 1;
        xmax = map->ufo.box.w - 1;

        print_status("Tuning noise...");

        perlin_smooth(pmap, ymax, xmax, SMOOTH, OCTAVES);

        print_status(FINISHED);
        print_status("Smoothing noise...");



        /*smooth_cycle(pmap, ymax, xmax, TERRA, SMOOTH_BO, 3);*/
        /*smooth_cycle(pmap, ymax, xmax, BEACH, SMOOTH_BO, 3);*/
        /*smooth_cycle(pmap, ymax, xmax, SHOAL, SMOOTH_HI, 3);*/
        /*smooth_cycle(pmap, ymax, xmax, BEACH, SMOOTH_HI, 3);*/

        smooth_cycle(pmap, ymax, xmax, TERRA, SMOOTH_BO, 3);

        smooth_cycle(pmap, ymax, xmax, BEACH, SMOOTH_BO, 3);
        smooth_cycle(pmap, ymax, xmax, SHOAL, SMOOTH_BO, 3);
        /*smooth_cycle(pmap, ymax, xmax, SHOAL, SMOOTH_BO, 1);*/
        smooth_cycle(pmap, ymax, xmax, BEACH, SMOOTH_BO, 1);
        smooth_cycle(pmap, ymax, xmax, TERRA, SMOOTH_BO, 3);
        /*smooth_cycle(pmap, ymax, xmax, TERRA, SMOOTH_I, 1);*/


        /*smooth_cycle(pmap, ymax, xmax, TERRA, SMOOTH_BO, 1);*/
        /*smooth_cycle(pmap, ymax, xmax, BEACH, SMOOTH_LO, 1);*/
        /*smooth_cycle(pmap, ymax, xmax, TERRA, SMOOTH_LO, 1);*/


        /*smooth_cycle(pmap, ymax, xmax, TERRA, SMOOTH_LO, 1);*/
        /*smooth_cycle(pmap, ymax, xmax, BEACH, SMOOTH_BO, 1);*/
        /*smooth_cycle(pmap, ymax, xmax, SHOAL, SMOOTH_BO, 1);*/

        print_status(FINISHED);
        print_status("Laying tiles...");

        for (y=0; y<ymax; y++) {
        for (x=0; x<xmax; x++) {

                fill_codes(ymax, xmax, y, x);

                if (pmap[y][x] < SHOAL) PLACE_OCEAN_TILE(map, z[CUR]); else
                if (pmap[y][x] < BEACH) PLACE_SHOAL_TILE(map, z[CUR]); else
                if (pmap[y][x] < TERRA) PLACE_BEACH_TILE(map, z[CUR]); else
                                        PLACE_TERRA_TILE(map, z[CUR]);
        }
        }
        print_status(FINISHED);
        print_status("Drawing cliffs...");

        for (y=0; y<ymax; y++) {
        for (x=0; x<xmax; x++) {

                fill_codes(ymax, xmax, y, x);

                if ((LAYER(z[CUR], 1, TOP)) && !(LAYER(z[D], 1, TOP))) 
                        PLACE_CLIFF_TILE(map, z[CUR]);
        }
        }

        print_status(FINISHED);
        print_status("Planting trees...");
        int n;
        bool trees;
        for (y=0; y<ymax; y++) {
        for (x=0; x<xmax; x++) {

                FILL_Z(y, x, ymax, xmax);

                trees = true;

                FOR_EACH_Z 
                {
                        if (!LAYER(_Z_, 1, TOP)) trees = false;
                } 
                END_EACH_Z

                if (trees == true) {

                        /*if (flip_biased(0.5)) continue;*/

                        FOR_EACH_Z 
                        {
                                PLACE_TREETOP_TILE(map, _Z_);

                        } 
                        END_EACH_Z
                }
        }
        }
        print_status(FINISHED);
        print_status("Trimming trees...");

        for (y=0; y<ymax; y++) {
        for (x=0; x<xmax; x++) {

                fill_codes(ymax, xmax, y, x);

                if (LAYER(z[CUR], 1, VEG) &&
                   (!LAYER(z[L],  2, VEG, TOP) ||
                    !LAYER(z[R],  2, VEG, TOP) ||
                    !LAYER(z[U],  2, VEG, TOP) ||
                    !LAYER(z[D],  2, VEG, TOP) ||
                    !LAYER(z[BL], 2, VEG, TOP) ||
                    !LAYER(z[BR], 2, VEG, TOP) ||
                    !LAYER(z[UL], 2, VEG, TOP) ||
                    !LAYER(z[UR], 2, VEG, TOP) ))
                {
                        WIPE_TILE(map, z[CUR], VEG);
                        PLACE_TERRA_TILE(map, z[CUR]);
                }
        }
        }

        for (y=0; y<ymax; y++) {
        for (x=0; x<xmax; x++) {

                fill_codes(ymax, xmax, y, x);
                if (LAYER(z[CUR], 1, VEG) &&
                    LAYER(z[D],   1, TOP)) {
                        WIPE_TILE(map, z[CUR], VEG);
                        PLACE_TREETRUNK_TILE(map, z[CUR]);
                }
        }
        }
        print_status(FINISHED);

}




void draw_water_rim(struct map_t *map)
{
        #define SURF_FRAMES 4
        /*static wchar_t *rshore[]={L"⠁⠈", L"⠂⠐", L"⠄⠠", L"⡀⢀",*/
                                  /*L"⡁⢈", L"⠅⠨", L"⠃⠘", L"⠇⠸",*/
                                  /*L"⡃⢘", L"⡅⢨", L"⡆⢰", L"⡇⢸",*/
                                  /*L"⡎⢱", L"⢇⡸"};*/

        /*static wchar_t *lshore[]={L"⠈⠁", L"⠐⠂", L"⠠⠄", L"⢀⡀",*/
                                  /*L"⢈⡁", L"⠨⠅", L"⠘⠃", L"⠸⠇",*/
                                  /*L"⢘⡃", L"⢨⡅", L"⢰⡆", L"⢸⡇",*/
                                  /*L"⢱⡎", L"⡸⢇"};*/

        /*static wchar_t *tshore[]={L"⠉⣀", L"⠒⠤", L"⠉⠒", L"⠑⡠", L"⠉⡠"};*/

        /*static wchar_t *shore[]={L"⢇⠰⠐⢰", L"⡸⠆⠂⡁⡆", L"⠉⠄⡠⠑"};*/
        /*static wchar_t *shore[]={L"⠆⠂⠆⠄", L"⠰⠐⠰⠠", L"⠉⠄⡠⠑"};*/
/*⠆⠰*/
/*⡕ ⢜⡣*/
/*⢣⡜*/
/*⡱⢎*/
        static wchar_t *shore[]={L"⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘", 
                                 L"⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠", 
                                 /*L"⠁⠂⠄⡀⠃⠅⠆⡁⡂⡄", */
                                 /*L"⠈⠐⠠⢀⠘⠨⠰⢈⢐⢠",*/
                                 L"⠁⠂⠄⡀⠃⠅⠆⡁⡂⡄⠈⠐⠠⢀⠘⠨⠰⢈⢐⢠"};

        /*static wchar_t *lshore[]={L"⠈⠁", L"⠐⠂", L"⠠⠄", L"⢀⡀",*/
                                  /*L"⢈⡁", L"⠨⠅", L"⠘⠃", L"⠸⠇",*/
                                  /*L"⢘⡃", L"⢨⡅", L"⢰⡆", L"⢸⡇",*/
                                  /*L"⢱⡎", L"⡸⢇"};*/

        /*static wchar_t *tshore[]={L"⠉⣀", L"⠒⠤", L"⠉⠒", L"⠑⡠", L"⠉⡠"};*/

        /*static wchar_t *lshore[]={L"⠸⠨⠑⠂⠅⠑", L"⠸⢈⠑⠂⡁⠑", L"⠸⠘⠑⠂⠃⠑",*/
                                  /*L"⢸⡸⠇⠂⠇⡸", L"⢸⡸⡁⠂⡃⡸", L"⢸⡸⠅⠂⠅⡸", L"⢸⡸⠃⠂⠃⡸",*/
                                  /*L"⢸⢱⠇⠂⠇⢱", L"⢸⢱⡁⠂⡃⢱", L"⢸⢱⠅⠂⠅⢱", L"⢸⢱⠃⠂⠃⢱"};*/

        /*static wchar_t *rshore[]={L"⠇⠅⠑⠐⠨⠑", L"⠇⡁⠑⠐⢈⠑", L"⠇⠃⠑⠐⠘⠑",*/
                                  /*L"⡇⢇⠸⠐⠸⢇", L"⡇⢇⢈⠐⢘⢇", L"⡇⢇⠨⠐⠨⢇", L"⡇⢇⠘⠐⠘⢇",*/
                                  /*L"⡇⡎⠸⠐⠸⡎", L"⡇⡎⢈⠐⢘⡎", L"⡇⡎⠅⠐⠨⡎", L"⡇⡎⠘⠐⠘⡎"};*/

        /*static wchar_t *tshore[]={L"⠁⠂⠄⡀⠄⠂", L"⠈⠐⠠⢀⠠⠐", L"⠉⠒⠤⣀⠤⠒", L"⠉⠑⡠⣀⡠⠑"};*/



        /*static wchar_t rshore[]={L"⠁⠂⠄⡀⡁⠅⠃⠇⡃⡅⡆⡇⡎⢇"};*/
        /*static wchar_t lshore[]={L"⠈⠐⠠⢀⢈⠨⠘⠸⢘⢨⢰⢸⢱⡸"};*/
        /*static wchar_t tshore[]={L"⠁⠈⠂⠐⠄⠠⡀⢀⠉⠑⠒⠤⡠⣀"};*/

        int i, j, k;             // Incrementors
        int tile;
        int imax;
        int jmax;
        cchar_t cch;
        WINDOW *rim1, *rim2;  // Where to draw seashore animation.
        wchar_t *wch;
        short color;
        attr_t attr;
        
        imax = map->ufo.box.h;
        jmax = map->ufo.box.w;

        for (i=0; i<imax; i++) {
        for (j=0; j<jmax; j++) {

                        fill_codes(imax, jmax, i, j);

                        // Draw nothing is the cursor is on land.
                        if (LAYER(z[CUR], 3, TOP, DRP, GRO)) 
                                continue;
                        // Draw nothing if the cursor is at a top corner.
                        if (LAYER(z[UL], 1, XXX) && LAYER(z[L], 1, TOP))
                                continue;
                        if (LAYER(z[UR], 1, XXX) && LAYER(z[R], 1, TOP))
                                continue;

                        // Draw an edge if there is an edge in the directions.
                        if (LAYER(z[U], 1, GRO) ||
                            LAYER(z[D], 1, GRO) ||
                            LAYER(z[L], 1, GRO) ||
                            LAYER(z[R], 1, GRO) ||
                            LAYER(z[UL], 1, GRO)||
                            LAYER(z[UR], 1, GRO)||
                            LAYER(z[BL], 1, GRO)||
                            LAYER(z[BR], 1, GRO)) {
                                wch = shore[0];
                                color = D_SEA_LAGOON;
                                attr = 0;
                        }

                        if (LAYER(z[U], 2, TOP, DRP)) {
                                wch = shore[0];
                                color = _SEA_SHALLOW;
                                attr = 0;
                        }
                        if (LAYER(z[L], 2, TOP, DRP)) {
                                wch = shore[2];
                                color = SEA_SHALLOW;
                                attr = 0;
                        }
                        if (LAYER(z[R], 2, TOP, DRP)) {
                                wch = shore[2];
                                color = SEA_SHALLOW;
                                attr = 0;
                        }
                        /*if (LAYER(z[U], 3, TOP, DRP, GRO)) {*/
                                /*wch = shore[0];*/
                                /*color = _SEA_SHALLOW;*/
                        /*}*/
                        /*if (LAYER(z[L], 3, TOP, DRP, GRO)) {*/
                                /*wch = shore[2];*/
                                /*color = SEA_SHALLOW;*/
                        /*}*/
                        /*if (LAYER(z[R], 3, TOP, DRP, GRO)) {*/
                                /*wch = shore[2];*/
                                /*color = SEA_SHALLOW;*/
                        /*}*/
                        if (wch != NULL) {
                                for (k=0; k<SURF_FRAMES; k++) {
                                        setcchar(&cch, &wch[roll_fair(19)], attr, color, NULL);
                                        mvwadd_wch(PEEK(map->L[RIM]), i, j, &cch);
                                        NEXT(map->L[RIM]);
                                }
                                set_state(map->tree, z[CUR], 0, LAY, RIM);
                        }
                        wch = NULL;
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
