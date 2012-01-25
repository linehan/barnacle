// vim:fdm=marker
/*******************************************************************************
 FILENAME:  perlin.c
 Implement a Perlin simplex noise algorithm to generate 2-dimensional noise.
 The algorithm is a bit complicated, so either treat it like a black box, or
 pay attention to the documentation here.
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <ncurses.h>
#include <panel.h>
#include <math.h>

#include "../pan/test.h"
#include "dice.h"
/******************************************************************************/
/* Perlin Noise
 * The basic idea behind Perlin noise is best explained in two dimensions.
 * Imagine an integer coordinate grid. At each grid point (x,y), a prandom 
 * 2D gradient is chosen. For an arbitrary point P on the surface of this
 * 2D plane, the noise value at P is computed from the gradient values at
 * the closest vertices of the square containing P.
 *
 * The key refinement to the classical Perlin noise algorithm is the swapping
 * of a regular square grid for a simplex grid, or simplical tesselation of
 * N-space.
 *
 *      For a space with N dimensions, the simplex is the most compact
 *      shape that can be repeated to fill the entire space.
 *
 * The simplest shape that tiles a 2D plane is not, in fact, a square, but
 * a triangle, an equilateral triangle to be precise. Two equilateral
 * triangles make a rhombus, which can be thought of as a square with a
 * compressed main diagonal.
 *
 * The simplex shape for N dimensions is a shape with N+1 vertices, and N!
 * of these shapes can fill an N-dimensional hypercube that has been squashed
 * along its main diagonal.
 *
 * A simplex shape has as few corners as possible, which makes it much easier
 * to interpolate values in the interior of the simplex based on the values at
 * its corners. While a hypercube in N dimensions has 2^N corners, a simplex
 * in N dimensions has only N+1 corners. This turns an O(2^N) problem into an
 * O(N) problem.
 *
 * Instead of computing the analytic derivative of the interpolated noise
 * function, simplex noise allows the simple summation of contributions from
 * each corner, because in simplex noise, the contribution of each corner is
 * a multiplication of the extrapolation of the gradient ramp and a radially
 * symmetric attenuation function which is carefully chosen so that the
 * influence from each corner reaches zero before crossing the boundary to the
 * next simplex. This means that points inside a simplex will only be
 * influenced by the contributions from the corners of that particular simplex.
 *
 * For 2D, the influence from each corner can be visualized as a small wiggle
 * function (Perlin uses the term "surflet") around the corner point, with
 * any point on the surface having at most three non-zero parts of a wiggle
 * covering it.
 *
 * The final trick is to decide which simplex you are in for any point in
 * space. First you skew the input coordinate space along the main diagonal
 * so that each squashed hypercube of N! simplices transforms to a regular
 * axis-aligned N-dimensional hypercube. Then you can easily decide by taking
 * the integer part of the coordinates for each dimension, and comparing these
 * magnitudes from the hypercube origin to the point in question.
 *
 *  ____________________     
 * |\  /\  /\  /\  /\  /| 
 * |_\/__\/__\/__\/__\/_|    
 * | /\  /\  /\  /\  /\ |   
 * |/__\/__\/__\/__\/__\|  
 * |\  /\  /\  /\  /\  /|  
 * |_\/__\/__\/__\/__\/_| 
 * | /\  /\  /\  /\  /\ |  
 * |/__\/__\/__\/__\/__\|   
 * |\  /\  /\  /\  /\  /|   
 * |_\/__\/__\/__\/__\/_| 
 *
 ******************************************************************************/
