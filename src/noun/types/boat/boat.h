#pragma once
#ifndef __BOAT_H
#define __BOAT_H

#include "../../../com/arawak.h"
#include "../../noun.h"
#include "../../../gfx/gfx.h"
#include "../../../lib/ufo.h"
#include "../../../map/snake.h"
#include "../../../mob/mob.h"
#include "compass.h"

/*
  ⎛⎝     
  ◥■◤ 
  ⎛⎝  
  ◥◤  
  ⎛⎞  
  ◥◤  
  ⎠⎞  
  ◥◤  
   ⎠⎞ 
  ◥■◤ 
    ⎠ 
   ◥■◤
    ⎠⎞
   ◥■◤
    ⎠⎞
    ◥◤
    ⎛⎞
    ◥◤
    ⎛⎝
    ◥◤
   ⎛⎝    
   ◥■◤
*/

////////////////////////////////////////////////////////////////////////////////

struct boat_t {
        int type;
        struct boat_gfx *gfx;
        uint32_t state;
        uint32_t hdg;
        uint32_t helm;
};


void new_boat(struct map_t *map, int type, const char *name);
struct boat_t *get_boat(const char *name);

int boat_control(void *mynoun, int order);
void sail_boat(void *mynoun);

////////////////////////////////////////////////////////////////////////////////


enum boat_genera { FUNBOAT, KETCH, CARAVEL };


/* BOAT NIBBLE GUIDE */
enum  boat_nibbles   {RIG,HDG,SAI,GUN,ROT,FLG,VEL};

enum hdg_t HDG_state;
enum grad_t VEL_state;

enum SAI_state_tags {                                                   
        POL, // Bare poles
        FOR, // Foresail
        MAI, // Mainsail
        MIZ, // Mizzen sail
        TPS, // Topsail
        LTG, // Lower topgallant
        UTG, // Upper topgalland
        ROY, // Royal sail
        JIB, // Jib
        SPA, // Spanker
        STU, // Studsail
        SPI, // Spinnaker
        KIT  // Kite
};

enum GUN_state_tags {
        HAS, // Vessel is equipped with cannon and they are operational.
        POW, // Vessel has powder availible.
        SHH, // Vessel has shot availible.
        LD0, // Loading stage 0
        LD1, // Loading stage 1
        RDY, // Loading complete (ready status)
        FIR, // Firing        ___________________________________
        ROU, // Round shot              XOR 0x8 with these to get 
        CAN, // Canister shot           the graphics indexes
        GRA, // Grape shot
        CHI, // Chain shot
        LAN, // Langrage (bags of junk)
        HOT, // Heated shot
        DBL, // Double shot
        EXP  // Exploding shot
};

enum boat_hull_gfx { HUL_N, HUL_E, HUL_S, HUL_W };

enum boat_mast_gfx { MAS_I, MAS_J, MAS_L };

enum boat_sail_gfx {
        SAI_DR, /* Down-right facing sail */ 
        SAI_DL, /* Down-left facing sail  */ 
        SAI_UR, /* Up-right facing sail   */ 
        SAI_UL, /* Up-left facing sail    */ 
        SAI_PI, /* Pinched sail           */ 
        SAI_SW, /* Swelling sail          */ 
        SAI_WL, /* Left-swelling sail     */ 
        SAI_WR  /* Right-swelling sail    */ 
};

enum helm_commands { TURN_L, TURN_R, TURN_HR, TURN_HL, COME_ABOUT };



/* -------------------------------------------------------------------------- */



////////////////////////////////////////////////////////////////////////////////
/*
  The sail state lookup table is used to decide the appropriate sail
  graphic to be drawn, and is indexed by the heading and the wind direction.
*/
static unsigned char stbl[16][16] = {
/* N       NNE     NE      ENE     E       ESE     SE      SSE  
   S       SSW     SW      WSW     W       WNW     NW      NNW
   ---------------------------------------------------------------------------*/
{  SAI_PI, SAI_PI, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_SW,  /*N*/
   SAI_SW, SAI_SW, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_PI  },
{  SAI_PI, SAI_PI, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_SW,  /*NNE*/
   SAI_SW, SAI_SW, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_PI  },
{  SAI_PI, SAI_PI, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_SW,  /*NE*/
   SAI_SW, SAI_SW, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_PI  },
/* --------------------------------------------------------------             */
{  SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR,  /*ENE*/
   SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR  },
{  SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR,  /*E*/
   SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR  },
{  SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR,  /*ESE*/
   SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR  },
{  SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR,  /*SE*/
   SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR  },
/* --------------------------------------------------------------             */
{  SAI_SW, SAI_SW, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_PI,  /*SSE*/
   SAI_PI, SAI_PI, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_SW  },
{  SAI_SW, SAI_SW, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_PI,  /*S*/
   SAI_PI, SAI_PI, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_SW  },
{  SAI_SW, SAI_SW, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_PI,  /*SSW*/
   SAI_PI, SAI_PI, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_SW  },
{  SAI_SW, SAI_SW, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_PI,  /*SW*/
   SAI_PI, SAI_PI, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_SW  },
/* --------------------------------------------------------------             */
{  SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL,  /*WSW*/
   SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL  },
{  SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL,  /*W*/
   SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL  },
{  SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL,  /*WNW*/
   SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL  },
{  SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL,  /*NW*/
   SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL, SAI_DL  },
/* --------------------------------------------------------------             */
{  SAI_PI, SAI_PI, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_SW,  /*NNW*/
   SAI_SW, SAI_SW, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_PI}};
