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

float **float2D(int h, int w)
{
        float **new;

        new = malloc(h * sizeof(float *));
        while (h-->0)
                new[h] = malloc(w * sizeof(float));

        return (new);
}

/*
 * Returns a linear interpolation between two values. The closer alpha 
 * is to 0, the closer the resulting value will be to x0; the closer 
 * alpha is to 1, the closer the resulting value will be to x1.
 */
inline float interpolate(float x0, float x1, float alpha)
{
   return x0 * (1 - alpha) + alpha * x1;
}


float **gen_smooth(double **base, int h, int w, int octave)
{
        float **smooth;
        float samp_freq;
        int samp_period;
        int samp_i[2];
        int samp_j[2];
        float blend_hoz;
        float blend_vrt;
        float top;
        float bot;
        int i;
        int j;
                
        smooth = float2D(h, w); 
 
        samp_period = 1 << octave; // calculates 2 ^ k
        samp_freq = 1.0/samp_period;
 
        for (i=0; i<h; i++) 
        {
                //calculate the vertical sampling indices
                samp_i[0] = (i/samp_period) * samp_period;
                samp_i[1] = (samp_i[0]+samp_period) % h; //wrap around
                blend_vrt = (i - samp_i[0]) * samp_freq;
         
                for (j=0; j<w; j++) 
                {
                        //calculate the horizontal sampling indices
                        samp_j[0] = (j / samp_period) * samp_period;
                        samp_j[1] = (samp_j[0] + samp_period) % w; //wrap around
                        blend_hoz = (j - samp_j[0]) * samp_freq;

                 
                        //blend the top two corners
                        top = interpolate(base[samp_i[0]][samp_j[0]],
                                          base[samp_i[1]][samp_j[0]], 
                                          blend_hoz);
                 
                        //blend the bottom two corners
                        bot = interpolate(base[samp_i[0]][samp_j[1]],
                                          base[samp_i[1]][samp_j[1]], 
                                          blend_hoz);
                 
                        //final blend
                        smooth[i][j] = interpolate(top, bot, blend_vrt);
                }
        }
        return smooth;
}




float fscale_factor(float max, float min)
{
        return (float)2.0/(max-min);
}




void perlin_boost(double **pmap, int h, int w, float persist, int octaves)
{
        float ***smooth;
        float scale;
        float amp;
        float tot;
        int i;
        int j;
        int o;

        smooth = malloc(octaves * sizeof(float **));

        for (o=0; o<octaves; o++) {
                smooth[o] = gen_smooth(pmap, h, w, o);
        }

        tot = 0.0;
        amp = 1.0;

        for (o=0; o<octaves; o++) {
                amp *= persist;
                tot += amp;

                for (i=0; i<h; i++) {
                for (j=0; j<w; j++) {
                        pmap[i][j] += (double)(smooth[o][i][j] * amp);
                }
                }
        }

        /*for (i=0; i<h; i++) {*/
        /*for (j=0; j<w; j++) {*/
                /*[>pmap[i][j] -= 1.0;<]*/
                /*pmap[i][j] /= (double)tot;*/
        /*}*/
        /*}*/
}

#define DEC(x, min) ((x) > (min)) ? ((x)-1) : (x)
#define INC(x, max) ((x) < (max)) ? ((x)+1) : (x)

inline void smooth_lo_vrt(double **pmap, int h, int w, float limit, int span)
{
        int u, d;
        int i;
        int j;
        int n;

        for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {

                for (n=0; n<span; n++) {
                        u = DEC(i+n, 0);
                        d = INC(i+n, h);

                        /* Cursor is lower than surroundings */
                        if ((pmap[i][j] <  limit) &&
                            (pmap[u][j] >= limit) &&
                            (pmap[d][j] >= limit)) 
                        { 
                                pmap[i][j] = pmap[u][j];
                                break;
                        }
                }
        }
        }
}
inline void smooth_hi_vrt(double **pmap, int h, int w, float limit, int span)
{
        int u, d;
        int i;
        int j;
        int n;

        for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {

                for (n=0; n<span; n++) {
                        u = DEC(i+n, 0);
                        d = INC(i+n, h);

                        /* Cursor is higher than surroundings */
                        if ((pmap[i][j] >= limit) &&
                            (pmap[u][j] <  limit) &&
                            (pmap[d][j] <  limit)) 
                        { 
                                pmap[i][j] = pmap[u][j];
                                break;
                        }
                }
        }
        }
}

