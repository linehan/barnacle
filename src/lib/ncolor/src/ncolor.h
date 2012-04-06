#pragma once
#ifndef __NCOLOR_H
#define __NCOLOR_H
#include <stdint.h>



/******************************************************************************
 *                                                                            *
 * ncolor.h - Data types and routines for loading and parsing .ggr files,     *
 *            and constructing a corresponding color palette, and providing   *
 *            the color data to ncurses                                       *
 *                                                                            *
 ******************************************************************************/



/*
 * Holds the three scaled and truncated RGB values
 */
typedef struct rgb_triplet { 
        uint32_t r; 
        uint32_t g; 
        uint32_t b; 
} TRIPLET;


/*
 * Information as provided by .ggr (GIMP Gradient Type)
 */
#define GGRBUFMAX 100
typedef struct ggr_t {
        char name[GGRBUFMAX];
        int len;
        TRIPLET *rgb;
} GGR;


/*
 * The visible palette type
 */
typedef struct palette_t {
        bool loaded;                // Is palette being used on-screen?
        short at_color;             // beginning ncurses COLOR number
        short at_pair;              // beginning ncurses COLOR_PAIR number
        GGR *colors;
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
#define __LOADED(pal) pal->loaded



PALETTE *new_pal(void);
void pal_load(PALETTE *pal, const char *path, short at_color);

void pal_printw(WINDOW *win, int y, int x, wchar_t *wcs, float val, PALETTE *pal);
void pal_testprint(WINDOW *win, PALETTE *pal);

float norm(int val, int min, int max);


#endif
