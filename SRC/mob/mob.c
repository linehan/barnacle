// vim:fdm=marker
/******************************************************************************
 * FILENAME:  mob.c
 * This module contains routines for initializing and manipulating MOB (i.e.
 * "mobile") graphical objects. These are objects such as player characters,
 * NPCs, vehicles, even certain environmental effects.
 ******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <pthread.h>
#include <semaphore.h>
<<<<<<< .merge_file_IrycLx
#include <stdint.h>

#include "../lib/llist/list.h"

=======

#include "../lib/llist/list.h"
>>>>>>> .merge_file_DQsM6w
#include "../gen/perlin.h"
#include "../gfx/gfx.h"
#include "mob.h"
#include "../gfx/palette.h"
#include "../gfx/sprite.h"
#include "../cmd/control.h"
#include "../geo/terrain.h"
<<<<<<< .merge_file_IrycLx
#include "../lib/ufo.h"
#include "../geo/map.h"


/* 
  Create and initialize a new mob_t of kind 'k', registering it as an itm_t
  with register_itm(). Return the uint32_t key for the mob_t's retreival.
*/
struct mob_t *new_mob(struct map_t *map, int h, int w, int y0, int x0)
{
        struct mob_t *mob = malloc(sizeof(*mob));

        mob->ufo = new_ufo(h, w, y0, x0, map->h, map->w, 0, 0);
        sem_init(&mob->sem, 0, 1);

        return (mob);
}

void move_mob(struct mob_t *mob, int dir)
{
        sem_wait(&mob->sem);

        int y = ufo_y(mob->ufo);
        int x = ufo_x(mob->ufo);

        switch (dir) {
        case 'u':       if (y>0 && (hit_test(GLOBE, --y, x)==1))
                                ufo_u(mob->ufo);
                        break;
        case 'd':       if (y<LINES && (hit_test(GLOBE, ++y, x)==1))
                                ufo_d(mob->ufo);
                        break;
        case 'l':       if (x>0 && (hit_test(GLOBE, y, --x)==1))
                                ufo_l(mob->ufo);
                        break;
        case 'r':       if (x<COLS && (hit_test(GLOBE, y, ++x)==1))
                                ufo_r(mob->ufo);
                        break;
        }
        // Scroll down at bottom boundary
        if (ufo_y(mob->ufo) == LINES-10) {
                roll_map(GLOBE, 'd');
                ufo_u(mob->ufo);
        }
        // Scroll right at right boundary
        if (ufo_x(mob->ufo) == COLS-10) {
                roll_map(GLOBE, 'r');
                ufo_l(mob->ufo);
        }
        move_panel(mob->pan, ufo_y(mob->ufo), ufo_x(mob->ufo));
        /*top_panel(mob->pan);*/
        scr_refresh();


        sem_post(&mob->sem);
=======

MOB *new_mob(void *ptr, int h, int w, int y0, int x0)
{
        MOB *mob = (MOB *)malloc(sizeof(MOB));
        mob->obj = ptr;

        WINDOW *win = newwin(h, w, y0, x0);
        mob->pan = new_panel(win);

        mob->dim.h = h;
        mob->dim.w = w;
        mob->dim.y0 = y0;
        mob->dim.x0 = x0;
        mob->dim.yco = y0;
        mob->dim.xco = x0;
        mob->dim.n = 0;

        mob->sem = malloc(sizeof(sem_t));
        sem_init(mob->sem, 0, 1);

        return mob;
}

void move_mob(MOB *mob, int dir)
{
        /*sem_wait(mob->sem);*/

        /*int y = mob->dim.yco;*/
        /*int x = mob->dim.xco;*/

        /*switch (dir) {*/
        /*case 'u':       if (((y>0)) && ((hit_test(GLOBE->P, --y, x) == 0)))*/
                                /*mob->dim.yco--;*/
                        /*break;*/
        /*case 'd':       if ((y<LINES) && ((hit_test(GLOBE->P, ++y, x) == 0)))*/
                                /*mob->dim.yco++;*/
                        /*break;*/
        /*case 'l':       if (((x>0)) && ((hit_test(GLOBE->P, y, --x) == 0)))*/
                                /*mob->dim.xco--;*/
                        /*break;*/
        /*case 'r':       if (((x<COLS)) && ((hit_test(GLOBE->P, y, ++x) == 0)))*/
                                /*mob->dim.xco++;*/
                        /*break;*/
        /*}*/
        /*[>if ((mob->dim.yco == LINES)) {<]*/
                /*[>screen_swap(SOUTH);<]*/
                /*[>mob->dim.yco = 2;<]*/
        /*[>} <]*/
        /*[>else if ((mob->dim.xco == 0)) {<]*/
                /*[>screen_swap(WEST);<]*/
                /*[>mob->dim.xco = COLS-2;<]*/
        /*[>} <]*/
        /*[>else if ((mob->dim.yco == 0)) {<]*/
                /*[>screen_swap(NORTH);<]*/
                /*[>mob->dim.yco = LINES-2;<]*/
        /*[>} <]*/
        /*[>else if ((mob->dim.xco == COLS)) {<]*/
                /*[>screen_swap(EAST);<]*/
                /*[>mob->dim.xco = 2;<]*/
        /*[>}<]*/
        /*move_panel(mob->pan, mob->dim.yco, mob->dim.xco);*/
        /*top_panel(mob->pan);*/
        /*scr_refresh();*/

        /*sem_post(mob->sem);*/
>>>>>>> .merge_file_DQsM6w
}
