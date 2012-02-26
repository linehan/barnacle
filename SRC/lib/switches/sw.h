// vim:fdm=marker
//{{{1 README
//##############################################################################
//#
//#   We think of the screen as a 2D grid of square tiles, with each tile
//#   being uniquely identified by a coordinate pair (y,x), where y is the
//#   row or line number, and x is the column number. 
//# 
//#   All information about a particular tile is stored in a single uint32_t,
//#   accessed by indexing a screen array. This array is linear, because we 
//#   generate a Morton number for each input pair (y,x), which serves as the
//#   index.
//#
//#   A "nibble" or "nybble" is a unit of 4 bits. There are 8 such nibbles in
//#   our uint32_t (32/4). Each nibble can take one of 2^4 (16) possible 
//#   states. Each nibble represents an option "category", and each of the
//#   16 states of a category represents a "setting". 
//#
//#   To keep myself from glazing over, each nibble is assigned a tag, which
//#   is composed of two complementary parts: an enum, and an array of labels
//#   which is indexed by each enum.
//#
//#   Likewise, all the possible states of a category are identified with an
//#   enum and corresponding label array.
//#
//#                  0000 0000 0000 0000 0000 0000 0000 0000                                
//#                  ENV  TER  SID  MOV  HDG  WND  WEA  ALT                                    
//#
//##############################################################################
//}}}1
#ifndef __SWITCH_TYPES
#define __SWITCH_TYPES

#include <stdint.h>
#include <ncurses.h>
#include "../redblack/rb.h"

#define NNIBS    8  // Number of nibbles in a MAP byte.
#define NSTATES 16  // Number of states each nibble can take.
#define NBITS   32  // Number of bits (total) in a MAP byte.

/*
  Bit mask for each of the 16 possible states of a single nibble.
*/
static const uint32_t 
state[NSTATES] = {0x00000000, 0x00000001, 0x00000002, 0x00000003,
                  0x00000004, 0x00000005, 0x00000006, 0x00000007,
                  0x00000008, 0x00000009, 0x0000000A, 0x0000000B,
                  0x0000000C, 0x0000000D, 0x0000000E, 0x0000000F};
/*
  Bit mask for each of the 8 nibbles in a 32-bit uint32_t.
*/
static const uint32_t 
scrub[NNIBS]   = {0x0FFFFFFF, 0xF0FFFFFF, 0xFF0FFFFF, 0xFFF0FFFF,
                  0xFFFF0FFF, 0xFFFFF0FF, 0xFFFFFF0F, 0xFFFFFFF0};
/*
  Inverted bit mask for each of the 8 nibbles in a 32-bit uint32_t.
*/
static const uint32_t 
gouge[NNIBS]   = {0xF0000000, 0x0F000000, 0x00F00000, 0x000F0000,
                  0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F};
/*
  The left offset of each of the 8 nibbles in a 32-bit uint32_t.
*/
static const uint32_t 
offset[NNIBS]  = {28, 24, 20, 16, 12, 8, 4, 0};


struct sw_t {
        struct rb_tree *tree;  // Data is stored in this red-black tree.
        uint32_t c;            // Used for copies, scratchwork, or errors.
        const char  **nib;     // Labels for each nibble.
        const char ***opt;     // Labels for the 16 states of each nibble.
};                                                                      

/*
  General option types
*/
//{{{1 Directional quantity
enum hdg_t {NORTH,NNE,NE,ENE,EAST,ESE,SE,SSE,SOUTH,SSW,SW,WSW,WEST,WNW,NW,NNW};        
static const 
char *hdg_tag[16] = {"NORTH","NNE","NE","ENE","EAST","ESE","SE", 
                                  "SSE","SOUTH","SSW","SW","WSW","WEST","WNW", 
                                  "NW", "NNW"};
//}}}1
//{{{1 Graduated quantity
enum grad_t {NIL=1,HI5=2,HI4=3,HI3=4,HI2=5,HI1=6,HI0=7,LO5=8,LO4=9,LO3=10,
             LO2=11,LO1=12,LO0=13,ALL=14};
static const char *grad_tag[16] = {"XXX","NIL","HI5","HI4","HI3","HI2","HI1",
                                   "HI0","LO5","LO4","LO3","LO2","LO1","LO0",
                                   "ALL"};
//}}}1
//{{{1 Stacking order 
//##############################################################################
//# These values represent the classification of a tile in the hierarchy of    #
//# the graphics stack. Rendering behaviors use this stacking order to         #
//# determine how to draw the screen, using the ncurses overlay() routine.     #
//##############################################################################
enum lay_t {
        XXX = 0,  // failsafe background (default)
        BGR = 1,  // background
        HIG = 2,  // background highlights (animated)
        RIM = 3,  // edge effects (animated)
        DRP = 4,  // the "drop" of the perspective
        TOP = 5,  // the "top" of the perspective 
        GRO = 6,  // the ground
        HUT = 7,  // dwelling or structure
        VEG = 8,  // vegetation
        SKY = 9   // weather and atmospheric effects
        //XX = 10, // reserved
        //XX = 11, // reserved
        //XX = 12, // reserved
        //XX = 13, // reserved
        //XX = 14, // reserved
        //XX = 15, // reserved
};                                                                      
static const // Label strings for terrain states
char *lay_tag[16] = {"XXX", "BGR", "HIG", "RIM", "DRP", "TOP", "GRO", "HUT",
                     "VEG", "SKY", "XXX", "XXX", "XXX", "XXX", "XXX", "XXX"};
//}}}1


struct sw_t *new_sw(const char **fields, const char ***options);
void             set_sw(struct sw_t *sw, uint32_t key, int nib, int opt);
int              get_sw(struct sw_t *sw, uint32_t key, int nib);
int               is_sw(struct sw_t *sw, uint32_t key, int nib, int opt);
const char   *get_swtag(struct sw_t *sw, uint32_t key, int nib);
void            stat_sw(WINDOW *win, struct sw_t *sw, uint32_t key);

#endif
