/* 
 * palette.c -- Rat's nest of color definitions, often changed.
 * 
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
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


void build_torch_colors(void)
{
        /* Light sources */
        /*init_color(TORCH_COLOR, 851, 490, 235);*/
        /*init_color(TORCH_COLOR, 956, 729, 286);*/
        /*init_color(TORCH_COLOR, 992, 600, 227);*/
        /*init_color(TORCH_COLOR, 988, 592, 94);*/
        init_color(TORCH_COLOR, 992, 655, 263);

        make_glow_colors(TORCH_COLOR, TORCHF0, TORCHP0);
        make_glow_colors(      BEIGE,   SUNF0,   SUNP0);
}


void build_ui_pairs(void)
{
        /* Greys -------------------------------------------*/
        init_pair(PUR_BLACK       , BLACK          , __DGREY);
        init_pair(PUR_GREY        , GREY           , __DGREY);
        init_pair(_PUR_GREY       , _GREY          , __DGREY);
        init_pair(__PUR_GREY      , __GREY         , __DGREY);
        init_pair(PUR_WHITE       , WHITE          , __DGREY);
        init_pair(PUR_PURPLE      , DGREY          , __DGREY);
        init_pair(_PUR_PURPLE     , _DGREY         , __DGREY);
        /* Yellow/Orange -----------------------------------*/
        init_pair(PUR_YELLOW      , YELLOW         , __DGREY);
        init_pair(PUR_ORANGE      , ORANGE         , __DGREY);
        init_pair(_PUR_YELLOW     , _YELLOW        , __DGREY);
        init_pair(_PUR_ORANGE     , _ORANGE        , __DGREY);
        init_pair(__PUR_YELLOW    , __YELLOW       , __DGREY);
        init_pair(__PUR_ORANGE    , __ORANGE       , __DGREY);
        /* Browns ------------------------------------------*/
        init_pair(PUR_BEIGE       , BEIGE          , __DGREY);
        init_pair(PUR_BROWN       , BROWN          , __DGREY);
        init_pair(_PUR_BEIGE      , _BEIGE         , __DGREY);
        init_pair(_PUR_BROWN      , _BROWN         , __DGREY);
        init_pair(__PUR_BEIGE     , __BEIGE        , __DGREY);
        init_pair(__PUR_BROWN     , __BROWN        , __DGREY);
        /* Blues -------------------------------------------*/
        init_pair(PUR_LBLUE       , LBLUE          , __DGREY);
        init_pair(PUR_BLUE        , BLUE           , __DGREY);
        init_pair(PUR_DBLUE       , DBLUE          , __DGREY);
        init_pair(_PUR_LBLUE      , _LBLUE         , __DGREY);
        init_pair(_PUR_BLUE       , _BLUE          , __DGREY);
        init_pair(_PUR_DBLUE      , _DBLUE         , __DGREY);
        init_pair(__PUR_LBLUE     , __LBLUE        , __DGREY);
        init_pair(__PUR_BLUE      , __BLUE         , __DGREY);
        init_pair(__PUR_DBLUE     , __DBLUE        , __DGREY);
        /* Greens ------------------------------------------*/
        init_pair(PUR_LGREEN      , LGREEN         , __DGREY);
        init_pair(PUR_GREEN       , GREEN          , __DGREY);
        init_pair(PUR_DGREEN      , DGREEN         , __DGREY);
        init_pair(_PUR_LGREEN     , _LGREEN        , __DGREY);
        init_pair(_PUR_GREEN      , _GREEN         , __DGREY);
        init_pair(_PUR_DGREEN     , _DGREEN        , __DGREY);
        init_pair(__PUR_LGREEN    , __LGREEN       , __DGREY);
        init_pair(__PUR_GREEN     , __GREEN        , __DGREY);
        init_pair(__PUR_DGREEN    , __DGREEN       , __DGREY);
        /* Reds --------------------------------------------*/
        init_pair(PUR_LRED        , PINK           , __DGREY);
        init_pair(PUR_RED         , RED            , __DGREY);
        init_pair(PUR_DRED        , DARK_RED       , __DGREY);
        init_pair(_PUR_LRED       , _PINK          , __DGREY);
        init_pair(_PUR_RED        , _RED           , __DGREY);
        init_pair(_PUR_DRED       , _DARK_RED      , __DGREY);
        init_pair(__PUR_LRED      , __PINK         , __DGREY);
        init_pair(__PUR_RED       , __RED          , __DGREY);
        init_pair(__PUR_DRED      , __DARK_RED     , __DGREY);
        /* Special -----------------------------------------*/ 
        init_pair(PUR_SOLID       , __DGREY        , __DGREY);
        init_pair(___PUR_PURPLE   , _DGREY         , ___DGREY);
        init_pair(____PUR_PURPLE  , _DGREY         , ____DGREY);
        init_pair(___PUR_GREY     , GREY           , ___DGREY);
        init_pair(____PUR_GREY    , GREY           , ____DGREY);
        /* Specific -----------------------------------------*/ 
        init_pair(PUR_SKY         , WILDBLUEYONDER , __DGREY);
        init_pair(PUR_BRONZE      , BLASTOFFBRONZE , __DGREY);
        init_pair(PUR_METALYELLOW , METALYELLOW    , __DGREY);
        init_pair(PUR_CELESTIAL   , CELESTIALBLUE  , __DGREY);
        init_pair(PUR_CERULEAN    , CERULEAN       , __DGREY);
}


