// vim:fdm=marker
/*******************************************************************************
  FILENAME:  dork.c
  This is the main program. 
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>
#include <ev.h>
#include <math.h>

#include "lib/memmacro.h"
#include "lib/list/list.h"
#include "lib/gfx.h"

#include "typedefs.h"
#include "sprite.h"
#include "terrain.h"
#include "control.h"
#include "boat.h"
#include "dice.h"
#include "palette.h"
#include "weather.h"
#include "instruments.h"
#include "test.h"
/******************************************************************************/
/* The callback function for the sailing event watcher. Manages the wind
 * and the sailboat's response to it. Checks a semaphore which is also
 * in the scope of the i/o thread. When that thread exits, the semaphore
 * is posted, and this event loop is broken as well, ending the program. */
void blow_wind(EV_P_ ev_timer *w, int revents)
{
        OO_time *bun = container_of(w, OO_time, w);
        seek_heading();
        mark_wind();
        seek_prevailing();
        draw_compass();

        sail_boat(NULL);

        /*int strength = get_wind_str();*/
        /*w->repeat = (float)(strength*0.10);*/

        if ((sem_trywait(bun->sem) == -1))
                ev_timer_again(EV_DEFAULT, w);
        else
                ev_break(EV_A_ EVBREAK_ALL);
}
/* Gentlemen. */
int main() 
{
        /* UTF-8 */
        setlocale(LC_ALL,"");

        /* ncurses */
        initscr();		
        start_color();
        cbreak();			  
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0); /* hide cursor */

        /* my inits */
        init_palette(0);
        init_gfx_colors();
        init_random();
        gfx_init();
        weather_init();
        boat_init();
        test_init();
        instrument_init();

        /* Graphics */
        LIST_HEAD(mywad);
        set_gfx_bg(&mywad, 0);
        new_terrain(&mywad, 's', 5, 5, 7, 5); 
        new_terrain(&mywad, 's', 10, 20, 20, 30); 

        MOB *boat = new_boat();
        nominate_boat(boat);

        /* master off switch */
        sem_t master_off; 
        sem_init(&master_off, 0, 1);

        set_wind("pre", 4);

        master_refresh();

        /* Main event loop */
        struct ev_loop *mainloop = EV_DEFAULT;

        /* Create the user input watcher */
        OO_io listener;
        listener.loop = ev_loop_new(0);
        listener.sem = &master_off;

        /* Create the sailboat/wind watcher */
        OO_time sailor;
        ev_init(&(sailor.w), &blow_wind);
        sailor.w.repeat = .1;
        ev_timer_again(mainloop, &(sailor.w));
        sailor.sem = &master_off;

        /* Asynchronus I/O */
        pthread_t thread_listen;
        pthread_create(&thread_listen, NULL, asyncio, &listener);

        ev_run (mainloop, 0);

        pthread_join(thread_listen, NULL); 

        endwin(); /* end curses mode */
        return 0;
   }
