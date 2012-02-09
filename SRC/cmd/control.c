// vim:fdm=marker
/*******************************************************************************
FILENAME:  control.c
Bindings and actions for the user's keyboard input.
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <ev.h>
#include <pthread.h>
#include <semaphore.h>

#include "lib/llist/list.h"
#include "../lib/memmacro.h"

#include "../pan/test.h"
#include "../typedefs.h"

#include "../gfx/gfx.h"
#include "../gfx/palette.h"
#include "../geo/weather.h"
#include "../pan/instruments.h"
#include "../mob/mob.h"
#include "../mob/boat.h"
#include "../pan/menus.h"
/******************************************************************************/
PANEL *deckp;

enum cardinal { 
        NORTH = 0,
        NNE   = 1,
        NE    = 2,
        ENE   = 3,
        EAST  = 4,
        ESE   = 5,
        SE    = 6,
        SSE   = 7,
        SOUTH = 8,
        SSW   = 9,
        SW    = 10,
        WSW   = 11,
        WEST  = 12,
        WNW   = 13,
        NW    = 14,
        NNW   = 15 
};
/******************************************************************************/
void *iolisten(EV_P_ ev_io *w, int revents)
{
        ev_io_stop (EV_A, w);

        int ch, hdg;

        while((ch = getch())) { 
                switch (ch) { 
                case '0':
                        set_wind(__pre__, 0);
                        break;
                case '1':
                        set_wind(__pre__, 2);
                        break;
                case '2':
                        set_wind(__pre__, 4);
                        break;
                case '3':
                        set_wind(__pre__, 6);
                        break;
                case '4':
                        set_wind(__pre__, 8);
                        break;
                case '5':
                        set_wind(__pre__, 10);
                        break;
                case '6':
                        set_wind(__pre__, 12);
                        break;
                case '7':
                        set_wind(__pre__, 14);
                        break;
                case '8':
                        set_wind(__pre__, 15);
                        break;
                case 'j':
                        order_boat('p', 0);
                        break;
                case 'k':
                        order_boat('s', 0);
                        break;
                case ' ':
                        order_boat('x', 0);
                        break;
                case 'i':
                        toggle_instrument_panel();
                        break;
                /*case 'k':*/
                        /*order_boat('h', NORTH);*/
                        /*break;*/
                /*case 'l':*/
                        /*order_boat('h', EAST);*/
                        /*break;*/
                case 'g':
                        order_boat('r', 0);
                        break;
                case 'd':
                        toggle_bigpan();
                        break;
                case '`':
                        toggle_mm();
                        break;
                case 'q':
                        ev_break(EV_A_ EVBREAK_ALL);
                        return NULL;
                }
        }
        return NULL;
}
/* Initialize and run the asynchronus io loop. Cast the pointer to the OO_io
 * bundle and (now that the callback iolisten is in scope) initialize the
 * watcher at bun->w. Start the watcher and connect it to the loop at
 * bun->loop, then run bun->loop. Do all of this behind a semaphore which is
 * checked by the main loop to determine whether the program needs to be
 * shut down. Only the key 'q' in this input thread will initiate a full
 * unloop.*/
void *asyncio(void *ptr)
{
        OO_io *bun = (OO_io *)ptr;

        sem_wait(bun->sem);
                ev_io_init(&(bun->w), &iolisten, /*STDIN FILENO*/0, EV_READ);
                ev_io_start(bun->loop, &(bun->w));
                ev_run(bun->loop, 0);
        sem_post(bun->sem);

        return NULL;
}

