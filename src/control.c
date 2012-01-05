#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

#include "lib/list/list.h"
#include "lib/gfx.h"

#include "boat.h"
#include "palette.h"
#include "weather.h"
#include "deck.h"

PANEL *deckp;

enum cardinal { NORTH = 0, SOUTH = 1, EAST = 2, WEST = 3 };
/*----------------------------------------------------------------------------*/
void *DoListen(void *ptr)
{
  int ch, hdg;

  WINDOW *compass = newwin(1, 1, 0, 0);
  PANEL *compass_p = new_panel(compass);

	while((ch = getch()) != 'q') { 

     switch (ch) { 

       /* boat heading */
       case 'h':
         BoatOrder('h', WEST); 
         break;
       case 'j':
         BoatOrder('h', SOUTH); 
         break;
       case 'k':
         BoatOrder('h', NORTH); 
         break;
       case 'l':
         BoatOrder('h', EAST); 
         break;

       /* sails */
       case 'g':
         BoatOrder('r', 0); 
         break;

       /* toggle panel */
       case 'i':
         DeckTog();
         break;

       /* wind dir */
       case 'n':
         wind_set(NORTH);
         wind_gfx(compass_p);
         break;
       case 's':
         wind_set(SOUTH);
         wind_gfx(compass_p);
         break;
       case 'e':
         wind_set(EAST);
         wind_gfx(compass_p);
         break;
       case 'w':
         wind_set(WEST);
         wind_gfx(compass_p);
         break;
      }
      update_panels();
      doupdate();
  }
  return NULL;
}
/*----------------------------------------------------------------------------*/

