// vim:fdm=marker
#ifndef __GFX_TYPES
#define __GFX_TYPES

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include "../lib/llist/list.h"
#include "../pan/test.h"


/* BITBOARD DEFINITIONS
*******************************************************************************/
#define BITBOARD_BLOCK_TYPE  uint32_t /* <---- THIS IS ALL YOU SHOULD TOUCH */
#define BITBOARD_BLOCK_SIZE  sizeof(BITBOARD_BLOCK_TYPE) /* In bytes */
#define BITBOARD_BLOCK_NBITS (BITBOARD_BLOCK_SIZE*8)     /* In bits  */

/* The width and height can be arbitrary, but I prefer to maximize the width,
 * because it means that if we ever need to use this with an inner loop, that
 * loop will run as quickly as possible, and only have to break into the outer
 * loop once (assuming we scan in row-major order). */
#define BOX_LENGTH 4
#define BOX_STRIDE 2

#define BLOCK_LINES ((LINES/2)+1)
#define BLOCK_COLS  ((COLS/16)+1)
#define BLOCK_TOTAL ((BLOCK_LINES)*(BLOCK_COLS))

#define ROW_TRANSFORM(row) (((row)/2)*(BLOCK_COLS))
#define COL_TRANSFORM(col) (((col)/16))

#define BIT_TRANSFORM(col) ((col%16)*BOX_STRIDE)

enum shitstain { __bgr__ = 0, __rim__ = 1, __hig__ = 2, __drt__ = 3,
                 __drd__ = 4, __grt__ = 5, __grd__ = 6, __trt__ = 7, 
                 __trd__ = 8, __sec__ = 9, __mob__ = 10, __wea__ = 11 };

/*
 * offset = ((row/rows_in_block)*(blocks_in_row))+(col/cols_in_block)
 */
#define BLOCK_INDEX(row, col) \
        ROW_TRANSFORM(row)+COL_TRANSFORM(col)


/* The bitboard type must consist at a minimum of a vector of type 
 * BITBOARD_BLOCK_TYPE, and a value indicating the number of blocks that are
 * in each row of the bitboard, so that the array offset can be computed. */
typedef struct bb { BITBOARD_BLOCK_TYPE *bb; int blocks_in_row; } BITBOARD;


/* Allocate memory for a new bitboard able to accomodate at least rowsxcols
 * individual bit values. 
 * NOTE: calloc() initializes all memory values to 0. */
#define INIT_BITBOARD(bitboard, rows, cols) \
        bitboard.blocks_in_row = (BLOCK_COLS); \
        bitboard.bb = calloc(BLOCK_TOTAL, BITBOARD_BLOCK_SIZE)



/* BITBOARD INDEXING 
*******************************************************************************/
static uint32_t row_even = 0x33333333; // 0x3 == 0011 
static uint32_t row_odd  = 0xCCCCCCCC; // 0xC == 1100

/* BITBOARD OPERATIONS 
*******************************************************************************/
inline static int bit_set(BITBOARD *B, unsigned int row, unsigned int col)
{
        unsigned int _row = (row/2)*BLOCK_COLS;
        unsigned int _col = (col/16);
        unsigned int _bit = (col%16);
        unsigned int block = _row+_col;

        unsigned int n; // the bit we want
        n = _bit;
        //n += (row & 1) ? 1 : 0; // add 1 offset if row is odd

        return ((B->bb[block] & (1<<n)) != 0) ? 1 : 0;
}


inline static void set_bit(BITBOARD *B, unsigned int row, unsigned int col)
{
        unsigned int _row = (row/2)*BLOCK_COLS;
        unsigned int _col = (col/16);
        unsigned int _bit = (col%16);
        unsigned int block = _row+_col;

        unsigned int n; // the bit we want
        n = _bit;
        //n += (row & 1) ? 1 : 0; // add 1 offset if row is odd

        B->bb[block] |= (1<<n);
}

inline static void set_block(BITBOARD *B, unsigned int block)
{
        B->bb[block] =~ (0); // yep
}

