#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "colortest.h"

#define NCOLORS 11
#define MAXRAMP 11 

wchar_t wch[]=L"█";
cchar_t cch[MAXRAMP];


#define __WIN colortest_win
#define __PAN colortest_pan

WINDOW *__WIN;
PANEL  *__PAN;

#define WINH 20
#define WINW 25
#define WINY  0
#define WINX  0

short ncolor[NCOLORS];
short ncpair[MAXRAMP];

void init_colortest(int id, short startnumber)
{
        if (__WIN!=NULL) return;

        int i=0;
        __WIN = newwin(WINH, WINW, WINY, WINX);
        __PAN = new_panel(__WIN);

        do { 
                ncolor[i] = (short)startnumber+i;
                ncpair[i] = (short)startnumber+i;

                init_color(ncolor[i], TRIPLET(spect11, i)); 
                init_pair(ncpair[i], ncolor[i], COLOR_BLACK);

        } while (++i < ramplen[1]);
}



static const short STARTNUM = 30;


inline void load_ramp(int id)
{
        int i=0;
        init_colortest(id, STARTNUM);

        do { setcchar(&cch[i], wch, 0, ncpair[i], NULL); 
        } while (++i < ramplen[1]);
}



PANEL *colortest_panel(void)
{
        return __PAN;
}





void paint_ramp(int id)
{
        #define SW_W 3
        #define SW_H 3
        #define SW_X 3
        #define SW_Y 3

        int i, j;

        load_ramp(id);

        for (i=0; i<ramplen[1]; i++) {

                j=SW_H;
                while (j-->0) {
                        mvwhline_set(__WIN, (SW_Y+i)+j, SW_X, &cch[i], SW_W);
                }

                update_panels();
        }
        doupdate();
}




