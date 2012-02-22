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

#include "lib/llist/list.h"
#include "lib/memmacro.h"

#include "pan/menus.h"
#include "typedefs.h"
#include "txt/psh.h"
#include "cmd/control.h"
#include "gen/dice.h"
#include "gfx/gfx.h"
#include "gen/perlin.h"
#include "gfx/palette.h"
#include "gfx/sprite.h"
#include "geo/map.h"
#include "geo/terrain.h"
#include "geo/weather.h"
#include "mob/mob.h"
#include "mob/boat.h"
#include "pan/instruments.h"
#include "pan/test.h"
#include "lib/ufo.h"
#include "lib/morton.h"
/******************************************************************************/
/* The callback function for the sailing event watcher. Manages the wind
 * and the sailboat's response to it. Checks a semaphore which is also
 * in the scope of the i/o thread. When that thread exits, the semaphore
 * is posted, and this event loop is broken as well, ending the program. */
void blow_wind(EV_P_ ev_timer *w, int revents)
{
        OO_time *bun = container_of(w, OO_time, w);
        
        sail_boat(NULL);

        if ((sem_trywait(bun->sem) == -1)) ev_timer_again(EV_DEFAULT, w);
        else                               ev_break(EV_A_ EVBREAK_ALL);
}
/******************************************************************************/
void tumbler(EV_P_ ev_timer *w, int revents)
{
        OO_time *bun = container_of(w, OO_time, w);

        mark_wind();
        seek_heading();
        seek_prevailing();
        draw_compass();
        /*render_clouds(GLOBE->P);*/
        /*shift_highlights(GLOBE->P);*/

        if ((sem_trywait(bun->sem) == -1)) ev_timer_again(EV_DEFAULT, w);
        else                               ev_break(EV_A_ EVBREAK_ALL);
}
/******************************************************************************/
void do_animate(EV_P_ ev_timer *w, int revents)
{
        OO_time *bun = container_of(w, OO_time, w);

        /*GLOBE->P->L[RIM]->step(GLOBE->P->L[RIM]);*/
        /*combine(GLOBE->P);*/

        if ((sem_trywait(bun->sem) == -1)) ev_timer_again(EV_DEFAULT, w);
        else                               ev_break(EV_A_ EVBREAK_ALL);
}
/******************************************************************************/

/* Day-toh-nah */
int main() 
{
        setlocale(LC_ALL,""); // UTF-8

        initscr();            // Start ncurses
        start_color();        // Initialize color palette
        cbreak();	      // Disable line buffering
        noecho();             // Do not echo input
        keypad(stdscr, TRUE); // Enable special keys
        curs_set(0);          // hide cursor

        geojug_start();       // Start the graphics engine
        terminal_check();     // No xterm allowed


        MAP *pad = gen_map((LINES*3), (COLS*3));
        GLOBE = pad;
        roll(pad, 0);
        roll(pad, 0);

        wprintw(DIAGNOSTIC_WIN, "%u", (LINES*COLS)*3);

        /*draw_water_rim(map->P);*/

        MOB *boat = new_boat();
        nominate_boat(boat);

        /* master off switch */
        sem_t master_off; 
        sem_init(&master_off, 0, 1);

        set_wind(__pre__, 4);

        /*swab_screen();*/

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
        ev_timer_again(mainloop, &(animate_waves.w));
        animate_waves.sem = &master_off;

        /* Prompt user input */
        /*OO_io prompt;*/
        /*ev_io_init (&prompt.w, &cmd_prompt, STDIN_FILENO, EV_READ);*/
        /*prompt.loop = mainloop;*/
        /*prompt.sem = &master_off;*/

        /* Asynchronus I/O */
        pthread_t thread_listen;
        pthread_create(&thread_listen, NULL, asyncio, &listener);

        ev_run (mainloop, 0);

        pthread_join(thread_listen, NULL); 

        endwin(); /* end curses mode */
        return 0;
   }
