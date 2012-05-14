// vim:fdm=marker
/*
   About Perlin Noise                                                       {{{1
 *
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
 *************************************************************************}}}1*/

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "stoc.h"


// Simplex Noise
//=============================================================================


#define PERMUTATIONS 512 /* Number of permutations of GRADIENT3 */
#define HALFPERM     256

/*
 * Unit vector magnitudes, representing the gradient values at each vertex
 * of a 2D simplex (which is a triangle) 
 */ 
static signed int GRADIENT3[12][3]={ {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
                                     {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
                                     {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1} };


/* Permutation tables. The length of the final permutation table is doubled 
 * to remove the need for index wrapping. */
signed int PERM_ORDERED[HALFPERM];   
signed int PERM_SHUFFLED[HALFPERM]; 
signed int PERM[PERMUTATIONS];       


//-----------------------------------------------------------------------------


/*
 * If you are completely positive that you won't be overflowing.
 */
inline signed int fastfloor(double x)
{
        return (x > 0) ? (signed int)x : (signed int)x-1;
}


/*
 * Return the dot product of x and y on the unit axes (from GRADIENT3) 
 */
inline double dot(int *g, double x, double y)
{
        return (g[0]*x + g[1]*y);
}


/* 
 * Load the ordered permutation array.
 */
void init_simplex(void)
{
        int i;
        for(i=0; i<HALFPERM; i++) {
                PERM_ORDERED[i] = i;
        }
}


/* 
 * Shuffle the permutation array and re-apply the permutation mask 
 */ 
inline void permute(void)
{
        int i;

        /*shuf(PERM_ORDERED, PERM_SHUFFLED, PERMUTATIONS, sizeof(signed int));*/
        fy_shuffle(PERM_ORDERED, PERM_SHUFFLED, HALFPERM);

        for(i=0; i<PERMUTATIONS; i++) {
                PERM[i] = PERM_SHUFFLED[i&255]; /* bit-o-magic */
        }
}


//-----------------------------------------------------------------------------


/* 
 * Return a simplex noise value given coordinates x, y
 */
double simplex_noise(double yin, double xin)
{
        double skew;          // Input space (x,y) skewed to simplex space (i,j)
        double unskew;        // Simplex space (i,j) skewed to input space (x,y)
        double skew_factor;   // Transforms (x,y) to (i,j)
        double unskew_factor; // Transforms (i,j) to (x,y)
        double X0, Y0;        // Simplex cell origin in (x,y)
        double x0, y0;        // Distance of xin/yin from cell origin in (x,y)
        double x1, y1;        // Offsets of second corner in (x,y)
        double x2, y2;        // Offsets of third corner in (x,y)

        signed int i, j;      // Simplex space coordinates
        signed int i1, j1;    // Offsets for second corner of simplex.
        /* 
         * The unit simplex has 3 corners with offsets (0,0), (1,0), or (1,1).
         * The second corner can be either (1,0) or (0,1), depending on
         * which simplex contains (xin,yin), so we need to compute which
         * ordered pair to use, and store it, in order to calculate the
         * appropriate equivalent offsets in (x,y) that will be stored in
         * x1 and y1.
         *
         * To determine whether to use (0,1) or (1,0) for the offset of
         * the second corner, we need to determine which of the two simplices 
         * contains (xin,yin). 
         * 
         * We can see that if x0>y0, we are in the "lower" triangle, and if 
         * y0>x0, we are in the "upper" triangle: 
         *      ___
         *     |  /|   (Imagine a square)
         *    y| / |
         *     |/__|
         *       x
         */
        signed int ii, jj; // Gradient index variables
        signed int gi0, gi1, gi2; // Hashed gradient indices

        double t0, t1, t2; // Gradient contribution from each corner
        double n0, n1, n2; // Noise contribution from each corners

        /* Compute skew and unskew constants for 2D */
        skew   = 0.5*(sqrt(3.0)-1.0); // ((sqrt(n+1)-1)/n)
        unskew = (3.0-sqrt(3.0))/6.0; // ((n+1)-(sqrt(n+1)))/(n*(n+1))

        /* Transform (xin,yin) to simplex space (i,j) */
        skew_factor = (xin+yin)*skew;
        i = fastfloor(xin+skew_factor);
        j = fastfloor(yin+skew_factor);

        /* Transform origin of simplex cell containing (i,j) to (x,y) */
        unskew_factor = (i+j)*unskew;
        X0 = i-unskew_factor; 
        Y0 = j-unskew_factor;

        /* Compute distance between xin/yin and the cell origin */
        x0 = xin-X0;
        y0 = yin-Y0;

        /* Determine which simplex and thus which ordered pairs to use */
        if (x0 > y0) {  /* lower triangle, XY order: (0,0)->(1,0)->(1,1) */
                i1=1; 
                j1=0; 
        } else {        /* upper triangle, YX order: (0,0)->(0,1)->(1,1) */
                i1=0;                 
                j1=1;
        }

        /* Compute offsets of second corner in (x,y) */
        x1 = x0 - i1 + unskew;
        y1 = y0 - j1 + unskew;

        /* Compute offsets of third corner in (x,y) */
        x2 = x0 - 1.0 + 2.0 * unskew; 
        y2 = y0 - 1.0 + 2.0 * unskew;
        /* 
          Compute hashed gradient indices for the three corners
        */
        ii = i & 255;
        jj = j & 255;
        gi0 = PERM[ii+PERM[jj]] % 12;
        gi1 = PERM[ii+i1+PERM[jj+j1]] % 12;
        gi2 = PERM[ii+1+PERM[jj+1]] % 12;

        /* Compute gradient contribution from each corner */
        t0 = 0.5 - x0*x0-y0*y0;
        if (t0 < 0) 
                n0 = 0.0;
        else {
                t0 *= t0;
                n0 = t0 * t0 * dot(GRADIENT3[gi0], x0, y0);
        }
        t1 = 0.5 - x1*x1-y1*y1;
        if (t1 < 0) 
                n1 = 0.0;
        else {
                t1 *= t1;
                n1 = t1 * t1 * dot(GRADIENT3[gi1], x1, y1);
        }
        t2 = 0.5 - x2*x2-y2*y2;
        if (t2 < 0) 
                n2 = 0.0;
        else {
                t2 *= t2;
                n2 = t2 * t2 * dot(GRADIENT3[gi2], x2, y2);
        }

        /* Sum corners to arrive at final noise value, scale to 
         * interval [-1,1], and return */
        return (70.0 * (n0 + n1 + n2));
}




/*
 * Return a matrix of simplex noise
 */
double **simplex_matrix(int h, int w)
{
        int shuffled;       // counts down the number of permutations left
        int row, col;  // the row and column number to be iterated over
        double **pmap;      // the perlin map to be returned
        
        shuffled = PERMUTATIONS; 

        pmap = malloc(h * sizeof(double *));
        for (row=0; row<h; row++) {
                pmap[row] = malloc(w * sizeof(double));
        }

        permute(); 

        for (row=0; row<h; row++) {
                for (col=0; col<w; col++) {
                        pmap[row][col] = simplex_noise(row, col);
                        if (shuffled-- <= 0) {
                                permute();
                                shuffled = PERMUTATIONS;
                        }
                }
        }
        return pmap;
}


/*
 * Return a matrix with no values
 */
double **empty_simplex_matrix(uint32_t h, uint32_t w)
{
        double **new;
        int i;

        new = malloc(h * sizeof(double *));

        for (i=0; i<h; i++)
                new[i] = calloc(w, sizeof(double));

        return (new);
}