inline void smooth_hi_hoz(double **pmap, int h, int w, float limit, int span)
{
        int l, r;
        int i;
        int j;
        int n;

        for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {

                for (n=0; n<span; n++) {
                        l = DEC(j+n, 0);
                        r = INC(j+n, w);

                        /* Cursor is higher than surroundings */
                        if ((pmap[i][j] >= limit) &&
                            (pmap[i][l] <  limit) &&
                            (pmap[i][r] <  limit)) 
                        { 
                                pmap[i][j] = pmap[i][l];
                                break;
                        }
                }
        }
        }
}

inline void smooth_lo_hoz(double **pmap, int h, int w, float limit, int span)
{
        int l, r;
        int i;
        int j;
        int n;

        for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {

                for (n=0; n<span; n++) {
                        l = DEC(j+n, 0);
                        r = INC(j+n, w);

                        /* Cursor is lower than surroundings */
                        if ((pmap[i][j] <  limit) &&
                            (pmap[i][l] >= limit) &&
                            (pmap[i][r] >= limit)) 
                        { 
                                pmap[i][j] = pmap[i][l];
                                break;
                        }
                }
        }
        }
}

inline void smooth_cycle(double **pmap, int h, int w, float limit, int cycle)
{
        while (cycle-->0) {
                smooth_hi_vrt(pmap, h, w, limit, 1);
                smooth_lo_vrt(pmap, h, w, limit, 1);
                smooth_hi_hoz(pmap, h, w, limit, 1);
                smooth_lo_hoz(pmap, h, w, limit, 1);
        }
}
inline void smooth_hi_cycle(double **pmap, int h, int w, float limit, int cycle)
{
        while (cycle-->0) {
                smooth_hi_vrt(pmap, h, w, limit, 1);
                smooth_hi_hoz(pmap, h, w, limit, 1);
        }
} 
inline void smooth_lo_cycle(double **pmap, int h, int w, float limit, int cycle)
{
        while (cycle-->0) {
                smooth_lo_vrt(pmap, h, w, limit, 1);
                smooth_lo_hoz(pmap, h, w, limit, 1);
        }
} 

enum zcodes { CUR=0, U=1, D=2, L=3, R=4, UL=5, UR=6, BL=7, BR=8 };
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


