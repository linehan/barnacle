#pragma once
#ifndef __PALETTE_H
#define __PALETTE_H
#include "fall.h"
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
        LSEA       = 36,
        LWOOD      = 37,
        WOOD       = 38,
        SHADOW     = 39, 
        DARK_RED   = 42, 
        DARK_BROWN = 43,
};

enum arne_shadow1 {
        /*_BLACK  = 43, black is always black */
        _DGREY  = 44,
        _GREY   = 45,
        _LGREY  = 46,
        _DBLUE  = 47,
        _BLUE   = 48,
        _LBLUE  = 49,
        _DGREEN = 50,
        _GREEN  = 51,
        _LGREEN = 52,
        _BROWN  = 53,
        _ORANGE = 54,
        _YELLOW = 55,
        _RED    = 56,
        _PINK   = 57,
        _BEIGE  = 58,
        _WHITE  = 59,
        _DSEA   = 60,
        _SEA    = 61,
        _LSEA   = 62,
        _LWOOD  = 64,
        _WOOD   = 65,
        _SHADOW = 66,
        _DARK_RED = 67
};

enum arne_shadow2 {
        /*__BLACK  = 68, black is always black */
        __DGREY  = 69,
        __GREY   = 70,
        __LGREY  = 71,
        __DBLUE  = 72,
        __BLUE   = 73,
        __LBLUE  = 74,
        __DGREEN = 75,
        __GREEN  = 76,
        __LGREEN = 77,
        __BROWN  = 78,
        __ORANGE = 79,
        __YELLOW = 80,
        __RED    = 81,
        __PINK   = 82,
        __BEIGE  = 83,
        __WHITE  = 84,
        __DSEA   = 85,
        __SEA    = 86,
        __LSEA   = 87,
        __LWOOD  = 88,
        __WOOD   = 89,
        __SHADOW = 90,
        __DARK_RED = 91,
        __BG = 97,
        CELESTIALBLUE = 92,
        CERULEAN = 93,
        WILDBLUEYONDER = 94,
        BLASTOFFBRONZE = 95,
        METALYELLOW    = 96,
        ZEN_BG = 97,
        TESTFG = 98,
        TESTBG = 99,
        SANDY = 100,
        SHALLOW = 101,
        LSHALLOW= 102,
        A_SAND_LAGOON = 103,
        B_SAND_LAGOON = 104,
        C_SAND_LAGOON = 105,
        D_SAND_LAGOON = 106,
        A_CLIFF = 107,
        B_CLIFF = 108,
        C_CLIFF = 109,
        D_CLIFF = 110,
        WATER_HINT = 111,
        PAL1 = 112,
        PAL2 = 113,
        PAL3 = 114,
        PAL4 = 115,
        PAL5 = 116,
        DARK_SAND0 = 117,
        DARK_SAND1 = 118,
        DARK_SAND2 = 119,
        DARK_SAND3 = 120,
        DARK_SAND4 = 121,
        DARK_SANDZ = 122,
        /* FJORD PALETTE 123-135*/
};
enum light {
        LIGHT1 = 123,
        LIGHT2 = 124,
        LIGHT3 = 125,
        LIGHT4 = 126,
        LIGHTB1 = 127,
        LIGHTB2 = 128,
        LIGHTB3 = 129,
        LIGHTB4 = 130,
};

enum flex {
        FLEXBG = 131,
        FLEXFG = 132,
        FLEXBG_DEFAULT = 133,
        FLEXFG_DEFAULT = 134 
};



/* ----- PAIRS ------ */

/* FG_BG color pairs; odd ones are swaps of even ones. */
enum ramp_pairs {
        SEA_DARK      = 17,     /* SHADE0 colors for environment */
        SEA_MED       = 18,
        BOAT_DEEP     = 19,
        BOAT_WOOD     = 20, 
        BOAT_WHITE    = 21,
        //SAIL_DEEP     = 22,
        LAND          = 23,
        TREETOP       = 24,
        TREETRUNK     = 25,
        GRASSY        = 26,
        SOILGRASS     = 27,
        SEA_LIG       = 28,

