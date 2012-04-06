#pragma once
#ifndef __RGB_NC_H
#define __RGB_NC_H
#include <stdint.h>



/******************************************************************************
 *                                                                            *
 * rgb_nc.h - Data types and routines for loading and parsing .ggr files,     *
 *            and constructing a corresponding color palette, and providing   *
 *            the color data to ncurses                                       *
 *                                                                            *
 ******************************************************************************/



/*
 * Holds the three scaled and truncated RGB values
 */
struct triplet_t { 
        uint32_t r; 
        uint32_t g; 
        uint32_t b; 
};

/*
 * Holds the name of the palette, the number of colors it contains
 * information for, and a TRIPLET array containing the actual 
 * color data 
 */
struct colordata_t {
        char name[100];
        int len;
        struct triplet_t *rgb;
};

/*
 * Contains a wide-character string and it's complex rendition,
 * used to print a set of swatches, one for each color in a palette.
 */
struct swatch_t {
        wchar_t *wch;
        cchar_t *cch;
};

/*
 * The visible palette type
 */
typedef struct palette_t {
       bool loaded;                // Is palette being used on-screen?
       short at_color;             // beginning ncurses COLOR number
       short at_pair;              // beginning ncurses COLOR_PAIR number
       struct colordata_t *colors; // RGB color data, palette name & length
       struct swatch_t *swatch;    // swatch character and renditions
} PALETTE;




/*
 * Macros for accessing color data
 */
#define __R(pal, n) pal->colors->rgb[n].r
#define __G(pal, n) pal->colors->rgb[n].g
#define __B(pal, n) pal->colors->rgb[n].b

#define __TRIPLET(pal, n) \
        __R(pal, n), __G(pal, n), __B(pal, n)

/* 
 * Macros for accessing palette data
 */   
#define __NAME(pal) pal->colors->name
#define __COLORCOUNT(pal) pal->colors->len
#define __COLOR(pal, i) ((pal)->at_color + (i))
#define __PAIR(pal, i) ((pal)->at_pair + (i))
#define __WCH(pal) pal->swatch->wch
#define __CCH(pal) pal->swatch->cch
#define __SWATCH(pal, n) (pal->swatch->cch[(n)])



PALETTE *new_pal(void);
void pal_load(PALETTE *pal, const char *path, short at_color);
void pal_swatch(PALETTE *pal, const wchar_t *tile);
void pal_printw(WINDOW *win, PALETTE *pal);



#endif
