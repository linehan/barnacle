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

#include "lib/list/list.h"
#include "lib/gfx.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 4
/******************************************************************************/
enum palette {
        C_BG       = 28,
        C_FG       = 29,
        BLACK      = 30,
        DGREY      = 31,
        GREY       = 32,
        LGREY      = 33,
        DBLUE      = 35,
        BLUE       = 36,
        LBLUE      = 37,
        DGREEN     = 38,
        GREEN      = 39,
        LGREEN     = 40,
        BROWN      = 41,
        ORANGE     = 42,
        YELLOW     = 43,
        RED        = 44,
        PINK       = 45,
        BEIGE      = 46,
        WHITE      = 47,

        SEAFOAM    = 48,
        SEAGREEN   = 49,
        YELLOWSAND = 50, 
        BROWNSAND  = 51,
        LWOOD      = 52,
        WOOD       = 53,
        SHADOW     = 54,
        FOREST_DARK = 55,
        FOREST_MID = 56,
        FOREST_LIGHT = 57 
};
/* FG_BG color pairs; odd ones are swaps of even ones. */
enum ramp_pairs {
        BLACK_DGREY   = 10,
        DGREY_BLACK   = 11,
        DGREY_GREY    = 12,
        GREY_DGREY    = 13,
        GREY_LGREY    = 14,
        LGREY_GREY    = 15,

        DGREY_DBLUE   = 16,
        DBLUE_DGREY   = 17,
        DBLUE_BLUE    = 18,
        BLUE_DBLUE    = 19,
        BLUE_LBLUE    = 20,
        LBLUE_BLUE    = 21,
        LBLUE_LGREY   = 22,
        LGREY_LBLUE   = 23,

        DGREY_DGREEN  = 24,
        DGREEN_DGREY  = 25,
        DGREEN_GREEN  = 26,
        GREEN_DGREEN  = 27,
        GREEN_LGREEN  = 28,
        LGREEN_GREEN  = 29,
        LGREEN_YELLOW = 30,
        YELLOW_LGREEN = 31,
        YELLOW_LGREY  = 32,
        LGREY_YELLOW  = 33,

        DGREY_BROWN   = 34,
        BROWN_DGREY   = 35,
        BROWN_ORANGE  = 36,
        ORANGE_BROWN  = 37,
        ORANGE_YELLOW = 38,
        YELLOW_ORANGE = 39,

        DGREY_RED     = 40,
        RED_DGREY     = 41,
        RED_PINK      = 42,
        PINK_RED      = 43,
        PINK_BEIGE    = 44,
        BEIGE_PINK    = 45,
        BEIGE_YELLOW  = 46,
        YELLOW_BEIGE  = 47,

        SEA_DEEP      = 48,
        SEA_SURF      = 49,
        SEA_SHORE     = 50,
        BOAT_DEEP     = 51,
        SAIL_DEEP     = 52,

        CMP_BEIGE     = 53,
        CMP_RED       = 54,
        CMP_WHITE     = 55,
        CMP_ORANGE    = 56,
        CMP_GREEN     = 57,
        CMP_PINK      = 58,
        CMP_YELLOW    = 59,
        CMP_SHADOW    = 60,
        BOAT_WHITE    = 61,
        BOAT_WOOD     = 62,
        TREES         = 63,
        MONO          = 64,
        LAND         = 65
};
/******************************************************************************/
/* Initialize the color palette with R,G,B values from 0-1000 and the color
 * pairs with the initialized colors. */
void init_palette(int set)
{
        init_color(C_BG,    255,  255,  255);
        init_color(C_FG,   1000, 1000, 1000);
        init_color(BLACK,     0,    0,    0);
        init_color(DGREY,   239,  204,  251);
        init_color(GREY,    564,  564,  564);
        init_color(LGREY,   902,  894,  835);
        init_color(DBLUE,   220,  259,  631);
        init_color(BLUE,    235,  557,  906);
        init_color(LBLUE,   541,  831,  945);
        init_color(DGREEN,   94,  321,  184);
        init_color(GREEN,   184,  459,  368);
        init_color(LGREEN,  564,  749,  286);
        init_color(BROWN,   541,  361,  137);
        init_color(ORANGE,  851,  490,  235);
        init_color(YELLOW, 1000,  874,  286);
        init_color(RED,     643,  192,  247);
        init_color(PINK,    729,  384,  463);
        init_color(BEIGE,   886,  725,  529);

        init_color(SEAFOAM,     439,  565,  502);
        init_color(SEAGREEN,    165,  235,  211);
        init_color(YELLOWSAND,  882,  835,  659);
        init_color(BROWNSAND,   729,  643,  341);
        init_color(LWOOD,       851,  647,  443);
        init_color(WOOD,        769,  545,  321);
        init_color(WHITE,       902,  894,  835);
        /*init_color(SHADOW,      372,  349,  376);*/
        init_color(SHADOW,      400,  372,  412);
        init_color(FOREST_MID,  223,  419,  220);
        init_color(FOREST_LIGHT,   561,  749, 329);
        init_color(FOREST_DARK, 137, 318, 172);

        init_pair(BOAT_WHITE, WHITE, SEAGREEN);
        init_pair(BOAT_WOOD, LWOOD, SEAGREEN);

        init_pair(CMP_BEIGE, BEIGE, SEAGREEN);
        init_pair(CMP_RED, RED, SEAGREEN);
        init_pair(CMP_WHITE, WHITE, SEAGREEN);
        init_pair(CMP_ORANGE, ORANGE, SEAGREEN);
        init_pair(CMP_GREEN, LGREEN, SEAGREEN);
        init_pair(CMP_PINK, PINK, SEAGREEN);
        init_pair(CMP_YELLOW, YELLOW, SEAGREEN);
        init_pair(CMP_SHADOW, SHADOW, SEAGREEN);
        init_pair(SEA_DEEP, SEAFOAM, SEAGREEN);
        init_pair(SEA_SHORE, BROWNSAND, YELLOWSAND);
        init_pair(BOAT_DEEP, LWOOD, SEAGREEN);
        init_pair(SAIL_DEEP, LGREY, SEAGREEN);
        init_pair(SEA_SURF, LGREY, SEAGREEN);
        init_pair(TREES, FOREST_MID, FOREST_LIGHT);
        init_pair(LAND, BROWN, BEIGE);
        init_pair(MONO, C_FG, C_BG);
}
