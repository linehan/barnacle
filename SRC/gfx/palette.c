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
/* The shaded color pairs are indexed by their lighter cousins */
/*int shade[128];*/

/*shade[SEA_DARK]    = _SEA_DARK;*/
/*shade[SEA_MED]     = _SEA_MED;*/
/*shade[SEA_LIG]     = _SEA_LIG;*/
/*shade[BOAT_DEEP]   = _BOAT_DEEP;*/
/*shade[BOAT_WOOD]   = _BOAT_WOOD;*/
/*shade[BOAT_WHITE]  = _BOAT_WHITE;*/
/*shade[BOAT_WOOD]   = _BOAT_WOOD;*/
/*shade[SAIL_DEEP]   = _SAIL_DEEP;*/
/*shade[LAND]        = _LAND;*/
/*shade[TREETOP]     = _TREETOP;*/
/*shade[TREETRUNK]   = _TREETRUNK;*/
/*shade[GRASSY]      = _GRASSY;*/
/*shade[SOILGRASS]   = _SOILGRASS;*/
/*shade[_SEA_DARK]   = __SEA_DARK;*/
/*shade[_SEA_MED]    = __SEA_MED;*/
/*shade[_SEA_LIG]    = __SEA_LIG;*/
/*shade[_BOAT_DEEP]  = __BOAT_DEEP;*/
/*shade[_BOAT_WOOD]  = __BOAT_WOOD;*/
/*shade[_BOAT_WHITE] = __BOAT_WHITE;*/
/*shade[_BOAT_WOOD]  = __BOAT_WOOD;*/
/*shade[_SAIL_DEEP]  = __SAIL_DEEP;*/
/*shade[_LAND]       = __LAND;*/
/*shade[_TREETOP]    = __TREETOP;*/
/*shade[_TREETRUNK]  = __TREETRUNK;*/
/*shade[_GRASSY]     = __GRASSY;*/
/*shade[_SOILGRASS]  = __SOILGRASS;*/

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
        init_color(WHITE,      902,  894,  835);
        init_color(SHADOW,     372,  349,  376);
        init_color(DARK_RED,   416,  141,  176);
        init_color(DARK_BROWN, 282, 220, 137);
        /*init_color(CELESTIALBLUE, 286, 592, 815);*/
        init_color(CELESTIALBLUE, 353, 537, 784);
        init_color(CERULEAN, 0, 482, 655);
        init_color(WILDBLUEYONDER, 635, 678, 815);
        /*#define WILDBLUEYONDER DSEA*/
        init_color(BLASTOFFBRONZE, 647, 443, 392);
        init_color(METALYELLOW, 972, 972, 576);
        init_color(ZEN_BG, 247, 247, 247);

/*struct triplet { short r; short g; short b; };*/
/*struct triplet _SHADE[]={*/
        /*{172,  153,  180},  // DGREY*/
        /*{490,  490,  490},  // GREY*/
        /*{831,  827,  796},  // LGREY*/
        /*{220,  251,  561},  // DBLUE*/
        /*{251,  533,  839},  // BLUE*/
        /*{537,  784,  878},  // LBLUE*/
        /*{82,   251,  149},  // DGREEN*/
        /*{172,  388,  318},  // GREEN*/
        /*{521,  678,  286},  // LGREEN*/
        /*{929,  819,  306},  // YELLOW*/
        /*{470,  321,  137},  // BROWN*/
        /*{749,  451,  239},  // ORANGE*/
        /*{568,  192,  239},  // RED*/
        /*{659,  376,  439},  // PINK*/
        /*{819 , 686 , 525},  // BEIGE*/
        /*{153 , 220 , 196},  // DSEA*/
        /*{314 , 427 , 396},  // SEA*/
        /*{368 , 490 , 478},  // LSEA*/
        /*{721 , 521 , 325},  // WOOD*/
        /*{721 , 521 , 325}   // LWOOD*/
/*};*/

/*struct triplet __SHADE[]={*/
        /*{78 , 71 , 78 },  // DGREY*/
        /*{74 , 74 , 74 },  // GREY*/
        /*{388, 388, 388},  // LGREY*/
        /*{729, 729, 729},  // DBLUE*/
        /*{204, 227, 459},  // BLUE*/
        /*{259, 490, 741},  // LBLUE*/
        /*{514, 706, 780},  // DGREEN*/
        /*{59 , 149, 94 },  // GREEN*/
        /*{141, 290, 243},  // LGREEN*/
        /*{459, 580, 274},  // YELLOW*/
        /*{831, 741, 314},  // BROWN*/
        /*{368, 263, 125},  // ORANGE*/
        /*{651, 412, 239},  // RED*/
        /*{470, 184, 220},  // PINK*/
        /*{561, 349, 396},  // BEIGE*/
        /*{721, 619, 498},  // DSEA*/
        /*{55 , 71 , 67 },  // SEA*/
        /*{196, 259, 243},  // LSEA*/
        /*{314, 388, 384}   // WOOD*/