        _SEA_DARK      = 29,    /* SHADE1 colors for environemnt */
        _SEA_MED       = 30,
        _BOAT_DEEP     = 31,
        _BOAT_WOOD     = 32, 
        _BOAT_WHITE    = 33,
        _SAIL_DEEP     = 34,
        _LAND          = 35,
        _TREETOP       = 36,
        _TREETRUNK     = 37,
        _GRASSY        = 38,
        _SOILGRASS     = 39,
        _SEA_LIG       = 40, 

        __SEA_DARK      = 41,   /* SHADE2 colors for environment */
        __SEA_MED       = 42,
        __BOAT_DEEP     = 43,
        __BOAT_WOOD     = 44, 
        __BOAT_WHITE    = 45,
        __SAIL_DEEP     = 46,
        __LAND          = 47,
        __TREETOP       = 48,
        __TREETRUNK     = 49,
        __GRASSY        = 50,
        __SOILGRASS     = 51,
        __SEA_LIG       = 52,

        SEA__MED = 53,
        SEA___MED = 54,

        CMP_BEIGE     = 55,   /* These colors are for UI elements */
        CMP_RED       = 56,
        CMP_WHITE     = 57,
        CMP_ORANGE    = 58,
        CMP_GREEN     = 59,
        CMP_PINK      = 60,
        CMP_YELLOW    = 61,
        CMP_SHADOW    = 62,
        WARNING       = 63,
        WARNSHADOW    = 64,
        GUN_FLASH     = 65,
        GUN_SMOKE     = 66,

        TESTING      = 67,
        SEA_SAND     = 68,
        SEA_SHALLOW  = 69,
        _SEA_SHALLOW = 70,
        A_SEA_LAGOON = 71,
        B_SEA_LAGOON = 72,
        C_SEA_LAGOON = 73,
        D_SEA_LAGOON = 74,
        CLIFF_PAIR   = 75,
};

enum stuff {
        BSAND = 76,
        CSAND = 77,
        DSAND = 78,
        ESAND = 79,
        FSAND = 80,
};



#define PUR_WHI PUR_WHITE
#define PUR_BLU PUR_CELESTIAL
#define PUR_GRE PUR_GREY
#define PUR_PUR PUR_PURPLE
#define PUR_DPU PUR_WHITE
#define PUR_DDP PUR_WHITE
#define PUR_YEL PUR_YELLOW
#define PUR_DYE __PUR_YELLOW
#define PUR_EEN PUR_LGREEN
#define PUR_BRZ PUR_BRONZE
#define PUR_PIN PUR_LRED
#define PUR_ORA PUR_ORANGE
#define PUR_BEI PUR_BEIGE
#define PUR_WOO PUR_WOOD
#define TITLE_SCREEN PUR_PURPLE


void init_palette(int);

