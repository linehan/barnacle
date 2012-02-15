#ifndef __GFX_TYPES
#define __GFX_TYPES
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include "../lib/llist/list.h"


/* BITBOARD DEFINITIONS
*******************************************************************************/
#define BITBOARD_BLOCK_TYPE  uint32_t /* <---- THIS IS ALL YOU SHOULD TOUCH */
#define BITBOARD_BLOCK_SIZE  sizeof(BITBOARD_BLOCK_TYPE) /* In bytes */
#define BITBOARD_BLOCK_WIDTH (BITBOARD_BLOCK_SIZE*8)     /* In bits  */


/* Expands to the minimum number of blocks that can hold 'nbits' bits. There
 * is always a +1, because the minimum number of blocks is 1, but the '/'
 * operation will reduce to 0 if you let it. */
#define BLOCKS_TO_HOLD(nbits) ((nbits/BITBOARD_BLOCK_WIDTH)+1)

/* Here we do not add 1, because we want the index of the block which contains
 * a particular bit, meaning 0 is a legitimate subscript. */
#define BLOCK_CONTAINING(bit) (bit/BITBOARD_BLOCK_WIDTH)

/* The bitboard type must consist at a minimum of a vector of type 
 * BITBOARD_BLOCK_TYPE, and a value indicating the number of blocks that are
 * in each row of the bitboard, so that the array offset can be computed. */
typedef struct bb { BITBOARD_BLOCK_TYPE *bb; int blocks_per_row; } BITBOARD;


/* Allocate memory for a new bitboard able to accomodate at least rowsxcols
 * individual bit values. 
 * NOTE: calloc() initializes all memory values to 0. */
#define INIT_BITBOARD(bitboard, rows, cols) \
        bitboard.bb = calloc((rows*(BLOCKS_TO_HOLD(cols))), BITBOARD_BLOCK_SIZE); \
        bitboard.blocks_per_row = BLOCKS_TO_HOLD(cols); \


/* BITBOARD INDEXING 
*******************************************************************************/
/* Pass a coordinate pair of *bit* values, NOT block values; expands to the 
 * index of the *block* which contains the bit at (row,col), assuming the
 * blocks are stored in a vector. */
#define BIT_INDEX(bitboard, row, col) \
        bitboard.bb[((row*(bitboard.blocks_per_row))+(BLOCK_CONTAINING(col)))]


/* Given a column value, compute the bit of the containing block which will
 * correspond to that column value. */
#define BIT(col) (col%BITBOARD_BLOCK_WIDTH)


/* BITBOARD OPERATIONS 
*******************************************************************************/
int looper;

#define LOOPER(n) looper = n

/* Expands to 1 if the bit at (row,col) in bitboard 'bitboard' is set, or 0
 * if the bit is clear. */
#define BIT_SET(bitboard, row, col) \
        (((BIT_INDEX(bitboard, row, col))&(1<<(BIT(col)))) != 0) ? 1 : 0


/* Set the bit at (row,col) on bitboard 'bitboard' to 1 */
#define SET_BIT(bitboard, row, col) \
        BIT_INDEX(bitboard, row, col) |= (1<<BIT(col))

#define SET_BITLINE(bitboard, row, col, len) \
        LOOPER((col+len)); \
        do { SET_BIT(bitboard, row, col); } while (++col < looper); \
        col -= len;


/* Clear the bit at (row,col) on bitboard 'bitboard' to 1 */
#define CLEAR_BIT(bitboard, row, col) \
        BIT_INDEX(bitboard, row, col) &= ~(1<<BIT(col))


/* Toggle the bit at (row,col) on bitboard 'bitboard' */
#define TOGGLE_BIT(bitboard, row, col) \
        BIT_INDEX(bitboard, row, col) ^= (1<<BIT(col))


/* BITBOARD TESTS
*******************************************************************************/
static inline void test_bitboards(WINDOW *win)
{
        wprintw(win, "BITBOARD_BLOCK_WIDTH = %d\n"
                     "BITBOARD_BLOCK_SIZE  = %d\n"
                     "LINES                = %d\n"
                     "COLS                 = %d\n"
                     "BITBOARD_COLS        = %d\n",
                     BITBOARD_BLOCK_WIDTH,
                     BITBOARD_BLOCK_SIZE,
                     LINES,
                     COLS,
                     BLOCKS_TO_HOLD(COLS));
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

enum environment_stack {  __bgr__ = 0, /* the background */
                          __rim__ = 1, /* the ocean edge effects */
                          __hig__ = 2, /* background highlights */
                          __drd__ = 3, /* dirt */
                          __drt__ = 4, 
                          __grd__ = 5, /* grass */
                          __grt__ = 6, 
                          __trd__ = 7, /* trees */
                          __trt__ = 8, 
                          __sec__ = 9, /* all secondary features on a terrane */

                          __mob__ = 10, /* mobile graphical elements */
                          __wea__ = 11  /* weather and atmospheric effects */
};
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
