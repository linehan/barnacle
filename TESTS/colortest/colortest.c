#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "rgb_nc.h"

#define NCOLORS 11

wchar_t wch[]=L"█";
cchar_t cch[MAXRAMPLEN];

#define __WIN colortest_win
#define __PAN colortest_pan

WINDOW *__WIN;
PANEL  *__PAN;

#define WINH 35
#define WINW 25
#define WINY  3
#define WINX  3

short ncolor[MAXRAMPLEN];
short ncpair[MAXRAMPLEN];

inline void init_colortest(void)
{
        if (__WIN!=NULL) return;
        else {
                __WIN = newwin(WINH, WINW, WINY, WINX);
                __PAN = new_panel(__WIN);
        }
}



PANEL *colortest_panel(void)
{
        return __PAN;
}


void paint_palette(struct palette_t *palette)
{
        #define SW_W 3
        #define SW_H 3

        int i;
        if (__WIN==NULL) init_colortest();

        werase(__WIN);
        build_swatches(palette);

        mvwprintw(__WIN, 0, (SW_W*3), "%s", ramp[0]->name);

        for (i=0; i<ramp[0]->len; i++) {
                mvwprintw(__WIN, i, 0, "%02d", i);
                mvwhline_set(__WIN, i, SW_W, &cch[i], SW_W);
        }
        update_panels();
        doupdate();
}


