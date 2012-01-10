/* ==========================================================================
    FILENAME:  boat.c
    Defines a BOAT datatype, containing graphics and options related to the
    display and navigation of a BOAT.
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

   #include "lib/list/list.h"
   #include "lib/gfx.h"

   #include "sprite.h"
   #include "terrain.h"
   #include "palette.h"
   #include "weather.h"
   #include "control.h"
   #include "deck.h"
/* ========================================================================== */
   typedef struct orders_t {
     int hdg;
     int rig;
     int anc;
   } ORDERS;

   typedef struct boat_t {
     cchar_t *aft;
     cchar_t *mid;
     cchar_t *bow;
     cchar_t *boom[2];
     cchar_t *sail[6];
     cchar_t *pole[2];
     cchar_t *mast[4][4][2]; /* mast state */
     ORDERS  now;
     ORDERS *ord;
   } BOAT;

   static ORDERS  _ORDERS;
   MOB *_BOAT;

   sem_t  *_IO;
   sem_t  *_GO;
/* ========================================================================== */
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   void boat_init(void)
   {
     _ORDERS.hdg = 0;
     _ORDERS.rig = 0;
     _ORDERS.anc = 0;

     _IO = malloc(sizeof(sem_t));
     _GO = malloc(sizeof(sem_t));
     sem_init(_IO, 0, 1);
     sem_init(_GO, 0, 1);
   }
/* --------------------------------------------------------------------------
   Set a boat as active
   -------------------------------------------------------------------------- */
   void nominate_boat(MOB *mob)
   {
     _BOAT = mob;
   }
/* --------------------------------------------------------------------------
   Create a new boat MOB 
   -------------------------------------------------------------------------- */
   MOB *new_boat(void)
   {
     BOAT *boat = malloc(sizeof(BOAT));

     /* Connect graphics to the data structure */
     boat->aft = &BOAT_HULL_AFT; 
     boat->mid = &BOAT_HULL_MID;
     boat->bow = &BOAT_HULL_BOW;

     boat->boom[0] = &BOAT_BOOM_L; 
     boat->boom[1] = &BOAT_BOOM_R;

     boat->sail[0] = &SAIL_L_CALM; /* ⎢ */
     boat->sail[1] = &SAIL_R_CALM; /* ⎟ */
     boat->sail[2] = &SAIL_L_HAUL; /* ⎝ */
     boat->sail[3] = &SAIL_R_HAUL; /* ⎠ */
     boat->sail[4] = &SAIL_L_RUN;  /* ⎛ */
     boat->sail[5] = &SAIL_R_RUN;  /* ⎞ */

     boat->pole[0] = &BOAT_POLE_L;
     boat->pole[1] = &BOAT_POLE_R;
     /*
     the mast object is used to retreive the proper character
     for the representation of the mast, based on the current
     wind direction, heading, and sail status.
     */
     /* wind from north ^ */
     boat->mast[NORTH][NORTH][0] = boat->pole[0]; /* north */
     boat->mast[NORTH][NORTH][1] = boat->sail[0];  
     boat->mast[NORTH][SOUTH][0] = boat->boom[0]; /* south */
     boat->mast[NORTH][SOUTH][1] = boat->sail[4]; 
      boat->mast[NORTH][EAST][0] = boat->boom[0]; /* east  */
      boat->mast[NORTH][EAST][1] = boat->sail[3]; 
      boat->mast[NORTH][WEST][0] = boat->boom[1]; /* west  */
      boat->mast[NORTH][WEST][1] = boat->sail[2]; 
     /* wind from south v */
     boat->mast[SOUTH][NORTH][0] = boat->boom[1]; /* north */
     boat->mast[SOUTH][NORTH][1] = boat->sail[5]; 
     boat->mast[SOUTH][SOUTH][0] = boat->pole[1]; /* south */
     boat->mast[SOUTH][SOUTH][1] = boat->sail[0]; 
      boat->mast[SOUTH][EAST][0] = boat->boom[0]; /* east  */
      boat->mast[SOUTH][EAST][1] = boat->sail[3]; 
      boat->mast[SOUTH][WEST][0] = boat->boom[1]; /* west  */
      boat->mast[SOUTH][WEST][1] = boat->sail[2]; 
     /* wind from east <-- */
     boat->mast[EAST][NORTH][0] = boat->boom[0]; /* north */
     boat->mast[EAST][NORTH][1] = boat->sail[2]; 
     boat->mast[EAST][SOUTH][0] = boat->boom[0]; /* south */
     boat->mast[EAST][SOUTH][1] = boat->sail[2]; 
      boat->mast[EAST][EAST][0] = boat->pole[1]; /* east  */
      boat->mast[EAST][EAST][1] = boat->sail[1]; 
      boat->mast[EAST][WEST][0] = boat->boom[1]; /* west  */
      boat->mast[EAST][WEST][1] = boat->sail[2];
     /* wind from west --> */ 
     boat->mast[WEST][NORTH][0] = boat->boom[1]; /* north */
     boat->mast[WEST][NORTH][1] = boat->sail[3]; 
     boat->mast[WEST][SOUTH][0] = boat->boom[1]; /* south */
     boat->mast[WEST][SOUTH][1] = boat->sail[3]; 
      boat->mast[WEST][EAST][0] = boat->boom[0]; /* east  */
      boat->mast[WEST][EAST][1] = boat->sail[3]; 
      boat->mast[WEST][WEST][0] = boat->pole[0]; /* west  */
      boat->mast[WEST][WEST][1] = boat->sail[0]; 

     boat->ord = &_ORDERS;
     boat->now.hdg = NORTH;
     boat->now.rig = 0;
     boat->now.anc = 0;

     MOB *mob = new_mob(boat, 2, 3, 0, 0);

     WINDOW *win = panel_window(mob->pan);
     wbkgrnd(win, &OCEAN);

     /* draw it for the first time */
     mvwadd_wch(win, 0, 1, boat->mast[NORTH][SOUTH][0]);
     mvwadd_wch(win, 1, 0, boat->aft);
       wadd_wch(win,       boat->mid);
       wadd_wch(win,       boat->bow);

     /*sem_wait(SCRLOCK);*/
     update_panels();  /* only update that shit here, unless you want to */
     doupdate();       /* be doing the segfault boogaloo                 */
     /*sem_post(SCRLOCK);*/

     return mob;
   }
