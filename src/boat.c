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
   #include "ticker.h"
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
     boat->mast[0][0][0] = boat->pole[0]; /* north */
     boat->mast[0][0][1] = boat->sail[0];  
     boat->mast[0][1][0] = boat->boom[0]; /* south */
     boat->mast[0][1][1] = boat->sail[4]; 
     boat->mast[0][2][0] = boat->boom[0]; /* east  */
     boat->mast[0][2][1] = boat->sail[3]; 
     boat->mast[0][3][0] = boat->boom[1]; /* west  */
     boat->mast[0][3][1] = boat->sail[2]; 
     /* wind from south v */
     boat->mast[1][0][0] = boat->boom[1]; /* north */
     boat->mast[1][0][1] = boat->sail[5]; 
     boat->mast[1][1][0] = boat->pole[1]; /* south */
     boat->mast[1][1][1] = boat->sail[0]; 
     boat->mast[1][2][0] = boat->boom[0]; /* east  */
     boat->mast[1][2][1] = boat->sail[3]; 
     boat->mast[1][3][0] = boat->boom[1]; /* west  */
     boat->mast[1][3][1] = boat->sail[2]; 
     /* wind from east <-- */
     boat->mast[2][0][0] = boat->boom[0]; /* north */
     boat->mast[2][0][1] = boat->sail[2]; 
     boat->mast[2][1][0] = boat->boom[0]; /* south */
     boat->mast[2][1][1] = boat->sail[2]; 
     boat->mast[2][2][0] = boat->pole[1]; /* east  */
     boat->mast[2][2][1] = boat->sail[1]; 
     boat->mast[2][3][0] = boat->boom[1]; /* west  */
     boat->mast[2][3][1] = boat->sail[2];
     /* wind from west --> */ 
     boat->mast[3][0][0] = boat->boom[1]; /* north */
     boat->mast[3][0][1] = boat->sail[3]; 
     boat->mast[3][1][0] = boat->boom[1]; /* south */
     boat->mast[3][1][1] = boat->sail[3]; 
     boat->mast[3][2][0] = boat->boom[0]; /* east  */
     boat->mast[3][2][1] = boat->sail[3]; 
     boat->mast[3][3][0] = boat->pole[0]; /* west  */
     boat->mast[3][3][1] = boat->sail[0]; 

     boat->ord = &_ORDERS;
     boat->now.hdg = 0;
     boat->now.rig = 0;
     boat->now.anc = 0;

     MOB *mob = new_mob(boat, 2, 3, 0, 0);

     WINDOW *win = panel_window(mob->pan);
     wbkgrnd(win, &OCEAN);

     /* draw it for the first time */
     mvwadd_wch(win, 0, 1, boat->mast[0][2][0]);
     mvwadd_wch(win, 1, 0, boat->aft);
       wadd_wch(win,       boat->mid);
       wadd_wch(win,       boat->bow);

     sem_wait(SCRLOCK);
     update_panels();  /* only update that shit here, unless you want to */
     doupdate();       /* be doing the segfault boogaloo                 */
     sem_post(SCRLOCK);

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
     int hdg = boat->now.hdg = boat->ord->hdg;
     int rig = boat->now.rig = boat->ord->rig;
     int anc = boat->now.anc = boat->ord->anc;
     sem_post(_IO);

     int wind = wind_get();

     switch (hdg) {
         case NORTH: /* set heading north */
              werase(win);
              if(wind == 2) mvwadd_wch(win, 0, 1, boat->mast[wind][hdg][rig]);
              else          mvwadd_wch(win, 0, 2, boat->mast[wind][hdg][rig]);
                            mvwadd_wch(win, 1, 1, boat->aft);
                              wadd_wch(win,       boat->bow);
              break;
         case SOUTH: /* set heading south */
              werase(win);
              if(wind == 3) mvwadd_wch(win, 0, 2, boat->mast[wind][hdg][rig]);
              else          mvwadd_wch(win, 0, 1, boat->mast[wind][hdg][rig]);
                            mvwadd_wch(win, 1, 1, boat->aft);
                              wadd_wch(win,       boat->bow);
              break;
         case EAST: /* set heading west */
              werase(win);
              mvwadd_wch(win, 0, 1, boat->mast[wind][hdg][rig]);
              mvwadd_wch(win, 1, 0, boat->aft);
                wadd_wch(win,       boat->mid);
                wadd_wch(win,       boat->bow);
              break;
         case WEST: /* set heading east */
              werase(win);
              mvwadd_wch(win, 0, 1, boat->mast[wind][hdg][rig]);
              mvwadd_wch(win, 1, 0, boat->aft);
                wadd_wch(win,       boat->mid);
                wadd_wch(win,       boat->bow);
              break;
     }
     sem_wait(SCRLOCK);
     update_panels();  /* only update that shit here, unless you want to */
     doupdate();       /* be doing the segfault boogaloo                 */
     sem_post(SCRLOCK);
   }
/* --------------------------------------------------------------------------
   Calculate the movement of the boat panel from the wind and hdg value
   -------------------------------------------------------------------------- */
   void *sail_boat(void *ptr)
   {
     sem_t *GO = (sem_t *)ptr;
     mesh(GO, 1);

     while(1) {
      sem_wait(GO);

      sync_boat();
      int wind = wind_get();

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
                if(_BOAT->dim.dy == 0) break;
                move_panel(_BOAT->pan, --_BOAT->dim.dy, _BOAT->dim.dx);
                break;
              case SOUTH:
                if(_BOAT->dim.dy == LINES) break;
                move_panel(_BOAT->pan, ++_BOAT->dim.dy, _BOAT->dim.dx);
                break;
              case EAST:
                if(_BOAT->dim.dx == COLS) break;
                move_panel(_BOAT->pan, _BOAT->dim.dy, ++_BOAT->dim.dx);
                break;
              case WEST:
                if(_BOAT->dim.dx == 0) break;
                move_panel(_BOAT->pan, _BOAT->dim.dy, --_BOAT->dim.dx);
                break;	
            }
         }
       }
       sem_wait(SCRLOCK);
       update_panels();  /* only update that shit here, unless you want to */
       doupdate();       /* be doing the segfault boogaloo                 */
       sem_post(SCRLOCK);
     }
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
     int wind = wind_get();
     sem_wait(_IO);
     int hdg  = _ORDERS.hdg;
     sem_post(_IO);

     return ((abs((wind - hdg)))%4);
   }
