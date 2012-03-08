// vim:fdm=marker
#ifndef __GFX_TYPES
#define __GFX_TYPES
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdlib.h>
#include <wchar.h>
#include <ncurses.h>
#include <pthread.h>
#include <semaphore.h>
#include "../lib/llist/list.h"
#include "../pan/test.h"
#include "../geo/map.h"

extern sem_t *REFRESH_LOCK; // keeps things from going crazy in the i/o thread

#define PEEK(winring) (winring)->peek->window
#define NEXT(winring) CYCLEF(winring->ring, winring->peek, struct wnode_t);
#define PREV(winring) CYCLEB(winring->ring, winring->peek, struct wnode_t);

//##############################################################################
//# Locks the REFRESH_LOCK semaphore before refreshing vertical or actual      #
//# screen. Prevents things from going haywire if something called from the    #
//# control thread tries to refresh the screen at the same time as the main    #
//# thread.                                                                    #
//##############################################################################
#define vrt_refresh()           \
        sem_wait(REFRESH_LOCK); \
        update_panels();        \
        sem_post(REFRESH_LOCK)

#define scr_refresh()           \
        sem_wait(REFRESH_LOCK); \
        update_panels();        \
        doupdate();             \
        sem_post(REFRESH_LOCK)

#define win_refresh(win)        \
        sem_wait(REFRESH_LOCK); \
        wrefresh(win);          \
        sem_post(REFRESH_LOCK)

#define map_refresh(map) \
        sem_wait(REFRESH_LOCK); \
        copywin(PEEK(map->W), map->win, ufo_y(map->ufo), ufo_x(map->ufo), 0, 0, LINES-1, COLS-1, 0); \
        update_panels(); \
        doupdate(); \
        sem_post(REFRESH_LOCK)

//##############################################################################
//# Toggles a PANEL, hiding it if it is not hidden, and showing it if it is.   #
//##############################################################################
#define TOGPAN(pan)              \
        if (panel_hidden(pan))   \
                show_panel(pan); \
        else                     \
                hide_panel(pan)  

static inline void reveal(PANEL *pan)
{
        if (panel_hidden(pan)) {
                show_panel(pan);
                top_panel(pan);
        }
        else
                hide_panel(pan);
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
struct wnode_t {
        int id;
        WINDOW *window;
        struct list_node node; 
};

/* Generalized ring type */
struct ring_t {
        int n;
        struct wnode_t *peek;
        struct list_head *ring;
};

struct gpkg {
        int n;
        int ofs[16];
        short pair;
        const wchar_t *wch[16];
        int len[16];
        cchar_t *cch[16];
};

struct recipe {
        int n;
        int xof;
        int yof;
        short pair[64];
        const wchar_t *wch[100];
        const char *bar[100];
        int len[100];
        cchar_t cch[100][100];
        int wid;
        WINDOW *win;
        void (*paint)(const void *self); // method
};


struct cbar {
        short pair[10];
        const char *bar;
};

void illuminate(struct recipe *R);
void build_gpkg(struct gpkg *g);
void build_gpkg_cbar(struct gpkg *g, struct cbar *c);
void geojug_start(void);

struct wnode_t   *new_wnode(int id, int h, int w, int y0, int x0);
struct ring_t  *new_winring(int h, int w, int y0, int x0, int nwindows);
int                hit_test(struct map_t *map, int y, int x);

void center_text(WINDOW *win, int y0, int x0, int w, char *string);
void wwrapstr(WINDOW *win, const char *string);
#endif
