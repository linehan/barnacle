// vim:fdm=marker
/******************************************************************************
 * FILENAME:  gfx.c
 * This module makes extensive use of the "famous" doubly linked list 
 * routines written by Rusty Russell <rusty@rustcorp.com.au> for use in 
 * the Linux kernel. He later modified them to an even more general form, 
 * which is implemented here. For more information, see the CCAN entry 
 * at <http://ccodearchive.net/info/list.html>.
 ******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <ncurses.h>
#include <panel.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

#include "../lib/constants.h"
#include "gfx.h"
#include "zbox.h"
#include "../pan/test.h"
#include "../gen/dice.h"
#include "../lib/llist/list.h"
#include "../gen/perlin.h"
#include "palette.h"
#include "sprite.h"
#include "../cmd/control.h"
#include "../geo/terrain.h"

WORLD *GLOBE;
WINDOW *WIPEWIN;
PANEL  *WIPEPAN;

/* Returns 1 if (y,x) is in GNODE 'G', else returns 0. */
#define IN_GNODE(G, y, x) \
        ((x>=G->dim.x0)&&(x<=(G->dim.xmax))&&(y>=G->dim.y0)&&(y<=(G->dim.ymax))) ? 1 : 0

/******************************************************************************/
sem_t *REFRESH_LOCK;

/* Initialize a blank window that can force a re-draw of the screen */
void init_wipewin(void)
{
        WIPEWIN = newwin(LINES, COLS, 0, 0);
        WIPEPAN = new_panel(WIPEWIN);
        bottom_panel(WIPEPAN);
        hide_panel(WIPEPAN);
}

/* Start the graphics engine */
void init_geojug(void)
{
        /* Initialize the refresh lock semaphore */
        REFRESH_LOCK = (sem_t *)malloc(sizeof(sem_t));
        sem_init(REFRESH_LOCK, 0, 1);
        init_palette(0);        /* Select color palette */
        init_gfx_colors();      /* "Paint" sprites with palette */
        init_weather();         /* Start the weather engine */
        init_wipewin();         /* Initialize a blank window */
}

/* Prime the background buffer to prevent flicker on the first animation 
 ******************************************************************************
 * ncurses can only detect if a line of the screen has been "touched", i.e.
 * altered, or not. In most cases, the background's animated elements will
 * cover much of the screen height, meaning that more or less the entire
 * screen will be "touched", and have to be redrawn, resulting in a 
 * substantial flicker.) 
 ******************************************************************************/
void swab_screen(void)
{
        GLOBE->P->step(GLOBE->P);
        vrt_refresh();
        GLOBE->P->step(GLOBE->P);
        scr_refresh();
}
                
/* Traverse a ring of GNODES and arrange the PANELS in the appropriate stack
 * order. */
void restack(PLATE *pl)
{
        GNODE *tmp;
        int i;
        for (i=0; i<ENV_LAYERS; i++) {
                top_panel(pl->L[i]->pan);
        }
        list_for_each(pl->gfx, tmp, node) {
                if (tmp->dim.layer == i++) {
                        top_panel(tmp->pan);
                        vrt_refresh();
                }
        }
}
GNODE *gnode_below(PLATE *pl, int y, int x)
{
        GNODE *tmp; 
        int i;
        for (i=ENV_LAYERS-1; i>=0; i--) {
                if (IN_GNODE(pl->L[i], y, x) == 1) 
                        return pl->L[i];
        }
        return NULL;
}


