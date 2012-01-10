/* ==========================================================================
    FILENAME:  dork.c
    This is the main program. 
   ========================================================================== */
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

   #include "lib/list/list.h"
   #include "lib/gfx.h"

   #include "sprite.h"
   #include "terrain.h"
   #include "control.h"
   #include "boat.h"
   #include "dice.h"
   #include "palette.h"
   #include "weather.h"
   #include "deck.h"
/* ========================================================================== */
   void BlowWind(EV_P_ ev_timer *w, int revents)
   {
     MOB *boat = (MOB *)w->data;
     sail_boat(boat);

     /*int strength = get_wind_str();*/
     /*w->repeat = (float)(strength*0.10);*/

     ev_timer_again(EV_DEFAULT, w);
   }
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   void WindShift(EV_P_ ev_timer *w, int revents)
   {
     DICE *D = (DICE *)w->data;
     wind_roll(D);
     wind_gfx();
     ev_timer_again(EV_DEFAULT, w);
   }
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   int main() 
   {
     /* UTF-8 */
     setlocale(LC_ALL,"");

     /* NCURSES ***************************************/
     initscr();		
     start_color();
     cbreak();			  
     noecho();
     keypad(stdscr, TRUE);
     curs_set(0); /* hide cursor */

     /* MYINITS ***************************************/
     init_palette(0);
     init_gfx_colors();
     init_random();
     weather_init();
     boat_init();
     deck_init();

     /* MAKE NEW GRAPHICS *****************************/
     LIST_HEAD(mywad);
     set_gfx_bg(&mywad, 0);

     new_terrain(&mywad, 's', 5, 5, 7, 5); 
     new_terrain(&mywad, 's', 10, 20, 20, 30); 

     MOB *boat = new_boat();
     nominate_boat(boat);
     DeckDraw();

     set_prevailing(NORTH, 0.9);

     DICE *mine = new_dice("1d4", 0.25, 0.25, 0.25);

     update_panels();
     doupdate();

     /* PREPARE EVENT LOOPS  **************************/
     struct ev_loop *mainloop = EV_DEFAULT;

     ev_timer sail_w;
     ev_init (&sail_w, &BlowWind);
     sail_w.data = boat;
     sail_w.repeat = .1;
     ev_timer_again(mainloop, &sail_w);

     ev_timer wind_w;
     ev_init (&wind_w, &WindShift);
     wind_w.data = mine;
     wind_w.repeat = .1;
     ev_timer_again(mainloop, &wind_w);

     /* THREADS ***************************************/
     pthread_t listener;
     pthread_create(&listener, NULL, DoListen, boat);

     ev_run (mainloop, 0);

     pthread_join(listener, NULL);

     endwin();			/* end curses mode */
     return 0;
   }
