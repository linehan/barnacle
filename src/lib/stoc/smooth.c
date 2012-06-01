/* 
 * smooth.c -- Filters for the Perlin generators 
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
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "../../gfx/gfx.h"

#include "stoc.h"

#define INCREASE(a, max) (a < max) ? (a+1) : a
#define DECREASE(a, min) (a > min) ? (a-1) : a


/*
 * Returns an allocated 2D array of floats, ordered 'array[h][w]' (row-major)
 */
inline float **float2D(int h, int w)
{
        float **new = malloc(h * sizeof(float *));
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





/*
 * Allocate, generate, and return a 2D array of smooth noise values.
 */
float **gen_smooth(double **base, int h, int w, int oct)
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
 
        samp_period = 1 << oct;  // 2 ^ k
        samp_freq = 1.0/samp_period;
 
        for (i=0; i<h; i++) {
                /* compute vertical sampling indices */
                samp_i[0] = (i/samp_period) * samp_period;
                samp_i[1] = (samp_i[0]+samp_period) % h; // wrap 
                blend_vrt = (i - samp_i[0]) * samp_freq;
         
                for (j=0; j<w; j++) {
                        /* compute horizontal sampling indices */
                        samp_j[0] = (j/samp_period) * samp_period;
                        samp_j[1] = (samp_j[0] + samp_period) % w; // wrap 
                        blend_hoz = (j - samp_j[0]) * samp_freq;
                 
                        /* blend top corners */
                        top = interpolate(base[samp_i[0]][samp_j[0]],
                                          base[samp_i[1]][samp_j[0]], 
                                          blend_hoz);
                 
                        /* blend bottom corners */
                        bot = interpolate(base[samp_i[0]][samp_j[1]],
                                          base[samp_i[1]][samp_j[1]], 
                                          blend_hoz);
                 
                        /* final blend */
                        smooth[i][j] = interpolate(top, bot, blend_vrt);
                }
        }
        return smooth;
}




/*
 * Given a perlin map, smooth it over some number of octaves, using the
 * persist parameter as a smoothing factor. 
 *
 * NOTE: The pmap parameter is modified.
 */
void perlin_smooth(double **pmap, int h, int w, float persist, int octs)
{
        float ***smooth;
        float amp;
        float tot;
        int i;
        int j;
        int o;

        smooth = malloc(octs * sizeof(float **));

        for (o=0; o<octs; o++) {
                smooth[o] = gen_smooth(pmap, h, w, o);
        }

        tot = 0.0;
        amp = 1.0;

        for (o=0; o<octs; o++) {
                amp *= persist;
                tot += amp;

                for (i=0; i<h; i++) {
                for (j=0; j<w; j++) {
                        pmap[i][j] += (double)(smooth[o][i][j] * amp);
                }
                }
        }
        free(smooth);
}



/*
 * Smooth tiles in a 3x1 vector
 */
void smooth_vrt(double **pmap, int h, int w, float lim, int opt)
{
        int u, d;
        int i, j;

        for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {

                u = (i > 0) ? (i-1) : i;
                d = (i < h) ? (i+1) : i;

                /* Cursor is lower than surroundings */
                if ((opt == SMOOTH_LO)  &&
                    (pmap[i][j] <  lim) &&
                    (pmap[u][j] >= lim) &&
                    (pmap[d][j] >= lim)) goto dosmooth;
                /* Cursor is higher than surroundings */
                if ((pmap[i][j] >= lim) &&
                    (pmap[u][j] <  lim) &&
                    (pmap[d][j] <  lim)) goto dosmooth;
                else
                        continue;

                dosmooth: pmap[i][j] = pmap[u][j];
        }
        }
}




/*
 * Smooth tiles in a 1x3 vector
 */
void smooth_hoz(double **pmap, int h, int w, float lim, int opt)
{
        int l, r;
        int i, j;

        for (i=0; i<h; i++) {
        for (j=0; j<w; j++) {

                l = (j > 0) ? (j-1) : j;
                r = (j < w) ? (j+1) : j;

                /* Cursor is lower than surroundings */
                if ((opt == SMOOTH_LO)  &&
                    (pmap[i][j] <  lim) &&
                    (pmap[i][l] >= lim) &&
                    (pmap[i][r] >= lim)) goto dosmooth;
                /* Cursor is higher than surroundings */
                if ((pmap[i][j] >= lim) &&
                    (pmap[i][l] <  lim) &&
                    (pmap[i][r] <  lim)) goto dosmooth;
                else
                        continue;

                dosmooth: pmap[i][j] = pmap[i][l];
        }
        }
}



/*
 * Smooth a perlin map in certain patterns
 */
void smooth_cycle(double **pmap, int h, int w, float lim, int opt, int cycle)
{
        if (opt == SMOOTH_BO) {
                while (cycle-->0) {
                        smooth_vrt(pmap, h, w, lim, SMOOTH_LO);
                        smooth_vrt(pmap, h, w, lim, SMOOTH_HI);
                        smooth_hoz(pmap, h, w, lim, SMOOTH_LO);
                        smooth_hoz(pmap, h, w, lim, SMOOTH_HI);
                }
        }
        else {
                while (cycle-->0) {
                        smooth_vrt(pmap, h, w, lim, opt);
                        smooth_hoz(pmap, h, w, lim, opt);
                }
        }
}



/* 
 * DejaVu Mono, the font this program is designed to use, has an
 * aspect ratio of 0.55, with an x-width of 0.54, at 100pt.
 * That is, a given character -- or more precisely, the uniform
 * rectangle that holds each individual glyph -- is a little over
 * half as wide as it is tall.
 */
/*double **fit_char(int h, int w)*/
/*{*/
        /*double **big;*/
        /*double **pmap;*/
        /*int i;*/
        /*int j;*/
        /*int k;*/
        /*int w2 = w * 2;*/

        /*[>big  = simplex_matrix(h, w2);<]*/
        /*pmap = simplex_matrix(h, w);*/

        /*[>k = 0;<]*/
        /*for (i=0; i<h; i++) {*/
        /*for (j=0; j<w; j++) {*/
                /*if (i < h-1) {*/
                        /*pmap[i][j] = ((pmap[i][j] + pmap[i+1][j]) / 2.0);*/
                        /*[>k = k+2;<]*/
                /*}*/
        /*}*/
        /*}*/

        /*return pmap;*/
/*}*/




