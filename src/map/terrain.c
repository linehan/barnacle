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
#include <math.h>

#include "terrain.h"
#include "weather.h"
#include "map.h"

#include "../lib/stoc/stoc.h"

#include "../eng/model/bytes.h"
#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../lib/morton.h"
#include "../test/test.h"
#include "../lib/ufo.h"

#include "../eng/state.h"
#include "../gfx/ui/titlecard.h"
//##############################################################################

enum zcodes { UL=0, U=1, UR=2, L=3, CUR=4, R=5, BL=6, D=7, BR=8};
uint32_t z[9];


#define MOORE_LEN 9

struct moore_t {
        uint32_t y[9];
        uint32_t x[9];
        uint32_t bz[9];
        uint32_t bcounter;
        uint32_t z;
        uint32_t h;
        uint32_t w;
};

inline void fill_moore(struct moore_t *m)
{
        #define RANGE 1
        enum {_y_, _x_};
        uint32_t forward, backward, upward, downward;

        int y = trom_y(m->z);
        int x = trom_x(m->z);

        forward  = (x < m->w - RANGE) ? x+RANGE : x;
        backward = (x > RANGE)   ? x-RANGE : x;
        upward   = (y < m->h - RANGE) ? y+RANGE : y;
        downward = (y > RANGE)   ? y-RANGE : y;

        /* Top row */
        m->y[0] = upward;
        m->x[0] = backward;
        m->y[1] = upward;
        m->x[1] = x;
        m->y[2] = upward;
        m->x[2] = forward;
        /* Middle row */
        m->y[3] = y;
        m->x[3] = backward;
        m->y[4] = y;
        m->x[4] = x;
        m->y[5] = y;
        m->x[5] = forward;
        /* Bottom row */
        m->y[6] = downward;
        m->x[6] = backward;
        m->y[7] = downward;
        m->x[7] = x;
        m->y[8] = downward;
        m->x[8] = forward;
}


inline void moore_set(struct moore_t *m, uint32_t z) 
{
        m->bz[m->bcounter++] = m->z; /* backtrack */
        m->z  = z;
        m->bcounter %= MOORE_LEN;
}
inline void moore_new(struct moore_t *m, int h, int w, uint32_t z)
{
        m->bcounter = 0;
        m->h = h;
        m->w = w;
        moore_set(m, z);
}



inline uint32_t moore_max(double **mx, struct moore_t *m)
{
        double hi;
        int ret;
        int i;

        fill_moore(m);

        for (i=0; i<MOORE_LEN; i++) {
                if (mx[m->y[i]][m->x[i]] > hi) {
                        hi = mx[m->y[i]][m->x[i]];
                        ret = i;
                }
        }

        return mort(m->y[ret], m->x[ret]);
}


inline uint32_t moore_min(double **mx, struct moore_t *m)
{
        double lo;
        int ret;
        int i, j;

        fill_moore(m);

        lo = mx[m->y[0]][m->x[0]];

        for (i=0; i<MOORE_LEN; i++) {
                if (mort(m->y[i], m->x[i]) == m->bz[0]) continue;
                if (mort(m->y[i], m->x[i]) == m->bz[1]) continue;
                if (mort(m->y[i], m->x[i]) == m->bz[2]) continue;
                if (mx[m->y[i]][m->x[i]] > lo) {
                        lo = mx[m->y[i]][m->x[i]];
                        ret = i;
                }
        }

        return mort(m->y[ret], m->x[ret]);
}



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

