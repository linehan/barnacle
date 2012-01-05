/* ==========================================================================
    FILENAME:  ticker.c
    Track time, custody of the main loop, issue and toggle semaphores
   ========================================================================== */
   #define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
   #include <stdio.h>
   #include <stdlib.h>
   #include <wchar.h>
   #include <locale.h>
   #include <ncurses.h>
   #include <panel.h>
   #include <unistd.h>
   #include <pthread.h>
   #include <semaphore.h>

    #include "lib/list/list.h"
   #include "lib/gfx.h"

   #include "boat.h"
   #include "control.h"
   #include "palette.h"
/* ========================================================================== */
   #define TICK 150 
   #define GEAR 12 
/* A linked list of switches to be toggled ---------------------------------- */
   typedef struct switch_t {
     sem_t *sem; 
     struct switch_t *next;
   } SWITCH;
      
/* ========================================================================== */
   sem_t *SCRLOCK;
   SWITCH **WHEEL; /* GLOBAL EVENT WHEEL */
   SWITCH **FEED;  /* AUXILIARY WHEEL */
/* ========================================================================== */
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   void ticker_init(void)
   {
     int i;
     FEED = malloc(GEAR * sizeof(SWITCH *));
     WHEEL  = malloc(GEAR * sizeof(SWITCH *));
     for (i=0; i<GEAR; i++) {
         FEED[i] = NULL;
         WHEEL[i]  = NULL;
     }
     SCRLOCK = malloc(sizeof(sem_t));
     sem_init(SCRLOCK, 0, 1);
   }
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   void mesh(void *ptr, int gear)
   {
     sem_t *sem = (sem_t *)ptr;
 
     SWITCH *new;
     new = malloc(sizeof(SWITCH));
 
     new->sem = sem;
 
     if (FEED[gear] == NULL) {
         FEED[gear] = new;
         FEED[gear]->next = NULL;
     }
     else {
         new->next = FEED[gear];
         FEED[gear] = new;
     }
   }
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   void clutch(void)
   {
     int i;
     for (i=0; i<GEAR; i++) {
         WHEEL[i] = FEED[i];
     }
   }
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   void *DoTick(void *thing)
   { 
     int i;
     signed int count = 0;
     signed int up = -1;

     while(1) {
         clutch(); /* incorporate any new events */
         for (i=1; i<GEAR; i++) {
             /* unlock HUB[i]'s semaphores every i ticks */
             while (((count%i) == 0) && (WHEEL[i] != NULL)) {
                   sem_post(WHEEL[i]->sem);
                   WHEEL[i] = WHEEL[i]->next;
             }
         }
         /* make sure count reverses at 0 and GEAR */
         if (count%GEAR == 0)
            up = (up * -1); /* toggles between 1 and -1 */
         count = (count+up);
         napms(TICK);
     }
     return NULL;
   }
     
