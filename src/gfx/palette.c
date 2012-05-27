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

#include "../lib/list.h"
#include "gfx.h"

#define CTRLD 4
/******************************************************************************/

inline void set_color(struct rgb_t *color)
{
        init_color(color->tag, color->r, color->g, color->b);
}





#define strips 3


void mix_colors(void)
{
        int i;
        for (i=0; i<strips; i++) {
                set_color(&wallbg[i]);
                set_color(&wallfg[i]);
                set_color(&backbg[i]);
                set_color(&backfg[i]);
                init_pair(FG[i], wallfg[i].tag, wallbg[i].tag);
                init_pair(BG[i], backfg[i].tag, backbg[i].tag);
        }
}


inline void darken(struct rgb_t *color, int step)
{
        color->r = (color->r - step > 0) ? (color->r - step) : 0;
        color->g = (color->g - step > 0) ? (color->g - step) : 0;
        color->b = (color->b - step > 0) ? (color->b - step) : 0;

        set_color(color);
}


void darken_colors(int step)
{
        int i;
        for (i=0; i<strips; i++) {
                darken(&wallbg[i], (i+step));
                darken(&wallfg[i], (i+step));
                darken(&backbg[i], (i+step));
                darken(&backfg[i], (i+step));
        }
}

inline void lighten(struct rgb_t *color, int step)
{
        color->r = (color->r + step < 1000) ? (color->r + step) : 1000;
        color->g = (color->g + step < 1000) ? (color->g + step) : 1000;
        color->b = (color->b + step < 1000) ? (color->b + step) : 1000;

        set_color(color);
}

void lighten_colors(int step)
{
        int i;
        for (i=0; i<strips; i++) {
                lighten(&wallbg[i], (i+step));
                lighten(&wallfg[i], (i+step));
                lighten(&backbg[i], (i+step));
                lighten(&backfg[i], (i+step));
        }
}



#define COLOR_BLOCK_SIZE (12)
#define PAIR_BLOCK_SIZE  (6)
#define FGBG_OFFSET      (6)

void make_glow_colors(short basecolor, short color_block, short pair_block)
{
        #define FG_BLEND         (__DGREY)
        #define BG_BASE          (__DGREY)
        #define BG_BLEND         (BLACK)
        float fgmix[]={1.0, 0.8, 0.5, 0.3, 0.2, 0.00};
        float bgmix[]={1.0, 0.8, 0.5, 0.3, 0.2, 0.2};
        short fg;
        short bg;
        short pa;
        float mix;
        short i;

        for (i=0; i<PAIR_BLOCK_SIZE; i++) {

                fg = color_block + i;
                bg = color_block + i + FGBG_OFFSET;
                pa = pair_block  + i;

                blend(basecolor, fgmix[i], FG_BLEND, 1.0, fg);
                blend(BG_BASE,   bgmix[i], BG_BLEND, 1.0, bg);

                init_pair(pa, fg, bg); 
        }
}


void assign_glow_colors(short base, short *fg, short *bg, short *pa)
{
        short co_start;
        short pa_start;
        short i;

        switch (base) {
        case TORCH_BASE:
                pa_start = TORCHP0;
                co_start = TORCHF0;
                break;
        case SUN_BASE:
                pa_start = SUNP0;
                co_start = SUNF0;
                break;
        case RLIGHT1_BASE:
                pa_start = RL1P0;
                co_start = RLIGHT1F0;
                break;
        case RLIGHT2_BASE:
                pa_start = RL2P0;
                co_start = RLIGHT2F0;
                break;
        }

        for (i=0; i<6; i++) {
                fg[i] = co_start + i;
                bg[i] = co_start + i + FGBG_OFFSET;
                pa[i] = pa_start + i;
        }
}


void torch_colors(void)
{
        /* Light sources */
        init_color(TORCH_COLOR, 851, 490, 235);

        make_glow_colors(TORCH_COLOR, TORCHF0, TORCHP0);
        make_glow_colors(      BEIGE,   SUNF0,   SUNP0);
}