#define FOR_EACH_Z                              \
        for (__zi__=0; __zi__<ZMAX; __zi__++) { \
        for (__zj__=0; __zj__<ZMAX; __zj__++)   \

#define END_EACH_Z }

inline void FILL_Z(int y, int x, int ymax, int xmax)
{
        int i;
        int j;
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


/*
 * Holdunter yer buttz... 
 *
 * Using the Perlin simplex noise at 'pmap', generate and then draw the 
 * terrain layers for the struct map_t pointed to by the 'map' argument.
 */
void smooth_layers(struct map_t *map, double **pmap)
{
        #define OCTAVES 6       /* number of octaves to smooth */
        #define PERSIST 0.99    /* persistence factor */

        #define SHOAL -0.003    /* shoal layer */
        #define BEACH 0.00      /* beach (sea-land transition) layer */
        #define TERRA 0.45      /* ground layer */

        int ymax;
        int xmax; 
        int y; 
        int x;     
        int frame;

        /* Set the loop (pmap) boundaries */
        ymax = map->ufo.box.h - 1;         
        xmax = map->ufo.box.w - 1;

        /*
         * Smooth the Perlin noise map over a given number of octaves,
         * with a persistence factor (think "clumping")
         */
        print_status("Tuning noise...");
        perlin_smooth(pmap, ymax, xmax, PERSIST, OCTAVES);
        print_status(SUCCESS);

        /*
         * Now, smooth the noise map further, targeting specific thresholds
         * at which to apply smoothing. The thresholds correspond to the
         * layer transition values.
         */
        print_status("Smoothing noise...");
        smooth_cycle(pmap, ymax, xmax, TERRA, SMOOTH_BO, 3);
        smooth_cycle(pmap, ymax, xmax, BEACH, SMOOTH_BO, 3);
        smooth_cycle(pmap, ymax, xmax, SHOAL, SMOOTH_BO, 3);
        smooth_cycle(pmap, ymax, xmax, BEACH, SMOOTH_BO, 1);
        smooth_cycle(pmap, ymax, xmax, TERRA, SMOOTH_BO, 3);
        print_status(SUCCESS);
}

void draw_layers(struct map_t *map, double **pmap)
{
        int ymax;
        int xmax; 
        int y; 
        int x;     
        int frame;

        /* Set the loop (pmap) boundaries */
        ymax = map->ufo.box.h - 1;         
        xmax = map->ufo.box.w - 1;

        /*
         * Traverse the screen, laying the appropriate tiles based on
         * the corresponding values in the Perlin map. Each coordinate
         * pair receives a graphical tile and a label which is set in
         * the state word of the map tree.
         */
        print_status("Laying tiles...");

        for (y=0; y<ymax; y++) {
        for (x=0; x<xmax; x++) {

                fill_codes(ymax, xmax, y, x);

                if (pmap[y][x] < SHOAL) place_ocean_tile(map, z[CUR]); else
                if (pmap[y][x] < BEACH) place_shoal_tile(map, z[CUR]); else
                if (pmap[y][x] < TERRA) place_beach_tile(map, z[CUR]); else
                                        place_terra_tile(map, z[CUR]);
        }
        }
        print_status(SUCCESS);

        /*
         * Now that the initial pass is complete, identify edges which need
         * to have a perspective tile affixed to them. These are usually
         * going to be the "bottom" edges of certain layers, in particular
         * the "TERRA" and "TREETOP" layers.
         */
        print_status("Drawing cliffs...");

        for (y=0; y<ymax; y++) {
        for (x=0; x<xmax; x++) {

                fill_codes(ymax, xmax, y, x);

                if ((LAYER(z[CUR], 1, TOP)) && !(LAYER(z[D], 1, TOP))) 
                        place_cliff_tile(map, z[CUR]);
        }
        }
        print_status(SUCCESS);

        /*
         * Overwrite TERRA tiles with TREETOP tiles, based on a size
         * heuristic (the TERRA must meet a minimum area requirement)
         */
        print_status("Planting trees...");
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

                        FOR_EACH_Z 
                        {
                                place_treetop_tile(map, _Z_);
                        } 
                        END_EACH_Z
                }
        }
        }
        print_status(SUCCESS);

        /*
         * Remove TREETOP tiles which have been placed at the edges of
         * a TERRA blob, so it doesn't look like shit
         */
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
                        wipe_tile(map, z[CUR], VEG);
                        place_terra_tile(map, z[CUR]);
                }
        }
        }

        /*
         * One more pass, place TREETRUNK tiles at the "bottom" edges of
         * every TREETOP tile
         */
        for (y=0; y<ymax; y++) {
        for (x=0; x<xmax; x++) {

                fill_codes(ymax, xmax, y, x);
                if (LAYER(z[CUR], 1, VEG) &&
                    LAYER(z[D],   1, TOP) &&
                    LAYER(z[U],   1, VEG)) {
                        wipe_tile(map, z[CUR], VEG);
                        place_treetrunk_tile(map, z[CUR]);
                }
                else if (LAYER(z[U],   1, TOP) && 
                         LAYER(z[CUR], 1, VEG) &&
                         LAYER(z[D],   1, TOP)) {
                                wipe_tile(map, z[CUR], VEG);
                                place_terra_tile(map, z[CUR]);
                }
        }
        }
        print_status(SUCCESS);


        print_status("Eating breakfast...");
        /* 
         * Now we focus our attention on the edges of the SHOAL tiles,
         * that is, the threshold between the background "sea level" 
         * and any sort of tile at all. We want to make them look nice,
         * so we draw a special animated tile that can be stepped through
         * by the event loop.
         */
        print_status(SUCCESS);
}


uint32_t any_yx(int h, int w)
{
        return MORT((roll1d(h)), (roll1d(w)));
}

uint32_t any_y(int h, int w, int x)
{
        return MORT(roll1d(h), roll1d(x));
}




#define RULE_R(z, w) (trom_x(z) < w) ? MORT(trom_y(z), trom_x(z)+1) : 0

#define RULE_D(z, h) (trom_y(z) < h) ? MORT(trom_y(z)+1, trom_x(z)) : 0
#define RULE_L(z, w) (trom_x(z) > 0) ? MORT(trom_y(z), trom_x(z)-1) : 0
#define RULE_U(z, h) (trom_y(z) > 0) ? MORT(trom_y(z)-1, trom_x(z)) : 0

