/* ==========================================================================
    FILENAME:  animate.c
    Create and manipulate animated PANEL types.
   ========================================================================== */
   #define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
   #include <stdio.h>
   #include <stdlib.h>
   #include <ncurses.h>
   #include <panel.h>
   #include <wchar.h>
   #include <locale.h>

    #include "lib/list/list.h"
   #include "lib/gfx.h"
   #include "palette.h"
/* ========================================================================== */
   const wchar_t HERO_GFX = L'ж';
   const wchar_t WAVE_GFX = L'◟';
   const wchar_t WAVE_BREAK_GFX = L'⟱';
   const wchar_t WAVE_FLOW_GFX = L'∭';
   const wchar_t SHORE_GFX = L'∵';
   const wchar_t SAND_GFX = L'⚋';
   const wchar_t BOAT_A = L'◥';
   const wchar_t BOAT_M = L'■';
   const wchar_t BOAT_B = L'◤';
   const wchar_t BOAT_P_R = L'⎣';
   const wchar_t BOAT_P_L = L'⎦';

   const wchar_t GFX_SAIL_L_HAUL = L'⎝';
   const wchar_t GFX_SAIL_R_HAUL = L'⎠';
 
   const wchar_t GFX_SAIL_L_CALM = L'⎢';
   const wchar_t GFX_SAIL_R_CALM = L'⎟';
 
   const wchar_t GFX_SAIL_L_RUN = L'⎛';
   const wchar_t GFX_SAIL_R_RUN = L'⎞';
 
   const wchar_t BOAT_BP_R = L'⎥';
   const wchar_t BOAT_BP_L = L'⎢';
   const wchar_t COMPASS_N = L'⬘';
   const wchar_t COMPASS_S = L'⬙';
   const wchar_t COMPASS_E = L'⬗';
   const wchar_t COMPASS_W = L'⬖';

   cchar_t HERO;
   cchar_t OCEAN;
   cchar_t SURF0, SURF1, SURF2;
   cchar_t SHORE, SAND;
   cchar_t BOAT_HULL_AFT, BOAT_HULL_MID, BOAT_HULL_BOW;
   cchar_t BOAT_BOOM_L, BOAT_BOOM_R;
   cchar_t BOAT_POLE_L, BOAT_POLE_R;
   cchar_t SAIL_L_RUN, SAIL_R_RUN, SAIL_L_HAUL, SAIL_R_HAUL, SAIL_L_CALM, SAIL_R_CALM;
   cchar_t COMPASS[4];
/* ========================================================================== */
   void init_gfx_colors(void)
   {
     setcchar(&OCEAN, &WAVE_GFX,       0, SEA_DEEP,  NULL);
     setcchar(&SURF0, &WAVE_BREAK_GFX, 0, SEA_SURF,  NULL);
     setcchar(&SURF1, &WAVE_FLOW_GFX,  0, SEA_SURF,  NULL);
     setcchar(&SAND,  &SAND_GFX,       0, SEA_SHORE, NULL);
     setcchar(&SHORE, &SHORE_GFX,      0, SEA_SHORE, NULL);

     setcchar(&BOAT_HULL_AFT, &BOAT_A, 0, BOAT_DEEP, NULL);
     setcchar(&BOAT_HULL_MID, &BOAT_M, 0, BOAT_DEEP, NULL);
     setcchar(&BOAT_HULL_BOW, &BOAT_B, 0, BOAT_DEEP, NULL);

     setcchar(&BOAT_BOOM_L, &BOAT_P_L, 0, BOAT_DEEP, NULL);
     setcchar(&BOAT_BOOM_R, &BOAT_P_R, 0, BOAT_DEEP, NULL);

     setcchar(&SAIL_L_RUN,  &GFX_SAIL_L_RUN,  0, SAIL_DEEP, NULL);
     setcchar(&SAIL_R_RUN,  &GFX_SAIL_R_RUN,  0, SAIL_DEEP, NULL);
     setcchar(&SAIL_L_HAUL, &GFX_SAIL_L_HAUL, 0, SAIL_DEEP, NULL);
     setcchar(&SAIL_R_HAUL, &GFX_SAIL_R_HAUL, 0, SAIL_DEEP, NULL);
     setcchar(&SAIL_L_CALM, &GFX_SAIL_L_CALM, 0, SAIL_DEEP, NULL);
     setcchar(&SAIL_R_CALM, &GFX_SAIL_R_CALM, 0, SAIL_DEEP, NULL);

     setcchar(&BOAT_POLE_L, &BOAT_BP_L, 0, BOAT_DEEP, NULL);
     setcchar(&BOAT_POLE_R, &BOAT_BP_R, 0, BOAT_DEEP, NULL);

     setcchar(&COMPASS[0], &COMPASS_N, 0, BOAT_DEEP, NULL);
     setcchar(&COMPASS[1], &COMPASS_S, 0, BOAT_DEEP, NULL);
     setcchar(&COMPASS[2], &COMPASS_E, 0, BOAT_DEEP, NULL);
     setcchar(&COMPASS[3], &COMPASS_W, 0, BOAT_DEEP, NULL);
   }
