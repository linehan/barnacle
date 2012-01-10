/* ==========================================================================
    FILENAME:  weather.c
    Weather and environmental (not those kind) variables that determine,
    among other things, the state and rate of a ship at sail.
   ========================================================================== */
   #define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
   #include <stdio.h>
   #include <stdlib.h>
   #include <math.h>
   #include <ncurses.h>
   #include <panel.h>
   #include <wchar.h>
   #include <locale.h>

   #include "lib/list/list.h"

   #include "sprite.h"

   #include "lib/gfx.h"
   #include "dice.h"
   #include "palette.h"
   
   #define WIND_DECIDE take_step(0)
   #define WIND_CW     take_step(1)
   #define WIND_CCW    take_step(2)

/* ========================================================================== */
   typedef struct wind_t {
     int dir;
     int str;
     int pre;
     double tr[3];
   } WIND;

   typedef struct spr_t {
     cchar_t cch;
     struct list_node node;
   } SPR;

   typedef struct alea_t {
     int n;
     int d;
     double *t;
   } ALEA;

   int nCMP = 24;
   const wchar_t gfxDN   = L'▾';
         cchar_t ARDN;
   const wchar_t gfxUP   = L'▴';
         cchar_t ARUP;
   const wchar_t gfxVER  = L'┃';
         cchar_t VER;
   const wchar_t gfxHOZ  = L'─';
         cchar_t HOZ;
   const wchar_t gfxTOPL = L'┎';
         cchar_t TOPL; 
   const wchar_t gfxTOPR = L'┒';
         cchar_t TOPR;
   const wchar_t gfxBOTL = L'┖';
         cchar_t BOTL;
   const wchar_t gfxBOTR = L'┚';
         cchar_t BOTR;
   const wchar_t gfxCMP[] = L"N⋅⋅∙⋅⋅E⋅⋅∙⋅⋅S⋅⋅∙⋅⋅W⋅⋅∙⋅⋅";

   LIST_HEAD(CMP);

   WIND *GW; /* the global wind state */

   int DIRS = 8;
   int HALFDIR = 4; 
   int WWID = 15;

   WINDOW *GC_W;
   PANEL  *GC_P;
/* ========================================================================== */
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   void weather_init(void)
   {
     GW = malloc(sizeof(WIND));

     GC_W = newwin(3, WWID, 0, 0);
     wbkgrnd(GC_W, &OCEAN);
     GC_P = new_panel(GC_W);

     GW->str = 2; 
     GW->pre = 0;

     setcchar(&ARDN, &gfxDN,   0, BOAT_DEEP, NULL);
     setcchar(&ARUP, &gfxUP,   0, BOAT_DEEP, NULL);
     setcchar(&VER,  &gfxVER,  0, BOAT_DEEP, NULL);
     setcchar(&HOZ,  &gfxHOZ,  0, BOAT_DEEP, NULL);
     setcchar(&TOPL, &gfxTOPL, 0, BOAT_DEEP, NULL);
     setcchar(&TOPR, &gfxTOPR, 0, BOAT_DEEP, NULL);
     setcchar(&BOTL, &gfxBOTL, 0, BOAT_DEEP, NULL);
     setcchar(&BOTR, &gfxBOTR, 0, BOAT_DEEP, NULL);

     mvwadd_wch(GC_W, 0,        7, &ARDN);
     mvwadd_wch(GC_W, 0,        0, &TOPL);
     mvwadd_wch(GC_W, 0, (WWID-1), &TOPR);

     mvwadd_wch(GC_W, 1, (WWID-1), &VER);

     mvwadd_wch(GC_W, 2,        7, &ARUP);
     mvwadd_wch(GC_W, 2,        0, &BOTL);
     mvwadd_wch(GC_W, 2, (WWID-1), &BOTR);

     int i;
     for (i=0; i<nCMP; i++)
     {
       SPR *new = (SPR *)malloc(sizeof(SPR));
       setcchar(&(new->cch), &gfxCMP[i], 0, BOAT_DEEP, NULL);
       list_add(&CMP, &(new->node));
     }
   }
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   void set_prevailing(int dir, double factor)
   {
     double ONE = 1.0;
     double TWO = 2.0;

     double frac = ((ONE-factor)/TWO);

     GW->dir = dir;
     GW->pre = dir;
     GW->tr[0] = factor;
     GW->tr[1] = frac;
     GW->tr[2] = frac;
   }
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   void tumble_compass(int a)
   {
     SPR *tmp;

     switch (a) {
       case 0:
         break;
       case 1:
         tmp = list_top(&CMP, SPR, node);
               list_del(&(tmp->node));
               list_add_tail(&CMP, &(tmp->node));
         break;
       case 2:
         tmp = list_tail(&CMP, SPR, node);
               list_del(&(tmp->node));
               list_add(&CMP, &(tmp->node));
         break;
     }
   }
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   void take_step(int opt)
   {
     signed int d = GW->dir;
     switch (opt) {
       case 0: if (GW->dir == GW->pre) return;
               if ((abs((GW->dir)-(GW->pre))) > HALFDIR) {
                 d++;
                 tumble_compass(1);
               }
               else {
                 d--;
                 tumble_compass(2);
               }
               break;
       case 1: d++;
               tumble_compass(1);
               break;
       case 2: d--;
               tumble_compass(2);
               break;
    }
    GW->dir = (int)abs((d%DIRS));
   }
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   int wind_roll(void *ptr)
   {
       DICE *D = (DICE *)ptr;

       switch ((vose_alias(D))) {
           case 0: WIND_DECIDE;
                   break;
           case 1: WIND_CW;
                   break;
           case 2: WIND_CCW;
                   break;
       }
     return 0;
   }
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   int get_wind_dir(void)
   {
     return GW->dir;
   }
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   int get_wind_str(void)
   {
     return GW->str;
   }
/* --------------------------------------------------------------------------
   -------------------------------------------------------------------------- */
   void wind_gfx(void)
   {
     int i;
     top_panel(GC_P);

     SPR *tmp = list_top(&CMP, SPR, node);

     wclrtoeol(GC_W);

     mvwadd_wch(GC_W, 1, 0, &VER);

     list_for_each(&CMP, tmp, node)
     {
       mvwadd_wch(GC_W, 1, (1+(i++)), &(tmp->cch));
       mvwadd_wch(GC_W, 1, WWID, &VER);
     }
     mvwadd_wch(GC_W, 1, (WWID-1), &VER);

   }

