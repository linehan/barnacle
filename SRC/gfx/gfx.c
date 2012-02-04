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
#include <ncurses.h>
#include <panel.h>
#include <pthread.h>
#include <semaphore.h>

#include "../lib/llist/list.h"
#include "../gen/perlin.h"
#include "gfx.h"
#include "palette.h"
#include "sprite.h"
#include "../cmd/control.h"
#include "../geo/terrain.h"

#define WIPE togpan(WIPEPAN); togpan(WIPEPAN)

WORLD *GLOBE;

WINDOW *WIPEWIN;
PANEL  *WIPEPAN;
/******************************************************************************/
sem_t *REFRESH_LOCK;
/******************************************************************************
 * Starterup...slammin'!
 ******************************************************************************/
void gfx_init(void)
{
        REFRESH_LOCK = (sem_t *)malloc(sizeof(sem_t));
        sem_init(REFRESH_LOCK, 0, 1);
        WIPEWIN = newwin(LINES, COLS, 0, 0);
        WIPEPAN = new_panel(WIPEWIN);
        bottom_panel(WIPEPAN);
        hide_panel(WIPEPAN);
}
/******************************************************************************
 * It's best to call vrt_refresh() until everything you needed to
 * fiddle with or change on the virtual screen has been written, and only
 * then call scr_refresh(), so you minimize flicker and disturbance of the
 * real screen.
 ******************************************************************************/