#define LAYER(mort, n, ...) or_nibble(rb_data(map->tree, (mort)), LAY, n, __VA_ARGS__)
#define ELEV(mort, n, ...) or_nibble(rb_data(map->tree, (mort)), ALT, n, __VA_ARGS__)




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
        #define CHUNK_INITIAL 3
        #define CHUNK_MIN 1
        /*#define OCEAN_LEVEL 0.10*/
        #define SHOAL_LEVEL 0.00
        #define BEACH_LEVEL 0.30
        #define TERRA_LEVEL 0.40
        #define TREE_PROB 1.0 

        int ymax;    // loop boundaries for map cursor
        int xmax; 
        int imax;    // loop boundaries for chunk drawing
        int jmax; 
        int map_h;   // Dimensions of entire map
        int map_w; 
        int map_y; 
        int map_x;     
        int tree_h;  // Dimensions of trees during drawing
        int tree_w; 
        int tree_y; 
        int tree_x;   
        int chunk;   // Square units to draw at a time 
        int chunk_h; // Height of chunk
        int chunk_w; // Width of chunk
        /*uint32_t z;  // for computing Morton codes*/
        int i;
        int j;

        map_h = map->ufo.box.h;
        map_w = map->ufo.box.w;

        chunk = CHUNK_INITIAL;
        ymax  = (map_h-1)-CHUNK_MIN; // beware the fencepost
        xmax  = (map_w-1)-CHUNK_MIN;

        cchar_t *bgtop; // background tile 
        bgtop = get_tile(__Gra__);

        perlin_boost(pmap, ymax, xmax, SMOOTH, OCTAVES);

        smooth_cycle(pmap, ymax, xmax, SHOAL_LEVEL, 4);
        smooth_hi_cycle(pmap, ymax, xmax, TERRA_LEVEL, 4);
        smooth_lo_cycle(pmap, ymax, xmax, TERRA_LEVEL, 1);
        smooth_cycle(pmap, ymax, xmax, SHOAL_LEVEL, 4);


        for (map_y=0; map_y<ymax; map_y++) {
        for (map_x=0; map_x<xmax; map_x++) {

                fill_codes(ymax, xmax, map_y, map_x);

                if (pmap[map_y][map_x] < SHOAL_LEVEL) 
                        mvwadd_wch(PEEK(map->L[BGR]), map_y, map_x, &OCEAN[0]);
                else if (pmap[map_y][map_x] < BEACH_LEVEL) 
                        mvwadd_wch(PEEK(map->L[TOP]), map_y, map_x, &__LAGOON[0]);
                else if (pmap[map_y][map_x] < TERRA_LEVEL) 
                        mvwadd_wch(PEEK(map->L[TOP]), map_y, map_x, &SAND);
                /*else if (pmap[map_y][map_x] < TERRA_LEVEL) */
                else {
                        mvwadd_wch(PEEK(map->L[TOP]), map_y, map_x, bgtop); // top
                        set_state(map->tree, z[CUR], 0, LAY, TOP);
                        set_state(map->tree, z[CUR], 0, SED, LIME);
                        set_state(map->tree, z[CUR], 0, SOI, MOLL);
                        set_state(map->tree, z[CUR], 0, ALT, 4);
                }
        }
        }

        for (map_y=0; map_y<ymax; map_y++) {
        for (map_x=0; map_x<xmax; map_x++) {

                fill_codes(ymax, xmax, map_y, map_x);

                if ((LAYER(z[CUR], 1, TOP)) &&
                   !(LAYER(z[D]  , 1, TOP))) 
                {
                        mvwadd_wch(PEEK(map->L[TOP]), map_y, map_x, &MTN[2]);
                        set_state(map->tree, z[CUR], 0, LAY, DRP);
                        set_state(map->tree, z[CUR], 0, SED, LIME);
                        set_state(map->tree, z[CUR], 0, SOI, MOLL);
                        set_state(map->tree, z[CUR], 0, ALT, 4);
                }
        }
        }

        return;


                /*chunk = (flip_biased(0.57)) ? (chunk+1) : (chunk-1);*/
                /*chunk = (chunk < CHUNK_MIN) ? CHUNK_MIN : chunk;*/

                /*chunk_h = chunk_w = chunk;*/

                /*imax  = map_y + chunk_h;*/
                /*jmax  = map_x + chunk_w;*/

                /*if (imax > map_h) chunk_h = imax-(map_h);*/
                /*if (jmax > map_w) chunk_w = jmax-(map_w);*/

                /*// Draw the ground box*/
                /*for (i=map_y; i<=imax; i++) {*/
                /*for (j=map_x; j<jmax; j++) {*/
                        /*z = MORT(i, j);*/
                        /*if (i == imax) {*/
                                /*mvwadd_wch(PEEK(map->L[DRP]), i, j, &MTN[2]);*/
                                /*set_state(map->tree, z, 0, LAY, DRP);*/
                                /*set_state(map->tree, z, 0, SED, LIME);*/
                                /*set_state(map->tree, z, 0, SOI, MOLL);*/
                                /*set_state(map->tree, z, 0, ALT, 4);*/
                        /*}*/
                        /*else {*/
                                /*mvwadd_wch(PEEK(map->L[TOP]), i, j, bgtop); // top*/
                                /*set_state(map->tree, z, 0, LAY, TOP);*/
                                /*set_state(map->tree, z, 0, SED, LIME);*/
                                /*set_state(map->tree, z, 0, SOI, MOLL);*/
                                /*set_state(map->tree, z, 0, ALT, 4);*/
                        /*}*/
                /*}*/
                /*}*/

                /*// Decide whether to draw the tree box*/
                /*if ((flip_biased(TREE_PROB))||(chunk_w < 4)) continue;*/
                /*else {*/
                        /*tree_x = (map_x+1);*/
                        /*tree_y = (map_y-1);*/
                        /*tree_h = (chunk_h-1);*/
                        /*tree_w = (chunk_w-2);*/
                        /*imax   = tree_y + tree_h; // recalculation*/
                        /*jmax   = tree_x + tree_w; // recalculation*/
                /*}*/

                /*// Draw the tree box*/
                /*for (i=tree_y; i<=imax; i++) {*/
                /*for (j=tree_x; j<jmax; j++) {*/
                        /*z = MORT(i, j);*/
                        /*if (i == imax) {*/
                                /*set_state(map->tree, z, 0, LAY, VEG);*/
                                /*set_state(map->tree, z, 0, SED, LIME);*/
                                /*set_state(map->tree, z, 0, SOI, MOLL);*/
                                /*mvwadd_wch(PEEK(map->L[VEG]), i, j, &TREE[0]);*/
                        /*}*/
                        /*else {*/
                                /*mvwadd_wch(PEEK(map->L[VEG]), i, j, &TREE[1]);*/
                                /*set_state(map->tree, z, 0, LAY, VEG);*/
                                /*set_state(map->tree, z, 0, SED, LIME);*/
                                /*set_state(map->tree, z, 0, SOI, SPOD);*/
                        /*}*/
                /*}*/
                /*}*/
        /*}*/
        /*}*/

}




