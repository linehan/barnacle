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
#include "rgb_nc.h"




/******************************************************************************
 *                                                                            *
 * rgb_nc.c - Data types and routines for loading and parsing .ggr files,     *
 *            and constructing a corresponding color palette, and providing   *
 *            the color data to ncurses                                       *
 *                                                                            *
 ******************************************************************************/




/******************************************************************************
 * ggr_parse - Load and parse a .ggr file
 *
 * @path: string indicating the path of the .ggr file to be loaded
 *
 * RETURNS: An initialized struct colordata_t *, with the parsed data
 *          from the .ggr file at 'path'
 ******************************************************************************/
struct colordata_t *ggr_parse(const char *path)
{
        static const uint32_t SCALE=1000;
        enum {_R_, _G_, _B_};
        struct colordata_t *new;
        FILE *ggr;
        float raw[3];
        int i;

        ggr = fopen(path, "r");
        new = malloc(sizeof(struct colordata_t));

        fscanf(ggr, "%*s %*s %*s %99s %d", new->name, &new->len);
        new->rgb = malloc(new->len * sizeof(struct triplet_t));

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
        new->swatch = NULL;

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





/******************************************************************************
 * pal_swatch - assign the wchar_t string used as the swatch graphic.
 *
 * @pal: pointer to a PALETTE
 * @tile: wide-character string
 ******************************************************************************/
void pal_swatch(PALETTE *pal, const wchar_t *tile)
{
        int i=0;

        pal->swatch = malloc(sizeof(struct swatch_t));

        __WCH(pal) = wcdup(tile);
        __CCH(pal) = malloc(__COLORCOUNT(pal)*sizeof(cchar_t));

        do { 
                setcchar(&__SWATCH(pal, i), __WCH(pal), 0, __COLOR(pal, i), NULL); 

        } while (++i < __COLORCOUNT(pal));
}




/******************************************************************************
 * pal_printw - print a palette using its swatch graphic
 * 
 * @win: WINDOW in which to print the swatches 
 * @pal: pointer to a PALETTE
 ******************************************************************************/
void pal_printw(WINDOW *win, PALETTE *pal)
{
        #define OFS_X 3
        #define OFX_Y 3

        int i;

        mvwprintw(win, 0, (OFS_X*3), "%s", __NAME(pal));

        for (i=0; i<__COLORCOUNT(pal); i++) {
                mvwprintw(win, i, 0, "%02d", i);
                mvwhline_set(win, i, OFS_X, &__SWATCH(pal, i), OFS_X);
        }
}


