/******************************************************************************/
#include "../lib/twindex/twindex.h"
#include "../lib/twindex/tw_boat.h"
/* Guns/Artillery tags and genera (flash) ----------------------------------- */
#define TWINDEX(tag, val) tag=val ,
DO_TWINDEX(ENUM, GUN)
#undef  TWINDEX
#define TWINDEX(tag, val) #tag ,
DO_TWINDEX(CHAR, GUN)
#undef  TWINDEX
/* Hull orientation and genera ---------------------------------------------- */
#define TWINDEX(tag, val) tag=val ,
DO_TWINDEX(ENUM, HULL)
#undef  TWINDEX
#define TWINDEX(tag, val) #tag ,
DO_TWINDEX(CHAR, HULL)
#undef  TWINDEX
/* Anchor genera and states ------------------------------------------------- */
#define TWINDEX(tag, val) tag=val ,
DO_TWINDEX(ENUM, ANCHOR)
#undef  TWINDEX
#define TWINDEX(tag, val) #tag ,
DO_TWINDEX(CHAR, ANCHOR)
#undef  TWINDEX
/* Mast genera -------------------------------------------------------------- */
#define TWINDEX(tag, val) tag=val ,
DO_TWINDEX(ENUM, MAST)
#undef  TWINDEX
#define TWINDEX(tag, val) #tag ,
DO_TWINDEX(CHAR, MAST)
#undef  TWINDEX
/* Sail genera -------------------------------------------------------------- */
#define TWINDEX(tag, val) tag=val ,
DO_TWINDEX(ENUM, SAIL)
#undef  TWINDEX
#define TWINDEX(tag, val) #tag ,
DO_TWINDEX(CHAR, SAIL)
#undef  TWINDEX
/* Generate string glossary ------------------------------------------------- */
GLOSSARIZE(boat)
/******************************************************************************/
/*
  These macros will map the 16 compass directions 
  to the 4 cardinal directions.
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
/******************************************************************************/
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
/* -------------------------------------------------------------              */
{  SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR,  /*ENE*/
   SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR  },
{  SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR,  /*E*/
   SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR  },
{  SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR,  /*ESE*/
   SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR  },
{  SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR,  /*SE*/
   SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR, SAI_DR  },
/* -------------------------------------------------------------              */
{  SAI_SW, SAI_SW, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_PI,  /*SSE*/
   SAI_PI, SAI_PI, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_SW  },
{  SAI_SW, SAI_SW, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_PI,  /*S*/
   SAI_PI, SAI_PI, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_SW  },
{  SAI_SW, SAI_SW, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_PI,  /*SSW*/
   SAI_PI, SAI_PI, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_SW  },
{  SAI_SW, SAI_SW, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_PI,  /*SW*/
   SAI_PI, SAI_PI, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_SW  },
/* -------------------------------------------------------------              */
{  SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR,  /*WSW*/
   SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR  },
{  SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR,  /*W*/
   SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR  },
{  SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR,  /*WNW*/
   SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR  },
{  SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR,  /*NW*/
   SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR, SAI_WR  },
/* -------------------------------------------------------------              */
{  SAI_PI, SAI_PI, SAI_WL, SAI_DL, SAI_DL, SAI_DL, SAI_WL, SAI_SW,  /*NNW*/
   SAI_SW, SAI_SW, SAI_WR, SAI_DR, SAI_DR, SAI_DR, SAI_WR, SAI_PI}};
