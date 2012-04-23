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
};

/* FG_BG color pairs; odd ones are swaps of even ones. */
enum ramp_pairs {

        CMP_BEIGE     = 100,   /* These colors are for UI elements */
        CMP_RED       = 101,
        CMP_WHITE     = 102,
        CMP_ORANGE    = 103,
        CMP_GREEN     = 104,
        CMP_PINK      = 105,
        CMP_YELLOW    = 106,
        CMP_SHADOW    = 107,
        WARNING       = 109,
        WARNSHADOW    = 110,
        GUN_FLASH     = 111,
        GUN_SMOKE     = 112,


        //PUR_WHI = 150,
        //PUR_DPU = 151,
        //PUR_DDP = 152,
        //PUR_PUR = 153,
#define PUR_WHI PUR_WHITE
#define PUR_BLU PUR_CELESTIAL
#define PUR_GRE PUR_GREY
#define PUR_PUR PUR_PURPLE
        //PUR_YEL = 154,
        //PUR_GRE = 155,
        //PUR_DYE = 156,
        //PUR_BLU = 157,
        //PUR_EEN = 159,
        //PUR_BRZ = 160,
        //PUR_PIN = 161,
        //PUR_ORA = 162,
        //PUR_BEI = 163,
        //PUR_WOO = 164,
        //TITLE_SCREEN = 166,
        TESTING = 167,
        SEA_SAND = 141,
        SEA_SHALLOW = 142,
        _SEA_SHALLOW = 143,
        A_SEA_LAGOON = 144,
        B_SEA_LAGOON = 145,
        C_SEA_LAGOON = 146,
        D_SEA_LAGOON = 147,
        CLIFF_PAIR = 148,
        //RED_PINK = 148,

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

};

enum stuff {
        BSAND = 113,
        CSAND = 114,
        DSAND = 115,
        ESAND = 116,
        FSAND = 117,
};




/* These are the first values of each shade level in the enum; they can be
 * used for boundary-checking to determine which shade level we're in. */
#define SHADE0 17 
#define SHADE1 29 
#define SHADE2 41 
#define SHADE_MAX 52 /* UI colors should be >> this number, to keep them out */

/* The "step" refers to the number of places each shade level is shifted from
 * it's equivalent color pair in the shader level above or below it. Obviously,
 * this step value should be the same for both transitions, but if it isn't,
 * it will equal 0, meaning when we calculate the offset, with a bum step
 * value, it will return the original value unchanged (since 0 is the additive 
 * identity). */
#define SHADE_STEP SHADE1-SHADE0

/* Secondary macro to decide between SHADE1 or SHADE 2, once SHADE0 is 
 * ruled out */
#define SHADE1_OR_SHADE2(pair) \
        ((pair < SHADE_MAX)&&(pair >= SHADE2)) ? 2 : 1

/* Return the shade level of the provided pair */
#define SHADE_LEVEL(pair) \
        ((pair < SHADE1)&&(pair >= SHADE0)) ? 0 : SHADE1_OR_SHADE2(pair)

static inline short to_shade(short pair, int lvl)
{
        int mylvl = SHADE_LEVEL(pair);
        short newpair = pair;

        do {
                if (mylvl<lvl) {
                        mylvl++;
                        newpair += SHADE_STEP;
                }
                else if (mylvl>lvl) {
                        mylvl--;
                        newpair -= SHADE_STEP;
                }
        } while (mylvl != lvl);
        return newpair;
}

void init_palette(int);

enum purpairs {
        PUR_YELLOW      = 168,
        PUR_ORANGE      = 169,
        _PUR_YELLOW     = 170,
        _PUR_ORANGE     = 171,
        __PUR_YELLOW    = 172,
        __PUR_ORANGE    = 173,
//-----
        PUR_BEIGE       = 174,
        PUR_BROWN       = 175,
        _PUR_BEIGE      = 177,
        _PUR_BROWN      = 178,
        __PUR_BEIGE     = 180,
        __PUR_BROWN     = 181,
//-----
        PUR_LBLUE       = 183,
        PUR_BLUE        = 184,
        PUR_DBLUE       = 185,
        _PUR_LBLUE      = 186,
        _PUR_BLUE       = 187,
        _PUR_DBLUE      = 188,
        __PUR_LBLUE     = 189,
        __PUR_BLUE      = 190,
        __PUR_DBLUE     = 191,
//-----
        PUR_LGREEN      = 192,
        PUR_GREEN       = 193,
        PUR_DGREEN      = 194,
        _PUR_LGREEN     = 195,
        _PUR_GREEN      = 196,
        _PUR_DGREEN     = 197,
        __PUR_LGREEN    = 198,
        __PUR_GREEN     = 199,
        __PUR_DGREEN    = 200,
//-----
        PUR_LRED        = 201,
        PUR_RED         = 202,
        PUR_DRED        = 203,
        _PUR_LRED       = 204,
        _PUR_RED        = 205,
        _PUR_DRED       = 206,
        __PUR_LRED      = 207,
        __PUR_RED       = 208,
        __PUR_DRED      = 209,
//-----
        PUR_LSEA        = 210,
        PUR_SEA         = 211,
        PUR_DSEA        = 212,
        _PUR_LSEA       = 213,
        _PUR_SEA        = 214,
        _PUR_DSEA       = 215,
        __PUR_LSEA      = 216,
        __PUR_SEA       = 217,
        __PUR_DSEA      = 218,
//-----
        PUR_LWOOD       = 219,
        PUR_WOOD        = 220,
        _PUR_LWOOD      = 221,
        _PUR_WOOD       = 222,
        __PUR_LWOOD     = 223,
        __PUR_WOOD      = 224,
//-----
        PUR_SKY         = 225,
        PUR_BRONZE      = 226,
        PUR_METALYELLOW = 227,
        PUR_CELESTIAL   = 228,
        PUR_CERULEAN    = 229,
        PUR_BLACK       = 230,
        PUR_GREY        = 231, 
        _PUR_GREY       = 232, 
        __PUR_GREY      = 233, 
        PUR_WHITE       = 234, 
        PUR_PURPLE      = 235, 
        _PUR_PURPLE     = 236, 
        PUR_ARSENIC = 237,
        FLEX=238,
        FLEX2=239,
        PALP1 = 240,
        PALP2 = 241,
        PALP3 = 242,
        PALP4 = 243,
        PALP5 = 244
};

void darken_colors(int step);
static struct rgb_t wallfg[3] = { {252, 431, 372, 290},{252, 431, 372, 290},{252, 431, 372, 290}};
static struct rgb_t wallbg[3] = { {254, 310, 270, 208},{254, 310, 270, 208},{254, 310, 270, 208}};
static struct rgb_t backbg[3] = { {254, 310, 270, 208},{254, 310, 270, 208},{254, 310, 270, 208}};
static struct rgb_t backfg[3] = { {255, 169, 137, 94},{255, 169, 137, 94},{255, 169, 137, 94}};

static short FG[3]={255,254,253};
static short BG[3]={252,251,250};

#define TITLE_COLOR PUR_BLACK

#endif


