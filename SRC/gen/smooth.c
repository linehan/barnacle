#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "smooth.h"



#define DEC(x, min) ((x) > (min)) ? ((x)-1) : (x)
#define INC(x, max) ((x) < (max)) ? ((x)+1) : (x)



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
        float scale;
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

                u = DEC(i, 0);
                d = INC(i, h);

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

                l = DEC(j, 0);
                r = INC(j, w);

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

