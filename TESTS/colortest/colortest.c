#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "colortest.h"

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



static const short STARTNUM = 30;


inline void load_ramp(int id)
{
        int i=0;

        do { 
                ncolor[i] = (short)STARTNUM+i;
                ncpair[i] = (short)STARTNUM+i;

                init_color(ncolor[i], TRIPLET(id, i)); 
                init_pair(ncpair[i], ncolor[i], COLOR_BLACK);
                
                setcchar(&cch[i], wch, 0, ncpair[i], NULL); 

        } while (++i < ramp[0]->len);
}



PANEL *colortest_panel(void)
{
        return __PAN;
}


void paint_ramp(int id)
{
        #define SW_W 3
        #define SW_H 3

        int i;

        if (__WIN==NULL) init_colortest();

        werase(__WIN);
        load_ramp(id);

        mvwprintw(__WIN, 0, (SW_W*3), "%s", ramp[0]->name);

        for (i=0; i<ramp[0]->len; i++) {
                mvwprintw(__WIN, i, 0, "%02d", i);
                mvwhline_set(__WIN, i, SW_W, &cch[i], SW_W);
        }
        update_panels();
        doupdate();
}


inline void skip(FILE *file, int n)
{
        char c;

        while (n-->0) {
                do { c=fgetc(file);
                } while (c!='\n');
        }
}


struct palette_t *TEST;

struct palette_t *ggrparse(const char *path) 
{
#define SCALE 1000
        struct palette_t *new;
        FILE *ggr;
        float raw[RGB];
        int i;

        new = malloc(sizeof(struct palette_t));

        ggr = fopen(path, "r");

        fscanf(ggr, "%*s %*s %*s %99s %d", new->name, &new->len);

        new->rgb = malloc(new->len * sizeof(struct triplet_t));

        for (i=0; i<new->len; i++) {
               fscanf(ggr, "%*f %*f %*f %f %f %f %*f %*f %*f %*f %*f %*d %*d",
                      &raw[_R_],
                      &raw[_G_],
                      &raw[_B_]);

               new->rgb[i].r = (uint32_t)(raw[_R_] * SCALE);
               new->rgb[i].g = (uint32_t)(raw[_G_] * SCALE);
               new->rgb[i].b = (uint32_t)(raw[_B_] * SCALE);

               printw("%u %u %u\n", new->rgb[i].r, new->rgb[i].g, new->rgb[i].b);
        }
        fclose(ggr);

        return (new);
}

void ggrtest(const char *path)
{
        ramp[0] = ggrparse(path);
}



