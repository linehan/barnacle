// vim:fdm=marker
/*******************************************************************************
   FILENAME:  palette.c
   Colors and color pairs that will be used by ncurses
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <ncurses.h>
#include <panel.h>

#include "../lib/llist/list.h"
#include "gfx.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 4
/******************************************************************************/
/* The system's colors go from 0-7 */
enum arne_colors {
        BLACK      = 17,
        DGREY      = 18,
        GREY       = 19,
        LGREY      = 20,
        DBLUE      = 21,
        BLUE       = 22,
        LBLUE      = 23,
        DGREEN     = 24, 
        GREEN      = 25,
        LGREEN     = 26,
        BROWN      = 27,
        ORANGE     = 28,
        YELLOW     = 29,
        RED        = 30,
        PINK       = 31,
        BEIGE      = 32,
        WHITE      = 33,
        DSEA       = 34, 
        SEA        = 35,
        LSEA       = 36

};
enum extra_colors {
        LWOOD      = 37,
        WOOD       = 38,
        SHADOW     = 39, 
        PALE_BLUE  = 40,
        OFF_WHITE  = 41,
        DARK_RED   = 42 
};

/* FG_BG color pairs; odd ones are swaps of even ones. */
enum ramp_pairs {
        SEA_DARK      = 17,
        SEA_MED       = 18,
        BOAT_DEEP     = 19,
        BOAT_WOOD     = 20, 
        BOAT_WHITE    = 21,
        SAIL_DEEP     = 22,

        CMP_BEIGE     = 23,
        CMP_RED       = 24,
        CMP_WHITE     = 25,
        CMP_ORANGE    = 26,
        CMP_GREEN     = 27,
        CMP_PINK      = 28,
        CMP_YELLOW    = 29,
        CMP_SHADOW    = 30,

        LAND          = 31,
        MENU          = 32,
        WARNING       = 33,
        WARNSHADOW    = 34,
        TREETOP       = 35,
        TREETRUNK     = 36,
        GRASSY        = 37,
        SOILGRASS     = 38,
        SEA_LIG       = 39
};

/******************************************************************************/
/* Initialize the color palette with R,G,B values from 0-1000 and the color
 * pairs with the initialized colors. */
void init_palette(int set)
{
        /* Color inits */
        init_color(BLACK,        0,    0,    0);
        init_color(DGREY,      239,  204,  251);
        init_color(GREY,       564,  564,  564);
        init_color(LGREY,      902,  894,  835);
        init_color(DBLUE,      220,  259,  631);
        init_color(BLUE,       235,  557,  906);
        init_color(LBLUE,      541,  831,  945);
        init_color(DGREEN,      94,  321,  184);
        init_color(GREEN,      184,  459,  368);
        init_color(LGREEN,     564,  749,  286);
        init_color(BROWN,      541,  361,  137);
        init_color(ORANGE,     851,  490,  235);
        init_color(YELLOW,    1000,  874,  286);
        init_color(RED,        643,  192,  247);
        init_color(PINK,       729,  384,  463);
        init_color(BEIGE,      886,  725,  529);
        init_color(LSEA,   506, 631, 600);
        init_color(SEA,    439,  565,  502);
        init_color(DSEA,   165,  235,  211);
        init_color(LWOOD,      851,  647,  443);
        init_color(WHITE,      902,  894,  835);
        init_color(SHADOW,   372,  349,  376);
        /*init_color(SHADOW,     400,  372,  412);*/
        init_color(DARK_RED,   416,  141,  176);

        /* Color pair inits */
        init_pair(CMP_BEIGE, BEIGE, ORANGE);
        init_pair(CMP_RED, RED, ORANGE);
        init_pair(CMP_WHITE, WHITE, ORANGE);
        init_pair(CMP_ORANGE, BEIGE, ORANGE);
        init_pair(CMP_GREEN, LGREEN, ORANGE);
        init_pair(CMP_PINK, BEIGE, BEIGE);
        init_pair(CMP_YELLOW, YELLOW, ORANGE);
        init_pair(CMP_SHADOW, DGREY, ORANGE);
        init_pair(SEA_DARK, GREEN, DSEA);
        init_pair(SEA_MED, SEA, DSEA);
        init_pair(SEA_LIG, LSEA, DSEA);
        init_pair(BOAT_WHITE, WHITE, DSEA);
        init_pair(BOAT_DEEP, WOOD, DSEA);
        init_pair(SAIL_DEEP, LGREY, DSEA);
        init_pair(LAND, BROWN, BEIGE);
        init_pair(MENU, DGREY, WHITE);
        init_pair(WARNING, WHITE, RED);
        init_pair(WARNSHADOW, RED, DARK_RED);
        init_pair(TREETOP, LGREEN, GREEN);
        init_pair(TREETRUNK, WOOD, DGREY);
        init_pair(GRASSY, LGREEN, GREEN);
}
