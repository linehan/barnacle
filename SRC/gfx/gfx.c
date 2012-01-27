// vim:fdm=marker
/* ==========================================================================
    FILENAME:  gfx.c
    This module makes extensive use of the "famous" doubly linked list 
    routines written by Rusty Russell <rusty@rustcorp.com.au> for use in 
    the Linux kernel. He later modified them to an even more general form, 
    which is implemented here. For more information, see the CCAN entry 
    at <http://ccodearchive.net/info/list.html>.
   ========================================================================== */
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <pthread.h>
#include <semaphore.h>

#include "../lib/llist/list.h"
#include "palette.h"
#include "sprite.h"
/* ========================================================================== */
/* Refresh the environment's pad by calling pnoutrefresh() followed by
 * doupdate(). */
#define ENV_REFRESH(env) prefresh(env->pad,                   \
                                  env->pminrow, env->pmincol, \
                                  env->sminrow, env->smincol, \
                                  env->smaxrow, env->smaxcol)
/* Synchronize the environment's pad by refreshing its VIRTUAL screen,
 * without yet refreshing the ACTUAL screen that the monitor displays.
 * There are many situations in which it is advantageous to modify the
 * virtual screen multiple times without calling for doupdate(). Using
 * both calls increases CPU usage and screen flicker. */
#define ENV_SYNC(env) pnoutrefresh(env->pad,                   \
                                   env->pminrow, env->pmincol, \
                                   env->sminrow, env->smincol, \
                                   env->smaxrow, env->smaxcol)
/* Calls ENV_SYNC, followed by doupdate(), to modify the virtual screen
 * one last time, and then write the virtual screen to the monitor. */
#define ENV_SYNCUP(env) ENV_SYNC(env) \
                        doupdate()

#define TOP_WNODE(gfx) (WNODE *)list_top(gfx->wins, WNODE, node)
#define LST_WNODE(gfx) (WNODE *)list_tail(gfx->wins, WNODE, node)
#define NXT_WNODE(gfx) (WNODE *)next_wnode(gfx)

#define TOP_GNODE(env) (GNODE *)list_top(&env->wad, GNODE, node)
#define LST_GNODE(env) (GNODE *)list_tail(&env->wad, GNODE, node)
#define NXT_GNODE(env) (GNODE *)next_gnode(env)

/* Panel options that can be used to identify the parameter that
 * one wishes returned from a GNODE structure */
enum pan_options { 
        __pan__ = 0,
        __win__ = 1,
        __dim__ = 2,
        __sem__ = 3,
        __obj__ = 4,
        __h__   = 5, 
        __w__   = 6, 
        __y0__  = 7,
        __x0__  = 8, 
        __dy__  = 9, 
        __dx__  = 10, 
        __n__   = 11
};

/* Panel identifiers that can be used to identify a particular
 * type of GNODE when searching the graphics stack. */
enum ids { __bg__ = 0,
           __fg__ = 1,
           __an__ = 2,
           __hi__ = 3,
           __rw__ = 4,
};

/* A structure that bundles all the window dimensions */
typedef struct dimension_t {
        int h;  /* window height */
        int w;  /* window width  */
        int y0; /* window initial y */
        int x0; /* window initial x */
        int dy; /* window current y */
        int dx; /* window current x */
        int n;  /* number of windows */
} DIMS;
/* A container to hold one entire "environment" */
typedef struct environment_t {
        struct list_head wad;
        signed int x_co;
        signed int y_co;
        WINDOW *bgw;
        PANEL  *bgp;
        sem_t  *sem;
} ENV;
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

sem_t *REFRESH_LOCK;
/* ========================================================================== */
/* Start the graphics engine */
void gfx_init(void)
{
        REFRESH_LOCK = (sem_t *)malloc(sizeof(sem_t));
        sem_init(REFRESH_LOCK, 0, 1);
}
/* Master refresh */
void scr_refresh(void)
{
        update_panels();
        doupdate();
}
/* Refresh the virtual screen */
void vrt_refresh(void)
{
        update_panels();
}
/******************************************************************************
 * These are the functions that will be referenced by the function pointers
 * (methods) in the various levels of the graphics stack. Their operation
 * has been described above, and the code should be simple enough to follow.
 ******************************************************************************/
