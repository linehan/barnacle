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

        
        sail_boat(NULL);

        /*int strength = get_wind_str();*/
        /*w->repeat = (float)(strength*0.10);*/

        if ((sem_trywait(bun->sem) == -1))
                ev_timer_again(EV_DEFAULT, w);
        else
                ev_break(EV_A_ EVBREAK_ALL);
}
void tumbler(EV_P_ ev_timer *w, int revents)
{
        OO_time *bun = container_of(w, OO_time, w);
        mark_wind();
        seek_heading();
        seek_prevailing();
        draw_compass();

        if ((sem_trywait(bun->sem) == -1))
                ev_timer_again(EV_DEFAULT, w);
        else
                ev_break(EV_A_ EVBREAK_ALL);
}
void do_animate(EV_P_ ev_timer *w, int revents)
{
        OO_time *bun = container_of(w, OO_time, w);
        ENV *myenv = (ENV *)bun->data;
        step_all_env(myenv);

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
        ENV *myenv = new_env();
        set_gfx_bg(&myenv->wad, 0);
        new_terrain(&myenv->wad, 's', 5, 5, 7, 5); 
        new_terrain(&myenv->wad, 's', 10, 20, 20, 30); 
        new_terrain(&myenv->wad, 'm', 20, 20, 30, 70); 

        MOB *boat = new_boat(myenv);
        nominate_boat(boat);

        /* master off switch */
        sem_t master_off; 
        sem_init(&master_off, 0, 1);

        set_wind(__pre__, 4);

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

        /* Create the sailboat/wind watcher */
        OO_time compass;
        ev_init(&(compass.w), &tumbler);
        compass.w.repeat = .1;
        ev_timer_again(mainloop, &(compass.w));
        compass.sem = &master_off;

        /* Wave animation watcher */
        OO_time animate_waves;
        ev_init(&(animate_waves.w), &do_animate);
        animate_waves.w.repeat = 2.5;
        animate_waves.data = myenv;
        ev_timer_again(mainloop, &(animate_waves.w));
        animate_waves.sem = &master_off;

        /* Asynchronus I/O */
        pthread_t thread_listen;
        pthread_create(&thread_listen, NULL, asyncio, &listener);

        ev_run (mainloop, 0);

        pthread_join(thread_listen, NULL); 

        endwin(); /* end curses mode */
        return 0;
   }