/* ---------------------------------------------------------------------------*/
/******************************************************************************/
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
/******************************************************************************/
/*
  A boat is some combination of a hull, mast, sail, anchor, and guns.
  When a boat genera is selected, the appropriate graphics for that
  genera will be looked up and mounted in this structure.
*/
struct boat_gfx {
        struct gpkg *hull;  // Indexed with NESW (rounded)
        struct gpkg *mast;  // When sails are not set
        struct gpkg *sail;  // 8 points of sail
        struct gpkg *anc;   // Anchor and sea anchor
        struct gpkg *gunf;  // (flash) indexed with last 8 bits of GUN state
        struct gpkg *guns;  // (smoke) indexed with last 8 bits of GUN state
};
/******************************************************************************/
/*
  These tallies are used in the function init_boats(), found in boat.c,
  where they increment a while loop, spooling the raw graphics into another 
  function -- make_gpkg() -- found in gfx.c. make_gpkg() initializes the 
  complex renditions of the raw characters as they are declared here.
  This process must be undergone at runtime. 
*/
#define HULLCOUNT 3 
#define MASTCOUNT 3 
#define SAILCOUNT 3 
#define GUNCOUNT  2 

enum boat_genera { FUNBOAT, KETCH, CARAVEL };
/******************************************************************************/
static struct gpkg _hull[HULLCOUNT] = {
/* It's the funboat! So fun! ------------------------------------------------ */
{ 4, {0,1,0,1}, BOAT_WOOD, {{L"◥◤"},{L"◥■◤"},{L"◥◤"},{L"◥■◤"}}},
/* Ketch -------------------------------------------------------------------- */
{ 4, {1,1,1,2}, BOAT_WOOD, {{L"▗▅▖"},{L"▃▂▃▖"},{L"▗▅▖"},{L"▗▃▂▃"}}},
/* Caravel ------------------------------------------------------------------ */
{ 4, {1,2,1,2}, BOAT_WOOD, {{L"▜▛"},{L"▜▄▂▃▖"},{L"▗▃▖"},{L"▗▃▂▄▛"}}}
};
static struct gpkg _mast[MASTCOUNT] = {
/* Funboat! Oh, my! --------------------------------------------------------- */
{ 3, {0}, BOAT_WOOD, {{L"⎢"},{L"⎦"},{L"⎣"}}},
/* Ketch -------------------------------------------------------------------- */
{ 3, {0}, BOAT_WOOD, {{L"⎢"},{L"⎦"},{L"⎣"}}},
/* Caravel ------------------------------------------------------------------ */
{ 3, {0}, BOAT_WOOD, {{L"⎢"},{L"⎦"},{L"⎣"}}}
};
static struct gpkg _sail[SAILCOUNT] = {
/* I can't believe that funboat! -------------------------------------------- */
{ 8, {0}, BOAT_WHITE, {{L"⎠"},{L"⎝"},{L"⎞"},{L"⎛"},{L"⎠⎝"},{L"⎛⎞"},{L"⎛⎝"},{L"⎠⎞"}}},
/* Ketch -------------------------------------------------------------------- */
{ 8, {0}, BOAT_WHITE, {{L"⎠"},{L"⎝"},{L"⎞"},{L"⎛"},{L"⎠⎝"},{L"⎛⎞"},{L"⎛⎝"},{L"⎠⎞"}}},
/* Caravel ------------------------------------------------------------------ */
{ 8, {0}, BOAT_WHITE, {{L"⎠"},{L"⎝"},{L"⎞"},{L"⎛"},{L"⎠⎝"},{L"⎛⎞"},{L"⎛⎝"},{L"⎠⎞"}}}
};
static struct gpkg _cann[GUNCOUNT] = {
{ 8, {0}, GUN_FLASH, {{L"✶"},{L"✶"},{L"✶"},{L"✶"},{L"✶"},{L"✶"},{L"✶"},{L"✶"}}},
{ 8, {0}, GUN_SMOKE, {{L"✺"},{L"✺"},{L"✺"},{L"✺"},{L"✺"},{L"✺"},{L"✺"},{L"✺"}}}
};
/******************************************************************************/
#define MY_BOAT(name, genera) \
        struct boat_gfx *name = malloc(sizeof(struct boat_gfx)); \
        name->hull = &_hull[genera]; \
        name->sail = &_sail[genera]; \
        name->mast = &_mast[genera]; \
        name->gunf = &_cann[0]; \
        name->guns = &_cann[1]