enum purpairs {
        PUR_YELLOW      = 81,
        PUR_ORANGE      = 82,
        _PUR_YELLOW     = 83,
        _PUR_ORANGE     = 84,
        __PUR_YELLOW    = 85,
        __PUR_ORANGE    = 86,
//-----
        PUR_BEIGE       = 87,
        PUR_BROWN       = 88,
        _PUR_BEIGE      = 89,
        _PUR_BROWN      = 90,
        __PUR_BEIGE     = 91,
        __PUR_BROWN     = 92,
//-----
        PUR_LBLUE       = 93,
        PUR_BLUE        = 94,
        PUR_DBLUE       = 95,
        _PUR_LBLUE      = 96,
        _PUR_BLUE       = 97,
        _PUR_DBLUE      = 98,
        __PUR_LBLUE     = 99,
        __PUR_BLUE      = 100,
        __PUR_DBLUE     = 101,
//-----
        PUR_LGREEN      = 102,
        PUR_GREEN       = 103,
        PUR_DGREEN      = 104,
        _PUR_LGREEN     = 105,
        _PUR_GREEN      = 106,
        _PUR_DGREEN     = 107,
        __PUR_LGREEN    = 108,
        __PUR_GREEN     = 109,
        __PUR_DGREEN    = 110,
//-----
        PUR_LRED        = 111,
        PUR_RED         = 112,
        PUR_DRED        = 113,
        _PUR_LRED       = 114,
        _PUR_RED        = 115,
        _PUR_DRED       = 116,
        __PUR_LRED      = 117,
        __PUR_RED       = 118,
        __PUR_DRED      = 119,
//-----
        PUR_LSEA        = 120,
        PUR_SEA         = 121,
        PUR_DSEA        = 122,
        _PUR_LSEA       = 123,
        _PUR_SEA        = 124,
        _PUR_DSEA       = 125,
        __PUR_LSEA      = 126,
        __PUR_SEA       = 127,
        __PUR_DSEA      = 128,
//-----
        PUR_LWOOD       = 129,
        PUR_WOOD        = 130,
        _PUR_LWOOD      = 131,
        _PUR_WOOD       = 132,
        __PUR_LWOOD     = 133,
        __PUR_WOOD      = 134,
//-----
        PUR_SKY         = 135,
        PUR_BRONZE      = 136,
        PUR_METALYELLOW = 137,
        PUR_CELESTIAL   = 138,
        PUR_CERULEAN    = 139,
        PUR_BLACK       = 140,
        PUR_GREY        = 141, 
        _PUR_GREY       = 142, 
        __PUR_GREY      = 143, 
        PUR_WHITE       = 144,
        PUR_PURPLE      = 145,
        _PUR_PURPLE     = 146,
        PUR_ARSENIC     = 147,
        FLEX            = 148,
        FLEX2           = 149,
        PALP1           = 150,
        PALP2           = 151,
        PALP3           = 152,
        PALP4           = 153,
        PALP5           = 154,
        PUR_SOLID       = 155,
};

enum light_pair {
        LIGHTP1 = 156,
        LIGHTP2 = 157,
        LIGHTP3 = 158,
        LIGHTP4 = 159
};

enum black_pair {
        BLACK_BEIGE = 160,
};



void darken_colors(int step);

static inline short BGCOLOR(short pair)
{
        short fg, bg;
        pair_content(pair, &fg, &bg);
        return bg;
}

static inline short FGCOLOR(short pair)
{
        short fg, bg;
        pair_content(pair, &fg, &bg);
        return fg;
}


static inline 
void blend(short color1, float i1, short color2, float i2, short dest)
{
        short r1, r2;
        short g1, g2;
        short b1, b2;

        color_content(color1, &r1, &g1, &b1); 
        color_content(color2, &r2, &g2, &b2); 

        r1 = (((i1*r1) + (i2*r2)) / 2);
        g1 = (((i1*g1) + (i2*g2)) / 2);
        b1 = (((i1*b1) + (i2*b2)) / 2);

        init_color(dest, r1, g1, b1);
}


static struct rgb_t wallfg[3] = { {252, 431, 372, 290},{252, 431, 372, 290},{252, 431, 372, 290}};
static struct rgb_t wallbg[3] = { {254, 310, 270, 208},{254, 310, 270, 208},{254, 310, 270, 208}};
static struct rgb_t backbg[3] = { {254, 310, 270, 208},{254, 310, 270, 208},{254, 310, 270, 208}};
static struct rgb_t backfg[3] = { {255, 169, 137, 94},{255, 169, 137, 94},{255, 169, 137, 94}};

static short FG[3]={255,254,253};
static short BG[3]={252,251,250};

#define TITLE_COLOR PUR_BLACK

#endif