/******************************************************************************
 * PLATE methods
 ******************************************************************************/
int next_gnode(const void *plate)
{
        PLATE *pl = (PLATE *)plate;

        if (list_empty(pl->gfx))
                return 0;
        else {
                pl->G = list_top(pl->gfx, GNODE, node);
                        list_del_from(pl->gfx, &pl->G->node);
                        list_add_tail(pl->gfx, &pl->G->node);
        } return 1;
}
int prev_gnode(const void *plate)
{
        PLATE *pl = (PLATE *)plate;

        if (list_empty(pl->gfx))
                return 0;
        else {
                pl->G = list_tail(pl->gfx, GNODE, node);
                        list_del_from(pl->gfx, &pl->G->node);
                        list_add(pl->gfx, &pl->G->node);
        } return 1;
}
/******************************************************************************
 * GNODE methods
 ******************************************************************************/
int next_wnode(const void *gnode)
{
        GNODE *gfx = (GNODE *)gnode;

        if (list_empty(gfx->wins))
                return 0;
        else {
                gfx->W = list_top(gfx->wins, WNODE, node);
                         list_del_from(gfx->wins, &gfx->W->node);
                         list_add_tail(gfx->wins, &gfx->W->node);
        } return 1;
}
int prev_wnode(const void *gnode)
{
        GNODE *gfx = (GNODE *)gnode;

        if (list_empty(gfx->wins))
                return 0;
        else {
                gfx->W = list_tail(gfx->wins, WNODE, node);
                         list_del_from(gfx->wins, &gfx->W->node);
                         list_add(gfx->wins, &gfx->W->node);
        } return 1;
}
int step_wnode_forward(const void *gnode)
{
        GNODE *gfx = (GNODE *)gnode;

        if ((gfx->id != __an__)||(list_empty(gfx->wins)))
                return 0;
        else {
                gfx->nextw(gfx);
                replace_panel(gfx->pan, gfx->W->window);
                vrt_refresh();
        } return 1;
}
int step_wnode_backward(const void *gnode)
{
        GNODE *gfx = (GNODE *)gnode;

        if ((gfx->id != __an__)||(list_empty(gfx->wins)))
                return 0;
        else {
                gfx->prevw(gfx);
                replace_panel(gfx->pan, gfx->W->window);
                vrt_refresh();
        } return 1;
}
int step_all_forward(const void *plate)
{
        PLATE *pl = (PLATE *)plate;
        GNODE *tmp;

        sem_wait(pl->sem);
        list_for_each(pl->gfx, tmp, node) {
                        tmp->stepf(tmp);
                        scr_refresh();
        }
        sem_post(pl->sem);
        return 1;
}

/******************************************************************************
 * Allocate memory for a new WNODE, and initialize it with the arguments
 * supplied. This function is almost always called from inside new_gnode(), 
 * for (pretty good) reasons, as described in that function's description.
 ******************************************************************************/
WNODE *new_wnode(int id, int h, int w, int y0, int x0)
{
        WNODE *new = (WNODE *)malloc(sizeof(WNODE));
        new->window = newwin(h, w, y0, x0);
        new->id = id;

        return new;
}
/******************************************************************************
 * Allocate memory for a new GNODE, and initialize it with the arguments
 * supplied. 
 *
 * Notice that every WINDOW in the WNODE ring has the exact same 
 * dimension and position (the h, w, y0, x0 passed to new_gfx()). This is a 
 * feature, not a bug -- PANELs assume (in both senses of the word) the size 
 * of the WINDOW connected to them, and S would begin to HTF, were this not
 * enforced.
 ******************************************************************************/