inline static void mort(unsigned int y, unsigned int x, unsigned int *z)
{
        static const unsigned int B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
        static const unsigned int S[] = {1, 2, 4, 8};

        x = (x | (x << S[3])) & B[3];
        x = (x | (x << S[2])) & B[2];
        x = (x | (x << S[1])) & B[1];
        x = (x | (x << S[0])) & B[0];
        
        y = (y | (y << S[3])) & B[3];
        y = (y | (y << S[2])) & B[2];
        y = (y | (y << S[1])) & B[1];
        y = (y | (y << S[0])) & B[0];

        *z = x | (y << 1);
}

/****************************************************************************** 
 * Anatomy of a MAP byte (32-bit unsigned int)                                *
 *                                                                            *
 *  0000 0000 0000 0000 0000 0000 0000 0000                                   *
 *  ENV  SID  MOB  HDG  WND  WEA  ALT                                         *
 ******************************************************************************/
#define NNIBBLES  8  // number of nibbles
#define NSTATES  16  // number of states each nibble can take
#define NBITS    32  // number of bits (total) that our datatype holds

static const uint32_t state[NSTATES] = { 
0x00000000, 0x00000001, 0x00000002, 0x00000003,
0x00000004, 0x00000005, 0x00000006, 0x00000007,
0x00000008, 0x00000009, 0x0000000A, 0x0000000B,
0x0000000C, 0x0000000D, 0x0000000E, 0x0000000F };

static const uint32_t scrub[NNIBBLES] = { 
0x0FFFFFFF, 0xF0FFFFFF, 0xFF0FFFFF, 0xFFF0FFFF,
0xFFFF0FFF, 0xFFFFF0FF, 0xFFFFFF0F, 0xFFFFFFF0 };

static const uint32_t gouge[NNIBBLES] = { 
0xF0000000, 0x0F000000, 0x00F00000, 0x000F0000,
0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F };

static const uint32_t offset[NNIBBLES] = { 28, 24, 21, 16, 12, 8, 4, 0 };

enum nibble_tags { ENV = 0, SID = 1, MOV = 2, HDG = 3,
                   WND = 4, WEA = 5, ALT = 6, RVD = 7 };

enum ENV_state_tags {                                                   /*{{{1*/
        BGR = 0,  // the failsafe background
        OCN = 1,  // the ocean (background)
        DRT = 2,  // dirt
        GRA = 3,  // grass
        TRE = 4,  // trees
        //XX = 5,  // reserved
        //XX = 6,  // reserved
        //XX = 7,  // reserved
        //XX = 8,  // reserved
        //XX = 9,  // reserved
        //XX = 10, // reserved
        //XX = 11, // reserved
        //XX = 12, // reserved
        //XX = 13, // reserved
        //XX = 14, // reserved
        //XX = 15  // reserved
};                                                                      
static const char *ENV_tags[16] = {"BGR", "OCN", "DRT", "GRA", 
                                   "TRE", "XXX", "XXX", "XXX", 
                                   "XXX", "XXX", "XXX", "XXX",
                                   "XXX", "XXX", "XXX", "XXX" };
                                                                        /*}}}1*/
enum SID_state_tags {                                                   /*{{{1*/
        TOP = 0, // top
        BOT = 1, // bottom
        LEF = 2, // left
        RIG = 3, // right 
        FRO = 4, // front 
        BAC = 5, // back 
        //XX = 6,  // reserved
        //XX = 7,  // reserved
        //XX = 8,  // reserved
        //XX = 9,  // reserved
        //XX = 10, // reserved
        //XX = 11, // reserved
        //XX = 12, // reserved
        //XX = 13, // reserved
        //XX = 14, // reserved
        //XX = 15  // reserved
};
static const char *SID_tags[16] = {"TOP", "BOT", "LEF", "RIG", 
                                   "FRO", "BAC", "XXX", "XXX", 
                                   "XXX", "XXX", "XXX", "XXX",
                                   "XXX", "XXX", "XXX", "XXX" };
                                                                        /*}}}1*/
