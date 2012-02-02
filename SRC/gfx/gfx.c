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
/******************************************************************************/
sem_t *REFRESH_LOCK;
/******************************************************************************
 * Starterup...slammin'!
 ******************************************************************************/
void gfx_init(void)
{
        REFRESH_LOCK = (sem_t *)malloc(sizeof(sem_t));
        sem_init(REFRESH_LOCK, 0, 1);
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
/******************************************************************************
 * These are the functions that will be referenced by the function pointers
 * (methods) in the various levels of the graphics stack. Their operation
 * has been described above, and the code should be simple enough to follow.
 ******************************************************************************/
/* PLATE methods
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
/* GNODE methods
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
/* PLATE methods
 ******************************************************************************/
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
/******************************************************************************
 * Allocate memory for and initialize a new PLATE structure. This is the only
 * 'new_' function that takes an argument, an int corresponding to an enum
 * entry (see ./sprites.h) that identifies a particular tile and palette that
 * will be set as the background of the PLATE. Everything else is relatively
 * straightforward, and essentially the same as the previous functions for
 * a new WNODE or GNODE.
 ******************************************************************************/
PLATE *new_plate(int type)
{
        PLATE *new = (PLATE *)malloc(sizeof(PLATE));

        new->id = type;
        new->h  = LINES;
        new->w  = COLS;

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

        /* Allocate and initialize semaphore */
        new->sem = (sem_t *)malloc(sizeof(sem_t));
        sem_init(new->sem, 0, 1);

        GNODE *trimg = new_gnode(__an__, LINES, COLS, 0, 0, 2);
                       wbkgrnd(trimg->W->window, tile);
                       trimg->nextw(trimg);
                       wbkgrnd(trimg->W->window, tile);
                       bottom_panel(trimg->pan);
       /* Create a new background GNODE, set its background cchar_t, and
        * place it at the bottom of the panel stack. */
        GNODE *bg = new_gnode(__bg__, LINES, COLS, 0, 0, 1);
                    wbkgrnd(bg->W->window, tile);
                    bottom_panel(bg->pan);

       /* Add the background GNODE to the GNODE ring, and reference it
        * at *BG, for quick access. */
        list_add(new->gfx, &trimg->node);
        new->trim = trimg;

        list_add(new->gfx, &bg->node);
        new->BG = bg;


        /* Advance the GNODE ring, so that *G points to something. */
        new->nextg(new);

        return new;
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
        mob->dim.dy = 0;
        mob->dim.dx = 0;
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