void scr_refresh(void)
{
        update_panels();
        doupdate();
}
void vrt_refresh(void)
{
        update_panels();
}
void togpan(PANEL *pan)
{
        if (panel_hidden(pan))                 
                show_panel(pan);
        else 
                hide_panel(pan);
}
/* GNODE methods *********************************************************{{{1*/
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
/*************************************************************************}}}1*/
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
        gfx->dim.y  = y0+h;
        gfx->dim.x  = x0+w;
        gfx->dim.dy = 0;
        gfx->dim.dx = 0;
        gfx->dim.n  = n;

        return gfx;
}
/* PLATE methods *********************************************************{{{1*/
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
int step_all_forward(const void *plate)
{
        PLATE *pl = (PLATE *)plate;
        GNODE *tmp;

        list_for_each(pl->gfx, tmp, node) {
                        tmp->stepf(tmp);
        }
        return 1;
}
int show_all_gnodes(const void *plate)
{
        PLATE *pl = (PLATE *)plate;
        GNODE *tmp;

        show_panel(pl->trim->pan);
        list_for_each(pl->gfx, tmp, node) {
                if (tmp->id == __drp__) {
                        show_panel(tmp->pan);
                        vrt_refresh();
                }
        }
        list_for_each(pl->gfx, tmp, node) {
                if (tmp->id == __top__) {
                        show_panel(tmp->pan);
                        vrt_refresh();
                }
        }
        /*unfuck_perspective(pl);*/
        return 1;
}
int hide_all_gnodes(const void *plate)
{
        PLATE *pl = (PLATE *)plate;
        GNODE *tmp;

        hide_panel(pl->trim->pan);
        list_for_each(pl->gfx, tmp, node) {
                if (tmp->id == __drp__) {
                        hide_panel(tmp->pan);
                        vrt_refresh();
                }
        }
        list_for_each(pl->gfx, tmp, node) {
                if (tmp->id == __top__) {
                        hide_panel(tmp->pan);
                        vrt_refresh();
                }
        }
        return 1;
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
PLATE *new_plate(int type, int yco, int xco)
{
        PLATE *new = (PLATE *)malloc(sizeof(PLATE));

        new->id = type;
        new->h  = LINES;
        new->w  = COLS;
        new->xco = xco;
        new->yco = yco;
        new->showall = &show_all_gnodes;
        new->hideall = &hide_all_gnodes;

        cchar_t *tile = bg_tile(type);

        /* Populate the PERLIN package */
        new->noise = get_perlin_map(LINES, COLS);

        /* Methods */
        new->nextg = &next_gnode;
        new->prevg = &prev_gnode;
        new->stepf = &step_all_forward;

        /* Allocate and initialize GNODE ring */
        new->gfx = (struct list_head *)malloc(sizeof(struct list_head));
        list_head_init(new->gfx);

        /* Allocate and initialize highlight ring */
        new->hi = (struct list_head *)malloc(sizeof(struct list_head));
        list_head_init(new->hi);



        /* Allocate and initialize semaphore */
        new->sem = (sem_t *)malloc(sizeof(sem_t));
        sem_init(new->sem, 0, 1);

        /* Create a new GNODE to hold the edge animation of fg objects */
        GNODE *trimg = new_gnode(__an__, LINES, COLS, 0, 0, 2);
                       wbkgrnd(trimg->W->window, tile);
                       trimg->nextw(trimg);
                       wbkgrnd(trimg->W->window, tile);

        list_add(new->gfx, &trimg->node);
        new->trim = trimg;

        highlights_init(new);

       /* Create a new background GNODE, set its background cchar_t, and
        * place it at the bottom of the panel stack. */
        GNODE *bg = new_gnode(__bg__, LINES, COLS, 0, 0, 1);
                    wbkgrnd(bg->W->window, tile);
                    bottom_panel(bg->pan);

        list_add(new->gfx, &bg->node);
        new->BG = bg;

        new->setme = 0;

        return new;
}
/* MAP methods ***********************************************************{{{1*/
/******************************************************************************
 * Cycle to the next PLATE in a PLATEROW
 ******************************************************************************/
int next_plate(const void *world)
{
        WORLD *map = (WORLD *)world;

        map->P = list_top(map->R->plate, PLATE, node);
                 list_del_from(map->R->plate, &map->P->node);
                 list_add_tail(map->R->plate, &map->P->node);

        /* Update the x coordinate */
        map->xco = map->P->xco;
                
        return 1;
}
/******************************************************************************
 * Cycle to the previous PLATE in a PLATEROW
 ******************************************************************************/
int prev_plate(const void *world)
{
        WORLD *map = (WORLD *)world;

        map->P = list_tail(map->R->plate, PLATE, node);
                 list_del_from(map->R->plate, &map->P->node);
                 list_add(map->R->plate, &map->P->node);

        /* Update the x coordinate */
        map->xco = map->P->xco;

        return 1;
}
/******************************************************************************
 * Step to the next PLATEROW
 ******************************************************************************/
int next_row(const void *world)
{
        WORLD *map = (WORLD *)world;

        int saved_xco = map->xco;

        map->R = list_top(map->row, PLATEROW, node);
                 list_del_from(map->row, &map->R->node);
                 list_add_tail(map->row, &map->R->node);

        /* Update the current y coordinate */
        map->yco = map->R->yco;
        /* Walk the ring in order to get the PANEL of the new ROW
         * that matches the current x coordinate */
        list_for_each(map->R->plate, map->P, node) {
                if (map->P->xco == saved_xco) {
                        map->xco = saved_xco;
                        return 1;
                }
        }
        return 0;
}
/******************************************************************************
 * Step to the previous PLATEROW
 ******************************************************************************/
int prev_row(const void *world)
{
        WORLD *map = (WORLD *)world;

        int saved_xco = map->xco;

        map->R = list_tail(map->row, PLATEROW, node);
                 list_del_from(map->row, &map->R->node);
                 list_add(map->row, &map->R->node);

        /* Update the current y coordinate */
        map->yco = map->R->yco;
        /* Walk the ring in order to get the PANEL of the new ROW
         * that matches the current x coordinate */
        list_for_each(map->R->plate, map->P, node) {
                if (map->P->xco == saved_xco) {
                        map->xco = saved_xco;
                        return 1;
                }
        }
        return 0;
}
/*************************************************************************}}}1*/
/******************************************************************************/
PLATEROW *new_plate_row(int type, int yco, int w)
{
        int i;
        PLATEROW *new = malloc(sizeof(PLATEROW));

        new->w = w;
        new->yco = yco;

        new->plate = (struct list_head *)malloc(sizeof(struct list_head));
        list_head_init(new->plate);

        for (i=0; i<w; i++) {
                PLATE *newpl = new_plate(type, yco, i);
                list_add(new->plate, &newpl->node);
        }

        return new;
}
/******************************************************************************
 * Make a new world map
 ******************************************************************************/
WORLD *new_world(int type, int h, int w)
{
        WORLD *new = malloc(sizeof(WORLD));      
        int i, j;

        /* Generate a Perlin map for the world */
        new->pmap = get_perlin_map(h, w);
        new->h = h;
        new->w = w;

        /* Current coordinate pair */
        new->xco = 0;
        new->yco = 0;

        /* Initialize ring of plate rows */
        new->row = (struct list_head *)malloc(sizeof(struct list_head));
        list_head_init(new->row);
        for (i=0; i<h; i++) {
                PLATEROW *newrow = new_plate_row(type, i, w);
                list_add(new->row, &newrow->node);
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
        mob->dim.dy = y0;
        mob->dim.dx = x0;
        mob->dim.n = 0;

        mob->sem = malloc(sizeof(sem_t));
        sem_init(mob->sem, 0, 1);

        return mob;
}
int hit_test(PLATE *pl, int y, int x)
{
        GNODE *tmp;
        int i = 0;

        sem_wait(pl->sem);
                list_for_each(pl->gfx, tmp, node) {
                        if ((tmp->id != __bg__)&&(tmp->id != __an__)) {
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

void screen_swap(int dir)
{
        GLOBE->P->hideall(GLOBE->P);

        switch (dir) {
        case NORTH:
                GLOBE->nextr(GLOBE);
                GLOBE->nextr(GLOBE);
                break;
        case EAST:
                GLOBE->prevp(GLOBE);
                GLOBE->prevp(GLOBE);
                break;
        case SOUTH:
                GLOBE->prevr(GLOBE);
                GLOBE->prevr(GLOBE);
                break;
        case WEST:
                GLOBE->nextp(GLOBE);
                GLOBE->nextp(GLOBE);
                break;
        }
        clear();

        if (GLOBE->P->setme == 0)     
                simpledraw(GLOBE->P);
        else                            
                GLOBE->P->showall(GLOBE->P);

        WIPE;
}

void move_mob(MOB *mob, int dir)
{
        sem_wait(mob->sem);

                int y = mob->dim.dy;
                int x = mob->dim.dx;

                switch (dir) {
                case 'u':
                        if (((y>0)) && ((hit_test(GLOBE->P, --y, x) == 0)))
                                mob->dim.dy--;
                        break;
                case 'd':
                        if ((y<LINES) && ((hit_test(GLOBE->P, ++y, x) == 0)))
                                mob->dim.dy++;
                        break;
                case 'l':
                        if (((x>0)) && ((hit_test(GLOBE->P, y, --x) == 0)))
                                mob->dim.dx--;
                        break;
                case 'r':
                        if (((x<COLS)) && ((hit_test(GLOBE->P, y, ++x) == 0)))
                                mob->dim.dx++;
                        break;
                }
                if ((mob->dim.dy == LINES)) {
                        screen_swap(SOUTH);
                        mob->dim.dy = 2;
                }
                else if ((mob->dim.dx == 0)) {
                        screen_swap(WEST);
                        mob->dim.dx = COLS-2;
                }
                else if ((mob->dim.dy == 0)) {
                        screen_swap(NORTH);
                        mob->dim.dy = LINES-2;
                }
                else if ((mob->dim.dx == COLS)) {
                        screen_swap(EAST);
                        mob->dim.dx = 2;
                }
                move_panel(mob->pan, mob->dim.dy, mob->dim.dx);

        sem_post(mob->sem);
}