GNODE *new_gnode(int id, int h, int w, int y0, int x0, int n)
{
        int i;
        GNODE *gfx = (GNODE *)malloc(sizeof(GNODE));

        gfx->id = id;

        /* Methods */
        gfx->nextw = &next_wnode;
        gfx->prevw = &prev_wnode;
        gfx->stepf = &step_wnode_forward;
        gfx->stepb = &step_wnode_backward;

        /* Initialize the WNODE ring */
        gfx->wins = malloc(sizeof(struct list_head));
        list_head_init(gfx->wins);

        /* Add WNODEs to the ring */
        for (i=0; i<n; i++) {
                WNODE * new = new_wnode(__an__, h, w, y0, x0);
                list_add(gfx->wins, &new->node);
        }

        /* Set *W and attach its window to the GNODE panel */
        gfx->nextw(gfx);
        gfx->pan = new_panel(gfx->W->window);

        /* Record the dimension parameters. */
        gfx->dim.h  = h;
        gfx->dim.w  = w;
        gfx->dim.y0 = y0;
        gfx->dim.x0 = x0;
        gfx->dim.dy = 0;
        gfx->dim.dx = 0;
        gfx->dim.n  = n;

        return gfx;
}
/* Create a new environment with an ocean background */
PLATE *new_plate(int type)
{
        PLATE *new = (PLATE *)malloc(sizeof(PLATE));

        new->id = type;
        new->h  = LINES;
        new->w  = COLS;

        /* Methods */
        new->nextg = &next_gnode;
        new->prevg = &prev_gnode;
        new->stepf = &step_all_forward;

        /* Allocate and initialize GNODE ring */
        new->gfx = (struct list_head *)malloc(sizeof(struct list_head));
        list_head_init(new->gfx);

        /* Allocate and initialize semaphore */
        new->sem = (sem_t *)malloc(sizeof(sem_t));
        sem_init(new->sem, 0, 1);

       /* Create a new background GNODE, set its background cchar_t, and
        * place it at the bottom of the panel stack. */
        GNODE *bg = new_gnode(__bg__, LINES, COLS, 0, 0, 1);
                    cchar_t *tile = bg_tile(type);
                    if ((tile != NULL))
                            wbkgrnd(bg->W->window, tile);
                    bottom_panel(bg->pan);

       /* Add the background GNODE to the GNODE ring, and reference it
        * at *BG, for quick access. */
        list_add(new->gfx, &bg->node);
        new->BG = bg;

        /* Advance the GNODE ring, so that *G points to something. */
        new->nextg(new);

        return new;
}

/* Create and initialize a new MOB */
MOB *new_mob(void *ptr, PLATE *pl, int h, int w, int y0, int x0)
{
        MOB *mob = (MOB *)malloc(sizeof(MOB));
        mob->obj = ptr;
        mob->plate = pl;

        WINDOW *win = newwin(h, w, y0, x0);
        mob->pan = new_panel(win);

        mob->dim.h = h;
        mob->dim.w = w;
        mob->dim.y0 = y0;
        mob->dim.x0 = x0;
        mob->dim.dy = 0;
        mob->dim.dx = 0;
        mob->dim.n = 0;

        mob->sem = malloc(sizeof(sem_t));
        sem_init(mob->sem, 0, 1);

        return mob;
}
/* Simple collision hit test */
int hit_test(PLATE *pl, int y, int x)
{
        GNODE *tmp;
        int i = 0;

        sem_wait(pl->sem);
                list_for_each(pl->gfx, tmp, node) {
                        if ((tmp->id != __bg__)) {
                                if (((x >= tmp->dim.x0) && 
                                    (x <= (tmp->dim.x0 + tmp->dim.w)) &&
                                    (y >= tmp->dim.y0) &&
                                    (y <= (tmp->dim.y0 + tmp->dim.h))))
                                        i++;
                        }
                }
        sem_post(pl->sem);
        return i;
}
/* Move a MOB type one step in one of four directions, checking for window
   edges and collision detection. Threadsafe. */
void move_mob(MOB *mob, int dir)
{
        sem_wait(mob->sem);

                int y = mob->dim.dy;
                int x = mob->dim.dx;

                switch (dir) {
                case 'u':
                        if (((y>0)) && ((hit_test(mob->plate, --y, x) == 0)))
                                mob->dim.dy--;
                        break;
                case 'd':
                        if ((y<LINES) && ((hit_test(mob->plate, ++y, x) == 0)))
                                mob->dim.dy++;
                        break;
                case 'l':
                        if (((x>0)) && ((hit_test(mob->plate, y, --x) == 0)))
                                mob->dim.dx--;
                        break;
                case 'r':
                        if (((x<COLS)) && ((hit_test(mob->plate, y, ++x) == 0)))
                                mob->dim.dx++;
                        break;
                }
                move_panel(mob->pan, mob->dim.dy, mob->dim.dx);
        sem_post(mob->sem);
}
