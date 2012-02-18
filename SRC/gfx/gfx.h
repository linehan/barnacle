// vim:fdm=marker
#ifndef __GFX_TYPES
#define __GFX_TYPES

#include <pthread.h>
#include <semaphore.h>

#include "../lib/llist/list.h"
#include "../lib/quadtree.h"
#include "../pan/test.h"
#include "zbox.h"

enum shitstain { __bgr__ = 0, __rim__ = 1, __hig__ = 2, __drt__ = 3,
                 __drd__ = 4, __grt__ = 5, __grd__ = 6, __trt__ = 7, 
                 __trd__ = 8, __sec__ = 9, __mob__ = 10, __wea__ = 11 };

extern sem_t *REFRESH_LOCK; // keeps things from going crazy in the i/o thread

/* Refresh the virtual screen */
#define vrt_refresh()           \
        sem_wait(REFRESH_LOCK); \
        update_panels();        \
        sem_post(REFRESH_LOCK)

/* Refresh the actual screen */
#define scr_refresh()           \
        sem_wait(REFRESH_LOCK); \
        update_panels();        \
        doupdate();             \
        sem_post(REFRESH_LOCK)

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

void geojug_start(void);
void set_nyb(ZBOX *Z, int y, int x, int n, ...);
void stat_nyb(ZBOX *Z, uint32_t y, uint32_t x);

WNODE *new_wnode(int id, int h, int w, int y0, int x0);
//void draw_water_rim(PLATE *pl);

#endif
