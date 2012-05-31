#pragma once
#ifndef __PALETTE_H
#define __PALETTE_H

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
        DARK_RED   = 34, 
};

enum arne_shade1 {
        /*_BLACK  = 43, black is always black */
        _DGREY  = 35,
        _GREY   = 36,
        _LGREY  = 37,
        _DBLUE  = 38,
        _BLUE   = 39,
        _LBLUE  = 40,
        _DGREEN = 41,
        _GREEN  = 42,
        _LGREEN = 43,
        _BROWN  = 44,
        _ORANGE = 45,
        _YELLOW = 46,
        _RED    = 47,
        _PINK   = 48,
        _BEIGE  = 49,
        _WHITE  = 50,
        _DARK_RED =51 
};

enum arne_shade2 {
        /*__BLACK  = 68, black is always black */
        __DGREY  = 52,
        __GREY   = 53,
        __LGREY  = 54,
        __DBLUE  = 55,
        __BLUE   = 56,
        __LBLUE  = 57,
        __DGREEN = 58,
        __GREEN  = 59,
        __LGREEN = 60,
        __BROWN  = 61,
        __ORANGE = 62,
        __YELLOW = 63,
        __RED    = 64,
        __PINK   = 65,
        __BEIGE  = 66,
        __WHITE  = 67,
        __DARK_RED = 68,
};

enum special_colors {
        CELESTIALBLUE = 69,
        CERULEAN = 70,
        WILDBLUEYONDER = 71,
        BLASTOFFBRONZE = 72,
        METALYELLOW    = 73,
        ZEN_BG = 74,
        ___DGREY = 75,
        ____DGREY = 76 
};

/* TERRAIN COLORS *************************************************/

enum ocean_colors {
        COCEAN_BG = 77,  /* Background of ocean swells */
        COCEAN_HI = 78,  /* "High" ocean swell */
        COCEAN_LO = 79,  /* "Trough" */
        _COCEAN_BG = 80,
};
enum sand_colors {
        CSAND_BG = 81,
        CSAND_FG = 82,
};
enum grasses {
        CLGRASS_BG = 83,
        CLGRASS_FG = 84,
        CDGRASS_BG = 85,
        CDGRASS_FG = 86,
};
enum tree_colors {
        CTREETOP_BG = 87,
        CTREETOP_FG = 88,
        CTREETRUNK_BG = 89,
        CTREETRUNK_FG = 90,
};
enum cliff_colors {
        CCLAY_BG = 91,
        CCLAY_FG = 92,
};

/** LIGHT SOURCES ***********************************************/

enum torch_light {
        TORCH_BASE = 93,
        TORCHF0 = 93,
        TORCHF1 = 94,
        TORCHF2 = 95,
        TORCHF3 = 96,
        TORCHF4 = 97,
        TORCHF5 = 98,
        TORCHB0 = 99,
        TORCHB1 = 100,
        TORCHB2 = 101,
        TORCHB3 = 102,
        TORCHB4 = 103,
        TORCHB5 = 104,
};

enum sun_light {
        SUN_BASE = 105,
        SUNF0 = 105,
        SUNF1 = 106,
        SUNF2 = 107,
        SUNF3 = 108,
        SUNF4 = 109,
        SUNF5 = 110,
        SUNB0 = 111,
        SUNB1 = 112,
        SUNB2 = 113,
        SUNB3 = 114,
        SUNB4 = 115,
        SUNB5 = 116,
};


enum reserved_light1 {
        RLIGHT1_BASE = 117,
        RLIGHT1F0 = 118,
        RLIGHT1F1 = 119,
        RLIGHT1F2 = 120,
        RLIGHT1F3 = 121,
        RLIGHT1F4 = 122,
        RLIGHT1F5 = 123,
        RLIGHT1B0 = 124,
        RLIGHT1B1 = 125,
        RLIGHT1B2 = 126,
        RLIGHT1B3 = 127,
        RLIGHT1B4 = 128,
        RLIGHT1B5 = 129,
};


enum reserved_light2 {
        RLIGHT2_BASE = 130,
        RLIGHT2F0 = 131,
        RLIGHT2F1 = 132,
        RLIGHT2F2 = 133,
        RLIGHT2F3 = 134,
        RLIGHT2F4 = 135,
        RLIGHT2F5 = 136,
        RLIGHT2B0 = 137,
        RLIGHT2B1 = 138,
        RLIGHT2B2 = 139,
        RLIGHT2B3 = 140,
        RLIGHT2B4 = 141,
        RLIGHT2B5 = 142,
};

enum flex {
        FLEXBG = 143,
        FLEXFG = 144,
        FLEXBG_DEFAULT = 145,
        FLEXFG_DEFAULT = 146 
};


enum lightsandcolors {
        TORCH_COLOR = 147 
};

#define GLOW_COLOR_START TORCHF0
#define GLOW_COLOR_END   RLIGHT2B5