void build_terrain_pairs(void)
{
        init_pair(PGRASS_HI, CDGRASS_FG, CDGRASS_BG);
        init_pair(PGRASS_LO, CLGRASS_FG, CLGRASS_BG);

        init_pair(POCEAN_HI, COCEAN_HI, COCEAN_BG);
        init_pair(POCEAN_LO, COCEAN_LO, COCEAN_BG);
        init_pair(POCEAN_DK, COCEAN_HI, _COCEAN_BG);

        init_pair(PTREETOP, CTREETOP_FG, CTREETOP_BG);
        init_pair(PTREETRUNK, CTREETRUNK_FG, CTREETRUNK_BG);

        init_pair(PBEACHSAND, CSAND_FG, CSAND_BG);
        init_pair(PCLAYCLIFF, CCLAY_FG, CCLAY_BG);

        init_pair(PCAVEBLANK, BLACK, BLACK);
        init_pair(PCAVEBLOCK, __DGREY, BLACK);
}


void build_special_pairs(void)
{
        init_pair(WARNING, WHITE, RED);
        init_pair(WARNSHADOW, RED, DARK_RED);

        init_pair(PROPE, __BROWN, BLACK);
}


/******************************************************************************/
/* Initialize the color palette with R,G,B values from 0-1000 and the color
 * pairs with the initialized colors. */
void init_my_colors(void)
{
        /* Init basic palette (Arne's Palette) */
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
        init_color(WHITE,      902,  894,  835);

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

        init_color(__DGREY,     78,   71,   78);
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

        /* Special colors for experiments */
        init_color(DARK_RED       , 416 , 141 , 176);
        init_color(CELESTIALBLUE  , 353 , 537 , 784);
        init_color(CERULEAN       , 0   , 482 , 655);
        init_color(WILDBLUEYONDER , 635 , 678 , 815);
        init_color(BLASTOFFBRONZE , 647 , 443 , 392);
        init_color(METALYELLOW    , 972 , 972 , 576);
        init_color(ZEN_BG         , 247 , 247 , 247);
        init_color(___DGREY       , 90  , 87  , 90);
        init_color(____DGREY      , 60  , 58  , 60);


        /* Environment element colors */
        init_color(COCEAN_BG, 165, 235, 211);
        init_color(COCEAN_HI, 259, 361, 318);
        init_color(COCEAN_LO, 223, 310, 274);
        init_color(_COCEAN_BG, 153, 220, 196);

        init_color(CSAND_BG, 800, 694, 541); /* Uses A_REVERSE attribute */
        init_color(CSAND_FG, 635, 623, 506);

        init_color(CLGRASS_BG, 208, 478, 270); 
        init_color(CLGRASS_FG, 480, 620, 370);
        init_color(CDGRASS_BG, 404, 568, 329);
        init_color(CDGRASS_FG, 500, 640, 329);

        init_color(CTREETOP_BG, 172, 388, 318);
        init_color(CTREETOP_FG, 537, 690, 310);

        init_color(CTREETRUNK_BG, 239, 204, 251);
        init_color(CTREETRUNK_FG, 769, 545, 321);

        init_color(CCLAY_BG, 600, 431, 212);
        init_color(CCLAY_FG, 670, 482, 255);


        /* FLEX colors */
        init_color(FLEXBG, 0, 0, 0);
        init_color(FLEXFG, 0, 0, 0);
        init_color(FLEXBG_DEFAULT, 0, 0, 0);
        init_color(FLEXFG_DEFAULT, 0, 0, 0);
}


void init_palette(int set)
{
        init_my_colors();
        build_ui_pairs();
        build_terrain_pairs();
        build_special_pairs();
        build_torch_colors();
}