void combine(PLATE *pl)
{
        int i;
        static int sw = 1;
        for (i=__rim__; i<__mob__; i++) {
                overlay(pl->L[i]->W->window, pl->L[BGR]->W->window);
        }
        if (sw) {
                top_panel(pl->L[BGR]->pan);
                sw = 0;
        }
        scr_refresh();
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
/* GNODE methods *********************************************************{{{1*/
void next_wnode(const void *gnode)
{
        GNODE *gfx = (GNODE *)gnode;
        CYCLE(gfx->wins, gfx->W, WNODE);
}
void step_wnode_forward(const void *gnode)
{
        GNODE *gfx = (GNODE *)gnode;
        if (gfx->dim.n <= 1) return;

        gfx->next(gfx);
        replace_panel(gfx->pan, gfx->W->window);
        scr_refresh();
}
/*************************************************************************}}}1*/
/******************************************************************************
 * Allocate memory for a new GNODE, and initialize it with the arguments
 * supplied. 
 *
 * Notice that every WINDOW in the WNODE ring has the exact same 
 * dimension and position (the h, w, y0, x0 passed to new_gfx()). This is a 
 * feature, not a bug -- PANELs assume (in both senses of the word) the size 
 * of the WINDOW connected to them, and S would begin to HTF if this rule 
 * wasn't being enforced.
 ******************************************************************************/
GNODE *new_gnode(int layer, int h, int w, int y0, int x0, int n)
{
        GNODE *gfx = (GNODE *)malloc(sizeof(GNODE));

        /* See DIMS datatype in gfx.h */
        gfx->dim.layer = layer;
        gfx->dim.h     = h;
        gfx->dim.w     = w;
        gfx->dim.y0    = y0;
        gfx->dim.x0    = x0;
        gfx->dim.ymax  = y0+h-1; 
        gfx->dim.xmax  = x0+w-1;
        gfx->dim.yco   = y0;    /* Current coords = starting coords */
        gfx->dim.xco   = x0;
        gfx->dim.n     = n;

        /* Method callbacks */
        gfx->next = &next_wnode;
        gfx->step = &step_wnode_forward;

        __RING(gfx->wins);

        /* Initialize n WNODEs */
        int i;
        for (i=0; i<n; i++) {
                WNODE * new = new_wnode(i, h, w, y0, x0);
                LINCREMENT(gfx, wins, &new->node);
        }
        gfx->next(gfx);
        gfx->pan = new_panel(gfx->W->window);

        return gfx;
}
/* PLATE methods *********************************************************{{{1*/
void next_gnode(const void *plate)
{
        PLATE *pl = (PLATE *)plate;
        CYCLE(pl->gfx, pl->G, GNODE);
}
void step_all_forward(const void *plate)
{
        PLATE *pl = (PLATE *)plate;
        GNODE *tmp;
        list_for_each(pl->gfx, tmp, node) {
                if (tmp->dim.n >= 1) tmp->step(tmp);
        }
}
void show_all_gnodes(const void *plate)
{
        PLATE *pl = (PLATE *)plate;
        GNODE *tmp;
        list_for_each(pl->gfx, tmp, node) {
                show_panel(tmp->pan);
                vrt_refresh();
        }
        /*restack(pl);*/
        scr_refresh();
}
void hide_all_gnodes(const void *plate)
{
        PLATE *pl = (PLATE *)plate;
        GNODE *tmp;
        list_for_each(pl->gfx, tmp, node) {
                hide_panel(tmp->pan);
        }
}
/*************************************************************************}}}1*/
/******************************************************************************
 * Allocate memory for and initialize a new PLATE structure. This is the only
 * 'new_' function that takes an argument, an int corresponding to an enum
 * entry (see ./sprites.h) that identifies a particular tile and palette that
 * will be set as the background of the PLATE. Everything else is relatively
 * straightforward, and essentially the same as the previous functions for
 * a new WNODE or GNODE.
 ******************************************************************************/
PLATE *new_plate(int kind, int yco, int xco)
{
        PLATE *new = (PLATE *)malloc(sizeof(PLATE));

        /* See DIMS datatype in gfx.h */
        new->dim.kind = kind;
        new->dim.h    = LINES;
        new->dim.w    = COLS;
        new->dim.xco  = xco;
        new->dim.yco  = yco;

        /* Method callbacks */
        new->showall = &show_all_gnodes;
        new->hideall = &hide_all_gnodes;
        new->next    = &next_gnode;
        new->step    = &step_all_forward;

        __RING(new->gfx);
        new->pmap = gen_perlin_map(LINES, COLS);
        cchar_t *tile = get_tile(kind);

        new->Z = new_zbox(LINES, COLS);

        /*new->env[__sec__] = new_gnode(__sec__, LINES, COLS, 0, 0, 1);*/

        int i, j, n;
        for (i=0; i<16; i++) {
                n = (i == RIM) ? 2 : 1;
                new->L[i] = new_gnode(i, LINES, COLS, 0, 0, n);
        }

        /* Background layer */
        for (i=0; i<LINES; i++) {
                mvwhline_set(new->L[BGR]->W->window, i, 0, &OCEAN[0], COLS);
                /*mvwhline_set(new->env[__bgr__]->W->window, i, 0, &OCEAN[0], COLS);*/
        }

        /* Highlight layer */
        /*int wid, x0, y0;*/
        /*for (i=0; i<5; i++) {*/
                /*wid = roll_fair(50);*/
                /*x0  = roll_fair(80);*/
                /*y0  = roll_fair(50);*/
                /*GNODE *newhi = new_gnode(__hig__, 1, wid, y0, x0, 1);*/
                /*wbkgrnd(newhi->W->window, &OCEAN[2]);*/
                /*LINCREMENT(new, gfx, &newhi->node);*/
        /*}*/
        /*for (i=0; i<5; i++) {*/
                /*wid = roll_fair(20)+1;*/
                /*x0  = roll_fair(80)+1;*/
                /*y0  = roll_fair(50)+1;*/
                /*GNODE *newwea = new_gnode(__wea__, 5, wid, y0, x0, 1);*/
                /*LINCREMENT(new, gfx, &newwea->node);*/
        /*}*/
        /*restack(new); [> Make sure everything is in order <]*/
        return new;
}
/******************************************************************************/
PLATEROW *new_plate_row(int type, int yco, int w)
{
        int i;
        PLATEROW *new = malloc(sizeof(PLATEROW));

        new->dim.w = w;
        new->dim.yco = yco;

        __RING(new->plate);

        for (i=0; i<w; i++) {
                PLATE *newpl = new_plate(type, yco, i);
                LINCREMENT(new, plate, &newpl->node);
        }

        return new;
}
/* MAP methods ***********************************************************{{{1*/
void next_plate(const void *world)
{
        WORLD *map = (WORLD *)world;

        map->P = list_top(map->R->plate, PLATE, node);
                 list_del_from(map->R->plate, &map->P->node);
                 list_add_tail(map->R->plate, &map->P->node);

        /* Update the x coordinate */
        map->dim.xco = map->P->dim.xco;
}
void prev_plate(const void *world)
{
        WORLD *map = (WORLD *)world;

        map->P = list_tail(map->R->plate, PLATE, node);
                 list_del_from(map->R->plate, &map->P->node);
                 list_add(map->R->plate, &map->P->node);

        /* Update the x coordinate */
        map->dim.xco = map->P->dim.xco;
}
void next_row(const void *world)
{
        WORLD *map = (WORLD *)world;

        int saved_xco = map->dim.xco;

        CYCLE(map->row, map->R, PLATEROW);

        /* Update the current y coordinate */
        map->dim.yco = map->R->dim.yco;
        /* Walk the ring in order to get the PANEL of the new ROW
         * that matches the current x coordinate */
        MATCH_FIELD(map->R->plate, map->P, dim.xco, saved_xco);
        map->dim.xco = saved_xco;
}
void prev_row(const void *world)
{
        WORLD *map = (WORLD *)world;

        int saved_xco = map->dim.xco;

        CYCLE(map->row, map->R, PLATEROW);
        map->R = list_tail(map->row, PLATEROW, node);
                 list_del_from(map->row, &map->R->node);
                 list_add(map->row, &map->R->node);

        /* Update the current y coordinate */
        map->dim.yco = map->R->dim.yco;
        /* Walk the ring in order to get the PANEL of the new ROW
         * that matches the current x coordinate */
        MATCH_FIELD(map->R->plate, map->P, dim.xco, saved_xco);
        map->dim.xco = saved_xco;
}
/*************************************************************************}}}1*/
WORLD *new_world(int type, int h, int w)
{
        WORLD *new = malloc(sizeof(WORLD));      
        int i;

        /* Generate a Perlin map for the world */
        new->pmap = gen_perlin_map(h, w);
        new->dim.h = h;
        new->dim.w = w;

        /* Current coordinate pair */
        new->dim.xco = 0;
        new->dim.yco = 0;

        /* Initialize ring of plate rows */
        __RING(new->row);

        for (i=0; i<h; i++) {
                PLATEROW *newrow = new_plate_row(type, i, w);
                LINCREMENT(new, row, &newrow->node);
        }
        new->R = list_top(new->row, PLATEROW, node);
        new->P = list_top(new->R->plate, PLATE, node);

        /* Methods */
        new->prevr = &prev_row;
        new->nextr = &next_row;
        new->prevp = &prev_plate;
        new->nextp = &next_plate;

        GLOBE = new;

        return new;
}
PLATE *current_plate(void)
{
        return GLOBE->P;
}

/******************************************************************************
 * DESPAIR, THE ROOST OF WRETCHED FUNCS DEPORTED TO THIS PENULT BARE
 * OF DOCUMENTS OR COMMENTS, YEA, OF HOPE, TO WASTE, UNTIL REPAIR'D.
 ******************************************************************************/
int hit_test(PLATE *pl, int y, int x)
{
        GNODE *tmp;
        int i = 0;

        list_for_each(pl->gfx, tmp, node) {
                if ((tmp->dim.layer == __grt__)||(tmp->dim.layer == __drd__)) {
                        if (((x >= tmp->dim.x0)                  && 
                             (x <= (tmp->dim.x0 + tmp->dim.w))   &&
                             (y >= tmp->dim.y0)                  &&
                             (y <= (tmp->dim.y0 + tmp->dim.h))))
                                i++;
                }
        }
        return i;
}

void screen_swap(int dir)
{
        GLOBE->P->hideall(GLOBE->P);

        switch (dir) {
        case NORTH:     GLOBE->nextr(GLOBE);
                        GLOBE->nextr(GLOBE);
                        break;
        case EAST:      GLOBE->prevp(GLOBE);
                        GLOBE->prevp(GLOBE);
                        break;
        case SOUTH:     GLOBE->prevr(GLOBE);
                        GLOBE->prevr(GLOBE);
                        break;
        case WEST:      GLOBE->nextp(GLOBE);
                        GLOBE->nextp(GLOBE);
                        break;
        }
        clear();

        /*if (GLOBE->P->dim.n <= 6)       simpledraw(GLOBE->P);*/
        /*else                            GLOBE->P->showall(GLOBE->P);*/
        restack(GLOBE->P);
        swab_screen();
}

void set_nyb(ZBOX *Z, int y, int x, int n, ...)
{
        if (n == 0) return;
        va_list ap; // accepts a variable number of nibble-state pairs
        int nyb, set; // will hold the next nibble-state pair

        va_start (ap, n); // initialize argument list

        mort(y, x, &Z->z); // prime the pump by converting (y,x) to zcode

        while (n--) {
                nyb = va_arg(ap, int);
                set = va_arg(ap, int);
                _set_nyb(&(Z->box[(Z->z)]), nyb, set);
        }
        va_end(ap); // tidy up
}