/* ----- PAIRS ------ */



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
#define TITLE_SCREEN PUR_PURPLE




enum purpairs {
        PUR_YELLOW      = 10,
        PUR_ORANGE      = 11,
        _PUR_YELLOW     = 12,
        _PUR_ORANGE     = 13,
        __PUR_YELLOW    = 14,
        __PUR_ORANGE    = 15,
        PUR_BEIGE       = 16,
        PUR_BROWN       = 17,
        _PUR_BEIGE      = 18,
        _PUR_BROWN      = 19,
        __PUR_BEIGE     = 20,
        __PUR_BROWN     = 21,
        PUR_LBLUE       = 22,
        PUR_BLUE        = 23,
        PUR_DBLUE       = 24,
        _PUR_LBLUE      = 25,
        _PUR_BLUE       = 26,
        _PUR_DBLUE      = 27,
        __PUR_LBLUE     = 28,
        __PUR_BLUE      = 29,
        __PUR_DBLUE     = 30,
        PUR_LGREEN      = 31,
        PUR_GREEN       = 32,
        PUR_DGREEN      = 33,
        _PUR_LGREEN     = 34,
        _PUR_GREEN      = 35,
        _PUR_DGREEN     = 36,
        __PUR_LGREEN    = 37,
        __PUR_GREEN     = 38,
        __PUR_DGREEN    = 39,
        PUR_LRED        = 40,
        PUR_RED         = 41,
        PUR_DRED        = 42,
        _PUR_LRED       = 43,
        _PUR_RED        = 44,
        _PUR_DRED       = 45,
        __PUR_LRED      = 46,
        __PUR_RED       = 47,
        __PUR_DRED      = 48,

        PUR_SKY         = 49,
        PUR_BRONZE      = 50,
        PUR_METALYELLOW = 51,
        PUR_CELESTIAL   = 52,
        PUR_CERULEAN    = 53,
        PUR_BLACK       = 54,
        PUR_GREY        = 55, 
        _PUR_GREY       = 56, 
        __PUR_GREY      = 57, 
        PUR_WHITE       = 58,
        PUR_PURPLE      = 59,
        _PUR_PURPLE     = 60,
        ___PUR_PURPLE  = 61,
        ____PUR_PURPLE = 62,
        ___PUR_GREY    = 63,
        ____PUR_GREY   = 64,
        PUR_SOLID       = 65,
};

enum terrain_pairs {
        POCEAN_HI = 66,
        POCEAN_LO = 67,
        POCEAN_DK = 68,

        PGRASS_HI = 69,
        PGRASS_LO = 70,

        PTREETOP = 71,
        PTREETRUNK = 72,

        PBEACHSAND = 73,
        PCLAYCLIFF = 74,

        PCAVEBLANK = 75,
        PCAVEBLOCK = 76,
};

enum flexpairs {
        FLEX    = 77,
        FLEX2   = 78,
};


enum light_pairs {
        TORCHP0 = 79,
        TORCHP1 = 80,
        TORCHP2 = 81,
        TORCHP3 = 82,
        TORCHP4 = 83,
        TORCHP5 = 84,

        SUNP0   = 85,
        SUNP1   = 86,
        SUNP2   = 87,
        SUNP3   = 88,
        SUNP4   = 89,
        SUNP5   = 90,

        RL1P0   = 91,
        RL1P1   = 92,
        RL1P2   = 93,
        RL1P3   = 94,
        RL1P4   = 95,
        RL1P5   = 96,

        RL2P0   = 97,
        RL2P1   = 98,
        RL2P2   = 99,
        RL2P3   = 100,
        RL2P4   = 101,
        RL2P5   = 102,
};


enum edible_pairs {
        FLEX_APPLE = 103,
        FLEX_GRAPEFRUIT = 104,
        FLEX_LG_POTION = 105,
        FLEX_SM_POTION = 106,
        FLEX_ELIXIR = 107,
        FLEX_PALM = 108,
};

enum junk_pairs {
        WARNING       = 109,
        WARNSHADOW    = 110,
        PROPE = 111
};


short bgcolor(short pair);
short fgcolor(short pair);

static inline 
void blend(short color1, float i1, short color2, float i2, short dest)
{
        short r1, r2;
        short g1, g2;
        short b1, b2;

        color_content(color1, &r1, &g1, &b1); 
        color_content(color2, &r2, &g2, &b2); 

        r1 = (i1 == 0.0) ? r2 : (((i1*r1) + (i2*r2)) / 2);
        g1 = (i1 == 0.0) ? g2 : (((i1*g1) + (i2*g2)) / 2);
        b1 = (i1 == 0.0) ? b2 : (((i1*b1) + (i2*b2)) / 2);

        init_color(dest, r1, g1, b1);
}

void assign_glow_colors(short base, short *fg, short *bg, short *pa);
void init_palette(int);

#define TITLE_COLOR PUR_BLACK

#endif