enum MOV_state_tags {                                                   /*{{{1*/
        RIM = 0, // edge effects (animated)
        HIG = 1, // mobile highlights
        CLO = 2, // cloud shadow 
        SHI = 3, // boat
        //XX = 4,  // reserved
        //XX = 5,  // reserved
        //XX = 6,  // reserved
        //XX = 7,  // reserved
        //XX = 8,  // reserved
        //XX = 9,  // reserved
        //XX = 10, // reserved
        //XX = 11, // reserved
        //XX = 12, // reserved
        //XX = 13, // reserved
        //XX = 14, // reserved
        //XX = 15  // reserved
};
static const char *MOV_tags[16] = {"RIM", "HIG", "CLO", "SHI", 
                                   "XXX", "XXX", "XXX", "XXX", 
                                   "XXX", "XXX", "XXX", "XXX",
                                   "XXX", "XXX", "XXX", "XXX" };
                                                                        /*}}}1*/
enum HDGWID_state_tags {                                               /*{{{1*/
        // NOTE: WID (wind direction) and HDG (heading) use the same tags
        NORTH = 0,  // north
        NNE   = 1,  // north-north-east
        NE    = 2,  // north-east
        ENE   = 3,  // east-north-east
        EAST  = 4,  // east
        ESE   = 5,  // east-south-east 
        SE    = 6,  // south-east
        SSE   = 7,  // south-south-east
        SOUTH = 8,  // south
        SSW   = 9,  // south-south-west
        SW    = 10, // south-west
        WSW   = 11, // west-south-west
        WEST  = 12, // west
        WNW   = 13, // west-north-west
        NW    = 14, // north-west
        NNW   = 15  // north-north-west
}; 
static const char *HDGWID_tags[16] = {"NORTH", "NNE", "NE", "ENE", 
                                      "EAST" , "ESE", "SE", "SSE", 
                                      "SOUTH", "SSW", "SW", "WSW",
                                      "WEST" , "WNW", "NW", "NNW" };
                                                                        /*}}}1*/
enum WEA_state_tags {                                                   /*{{{1*/
        SUN = 0,  // sunny
        PAC = 1,  // partly cloudy
        MOC = 2,  // mostly cloudy
        OVC = 3,  // overcast
        FOG = 4,  // fog
        SHW = 5,  // showers
        RAI = 6,  // rain
        THU = 7,  // thunderstorms
        SNO = 8,  // snow
        SLE = 9,  // sleet
        HAI = 10, // hail
        WOU = 11, // whiteout
        HUR = 12, // hurricane
        TOR = 13, // tornado
        TSU = 14, // tsunami
        FRG = 15  // frogs
};
static const char *WEA_tags[16] = {"SUN", "PAC", "MOC", "OVC", 
                                   "FOG", "SHW", "RAI", "THU", 
                                   "SNO", "SLE", "HAI", "WOU",
                                   "HUR", "TOR", "TSU", "FRG" };
                                                                        /*}}}1*/
enum ALT_state_tags {                                                   /*{{{1*/
        ZZ = 0,  // max height
        ZA = 1,  //  
        ZB = 2,  // 
        ZC = 3,  // 
        ZD = 4,  // 
        ZE = 5,  // 
        ZF = 6,  //
        Zz = 7,  // sea level 
        Za = 8,  // 
        Zb = 9,  // 
        Zc = 10, // 
        Zd = 11, // 
        Ze = 12, // 
        Zf = 13, // 
        Zg = 14, // 
        zz = 15  // max depth 
};
static const char *ALT_tags[16] = {"ZZ", "ZA", "ZB", "ZC", 
                                   "ZD", "ZE", "ZF", "Zz", 
                                   "Za", "Zb", "Zc", "Zd",
                                   "Ze", "Zf", "Zg", "zz" };
                                                                        /*}}}1*/

static const char **tags[8] = { ENV_tags, SID_tags, MOV_tags, HDGWID_tags, 
                                HDGWID_tags, WEA_tags, ALT_tags, ALT_tags };