#define MAKE_TILE_BGR                                \
        set_state(map->tree, z[CUR], 0, LAY, XXX);   \
        mvwadd_wch(PEEK(map->L[TOP]), i, j, &BLANK)  \

#define ERODE(z)                                    \
        dec_nibble(_rb_data(map->tree, (z)), ALT); \
        set_state(map->tree, (z), 0, LAY, GRO);    \
        mvwadd_wch(PEEK(map->L[TOP]), i, j, &BLANK)

void erode_beach(struct map_t *map)
{
        #define INV_EROSION_POTENTIAL 7
        #define GENERATIONS 2 
        #define HIGROUND 4
        #define BEACH 3 
        #define SHALLOW 2
        #define SHELF 1 
        #define SEALEVEL 0
        cchar_t cch;
        int i, j, g, x;
        int imax;
        int jmax;

        imax = map->ufo.box.h;
        jmax = map->ufo.box.w;

        /* Seed */
        /*for (i=0; i<imax; i++) {*/
        /*for (j=0; j<jmax; j++) {*/

                /*fill_codes(imax, jmax, i, j);*/

                /*if (LAYER(z[CUR], 1, XXX)) continue;*/
                /*[>if (LAYER(z[U],   1, XXX)) continue;<]*/

                /*if (roll_fair(INV_EROSION_POTENTIAL) != 0) */
                        /*continue;*/

                /*if (LAYER(z[R], 1, XXX) || */
                    /*LAYER(z[L], 1, XXX) ||*/
                    /*LAYER(z[U], 1, XXX) || // NO TOUCH*/
                    /*LAYER(z[D], 1, XXX)) {*/
                        /*set_state(map->tree, z[CUR], 0, LAY, GRO);*/
                        /*dec_nibble(_rb_data(map->tree, z[CUR]), ALT);*/
                /*}*/
        /*}*/
        /*}*/

        g = GENERATIONS;

        while (g-->0) {
                for (i=0; i<imax; i++) {
                for (j=0; j<jmax; j++) {

                        fill_codes(imax, jmax, i, j);

                        /*if (ELEV(z[CUR], 2, HIGROUND, SEALEVEL)) */
                                /*continue;*/
                        if (LAYER(z[CUR], 1, XXX)) continue;


                        if (roll_fair(INV_EROSION_POTENTIAL) != 0)
                                continue;

                        if (LAYER(z[R], 1, XXX) ||
                            LAYER(z[L], 1, XXX) ||
                            LAYER(z[U], 1, XXX) || // NO TOUCH
                            LAYER(z[D], 1, XXX)) {
                                for (x=0; x<9; x++) {
                                        if (flip_biased(0.7))
                                                ERODE(z[x]);
                                }
                        }
                }
                }
        }
        for (i=0; i<imax; i++) {
        for (j=0; j<jmax; j++) {

                fill_codes(imax, jmax, i, j);

                if (ELEV(z[CUR], 1, HIGROUND) &&
                    ELEV(z[D], 4, BEACH, SHALLOW, SHELF, SEALEVEL))
                {
                        /*if (ELEV(z[U], 1, HIGROUND))*/
                        /*{*/
                                /*mvwadd_wch(PEEK(map->L[TOP]), i, j, &MTN[2]);*/
                                /*set_state(map->tree, z, 0, LAY, DRP);*/
                                mvwadd_wch(PEEK(map->L[TOP]), i-1, j, &GRASS[1]);
                                /*mvwadd_wch(PEEK(map->L[TOP]), i, j, &MTN[2]);*/
                                /*continue;*/
                        /*}*/
                        /*else*/
                        /*{*/
                                /*set_state(map->tree, z[CUR], 0, LAY, GRO);*/
                                /*dec_nibble(_rb_data(map->tree, z[CUR]), ALT);*/
                        /*}*/
                }

                if (!LAYER(z[CUR], 1, GRO)) continue;

                if (ELEV(z[CUR], 1, SEALEVEL)) {
                        MAKE_TILE_BGR;
                        continue;
                }


                if (ELEV(z[L], 1, HIGROUND) &&
                    ELEV(z[R], 1, HIGROUND))
                {
                        set_state(map->tree, z[CUR], 0, LAY, TOP);
                        set_state(map->tree, z[CUR], 0, ALT, HIGROUND);
                        if (LAYER(z[U], 1, XXX))
                                mvwadd_wch(PEEK(map->L[TOP]), i, j, &GRASS[1]);
                        else if (LAYER(z[D], 1, XXX)) {
                                mvwadd_wch(PEEK(map->L[TOP]), i, j, &MTN[2]);
                                mvwadd_wch(PEEK(map->L[TOP]), i-1, j, &GRASS[1]);
                        }
                        continue;
                }
                if (ELEV(z[CUR], 1, SHELF)) {
                        setcchar(&cch, &BRDOT[2][roll_fair(DOT2)], A_REVERSE, A_SEA_LAGOON, NULL);
                        /*if (ELEV(z[D], 1, SEALEVEL))*/
                                /*mvwadd_wch(PEEK(map->L[TOP]), i+1, j, &cch);*/
                        /*else if (ELEV(z[U], 1, SEALEVEL))*/
                                /*mvwadd_wch(PEEK(map->L[TOP]), i-1, j, &cch);*/
                        /*else if (ELEV(z[L], 1, SEALEVEL))*/
                                /*mvwadd_wch(PEEK(map->L[TOP]), i, j-1, &cch);*/
                        /*else if (ELEV(z[R], 1, SEALEVEL))*/
                                /*mvwadd_wch(PEEK(map->L[TOP]), i, j+1, &cch);*/

                        if (flip_biased(0.7))
                                mvwadd_wch(PEEK(map->L[TOP]), i, j, &__LAGOON[0]);
                        else
                                mvwadd_wch(PEEK(map->L[TOP]), i, j, &cch);
                }

                if (ELEV(z[CUR], 1, SHALLOW))
                        mvwadd_wch(PEEK(map->L[TOP]), i, j, &__LAGOON[0]);
                if (ELEV(z[CUR], 1, BEACH)) {
                        if (LAYER(z[U], 1, XXX) &&
                            LAYER(z[UR], 1, XXX) &&
                            LAYER(z[UL], 1, XXX)) {
                                MAKE_TILE_BGR;
                                continue;
                        }
                        mvwadd_wch(PEEK(map->L[TOP]), i, j, &SAND);
                }
                /*}*/
        }
        }
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