/*};*/

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
        /*init_color(_DSEA,       153,  212,  188);*/
        /*init_color(_DSEA,       161,  220,  200);*/
        /*init_color(_DSEA,      125,  169,  157);*/
        /*init_color(_DSEA,      145,  200,  180);*/
        init_color(_DSEA,       153,    220,    196);
        /*!!*//*init_color(_DSEA,       153,    220,    196);*/
        /*init_color(_DSEA,       169,    231,    208);*/
        /*init_color(_SEA,       255,  361,  329);*/
        /*init_color(_SEA,        286, 419,  384);*/
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
        /*init_color(SANDY, 886,  737,  557);*/

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

        init_pair(BEI_SKY, BEIGE, WILDBLUEYONDER);
        init_pair(BEI_SEA, BEIGE, DSEA);
        init_pair(BEI_DGR, BEIGE, DGREY);
        init_pair(BEI_BRO, BEIGE, BROWN);
        init_pair(BEI_DBR, BEIGE, DARK_BROWN);
        init_pair(BEI_WOO, BEIGE, WOOD);

        init_pair(WOO_SKY, WOOD, WILDBLUEYONDER);
        init_pair(WOO_SEA, WOOD, DSEA);
        init_pair(WOO_DGR, WOOD, DGREY);
        init_pair(WOO_BEI, WOOD, BEIGE);
        init_pair(WOO_BRO, WOOD, BROWN);
        init_pair(WOO_DBR, WOOD, DARK_BROWN);

        init_pair(BRO_SKY, BROWN, WILDBLUEYONDER);
        init_pair(BRO_SEA, BROWN, DSEA);
        init_pair(BRO_DGR, BROWN, DGREY);
        init_pair(BRO_BEI, BROWN, BEIGE);
        init_pair(BRO_DBR, BROWN, DARK_BROWN);
        init_pair(BRO_WOO, BROWN, WOOD);

        init_pair(DBR_SKY, DARK_BROWN, WILDBLUEYONDER);
        init_pair(DBR_SEA, DARK_BROWN, DSEA);
        init_pair(DBR_DGR, DARK_BROWN, DGREY);
        init_pair(DBR_DGR, DARK_BROWN, DGREY);
        init_pair(DBR_BEI, DARK_BROWN, BEIGE);
        init_pair(DBR_BRO, DARK_BROWN, BROWN);
        init_pair(DBR_WOO, DARK_BROWN, WOOD);

        init_pair(YEL_SKY, YELLOW, WILDBLUEYONDER);
        init_pair(UND_SEA, DSEA, DSEA);
        init_pair(D_SEA,  DSEA, _DSEA);
        init_pair(DD_SEA, _DSEA, DSEA);
        init_pair(DAY_SKY, WILDBLUEYONDER, WILDBLUEYONDER);
        init_pair(RED_SKY, RED, WILDBLUEYONDER);
        init_pair(SKY_BRO, WILDBLUEYONDER, BROWN);
        init_pair(BRZ_SKY, BLASTOFFBRONZE, WILDBLUEYONDER);
        init_pair(BRZ_DBR, BLASTOFFBRONZE, DARK_BROWN);
        init_pair(BRZ_WOO, BLASTOFFBRONZE, WOOD);
        init_pair(BRZ_BRO, BLASTOFFBRONZE, BROWN);
        init_pair(WHI_SKY, WHITE, WILDBLUEYONDER);
        init_pair(GRE_SKY, CELESTIALBLUE, WILDBLUEYONDER);




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
        init_pair(LAND, BROWN, BEIGE);
        init_pair(TREETOP, LGREEN, GREEN);
        init_pair(TREETRUNK, WOOD, DGREY);
        init_pair(GRASSY, LGREEN, GREEN);
        init_pair(SEA_DARK, GREEN, DSEA);
        init_pair(SEA_MED, SEA, DSEA);
        init_pair(SEA_LIG, LSEA, DSEA);
        init_pair(SEA_SAND, SANDY, BEIGE);
        init_pair(SEA_SHALLOW, LSHALLOW, DSEA);
        init_pair(_SEA_SHALLOW, LSHALLOW, _DSEA);
        /*init_pair(_SEA_SHALLOW, _SEA, DSEA);*/

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
        init_pair(SEA__MED, _SEA, DSEA);
        init_pair(SEA___MED, __LSEA, DSEA);
}
