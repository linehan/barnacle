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
     BoatInit();
     DeckInit();

     /* Make new graphics */
     LIST_HEAD(mywad);

     set_gfx_bg(&mywad, 0);

     new_terrain(&mywad, 's', 5, 5, 7, 5); 
     new_terrain(&mywad, 's', 10, 20, 20, 30); 

     update_panels();
     doupdate();
/*
     GFX *boat = BoatMake();
     DeckDraw();
     */

     napms(1500);
     endwin();			/* end curses mode */
     return 0;
   }