/* --------------------------------------------------------------------------
   Refresh the graphics used to draw a boat, and update its .now to reflect
   the current values in &.ord
   -------------------------------------------------------------------------- */
   void sync_boat(void)
   {
     /* Protected block, percolate control input */
     sem_wait(_IO);
     BOAT *boat = (BOAT *)_BOAT->obj;
     WINDOW *win = panel_window(_BOAT->pan);
     int h = boat->now.hdg = boat->ord->hdg;
     int r = boat->now.rig = boat->ord->rig;
     int a = boat->now.anc = boat->ord->anc;
     sem_post(_IO);

     int w = get_wind_dir();

     switch (h) {
         case NORTH:
              werase(win);
              if (w == SOUTH) mvwadd_wch(win, 0, 1, boat->mast[w][h][r]);
              else            mvwadd_wch(win, 0, 2, boat->mast[w][h][r]);
                mvwadd_wch(win, 1, 1, boat->aft);
                  wadd_wch(win,       boat->bow);
              break;

         case WEST:
              werase(win);
              mvwadd_wch(win, 0, 1, boat->mast[w][h][r]);
              mvwadd_wch(win, 1, 0, boat->aft);
                wadd_wch(win,       boat->mid);
                wadd_wch(win,       boat->bow);
              break;

         case EAST:
              werase(win);
              mvwadd_wch(win, 0, 1, boat->mast[w][h][r]);
              mvwadd_wch(win, 1, 0, boat->aft);
                wadd_wch(win,       boat->mid);
                wadd_wch(win,       boat->bow);

              break;

         case SOUTH:
              werase(win);
              if (w == WEST) mvwadd_wch(win, 0, 2, boat->mast[w][h][r]);
              else           mvwadd_wch(win, 0, 1, boat->mast[w][h][r]);
              mvwadd_wch(win, 1, 1, boat->aft);
                wadd_wch(win,       boat->bow);
              break;
     }
     /*sem_wait(SCRLOCK);*/
     update_panels();  /* only update that shit here, unless you want to */
     doupdate();       /* be doing the segfault boogaloo                 */
     /*sem_post(SCRLOCK);*/
   }
/* --------------------------------------------------------------------------
   Calculate the movement of the boat panel from the wind and hdg value
   -------------------------------------------------------------------------- */
   void *sail_boat(void *ptr)
   {

      int x = _BOAT->dim.dx;
      int y = _BOAT->dim.dy;

      int wind = get_wind_dir();

      sem_wait(_IO);
      BOAT *boat = (BOAT *)_BOAT->obj;
      int hdg = boat->now.hdg;
      int rig = boat->now.rig;
      int anc = boat->now.anc;
      sem_post(_IO);

      if (rig == 1) {
         if ( ((abs((wind - hdg)))%4) != 0 ) { /* not in irons */
            switch(hdg) {
              case NORTH:
                y--;
                break;
              case EAST:
                x++;
                break;
              case SOUTH:
                y++;
                break;
              case WEST:
                x--;
                break;
            }
            move_panel(_BOAT->pan, y, x);

            _BOAT->dim.dx = x;
            _BOAT->dim.dy = y;
         }
       }
       /*sem_wait(SCRLOCK);*/
       update_panels();  /* only update that shit here, unless you want to */
       doupdate();       /* be doing the segfault boogaloo                 */
       /*sem_post(SCRLOCK);*/
       return NULL;
   } 
/* --------------------------------------------------------------------------
   Issue orders to the _ORDERS global
   -------------------------------------------------------------------------- */
   void order_boat(int order, int val)
   {
      switch(order) { 
        case 'h': /* heading */
          _ORDERS.hdg = val;
          break;
        case 'r': /* rigging */
          _ORDERS.rig = ((_ORDERS.rig)+1)%2; /* toggle 0/1 */
          break;
        case 'a': /* anchor */
          _ORDERS.anc = val;
          break;
      }
      sync_boat();

   }
/* --------------------------------------------------------------------------
   Return the current point of sail
   -------------------------------------------------------------------------- */
   int PointOfSail(void)
   {
     int wind = get_wind_dir();
     sem_wait(_IO);
     int hdg  = _ORDERS.hdg;
     sem_post(_IO);

     return ((abs((wind - hdg)))%4);
   }