/******************************************************************************/
/* Initialize the color palette with R,G,B values from 0-1000 and the color
 * pairs with the initialized colors. */
void init_palette(int set)
{
/* Hexadecimal values for colors used

                                SHADOW 1        SHADOW 2
        DARK GREY (PURPLE)      2c272e          141214
        GREY                    7d7d7d          636363
        LIGHT GREY (WHITE)      d4d3cb          bababa
        DARK BLUE               38408f          343a75
        BLUE                    4088d6          427dbd  
        LIGHT BLUE              89c8e0          83b4c7
        DARK GREEN              154026          0f2618
        GREEN (TEAL)            2c6351          244a3e
        LIGHT GREEN             85ad49          759446
        YELLOW                  edd14e          d4bd50
        BROWN                   785223          5e4320
        ORANGE                  bf733d          a6693d
        RED                     91313d          782f38
        PINK                    a86070          8f5965
        BEIGE                   d1af86          b89e7f
        OCEAN (DARK)            202b28          0e1211
        OCEAN                   415c54          32423e
        OCEAN (LIGHT)           5e7d7a          506362          */


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
        init_color(LSEA,       572,  678,  662);
        init_color(SEA,        439,  565,  502);
        init_color(DSEA,       165,  235,  211);
        init_color(LWOOD,      851,  647,  443);
        init_color(WOOD,       769,  545,  321);
        init_color(__WOOD, 673, 497, 329);
        init_color(WHITE,      902,  894,  835);
        init_color(SHADOW,     372,  349,  376);
        init_color(DARK_RED,   416,  141,  176);
        init_color(DARK_BROWN, 282, 220, 137);
        init_color(CELESTIALBLUE, 353, 537, 784);
        init_color(CERULEAN, 0, 482, 655);
        init_color(WILDBLUEYONDER, 635, 678, 815);
        init_color(BLASTOFFBRONZE, 647, 443, 392);
        init_color(METALYELLOW, 972, 972, 576);
        init_color(ZEN_BG, 247, 247, 247);

        init_color(__DGREY,     78,   71,   78);
        init_color(__BG,     74, 74, 74);
        init_color(__GREY,     388,  388,  388);
        init_color(__LGREY,    729,  729,  729);
        init_color(__DBLUE,    204,  227,  459);
        init_color(__BLUE,     259,  490,  741);
        init_color(__LBLUE,    514,  706,  780);
        init_color(__DGREEN,    59,  149,   94);
        init_color(__GREEN,    141,  290,  243);
        init_color(__LGREEN,   459,  580,  274);
        init_color(__YELLOW,   831,  741,  314);
        init_color(__BROWN,    368,  263,  125);
        init_color(__ORANGE,   651,  412,  239);
        init_color(__RED,      470,  184,  220);
        init_color(__PINK,     561,  349,  396);
        init_color(__BEIGE,    721,  619,  498);
        init_color(__DSEA,      55,   71,   67);
        init_color(__SEA,      196,  259,  243);
        init_color(__LSEA,     314,  388,  384);

        init_color(_DGREY,     172,  153,  180);
        init_color(_GREY,      490,  490,  490);
        init_color(_LGREY,     831,  827,  796);
        init_color(_DBLUE,     220,  251,  561);
        init_color(_BLUE,      251,  533,  839);
        init_color(_LBLUE,     537,  784,  878);
        init_color(_DGREEN,     82,  251,  149);
        init_color(_GREEN,     172,  388,  318);
        init_color(_LGREEN,    521,  678,  286);
        init_color(_YELLOW,    929,  819,  306);
        init_color(_BROWN,     470,  321,  137);
        init_color(_ORANGE,    749,  451,  239);
        init_color(_RED,       568,  192,  239);
        init_color(_PINK,      659,  376,  439);
        init_color(_BEIGE,     819,  686,  525);
        init_color(_DSEA,       153,    220,    196);
        init_color(_SEA,        314, 427,  396);
        init_color(_LSEA,      368,  490,  478);
        init_color(_WOOD,       721,    521,    325);
        init_color(_LWOOD,       721,    521,    325);

        init_color(__DGREY,     78,   71,   78);
        init_color(__BG,     74, 74, 74);
        init_color(__GREY,     388,  388,  388);
        init_color(__LGREY,    729,  729,  729);
        init_color(__DBLUE,    204,  227,  459);
        init_color(__BLUE,     259,  490,  741);
        init_color(__LBLUE,    514,  706,  780);
        init_color(__DGREEN,    59,  149,   94);
        init_color(__GREEN,    141,  290,  243);
        init_color(__LGREEN,   459,  580,  274);
        init_color(__YELLOW,   831,  741,  314);
        init_color(__BROWN,    368,  263,  125);
        init_color(__ORANGE,   651,  412,  239);
        init_color(__RED,      470,  184,  220);
        init_color(__PINK,     561,  349,  396);
        init_color(__BEIGE,    721,  619,  498);
        init_color(__DSEA,      55,   71,   67);
        init_color(__SEA,      196,  259,  243);
        init_color(__LSEA,     314,  388,  384);

        init_color(SANDY, 886,  741,  568);
        init_color(SHALLOW, 188,  270,  243);
        init_color(LSHALLOW, 400,  526,  472);
        init_color(A_SAND_LAGOON, 800, 694, 541);
        init_color(B_SAND_LAGOON, 635, 623, 506);
        init_color(C_SAND_LAGOON, 259, 361, 318);
        init_color(D_SAND_LAGOON, 188, 267, 235);
        init_color(WATER_HINT, 223, 310, 274);



        init_color(FLEXBG, 0, 0, 0);
        init_color(FLEXFG, 0, 0, 0);
        init_color(FLEXBG_DEFAULT, 0, 0, 0);
        init_color(FLEXFG_DEFAULT, 0, 0, 0);
        init_color(___DGREY, 90, 87, 90);
        init_color(____DGREY, 60, 58, 60);

        /* UI COLOR PAIRS */
        init_pair(CMP_BEIGE, BEIGE, ORANGE);
        init_pair(CMP_RED, RED, ORANGE);
        init_pair(CMP_WHITE, WHITE, ORANGE);
        init_pair(CMP_ORANGE, BEIGE, ORANGE);
        init_pair(CMP_GREEN, LGREEN, __DGREY);
        init_pair(CMP_PINK, BEIGE, BEIGE);
        init_pair(CMP_YELLOW, YELLOW, __DGREY);
        init_pair(CMP_SHADOW, DGREY, ORANGE);
        init_pair(WARNING, WHITE, RED);
        init_pair(WARNSHADOW, RED, DARK_RED);

        init_pair(GUN_FLASH, WHITE, RED);
        init_pair(GUN_SMOKE, WHITE, DSEA);

        init_pair(PUR_BLACK       , BLACK          , __DGREY);
        init_pair(PUR_GREY        , GREY           , __DGREY);
        init_pair(_PUR_GREY       , _GREY          , __DGREY);
        init_pair(__PUR_GREY      , __GREY         , __DGREY);
        init_pair(PUR_WHITE       , WHITE          , __DGREY);
        init_pair(PUR_PURPLE      , DGREY          , __DGREY);
        init_pair(_PUR_PURPLE     , _DGREY         , __DGREY);
//-----
        init_pair(PUR_YELLOW      , YELLOW         , __DGREY);
        init_pair(PUR_ORANGE      , ORANGE         , __DGREY);
        init_pair(_PUR_YELLOW     , _YELLOW        , __DGREY);
        init_pair(_PUR_ORANGE     , _ORANGE        , __DGREY);
        init_pair(__PUR_YELLOW    , __YELLOW       , __DGREY);
        init_pair(__PUR_ORANGE    , __ORANGE       , __DGREY);
//-----
        init_pair(PUR_BEIGE       , BEIGE          , __DGREY);
        init_pair(PUR_BROWN       , BROWN          , __DGREY);
        init_pair(_PUR_BEIGE      , _BEIGE         , __DGREY);
        init_pair(_PUR_BROWN      , _BROWN         , __DGREY);
        init_pair(__PUR_BEIGE     , __BEIGE        , __DGREY);
        init_pair(__PUR_BROWN     , __BROWN        , __DGREY);
//-----
        init_pair(PUR_LBLUE       , LBLUE          , __DGREY);
        init_pair(PUR_BLUE        , BLUE           , __DGREY);
        init_pair(PUR_DBLUE       , DBLUE          , __DGREY);
        init_pair(_PUR_LBLUE      , _LBLUE         , __DGREY);
        init_pair(_PUR_BLUE       , _BLUE          , __DGREY);
        init_pair(_PUR_DBLUE      , _DBLUE         , __DGREY);
        init_pair(__PUR_LBLUE     , __LBLUE        , __DGREY);
        init_pair(__PUR_BLUE      , __BLUE         , __DGREY);
        init_pair(__PUR_DBLUE     , __DBLUE        , __DGREY);
//-----
        init_pair(PUR_LGREEN      , LGREEN         , __DGREY);
        init_pair(PUR_GREEN       , GREEN          , __DGREY);
        init_pair(PUR_DGREEN      , DGREEN         , __DGREY);
        init_pair(_PUR_LGREEN     , _LGREEN        , __DGREY);
        init_pair(_PUR_GREEN      , _GREEN         , __DGREY);
        init_pair(_PUR_DGREEN     , _DGREEN        , __DGREY);
        init_pair(__PUR_LGREEN    , __LGREEN       , __DGREY);
        init_pair(__PUR_GREEN     , __GREEN        , __DGREY);
        init_pair(__PUR_DGREEN    , __DGREEN       , __DGREY);
//-----
        init_pair(PUR_LRED        , PINK           , __DGREY);
        init_pair(PUR_RED         , RED            , __DGREY);
        init_pair(PUR_DRED        , DARK_RED       , __DGREY);
        init_pair(_PUR_LRED       , _PINK          , __DGREY);
        init_pair(_PUR_RED        , _RED           , __DGREY);
        init_pair(_PUR_DRED       , _DARK_RED      , __DGREY);
        init_pair(__PUR_LRED      , __PINK         , __DGREY);
        init_pair(__PUR_RED       , __RED          , __DGREY);
        init_pair(__PUR_DRED      , __DARK_RED     , __DGREY);
//-----
        init_pair(PUR_LSEA        , LSEA           , __DGREY);
        init_pair(PUR_SEA         , SEA            , __DGREY);
        init_pair(PUR_DSEA        , DSEA           , __DGREY);
        init_pair(_PUR_LSEA       , _LSEA          , __DGREY);
        init_pair(_PUR_SEA        , _SEA           , __DGREY);
        init_pair(_PUR_DSEA       , _DSEA          , __DGREY);
        init_pair(__PUR_LSEA      , __LSEA         , __DGREY);
        init_pair(__PUR_SEA       , __SEA          , __DGREY);
        init_pair(__PUR_DSEA      , __DSEA         , __DGREY);
//-----
        init_pair(PUR_LWOOD       , LWOOD          , __DGREY);
        init_pair(PUR_WOOD        , WOOD           , __DGREY);
        init_pair(_PUR_LWOOD      , _LWOOD         , __DGREY);
        init_pair(_PUR_WOOD       , _WOOD          , __DGREY);
        init_pair(__PUR_LWOOD     , __LWOOD        , __DGREY);
        init_pair(__PUR_WOOD      , __WOOD         , __DGREY);
//-----
        init_pair(PUR_SKY         , WILDBLUEYONDER , __DGREY);
        init_pair(PUR_BRONZE      , BLASTOFFBRONZE , __DGREY);
        init_pair(PUR_METALYELLOW , METALYELLOW    , __DGREY);
        init_pair(PUR_CELESTIAL   , CELESTIALBLUE  , __DGREY);
        init_pair(PUR_CERULEAN    , CERULEAN       , __DGREY);


        /* IN-GAME COLOR PAIRS */
        init_pair(BOAT_WHITE, WHITE, DSEA);
        init_pair(BOAT_WOOD, LWOOD, DSEA);
        init_pair(LAND, BROWN, LWOOD);
        init_pair(TREETOP, LGREEN, GREEN);
        init_pair(TREETRUNK, WOOD, DGREY);
        init_pair(GRASSY, LGREEN, GREEN);
        init_pair(SEA_DARK, GREEN, DSEA);
        init_pair(SEA_MED, C_SAND_LAGOON, DSEA);
        init_pair(SEA_LIG, LSEA, DSEA);
        init_pair(SEA_SAND, SANDY, BEIGE);
        init_pair(A_SEA_LAGOON, B_SAND_LAGOON, A_SAND_LAGOON);
        init_pair(B_SEA_LAGOON, C_SAND_LAGOON, D_SAND_LAGOON);
        init_pair(C_SEA_LAGOON, C_SAND_LAGOON, C_SAND_LAGOON);
        init_pair(D_SEA_LAGOON, C_SAND_LAGOON, D_SAND_LAGOON);
        init_pair(SEA_SHALLOW, C_SAND_LAGOON, DSEA);
        init_pair(_SEA_SHALLOW, C_SAND_LAGOON, _DSEA);

        init_pair(_BOAT_WHITE, _WHITE,  _DSEA);
        init_pair(_BOAT_WOOD,  _LWOOD,  _DSEA);
        init_pair(_LAND,       _BROWN,  _BEIGE);
        init_pair(_TREETOP,    _LGREEN, _GREEN);
        init_pair(_TREETRUNK,  _WOOD,   _DGREY);
        init_pair(_GRASSY,     _LGREEN, _GREEN);
        init_pair(_SEA_DARK,   _GREEN,  _DSEA);
        init_pair(_SEA_MED,    _SEA,    _DSEA);
        init_pair(_SEA_LIG,    _LSEA,   _DSEA);

        init_pair(__BOAT_WHITE, __WHITE,  __DSEA);
        init_pair(__BOAT_WOOD,  __LWOOD,  __DSEA);
        init_pair(__LAND,       __BROWN,  __BEIGE);
        init_pair(__TREETOP,    __LGREEN, __GREEN);
        init_pair(__TREETRUNK,  __WOOD,   __DGREY);
        init_pair(__GRASSY,     __LGREEN, __GREEN);
        init_pair(__SEA_DARK,   __GREEN,  __DSEA);
        init_pair(__SEA_MED,    __SEA,    __DSEA);
        init_pair(__SEA_LIG,    __LSEA,   __DSEA);
        init_pair(SEA__MED, WATER_HINT, DSEA);
        init_pair(SEA___MED, __LSEA, DSEA);
        init_pair(PUR_SOLID, __DGREY, __DGREY);
        init_pair(___PUR_PURPLE, _DGREY, ___DGREY);
        init_pair(____PUR_PURPLE, _DGREY, ____DGREY);
        init_pair(___PUR_GREY,  GREY, ___DGREY);
        init_pair(____PUR_GREY, GREY, ____DGREY);

        init_color(DARK_SANDZ, 741,  643,  502);
        init_color(DARK_SAND0, 670,  580,  455);
        init_color(DARK_SAND1, 541,  466,  365);
        init_color(DARK_SAND2, 431,  372,  290);
        init_color(DARK_SAND3, 259,  216,  157);
        init_color(DARK_SAND4, 169,  137,  94);

        init_pair(BSAND, DARK_SANDZ, A_SAND_LAGOON);
        init_pair(CSAND, DARK_SANDZ, A_SAND_LAGOON);
        init_pair(DSAND, DARK_SANDZ, DARK_SAND0);
        init_pair(ESAND, DARK_SAND1, DARK_SAND2);
        init_pair(FSAND, DARK_SAND4, DARK_SAND3);

        init_pair(BLACK_BEIGE, __BROWN, BLACK);
        init_pair(BLACK_BLACK, BLACK, BLACK);

        torch_colors();
        mix_colors();
}
