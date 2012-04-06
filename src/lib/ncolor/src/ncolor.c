#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <ncurses.h>
#include <panel.h>
#include <locale.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#include "deps/textutils.h"
#include "ncolor.h"




/******************************************************************************
 *                                                                            *
 * ncolor.c - Data types and routines for loading and parsing .ggr files,     *
 *            and constructing a corresponding color palette, and providing   *
 *            the color data to ncurses                                       *
 *                                                                            *
 ******************************************************************************/



static attr_t saved_attr;
static short saved_pair;

#define SAVED_ATTR saved_attr
#define SAVED_PAIR saved_pair

#define SAVEWIN(win) wattr_get((win), &SAVED_ATTR, &SAVED_PAIR, NULL)
#define RESTORE(win) wattr_set((win), SAVED_ATTR, SAVED_PAIR, NULL)


#define TEST_WCS L"█"


/*uint32_t coloralloc[BITSTRING(COLORS)];*/

/*#define NEXTCOLOR (free_color < COLORS) ? ++free_color : free_color*/
/*#define FREECOLOR */


/******************************************************************************
 * ggr_parse - Load and parse a .ggr file
 *
 * @path: string indicating the path of the .ggr file to be loaded
 *
 * RETURNS: An initialized struct colordata_t *, with the parsed data
 *          from the .ggr file at 'path'
 ******************************************************************************/
GGR *ggr_parse(const char *path)
{
        static const uint32_t SCALE=1000;
        enum {_R_, _G_, _B_};

        FILE *ggr;
        GGR *new;
        float raw[3];
        int i;

        ggr = fopen(path, "r");
        assert(ggr != NULL);

        new = malloc(sizeof(GGR));

        fscanf(ggr, "%*s %*s %*s %99s %d", new->name, &new->len);
        new->rgb = malloc(new->len * sizeof(TRIPLET));

        for (i=0; i<new->len; i++) {
               fscanf(ggr, "%*f %*f %*f %f %f %f %*f %*f %*f %*f %*f %*d %*d",
                      &raw[_R_], &raw[_G_], &raw[_B_]);

               new->rgb[i].r = (uint32_t)(raw[_R_] * SCALE);
               new->rgb[i].g = (uint32_t)(raw[_G_] * SCALE);
               new->rgb[i].b = (uint32_t)(raw[_B_] * SCALE);
        }
        fclose(ggr);

        return (new);
}





/******************************************************************************
 * new_pal - Allocate and initialize a new PALETTE
 *
 * RETURNS: pointer to an allocated PALETTE
 ******************************************************************************/
PALETTE *new_pal(void)
{
        PALETTE *new = malloc(sizeof(PALETTE));

        new->loaded = false;
        new->colors = NULL;

        return (new);
}





/******************************************************************************
 * pal_load - assign RGB colors and ncurses color numbers and pairs to a
 *            previously-allocated PALETTE 
 *
 * @pal: Pointer to a PALETTE
 * @path: Path of the .ggr file containing the desired RGB values 
 * @at: The ncurses COLOR number at which the palette should begin.
 *      Loaded palettes will automatically be assigned COLOR numbers from 
 *      at_color to (at_color + <colors in palette>) - 1
 ******************************************************************************/
void pal_load(PALETTE *pal, const char *path, short at)
{
        assert(pal != NULL);

        int i=0;
        if (pal->loaded) return;

        pal->colors = ggr_parse(path);

        do { 
                pal->at_color = at;
                pal->at_pair  = at;
                init_color(__COLOR(pal, i), __TRIPLET(pal, i));
                init_pair(__PAIR(pal, i), __COLOR(pal, i), COLOR_BLACK);

        } while (++i < (__COLORCOUNT(pal)));

        pal->loaded = true;
}





inline short pal_mapcolor(PALETTE *pal, float val)
{
        return pal->at_color + (int)(val * pal->colors->len);
}


inline short pal_mappair(PALETTE *pal, float val)
{
        return pal->at_pair + (short)(val * pal->colors->len);
}


float norm(int val, int min, int max)
{
        return (float)((float)val / (float)(max - min));
}


/*short pal_maploop(PALETTE *pal, int i, int min, int max)*/
/*{*/
        /*return norm(i, min, max);*/
/*}*/


/******************************************************************************
 * pal_printw - given an input, a PALETTE, and a wchar_t string, print the
 *              wide-character string with the appropriate color, based on
 *              the value of the input and the range of the PALETTE. 
 * 
 * @win: WINDOW in which to print the wide-character string
 * @y:   y-coordinate in window
 * @x:   x-coordinate in window
 * @wcs: wide-character string to be printed
 * @pal: pointer to a PALETTE
 * @val: input value, which must lie in the range [0,1]
 *
 * RETURNS: void
 ******************************************************************************/
void pal_printw(WINDOW *win, int y, int x, wchar_t *wcs, float val, PALETTE *pal)
{
        if (!__LOADED(pal)) return;

        SAVEWIN(win);
        wcolor_set(win, pal_mappair(pal, val), NULL);
        mvwaddwstr(win, y, x, wcs);
        RESTORE(win);
}





/******************************************************************************
 * pal_testprint - ouput an entire palette with diagnostic information
 * 
 * @win: WINDOW in which to print the swatches 
 * @pal: pointer to a PALETTE
 ******************************************************************************/
void pal_testprint(WINDOW *win, PALETTE *pal)
{
        #define OFS_X 1 
        #define OFS_Y (LINES-(__COLORCOUNT(pal)))/2 

        float f;
        int i;

        mvwprintw(win, OFS_Y, (OFS_X+6), "%s", __NAME(pal));

        for (i=0; i<__COLORCOUNT(pal); i++) {
                f = norm(i, 0, __COLORCOUNT(pal));

                mvwprintw(win, i+OFS_Y, OFS_X, "%02d", i);
                pal_printw(win, i+OFS_Y, OFS_X+3, TEST_WCS, f, pal);
        }
}



