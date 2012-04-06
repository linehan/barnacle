#include "../testplex.h"
#include "../../lib/stoc/noise.h"
#include "../../lib/stoc/smooth.h"
#include "../../lib/mymath.h"
#include "../../lib/ncolor/src/ncolor.h"

#define _C_(n, tot) (tot-n)/2

void pmap_test(int testpanel_index, uint32_t h, uint32_t w)
{
        #define OCTAVES 6
        #define SMOOTH 0.99

        int i, j;
        double **pmap;
       
        pmap = simplex_matrix(h, w);
        perlin_smooth(pmap, h, w, SMOOTH, OCTAVES);

        for (i=0; i<h; i++) {
                for (j=0; j<w; j++) {

                        pal_printw(TESTWIN[testpanel_index], 
                                   _C_(h, LINES)+i, _C_(w, COLS)+j, L"█", 
                                   fnorm(pmap[i][j], (float)-1.0, (float)1.0), TESTPAL);

                        update_panels();
                        doupdate();
                }
        }
}