// Bit twiddling macros starting with '_' work on the entire byte
#define _SET(B)       B =~ 0 
#define _IS_SET(B)    B &  1
#define _TOGGLE(B)    B ^= 1
#define _CLEAR(B)     B &  0

// These macros operate on the n-th bit
#define SET(B, n)     B |=  (1 << n) 
#define IS_SET(B, n)  B &   (1 << n)
#define TOGGLE(B, n)  B ^=  (1 << n)
#define CLEAR(B, n)   B &= ~(1 << n)


static inline const char *gettag(uint32_t B, int nyb)
{
        B &= ~(scrub[nyb]); // wipe all but the nibble we want
        B >>= offset[nyb];  // push our nibble to the end

        wprintw(DIAGNOSTIC_WIN, "tagnum: %d\nind: %u\n", nyb, B);

        return tags[nyb][B];
}




#define NEWMORT(maxmort) \
        calloc((maxmort), sizeof(unsigned int))

/******************************************************************************

   TILE   0010 0011 0100 1110 1111 \__
  SCRUB   1111 0000 1111 1111 1111 /  \
          |||| |||| |||| |||| ||||    AND
  CLEAN   0010 0000 0100 1110 1111 \__
  STATE   0000 1001 0000 0000 0000 /  \
          |||| |||| |||| |||| ||||    OR
  FINAL   0010 1001 0100 1110 1111


  state[opt]    the hex bitmask indexed by opt, (an enum)
  offset[tag]   the offset in bits of 'tag', indexed by 'tag' (an enum)


  (state[O]<<offset[tag])
     |     |    | 
     |     |    +-----------------------------+
     +-----|------------+                     |
           |         ___|___           _______|________
           |   Shift state[O] right by offset[tag] bits.
           |   ``|``          ``|``
           +-----+--------------+     

*******************************************************************************/
// small but deadly
#define SET_TILE(T, tag, opt)         \
        T &= scrub[tag];              \
        T |= (state[opt]<<offset[tag])


//inline static void 
/*
     0 0  
     0 1 ---> 0 0 0 1
*/
//static inline void borders(BITBOARD *B, BITBOARD *R)
//{
        //unsigned int z; // number of bits in one block
        //unsigned int n; // number of consecutive bits in sequence (padding)

        //z = 8*(sizeof(B.bb[0]));
        //n = 4;

/* Clear the bit at (row,col) on bitboard 'bitboard' to 1 */
#define CLEAR_BIT(bitboard, row, col) \
        BIT_INDEX(bitboard, row, col) &= ~(1<<BIT(row, col))

/* Toggle the bit at (row,col) on bitboard 'bitboard' */
#define TOGGLE_BIT(bitboard, row, col) \
        BIT_INDEX(bitboard, row, col) ^= (1<<BIT(row, col))

/* BITBOARD TESTS
*******************************************************************************/
static inline void test_bitboards(WINDOW *win)
{
        wprintw(win, "BITBOARD_BLOCK_SIZE   = %d\n"
                     "BITBOARD_BLOCK_NBITS  = %d\n"
                     "LINES                 = %d\n"
                     "COLS                  = %d\n"
                     "BLOCK_LINES           = %d\n"
                     "BLOCK_COLS            = %d\n"
                     "BLOCK_TOTAL           = %d\n"
                     "BLOCK_INDEX(3,17)     = %d\n",
                     BITBOARD_BLOCK_SIZE,
                     BITBOARD_BLOCK_NBITS,
                     LINES,
                     COLS,
                     BLOCK_LINES,
                     BLOCK_COLS,
                     BLOCK_TOTAL,
                     BLOCK_INDEX(3, 17));
}



/* Refresh the virtual screen */
#define vrt_refresh() update_panels()
/* Refresh the actual screen */
#define scr_refresh() vrt_refresh(); doupdate()
/* Toggle a panel */
#define TOGPAN(pan) \
        if (panel_hidden(pan))  show_panel(pan); \
        else                    hide_panel(pan)  