static signed int GRADIENT3[12][3] = { {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
                                       {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
                                       {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1}  };

static signed int PERM_ORDERED[256];
static signed int PERM_SHUFFLED[256];

/* To remove the need for index wrapping, double the permutation table length */
static signed int PERM[512];

/* Don't waste time using the insanely slow floor() function */
signed int fastfloor(double x)
{
        return x>0 ? (signed int)x : (signed int)x-1;
}

/* Return the scalar (dot) product of x and y on the unit axes (from GRADIENT3) */
double dot(int *g, double x, double y)
{
        return (g[0]*x + g[1]*y);
}

/* Implement the inside-out version of the Fisher-Yates shuffle algorithm,
 * which generates a random permutation of a finite set. Here, we use the
 * algorithm to shuffle the permutation table. */
void fy_shuffle(int *source, int *shuffle, int n)
{
        int i, j;
        shuffle[0] = source[0];
        for (i=1; i<(n-1); i++) {
             j = roll_fair(i);
             shuffle[i] = shuffle[j];
             shuffle[j] = source[i];
        }
}

/* Initialize the ordered permutation array. */
void simplex_init(void)
{
        int i;
        for(i=0; i<256; i++) {
                PERM_ORDERED[i] = i;
        }

}

/* Shuffle the permutation array and re-apply the permutation mask */
void perm_shuffle(void)
{
        int i;

        fy_shuffle(PERM_ORDERED, PERM_SHUFFLED, 256);
        for(i=0; i<512; i++) {
                PERM[i] = PERM_SHUFFLED[i&255];
        }
}

/* Generate a simplex noise value for coordinates xin, yin */
double simplex_noise(double xin, double yin)
{
        /* Noise contributions from the three corners */
        double n0, n1, n2;
        /* The generalized skew and unskew equations are
         *
         *
         *   Skew:  ((sqrt(n+1)-1)/n)
         * Unskew:  ((n+1)-(sqrt(n+1)))/(n*(n+1))   
         *
         */
        /* Skew the input space (x,y) to the simplex space (i,j) */
        double skew = 0.5*(sqrt(3.0)-1.0);
        /* Skew the simplex space (i,j) to the input space (x,y) */
        double unskew = (3.0-sqrt(3.0))/6.0;

        double skew_factor = (xin+yin)*skew;
        /* The skewed equivalents of the input space (x,y) are (i,j). */
        signed int i = fastfloor(xin+skew_factor);
        signed int j = fastfloor(yin+skew_factor);
        /* Unskew the cell origin back to (x,y) space */
        double unskew_factor = (i+j)*unskew;
        double X0 = i-unskew_factor; 
        double Y0 = j-unskew_factor;
        /* Calculate the (x,y) distances from the cell origin */
        double x0 = xin-X0;
        double y0 = yin-Y0;

        /* For the 2D case, the simplex shape is an equilateral triangle.
         * Determine which simplex we are in. It should be apparent that
         * if x>y, we are in the lower triangle, and if y>x, we are in the
         * upper triangle:     ___
         *                    |  /|   (Imagine it 's a square, ok?)
         *                   y| / |
         *                    |/__|
         *                      x
         */
        /* Offsets for second (middle) corner of simplex in (i,j) coordinates */
        signed int i1, j1; 
        
        if (x0>y0) {  /* lower triangle, XY order: (0,0)->(1,0)->(1,1) */
                i1=1; 
                j1=0; 
        } else {      /* upper triangle, YX order: (0,0)->(0,1)->(1,1) */
                i1=0;                 
                j1=1;
        }

        /* A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
           a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
           c = (3-sqrt(3))/6 */
        
        /* Offsets for middle corner in (x,y) unskewed */
        double x1 = x0 - i1 + unskew;
        double y1 = y0 - j1 + unskew;
        /* Offsets for last corner in (x,y) unskewed */
        double x2 = x0 - 1.0 + 2.0 * unskew; 
        double y2 = y0 - 1.0 + 2.0 * unskew;

        /* Work out the hashed gradient indices of the three simplex corners */
        signed int ii = i & 255;
        signed int jj = j & 255;
        signed int gi0 = PERM[ii+PERM[jj]] % 12;
        signed int gi1 = PERM[ii+i1+PERM[jj+j1]] % 12;
        signed int gi2 = PERM[ii+1+PERM[jj+1]] % 12;

        /* Calculate the contribution from the three corners */
        double t0 = 0.5 - x0*x0-y0*y0;
        if (t0<0) n0 = 0.0;
        else {
                t0 *= t0;
                n0 = t0 * t0 * dot(GRADIENT3[gi0], x0, y0);
        }

        double t1 = 0.5 - x1*x1-y1*y1;
        if (t1<0) n1 = 0.0;
        else {
                t1 *= t1;
                n1 = t1 * t1 * dot(GRADIENT3[gi1], x1, y1);
        }

        double t2 = 0.5 - x2*x2-y2*y2;
        if (t2<0) n2 = 0.0;
        else {
                t2 *= t2;
                n2 = t2 * t2 * dot(GRADIENT3[gi2], x2, y2);
        }
        /* Add contributions from each corner to get the final noise value */
        /* The result is then scaled to return values in the interval [-1,1] */
        return (70.0 * (n0 + n1 + n2));
}
        
void test_simplex_noise(double sea_level)
{
        perm_shuffle();

        int i, j;
        double noise;

        for (i=0;i<30;i++) {
                wprintw(DIAGNOSTIC_WIN, "\n  ");
                for (j=0;j<30;j++) {
                       noise = simplex_noise(i, j); 
                       /*printf("%+g\n", noise);*/
                       if (noise>sea_level) wprintw(DIAGNOSTIC_WIN, "#");
                       else wprintw(DIAGNOSTIC_WIN, "~");
                }
        }
        wprintw(DIAGNOSTIC_WIN, "\n  Perlin map with sea level == %g", sea_level);
}
