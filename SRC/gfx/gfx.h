#include <pthread.h>
#include <semaphore.h>
#include "../lib/llist/list.h"
#include "../gen/perlin.h"
/******************************************************************************
 * These values are used to code/check the 'role' of a GNODE, that is, the 
 * particular, unambiguous purpose it serves in the PLATE, and which might
 * need to be identified, e.g., whether or not a GNODE can or should be 
 * animated, or whether it is a background, and so should always remain on 
 * the bottom of the PANEL stack, etc.
 ******************************************************************************/
enum roles { __bg__ = 0, /* the background */
             __fg__ = 1, /* not the background (default) */
             __an__ = 2, /* can be animated (contains more than one WNODE) */
             __hi__ = 3  /* is a highlight */
};
/******************************************************************************
 * Using the DIMS structure is all about laziness, plain and simple. Anything
 * that has dimensions of any kind gets slapped with one of these, so that I
 * know what to expect and can be lazy, since there is one standardized bundle 
 * of int values.
 ******************************************************************************/
typedef struct dimension_t {
        int h;  /* height */
        int w;  /* width  */
        int y0; /* initial y position */
        int x0; /* initial x position */
        int dy; /* current y position */
        int dx; /* current x position */
        int n;  /* number of...things */
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
        int id;
        DIMS   dim;
        PANEL *pan;
        WNODE *W;
        struct list_head *wins;
        struct list_node node;
        int (*nextw)(const void *self);
        int (*prevw)(const void *self);
        int (*stepf)(const void *self);
        int (*stepb)(const void *self);
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
        sem_t *sem;
        int id;
        int h;
        int w;
        GNODE *G;
        GNODE *BG;
        PERLIN *noise;
        struct list_head *gfx; /* Terrain graphics */
        struct list_node node;
        int (*nextg)(const void *self);
        int (*prevg)(const void *self);
        int (*stepf)(const void *self);
} PLATE;
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
        int h;
        int w;
        PLATE *P;
        PLATE **plate;
        PERLIN *noise;
        int (*gonorth)(const void *self);
        int (*gosouth)(const void *self);
        int (*goeast)(const void *self);
        int (*gowest)(const void *self);
} MAP;

/* A mobile data type containing some object */
typedef struct mob_t {
        PANEL *pan;
        DIMS   dim;
        PLATE   *plate;
        sem_t *sem;
        void  *obj;
} MOB;
/******************************************************************************
 * Function prototypes
 ******************************************************************************/
void gfx_init(void);
void scr_refresh(void);
void vrt_refresh(void);

PLATE *new_plate(int type);
GNODE *new_gnode(int id, int h, int w, int y0, int x0, int n);
WNODE *new_wnode(int id, int h, int w, int y0, int x0);

MOB *new_mob(void *ptr, PLATE *pl, int h, int w, int y0, int x0);
void move_mob(MOB *mob, int dir);
int hit_test(PLATE *pl, int y, int x);





