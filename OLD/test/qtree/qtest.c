#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "quadtree.h"
#include "morton.h"
#include "mergesort.h"

#define COLS 10
#define ROWS 10

int main(void)
{
        uint32_t cols = 5;
        uint32_t rows = 5;

        uint32_t i, j, k;

        uint32_t m[cols*rows];
        /*struct qtree *qtr;*/

        int n = 0;
        int nn;

        for (i=0; i<rows; i++) {
                for (j=0; j<cols; j++) {
                        mort(i, j, &k);
                        m[n] = k;
                        n++;
                        /*qtr = qtree_insert(qtr, k);*/
                }
        }
        merge_sort(m, n);

        printf("n: %u\n", n);
        for (nn=0; nn<n; nn++) {
                printf("m[%u]: %ju\n", nn, (uintmax_t)m[nn]);
        }
        /*for (nn=0; nn<n-1; nn++) {*/
                /*printf("m[%u] ^ m[%u]: %ju\n", nn, (nn+1), (uintmax_t)msb(m[nn], m[nn+1]));*/
        /*}*/
        for (nn=0; nn<n; nn++) {
                printf("msb of m[%u]: %u\n", nn, highest_bit_set(m[nn]));
        }

        int hsb0, hsb1;
        int quadrant;
        for (nn=0; nn<n; nn++) {
                hsb0 = highest_bit_set(m[nn]);
                hsb1 = highest_bit_set(m[nn+1]);

                if (hsb0 == hsb1)
                        quadrant = m[nn] ^ m[nn+1];
                else
                        quadrant = 9;
                printf("m[%u] hsb: %u\tm[%u] hsb: %u\tquadrant: %u\n", nn, hsb0, nn+1, hsb1, quadrant);

                /*printf("msb of m[%u]: %u\n", nn, highest_bit_set(m[nn]));*/
        }



        return (0);
}
