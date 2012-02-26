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
#include "../pan/test.h"
#include "../lib/redblack/rb.h"

#define NNIBS    8  // Number of nibbles in a MAP byte.
#define NSTATES 16  // Number of states each nibble can take.
#define NBITS   32  // Number of bits (total) in a MAP byte.
// The 16 possible states of a cell nibble.
static const
uint32_t state[NSTATES] = {0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0x00000004, 0x00000005, 0x00000006, 0x00000007,
                           0x00000008, 0x00000009, 0x0000000A, 0x0000000B,
                           0x0000000C, 0x0000000D, 0x0000000E, 0x0000000F};

// Mask values for each of the 8 nibbles in a cell.
static const
uint32_t scrub[NNIBS]   = {0x0FFFFFFF, 0xF0FFFFFF, 0xFF0FFFFF, 0xFFF0FFFF,
                           0xFFFF0FFF, 0xFFFFF0FF, 0xFFFFFF0F, 0xFFFFFFF0};

// Inverted mask values for each of the 8 nibbles in a cell.
static const
uint32_t gouge[NNIBS]   = {0xF0000000, 0x0F000000, 0x00F00000, 0x000F0000,
                           0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F};

// The offset, in bits, of each of the 8 nibbles in a cell.
static const
uint32_t offset[NNIBS] = {28, 24, 20, 16, 12, 8, 4, 0};


struct switch_t {
        struct rb_tree *tree;    // Data is stored in this red-black tree.
        const char  *nib[8];     // Labels for each nibble.
        const char **opt[8]; // Labels for the 16 states of each nibble.
};                                                                      





void           set_cell(struct switch_t *sw, uint32_t key, int nib, int opt);
int             is_cell(struct switch_t *sw, uint32_t key, int nib, int opt);
const char *get_celltag(struct switch_t *sw, uint32_t key, int nib);
void          stat_cell(struct switch_t *sw, uint32_t key);

#endif