/* Wipe the screen (force re-draw) */
#define WIPE TOGPAN(WIPEPAN); \
             TOGPAN(WIPEPAN)
/* Add a node to a ring (circular linked list) and increment dim.n */
#define LINCREMENT(parent, headname, node) \
        list_add(parent->headname, node);  \
        parent->dim.n++
/* Move through a ring, and leave tmp pointing at the first matching node */
#define MATCH_FIELD(head, tmp, field, value)    \
        list_for_each(head, tmp, node) {        \
                if (tmp->field == value) break; \
        }

/******************************************************************************
 * These values are used to code/check the 'role' of a GNODE, that is, the 
 * particular, unambiguous purpose it serves in the PLATE, and which might
 * need to be identified, e.g., whether or not a GNODE can or should be 
 * animated, or whether it is a background, and so should always remain on 
 * the bottom of the PANEL stack, etc.
 ******************************************************************************/
#define STACK_LAYERS 12 
#define ENV_LAYERS 10 


/******************************************************************************
 * Using the DIMS structure is all about laziness, plain and simple. Anything
 * that has dimensions of any kind gets slapped with one of these, so that I
 * know what to expect and can be lazy, since there is one standardized bundle 
 * of int values.
 ******************************************************************************/
typedef struct dimension_t {
        int id;    /* A unique incremented identifier */
        int kind;  /* Some type of class identifier */
        int layer; /* The layer on the stack it should be drawn at */
        int h;     /* height */
        int w;     /* width  */
        int y0;    /* initial y position */
        int x0;    /* initial x position */
        int ymax;  /* maximum y position */
        int xmax;  /* maximum x position */
        int yco;   /* current y position */
        int xco;   /* current x position */
        int n;     /* number of subjects in ring */
} DIMS;
/******************************************************************************
 * The WNODE data type is used to store a WINDOW and some related state
 * information. The WINDOW is certainly not the star of the current graphics
 * scheme, and because of the panels(3X) library, it is practically a second-
 * class citizen.
 *
 * In the current scheme, the only objects (in general) which are permitted
 * to output to the screen are PANELs. By familiarizing yourself with the 
 * documentation (man panels(3X)), it will become clear that one may imagine 
 * the relationship between a PANEL and a WINDOW as being similar to that
 * between a projector and a slide, things which I hope the reader is yet
 * young enough to remember. 
 *
 * A projector has only one lens and one backlight, and so it can project only 
 * one slide at a time. Any slides which are not currently enjoying the prime 
 * spot between the backlight and the lens contribute nothing to the image on 
 * the screen. Still, these slides exist and are fully prepared to be projected 
 * on to the screen, whenever they get loaded into the prime spot.
 *
 * Each PANEL is like a projector, or rather the image cast by a projector 
 * on to some screen -- the user's screen! As you have surely guessed, the 
 * WINDOW is analagous to a single slide, in that it surely exists, somewhere
 * in memory, but contributes nothing to the screen until it is loaded into
 * the projector. A ring of WNODEs is, naturally, a reel.
 ******************************************************************************/
typedef struct win_wad {
        int id;
        WINDOW *window;
        struct list_node node; 
} WNODE;
/******************************************************************************
 * In a given plate/screen of the world map, the (more or less) atomic
 * element of graphics is the GNODE. A contiguous mass of terrain is likely
 * to be a GNODE, as is the background of the plate itself.
 *
 * Every GNODE contains a unique id and a class, which determines the types
 * of properties it can be counted on to have. For the full list of classes,
 * see the 'enum gclass' above. In addition, it contains a special DIMS data
 * type, outlined above, which is a structure containing various attributes 
 * of the GNODE and, more specifically, the PANEL pointed to by *pan, which
 * is the sole interface between the GNODE and the screen.
 *
 * Each GNODE contains one and only one PANEL, but also holds a ring of  
 * WNODEs, each of which can contain a different WINDOW that can be connected
 * to *pan as desired. A common example would be an animated element, in which 
 * each "frame" of the animation is stored in a WNODE. By cycling through the
 * WNODE list and calling panel_replace() on *pan (see panel(3X)) at some
 * desired interval, the PANEL on-screen will effectively be an animation.
 *
 * Convenient methods for traversing the WNODE ring are included, with the
 * currently active WNODE being referenced by *W. An additional method called 
 * 'step' is used to advance a sequential animation, i.e., it
 * performs a call analagous to 'nextw', then calls replace_panel() with the
 * new current_wnode->window.
 ******************************************************************************/