#define RULE_DR(z, h, w) RULE_D((RULE_R(z, w)), h)
#define RULE_DL(z, h, w) RULE_D((RULE_L(z, w)), h)
#define RULE_UL(z, h, w) RULE_U((RULE_L(z, w)), h)
#define RULE_UR(z, h, w) RULE_U((RULE_R(z, w)), h)

static int num_request;
static int num_active;
static int num_active_max;

void init_surface_flow(int n)
{
        num_active_max = LINES * COLS;
        num_request = n;
}

void inc_surface_flow(void)
{
        num_request = (num_request < num_active_max) ? (num_request+1) : num_active_max;
}
void dec_surface_flow(void)
{
        num_request = (num_request > 0) ? (num_request-1) : 0;
}

void set_surface_flow(int n)
{
        num_request = (n < num_active_max) ? n : num_active_max;
}

#define NRULES 8
static int rule;
void rot_surface_rule(void)
{
        rule = (rule + 1) % NRULES;
}

        


void surface_flow(struct map_t *map)
{
        static bool testing = true;

        static struct moore_t particle[9500];
        int i=0;
        static int h;
        static int w;
        static int y=0;
        static int x=0;

        h = LINES;
        w = COLS;

        while (num_active < num_request) {
                while (particle[i++].z != 0);
                --i;
                moore_new(&particle[i], LINES, COLS, any_yx(h, w));
                num_active++;
        }

        for (i=0; i<num_active; i++) {
                place_ocean_tile(map, particle[i].z);

                switch (rule) {
                case 0:
                        moore_set(&particle[i], RULE_R(particle[i].z, w));
                        break;
                case 1:
                        moore_set(&particle[i], RULE_DR(particle[i].z, h, w));
                        break;
                case 2:
                        moore_set(&particle[i], RULE_D(particle[i].z, w));
                        break;
                case 3:
                        moore_set(&particle[i], RULE_DL(particle[i].z, h, w));
                        break;
                case 4:
                        moore_set(&particle[i], RULE_L(particle[i].z, w));
                        break;
                case 5:
                        moore_set(&particle[i], RULE_UL(particle[i].z, h, w));
                        break;
                case 6:
                        moore_set(&particle[i], RULE_U(particle[i].z, w));
                        break;
                case 7:
                        moore_set(&particle[i], RULE_UR(particle[i].z, h, w));
                        break;
                }

                if (particle[i].z == 0) 
                        num_active--;
                else
                        place_swell_tile(map, particle[i].z, 4);
        }


        if (testing) {
                werase(DIAGNOSTIC_WIN);
                wprintw(DIAGNOSTIC_WIN, "num_request: %d\n"
                                        "num_active: %d\n", num_request, num_active);

                for (i=0; i<(LINES/5)-3; i++) {                      
                        wprintw(DIAGNOSTIC_WIN, "particle[%02d]: %u\n", i, particle[i].z);
                }
                doupdate();
        }


}



void draw_water_rim(struct map_t *map)
{
        #define SURF_FRAMES 4
        wchar_t shore[4][24]={L"⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘⠁⠈⠉⠑⠃⠘", 
                              L"⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠⡀⢀⣀⢄⡄⢠", 
                              L"⠁⠁⠂⠂⠁⠁⠂⠂⠄⡀⠃⠅⠆⠄⡀⠃⡂⠄⡀⠃⠅⠆⡁⡂",
                              L"⠈⠐⠠⢀⠘⠈⠐⠠⢀⠘⠨⠰⠰⢈⢈⢐⢠⢠⠨⠰⠰⢈⢐⢠"};

        int i, j, k; 
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
                            LAYER(z[UL], 1, GRO)||
                            LAYER(z[UR], 1, GRO)){
                                wch = shore[0];
                                color = D_SEA_LAGOON;
                                attr = 0;
                        }
                        else if (LAYER(z[D],  1, GRO)||
                                 LAYER(z[BL], 1, GRO)||
                                 LAYER(z[BR], 1, GRO)){
                                wch = shore[1];
                                color = D_SEA_LAGOON;
                                attr = 0;
                        }
                        else if (LAYER(z[L], 1, GRO)) {
                                wch = shore[2];
                                color = D_SEA_LAGOON;
                                attr = 0;
                        }
                        else if (LAYER(z[R], 1, GRO)) {
                                wch = shore[3];
                                color = D_SEA_LAGOON;
                                attr = 0;
                        }
                        else if (LAYER(z[U], 1, DRP)) {
                                wch = shore[0];
                                color = _SEA_SHALLOW;
                                attr = 0;
                        }
                        if (wch != NULL) {
                                for (k=0; k<SURF_FRAMES; k++) {
                                        mvwp(PEEK(map->L[RIM]), i, j, &(wch[roll1d(10)]), color, 0);
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
