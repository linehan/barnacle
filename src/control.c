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

enum cardinal { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };
/*----------------------------------------------------------------------------*/
void *DoListen(void *ptr)
{
  int ch, hdg;



	while((ch = getch()) != 'q') { 

     switch (ch) { 

       /* boat heading */
       case 'h':
         order_boat('h', WEST); 
         break;
       case 'j':
         order_boat('h', SOUTH); 
         break;
       case 'k':
         order_boat('h', NORTH); 
         break;
       case 'l':
         order_boat('h', EAST); 
         break;

       /* sails */
       case 'g':
         order_boat('r', 0); 
         break;

       /* toggle panel */
       case 'i':
         DeckTog();
         break;
     }

       /* wind dir */
       /*case 'n':*/
         /*wind_set(NORTH);*/
         /*break;*/
       /*case 's':*/
         /*wind_set(SOUTH);*/
         /*break;*/
       /*case 'e':*/
         /*wind_set(EAST);*/
         /*break;*/
       /*case 'w':*/
         /*wind_set(WEST);*/
         /*break;*/
      /*}*/
      /*update_panels();*/
      /*doupdate();*/
  }
  return NULL;
}
/*----------------------------------------------------------------------------*/