typedef struct graphics_node {
        BITBOARD bb;
        unsigned int *M;
        DIMS  dim;
        PANEL *pan;
        WNODE *W;
        struct list_head *wins;
        struct list_node node;
        void (*next)(const void *self);
        void (*step)(const void *self);
} GNODE;
/******************************************************************************
 * The world map is divided into "plates", which can be thought of as
 * screens, in that the player transitions to the next plate when reaching
 * one of the four boundaries of the current plate. Thus, a given plate will
 * always fill the entire screen. The PLATE data structure represents these
 * elements.
 *
 * Each plate is composed of a unique coordinate id number, a height and width
 * value corresponding to the number of character rows and columns (which will
 * hopefully match the screen), a circularly linked list of GNODEs, and a
 * pointer *G to the active GNODE, which is set using the methods nextg and
 * prevg.
 ******************************************************************************/
typedef struct map_plate {
        DIMS  dim;
        GNODE *env[ENV_LAYERS];
        GNODE *G;
        double **pmap;
        struct list_head *gfx; /* Terrain graphics */
        struct list_node node;
        void (*next)(const void *self);
        void (*step)(const void *self);
        void (*hideall)(const void *self);
        void (*showall)(const void *self);
} PLATE;
/******************************************************************************
 * The PLATE_ROW structure is a node in a linked list of rows which comprise
 * the entire collection of PLATEs that make up a world map. Each PLATE_ROW
 * itself contains the head of a linked list of PLATE nodes -- the PLATEs
 * which make up the row.
 ******************************************************************************/
typedef struct map_row {
        DIMS dim;
        struct list_head *plate;
        struct list_node node;
} PLATEROW;
/******************************************************************************
 * The world map is represented by the MAP data structure -- this is the
 * complete world generated by the program, the omnes mundus novus. 
 *
 * It consists of the total height and width of the world map, that is, the
 * number of vertical and horizontal plates, to be used as sentinel values 
 * when indexing the array of PLATES. The currently inhabited plate, that is,
 * presumably, the PLATE that is currently displayed on-screen, is referenced 
 * by the *P pointer, to avoid direct dereferencing of the full plate array.
 *
 * All access to the plate array is provided through a collection of methods
 * that will update the PLATE being referenced by *P, and return
 * the new coordinate value.
 ******************************************************************************/
typedef struct world_map {
        DIMS dim;
        double **pmap;
        PLATE *P;
        PLATEROW *R;
        struct list_head *row;
        void (*prevr)(const void *self);
        void (*nextr)(const void *self);
        void (*prevp)(const void *self);
        void (*nextp)(const void *self);
} WORLD;

#endif
/******************************************************************************/
extern WORLD *GLOBE;

void init_geojug(void);
void swab_screen(void);
void restack(PLATE *pl);

WNODE *new_wnode(int id, int h, int w, int y0, int x0);
GNODE *new_gnode(int layer, int h, int w, int y0, int x0, int n);
GNODE *get_gnode(PLATE *pl, int layer);
PLATE *new_plate(int type, int yco, int xco);
WORLD *new_world(int type, int h, int w);

int hit_test(PLATE *pl, int y, int x);
void merge_layer(PLATE *pl, int layer);
void merge_all(PLATE *pl);
void masktestprint(PLATE *pl, int layer);
void bitboard_mark(GNODE *G, int h, int w, int y0, int x0);
GNODE *gnode_below(PLATE *pl, int y, int x);

void draw_water_rim(PLATE *pl);
