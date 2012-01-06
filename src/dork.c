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
   #include "palette.h"
   #include "ticker.h"
   #include "weather.h"
   #include "deck.h"
/* ========================================================================== */
   timeout_cb (EV_P_ ev_timer *w, int revents)
   {
     puts ("timeout");
     /* this causes the innermost ev_run to stop iterating */
     ev_break(EV_A_ EVBREAK_ONE);
   }

   int main() 
   {
     /* UTF-8 */
     setlocale(LC_ALL,"");

     /* Curses inits */
     initscr();		
     start_color();
     cbreak();			  
     noecho();
     keypad(stdscr, TRUE);
     curs_set(0); /* hide cursor */

     /* My inits */
     init_palette(0);
     init_gfx_colors();
     weather_init();
     ticker_init();
     boat_init();
     deck_init();

     /* Make new graphics */
     LIST_HEAD(mywad);
     set_gfx_bg(&mywad, 0);
     new_terrain(&mywad, 's', 5, 5, 7, 5); 
     new_terrain(&mywad, 's', 10, 20, 20, 30); 

     GFXNODE *boat = new_boat();
     nominate_boat(boat);
     DeckDraw();

     update_panels();
     doupdate();

     struct ev_loop *loop = EV_DEFAULT;
     ev_timer timeout_watcher;

     ev_timer_init (&timeout_watcher, timeout_cb, 5.5, 0.);
     ev_timer_start (loop, &timeout_watcher);

     pthread_t listener;
     pthread_create(&listener, NULL, DoListen, boat);

     pthread_join(&listener, NULL);

     ev_run (loop, 0);

     endwin();			/* end curses mode */
     return 0;
   }