/* ---------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
/*
  The mast state lookup table is similar to the sail table. There are
  some minor differences, but they are indexed the same.
*/
static unsigned char mtbl[16][16] = {
/* N       NNE     NE      ENE     E       ESE     SE      SSE  
   S       SSW     SW      WSW     W       WNW     NW      NNW
   ---------------------------------------------------------------------------*/
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*N*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*NNE*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*NE*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
/* -------------------------------------------------------------              */
{  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,   /*ENE*/
   MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J  },
{  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,   /*E*/
   MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J  },
{  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,   /*ESE*/
   MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J  },
{  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,   /*SE*/
   MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J  },
/* -------------------------------------------------------------              */
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*SSE*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*S*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*SSW*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*SW*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  },
/* -------------------------------------------------------------              */
{  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,   /*WSW*/
   MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L  },
{  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,   /*W*/
   MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L  },
{  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,   /*WNW*/
   MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L  },
{  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,   /*NW*/
   MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L  },
/* -------------------------------------------------------------              */
{  MAS_I,  MAS_I,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_L,  MAS_I,   /*NNW*/
   MAS_I,  MAS_I,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_J,  MAS_I  }};
/* ---------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////



/*
 * These macros will map the 16 compass directions 
 * to the 4 cardinal directions.
 */
#define NORTHING(x)        ((x<=NE)||(x>=NW))
#define EASTING(x)         ((x>NE)&&(x<SE)) 
#define SOUTHING(x)        ((x>=SE)&&(x<=SW))
#define WESTING(x)         ((x<NW)&&(x>SW))
#define STRICTNORTHING(x)  ((x<NE)||(x>NW))
#define STRICTEASTING(x)   ((x>NE)&&(x<SE))
#define STRICTSOUTHING(x)  ((x>SE)&&(x<SW))
#define STRICTWESTING(x)   ((x<NW)&&(x>SW))
#define NWESTING(x)        ((x==NW))     
#define SWESTING(x)        ((x==SW))    
#define NEASTING(x)        ((x==NE))   
#define SEASTING(x)        ((x==SE)) 


////////////////////////////////////////////////////////////////////////////////

#define HULLCOUNT 3 
#define MASTCOUNT 3 
#define SAILCOUNT 3 
#define GUNCOUNT  2 

static struct gpkg _hull[HULLCOUNT]={
{ 4 , {1 , 1 , 1 , 1} , BOAT_WOOD , {L"◥◤"  , L"◥■◤"   , L"◥◤"  , L"◥■◤"}}   ,
{ 4 , {1 , 1 , 1 , 2} , BOAT_WOOD , {L"▗▅▖" , L"▃▂▃▖"  , L"▗▅▖" , L"▗▃▂▃"}}  ,
{ 4 , {1 , 2 , 1 , 2} , BOAT_WOOD , {L"▜▛"  , L"▜▄▂▃▖" , L"▗▃▖" , L"▗▃▂▄▛"}}
};

static struct gpkg _mast[MASTCOUNT] = {
{ 3, {0}, BOAT_WOOD, {L"⎢",L"⎦",L"⎣"}},
{ 3, {0}, BOAT_WOOD, {L"⎢",L"⎦",L"⎣"}},
{ 3, {0}, BOAT_WOOD, {L"⎢",L"⎦",L"⎣"}}
};

static struct gpkg _sail[SAILCOUNT] = {
{ 8, {0}, BOAT_WHITE, {L"⎠",L"⎝",L"⎞",L"⎛",L"⎠⎝",L"⎛⎞",L"⎛⎝",L"⎠⎞"}},
{ 8, {0}, BOAT_WHITE, {L"⎠",L"⎝",L"⎞",L"⎛",L"⎠⎝",L"⎛⎞",L"⎛⎝",L"⎠⎞"}},
{ 8, {0}, BOAT_WHITE, {L"⎠",L"⎝",L"⎞",L"⎛",L"⎠⎝",L"⎛⎞",L"⎛⎝",L"⎠⎞"}}
};

static struct gpkg _cann[GUNCOUNT] = {
{ 8, {0}, GUN_FLASH, {L"✶",L"✶",L"✶",L"✶",L"✶",L"✶",L"✶",L"✶"}},
{ 8, {0}, GUN_SMOKE, {L"✺",L"✺",L"✺",L"✺",L"✺",L"✺",L"✺",L"✺"}}
};


static inline void init_boatgfx(void)
{
        if (_hull[0].cch[0] != NULL) return;

        unsigned char i;
        i = HULLCOUNT;  while (i-->0) { build_gpkg(&_hull[i]); };
        i = MASTCOUNT;  while (i-->0) { build_gpkg(&_mast[i]); };
        i = SAILCOUNT;  while (i-->0) { build_gpkg(&_sail[i]); };
        i = GUNCOUNT;   while (i-->0) { build_gpkg(&_cann[i]); };
}
////////////////////////////////////////////////////////////////////////////////
#endif



