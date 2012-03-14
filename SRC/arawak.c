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

#include "pan/dialog.h"
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
#include "gen/wave.h"
#include "env/deck.h"
#include "pan/menus.h"
#include "gen/name/name.h"
#include "pan/crewgen.h"
#include "pan/ctrlpanels.h"
#include "pan/new_character.h"
#include "guy/guy_model.h"
#include "guy/guy_control.h"



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

        wrap_wave('r');
        mark_wind();
        seek_heading();
        seek_prevailing();
        draw_compass();
        view_dock();
        do_pulse();

        if ((sem_trywait(bun->sem) == -1)) ev_timer_again(EV_DEFAULT, w);
        else                               ev_break(EV_A_ EVBREAK_ALL);
}
/******************************************************************************/
void do_animate(EV_P_ ev_timer *w, int revents)
{
        OO_time *bun = container_of(w, OO_time, w);

        NEXT(GLOBE->L[RIM]);
        restack_map(GLOBE);
        map_refresh(GLOBE);

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

        init_deck();
        draw_deck();

        init_ctrlpanels();

        struct map_t *pad = new_map((LINES*3), (COLS*3));
        gen_map(pad);
        GLOBE = pad;

        roll_map(pad, 0);
        roll_map(pad, 0);

        wprintw(DIAGNOSTIC_WIN, "%u\n", (LINES*COLS)*3);
        wprintw(DIAGNOSTIC_WIN, "%i\n", init_surnames());

        uint32_t boatkey = new_boat(pad);
        nominate_boat(boatkey);

        load_guy_test();

        /* master off switch */
        sem_t master_off; 
        sem_init(&master_off, 0, 1);

        set_wind(__pre__, 4);

        /* Main event loop */
        struct ev_loop *mainloop = EV_DEFAULT;

        TOGPAN(DECKP);

        /* Create the user input watcher */
        OO_io listener;
        listener.loop = ev_loop_new(0);
        listener.sem = &master_off;

        /* Create the sailboat/wind watcher */
        OO_time sailor;
        ev_init(&(sailor.w), &blow_wind);
        /*sailor.w.repeat = .1;*/
        sailor.w.repeat = .12;
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
