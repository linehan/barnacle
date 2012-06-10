#pragma once
#ifndef __PALETTE_H
#define __PALETTE_H

/* The system's colors go from 0-7 */

enum arne_colors {
        BLACK   = 17,
        DGREY,
        GREY,
        LGREY,
        DBLUE,
        BLUE,
        LBLUE,
        DGREEN, 
        GREEN,
        LGREEN,
        BROWN,
        ORANGE,
        YELLOW,
        RED,
        PINK,
        BEIGE,
        WHITE,
        DARK_RED, 
};

enum arne_shade1 {
        /*_BLACK  = 43, black is always black */
        _DGREY  = 35,
        _GREY,
        _LGREY,
        _DBLUE,
        _BLUE,
        _LBLUE,
        _DGREEN,
        _GREEN,
        _LGREEN,
        _BROWN,
        _ORANGE,
        _YELLOW,
        _RED,
        _PINK,
        _BEIGE,
        _WHITE,
        _DARK_RED
};

enum arne_shade2 {
        /*__BLACK  = 68, black is always black */
        __DGREY  = 52,
        __GREY,
        __LGREY,
        __DBLUE,
        __BLUE,
        __LBLUE,
        __DGREEN,
        __GREEN,
        __LGREEN,
        __BROWN,
        __ORANGE,
        __YELLOW,
        __RED,
        __PINK,
        __BEIGE,
        __WHITE,
        __DARK_RED,
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


enum special_colors {
        CELESTIALBLUE = 93,
        CERULEAN,
        WILDBLUEYONDER,
        BLASTOFFBRONZE,
        METALYELLOW,
        ZEN_BG,
        ___DGREY,
        ____DGREY,
        RASPBERRY,
        HOUSEWALL,
        HOUSEROOF,
        HOUSEDOOR
};


/** LIGHT SOURCES ***********************************************/

enum torch_light {
        TORCH_BASE = 207,
        TORCHF0 = TORCH_BASE,
        TORCHF1,
        TORCHF2,
        TORCHF3,
        TORCHF4,
        TORCHF5,
        TORCHB0,
        TORCHB1,
        TORCHB2,
        TORCHB3,
        TORCHB4,
        TORCHB5,
};

enum sun_light {
        SUN_BASE = 219,
        SUNF0 = SUN_BASE,
        SUNF1,
        SUNF2,
        SUNF3,
        SUNF4,
        SUNF5,
        SUNB0,
        SUNB1,
        SUNB2,
        SUNB3,
        SUNB4,
        SUNB5,
};


enum reserved_light1 {
        RLIGHT1_BASE = 231,
        RLIGHT1F0 = RLIGHT1_BASE,
        RLIGHT1F1,
        RLIGHT1F2,
        RLIGHT1F3,
        RLIGHT1F4,
        RLIGHT1F5,
        RLIGHT1B0,
        RLIGHT1B1,
        RLIGHT1B2,
        RLIGHT1B3,
        RLIGHT1B4,
        RLIGHT1B5,
};


enum reserved_light2 {
        RLIGHT2_BASE = 243,
        RLIGHT2F0 = RLIGHT2_BASE,
        RLIGHT2F1,
        RLIGHT2F2,
        RLIGHT2F3,
        RLIGHT2F4,
        RLIGHT2F5,
        RLIGHT2B0,
        RLIGHT2B1,
        RLIGHT2B2,
        RLIGHT2B3,
        RLIGHT2B4,
        RLIGHT2B5,
};

enum flex {
        FLEXBG = 244,
        FLEXFG = 245,
        FLEXBG_DEFAULT = 246,
        FLEXFG_DEFAULT = 247 
};


enum lightsandcolors {
        TORCH_COLOR = 248 
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
        PUR_ORANGE,
        _PUR_YELLOW,
        _PUR_ORANGE,
        __PUR_YELLOW,
        __PUR_ORANGE,
        PUR_BEIGE,
        PUR_BROWN,
        _PUR_BEIGE,
        _PUR_BROWN,
        __PUR_BEIGE,
        __PUR_BROWN,
        PUR_LBLUE,
        PUR_BLUE,
        PUR_DBLUE,
        _PUR_LBLUE,
        _PUR_BLUE,
        _PUR_DBLUE,
        __PUR_LBLUE,
        __PUR_BLUE,
        __PUR_DBLUE,
        PUR_LGREEN,
        PUR_GREEN,
        PUR_DGREEN,
        _PUR_LGREEN,
        _PUR_GREEN,
        _PUR_DGREEN,
        __PUR_LGREEN,
        __PUR_GREEN,
        __PUR_DGREEN,
        PUR_LRED,
        PUR_RED,
        PUR_DRED,
        _PUR_LRED,
        _PUR_RED,
        _PUR_DRED,
        __PUR_LRED,
        __PUR_RED,
        __PUR_DRED,

        PUR_SKY,
        PUR_BRONZE,
        PUR_METALYELLOW,
        PUR_CELESTIAL,
        PUR_CERULEAN,
        PUR_BLACK,
        PUR_GREY, 
        _PUR_GREY, 
        __PUR_GREY, 
        PUR_WHITE,
        PUR_PURPLE,
        _PUR_PURPLE,
        ___PUR_PURPLE,
        ____PUR_PURPLE,
        ___PUR_GREY,
        ____PUR_GREY,
        PUR_SOLID,
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
        TORCHP1,
        TORCHP2,
        TORCHP3,
        TORCHP4,
        TORCHP5,

        SUNP0   = 85,
        SUNP1,
        SUNP2,
        SUNP3,
        SUNP4,
        SUNP5,

        RL1P0   = 91,
        RL1P1,
        RL1P2,
        RL1P3,
        RL1P4,
        RL1P5,

        RL2P0   = 97,
        RL2P1,
        RL2P2,
        RL2P3,
        RL2P4,
        RL2P5,
};


enum edible_pairs {
        FLEX_APPLE = 103,
        FLEX_GRAPEFRUIT = 104,
        FLEX_LG_POTION = 105,
        FLEX_SM_POTION = 106,
        FLEX_ELIXIR = 107,
        FLEX_PALM = 108,
};

enum flower_pairs {
        RED_FLOWER = 109,
        BLUE_FLOWER = 110,
        YELLOW_FLOWER = 111
};

enum junk_pairs {
        WARNING       = 112,
        WARNSHADOW    = 113,
        PROPE = 114,
        INSET_UL = 115,
        INSET_UR = 116,
        INSET_DL = 117,
        INSET_DR = 118,
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


