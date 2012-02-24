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
        DARK_RED   = 42 
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
        __DARK_RED = 91 
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
        MENU          = 108,
        WARNING       = 109,
        WARNSHADOW    = 110,
        GUN_FLASH     = 111,
        GUN_SMOKE     = 112,

        SEA_DARK      = 17,     /* SHADE0 colors for environment */
        SEA_MED       = 18,
        BOAT_DEEP     = 19,
        BOAT_WOOD     = 20, 
        BOAT_WHITE    = 21,
        SAIL_DEEP     = 22,
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
        __SEA_LIG       = 52 
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

int init_palette(int);
