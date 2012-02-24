// vim:fdm=marker
/*******************************************************************************
FILENAME: boat.c
Defines a BOAT datatype, containing graphics and options related to the
display and navigation of a BOAT.
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "../lib/llist/list.h"
#include "../lib/macross.h"
#include "../pan/test.h"

#include "../gfx/gfx.h"
#include "mob.h"
#include "boat.h"
#include "../cmd/control.h"

#include "../gfx/sprite.h"
#include "../gfx/palette.h"
#include "../geo/terrain.h"
#include "../geo/weather.h"
#include "../pan/instruments.h"
#include "../lib/ufo.h"
/******************************************************************************/
/* Make things a little more intelligible */
#define NORTHING(x) ((x<=NE)||(x>=NW))
#define EASTING(x)  ((x>NE)&&(x<SE))
#define SOUTHING(x) ((x>=SE)&&(x<=SW))
#define WESTING(x)  ((x<NW)&&(x>SW))
#define STRICTNORTHING(x) ((x<NE)||(x>NW))
#define STRICTEASTING(x)  ((x>NE)&&(x<SE))
#define STRICTSOUTHING(x) ((x>SE)&&(x<SW))
#define STRICTWESTING(x)  ((x<NW)&&(x>SW))
#define NWESTING(x) ((x==NW))
#define SWESTING(x) ((x==SW))
#define NEASTING(x) ((x==NE))
#define SEASTING(x) ((x==SE))
/******************************************************************************/
/* BOAT MAP VIEW                                                              */
/*
  { L"⎛⎝  ",   
    L"◥■◤ " },
  { L"⎛⎝  ",
    L"◥◤  " },
  { L"⎛⎞  ",
    L"◥◤  " },
  { L"⎠⎞  ",
    L"◥◤  " },
  { L" ⎠⎞ ",
    L"◥■◤ " },
  { L"  ⎠ ",
    L" ◥■◤" },
  { L"  ⎠⎞",
    L" ◥■◤" },
  { L"  ⎠⎞",
    L"  ◥◤" },
  { L"  ⎛⎞",
    L"  ◥◤" },
  { L"  ⎛⎝",
    L"  ◥◤" }
  { L" ⎛⎝ ",   
    L" ◥■◤" },
  };
*/
/******************************************************************************/
enum boat_hull_graphics {
        HUL_N=0,  // Up-facing hull
        HUL_E=1,  // Right-facing hull
        HUL_S=2,  // Down-facing hull
        HUL_W=3   // Left-facing hull
};
enum boat_anchor_graphics {
        ANC_D=0,  // Anchor dropped
        ANC_S=1   // Sea anchor dropped
};
enum boat_sail_graphics {
        MAS_I=0,  // "I" shaped mast
        MAS_J=1,  // "J" shaped mast
        MAS_L=2,  // "L" shaped mast
        SAI_DR=3, // Down-right facing sail
        SAI_DL=4, // Down-left facing sail
        SAI_UR=5, // Up-right facing sail
        SAI_UL=6, // Up-left facing sail
        SAI_PI=7, // Pinched sail
        SAI_SW=8, // Swelling sail
        SAI_WL=9, // Left-swelling sail
        SAI_WR=10 // Right-swelling sail
};

enum boat_gun_flash_graphics {
        ROU_F = 0,
        CAN_F = 1,
        GRA_F = 2,
        CHA_F = 3,
        LAN_F = 4,
        HOT_F = 5,
        DBL_F = 6,
        EXP_F = 7
};
enum boat_gun_smoke_graphics {
        ROU_S = 0,
        CAN_S = 1,
        GRA_S = 2,
        CHA_S = 3,
        LAN_S = 4,
        HOT_S = 5,
        DBL_S = 6,
        EXP_S = 7
};

struct boat_gfx {
        struct gpkg *hull[4];  // Indexed with NESW (rounded)
        struct gpkg *anc[2];   // Anchor and sea anchor
        struct gpkg *gunf[8];  // Indexed with last 8 bits of GUN state
        struct gpkg *guns[8];  // Indexed with last 8 bits of GUN state
        struct gpkg *sail[11]; // 11 points of sail
};

#define TYPE_FOR_BOAT struct boat_gfx
#define ALLO_FOR_BOAT malloc(sizeof(TYPE_FOR_BOAT));

/*
  Expands to a series of calls to new_gpkg, which assigns the appropriate
  items to 'item'.
*/
#define MADE_OF(wch, pair, xo, yo, xr, yr, memb, ind) \
        memb[ind] = new_gpkg(wch, pair, xo, yo, xr, yr); \
/*
  Expands to the appropriate series of calls to construct a yawl.
*/
#define MAKE_BOAT_yawl(name) \
        name->MADE_OF(L"▗▅▖",  BOAT_WOOD,  1,0,0,1, hull, HUL_N)  \
        name->MADE_OF(L"▃▂▃▖", BOAT_WOOD,  1,0,0,1, hull, HUL_E)  \
        name->MADE_OF(L"▗▅▖",  BOAT_WOOD,  1,0,0,1, hull, HUL_S)  \
        name->MADE_OF(L"▗▃▂▃", BOAT_WOOD,  2,0,0,2, hull, HUL_W)  \
        name->MADE_OF(L"⎢",    BOAT_WOOD,  0,0,0,0, sail, MAS_I)  \
        name->MADE_OF(L"⎦",    BOAT_WOOD,  0,0,0,0, sail, MAS_J)  \
        name->MADE_OF(L"⎣",    BOAT_WOOD,  0,0,0,0, sail, MAS_L)  \
        name->MADE_OF(L"⎠",    BOAT_WHITE, 0,0,0,0, sail, SAI_DR) \
        name->MADE_OF(L"⎝",    BOAT_WHITE, 0,0,0,0, sail, SAI_DL) \
        name->MADE_OF(L"⎞",    BOAT_WHITE, 0,0,0,0, sail, SAI_UR) \
        name->MADE_OF(L"⎛",    BOAT_WHITE, 0,0,0,0, sail, SAI_UL) \
        name->MADE_OF(L"⎠⎝",   BOAT_WHITE, 0,0,0,0, sail, SAI_PI) \
        name->MADE_OF(L"⎛⎞",   BOAT_WHITE, 0,0,0,0, sail, SAI_SW) \
        name->MADE_OF(L"⎛⎝",   BOAT_WHITE, 0,0,0,0, sail, SAI_WL) \
        name->MADE_OF(L"⎠⎞",   BOAT_WHITE, 0,0,0,0, sail, SAI_WR) \
        name->MADE_OF(L"✶",    GUN_FLASH,  0,0,0,0, gunf, ROU_F)  \
        name->MADE_OF(L"✺",    GUN_SMOKE,  0,0,0,0, guns, ROU_S)  \
        name->MADE_OF(L"✶",    GUN_FLASH,  0,0,0,0, gunf, CAN_F)  \
        name->MADE_OF(L"✺",    GUN_SMOKE,  0,0,0,0, guns, CAN_S)  \
        name->MADE_OF(L"✶",    GUN_FLASH,  0,0,0,0, gunf, GRA_F)  \
        name->MADE_OF(L"✺",    GUN_SMOKE,  0,0,0,0, guns, GRA_S)  \
        name->MADE_OF(L"✶",    GUN_FLASH,  0,0,0,0, gunf, CHA_F)  \
        name->MADE_OF(L"✺",    GUN_SMOKE,  0,0,0,0, guns, CHA_S)  \
        name->MADE_OF(L"✶",    GUN_FLASH,  0,0,0,0, gunf, LAN_F)  \
        name->MADE_OF(L"✺",    GUN_SMOKE,  0,0,0,0, guns, LAN_S)  \
        name->MADE_OF(L"✶",    GUN_FLASH,  0,0,0,0, gunf, HOT_F)  \
        name->MADE_OF(L"✺",    GUN_SMOKE,  0,0,0,0, guns, HOT_S)  \
        name->MADE_OF(L"✶",    GUN_FLASH,  0,0,0,0, gunf, DBL_F)  \
        name->MADE_OF(L"✺",    GUN_SMOKE,  0,0,0,0, guns, DBL_S)  \
        name->MADE_OF(L"✶",    GUN_FLASH,  0,0,0,0, gunf, EXP_F)  \
        name->MADE_OF(L"✺",    GUN_SMOKE,  0,0,0,0, guns, EXP_S)  \


/*
  Declare a boat of class 'CLASS' with name 'name'.
*/
/*#define MY_BOAT(name, kind, type)                          \*/
        /*type * ## name = malloc(sizeof( ## type ## )); \*/

        /*MADE_BY(new_gpkg) \*/
        /*MAKE_IT \*/
         /**name = malloc(sizeof(struct boat_gfx)); \*/
        /*TIDY_UP*/


const wchar_t gMAST_I[] = L"⎢";
const wchar_t gMAST_J[] = L"⎦";
const wchar_t gMAST_L[] = L"⎣";
/*const wchar_t gHULL_V[] = L"◥◤";*/
/*const wchar_t gHULL_W[] = L"◥■◤";*/ 
const wchar_t gHULL_V[] = L"▗▅▖";                  
const wchar_t gHULL_R[] = L"▃▂▃▖";
const wchar_t gHULL_L[] = L"▗▃▂▃";
const wchar_t gSAIL_DL[] = L"⎝";
const wchar_t gSAIL_DR[] = L"⎠";
const wchar_t gSAIL_UL[] = L"⎛";
const wchar_t gSAIL_UR[] = L"⎞";
const wchar_t gSAIL_PINCH[] = L"⎠⎝";
const wchar_t gSAIL_SWELL[] = L"⎛⎞";
const wchar_t gSAIL_LSWELL[] = L"⎛⎝";
const wchar_t gSAIL_RSWELL[] = L"⎠⎞";

const wchar_t gCANN_PUFF = L'✺';


cchar_t MAST_I[2], MAST_J[2], MAST_L[2];
cchar_t HULL_V[3], HULL_L[4], HULL_R[4];
cchar_t SAIL_DL[2], SAIL_DR[2], SAIL_UL[2], SAIL_UR[2];
cchar_t SAIL_PINCH[3], SAIL_SWELL[3], SAIL_LSWELL[3], SAIL_RSWELL[3];
cchar_t CANN[2];

cchar_t *HULL[16];
cchar_t *SAIL[16][16];
cchar_t *MAST[16][16];


int ofs_H[16];
int ofs_S[16][16];
int ofs_M[16][16];

static int NON = 0;
static int ONE = 1;
static int TWO = 2;

struct mob_t *_BOAT;

struct boat_gfx *GGG;


/******************************************************************************/
void boat_test(void)
{

        /*MY_BOAT(my_yawl, yawl);*/
        /*struct boat_gfx *my_yawl = malloc(sizeof *my_yawl);*/
        MAKE_MY(BOAT, my_yawl, yawl);

        GGG = my_yawl;
}

/* Initialize the standing orders global variable and the semaphores used for 
 * locking boat objects. */
void init_boat(void)
{
        int i, j;
        for (i=0; i<2; i++) {
                setcchar(&MAST_I[i], &gMAST_I[i], 0, BOAT_WOOD, NULL);
                setcchar(&MAST_J[i], &gMAST_J[i], 0, BOAT_WOOD, NULL);
                setcchar(&MAST_L[i], &gMAST_L[i], 0, BOAT_WOOD, NULL);

                setcchar(&SAIL_DL[i], &gSAIL_DL[i], 0, BOAT_WHITE, NULL);
                setcchar(&SAIL_DR[i], &gSAIL_DR[i], 0, BOAT_WHITE, NULL);
                setcchar(&SAIL_UL[i], &gSAIL_UL[i], 0, BOAT_WHITE, NULL);
                setcchar(&SAIL_UR[i], &gSAIL_UR[i], 0, BOAT_WHITE, NULL);
        }
        for (i=0; i<3; i++) {
                setcchar(&HULL_V[i], &gHULL_V[i], 0, BOAT_WOOD, NULL);

                setcchar(&SAIL_PINCH[i], &gSAIL_PINCH[i], 0, BOAT_WHITE, NULL);
                setcchar(&SAIL_SWELL[i], &gSAIL_SWELL[i], 0, BOAT_WHITE, NULL);
                setcchar(&SAIL_LSWELL[i], &gSAIL_LSWELL[i], 0, BOAT_WHITE, NULL);
                setcchar(&SAIL_RSWELL[i], &gSAIL_RSWELL[i], 0, BOAT_WHITE, NULL);
        }
        for (i=0; i<4; i++) {
                setcchar(&HULL_L[i], &gHULL_L[i], 0, BOAT_WOOD, NULL);
                setcchar(&HULL_R[i], &gHULL_R[i], 0, BOAT_WOOD, NULL);
        }
        setcchar(&CANN[0], &gCANN_PUFF, 0, BOAT_WHITE, NULL); // Flash
        setcchar(&CANN[1], &gCANN_PUFF, 0, BOAT_WOOD, NULL);  // Smoke

        for (i=0; i<16; i++) {

                /* Hull is dependent on heading only */
                if NORTHING(i)      HULL[i] = HULL_V;
                else if EASTING(i)  HULL[i] = HULL_R;
                else if SOUTHING(i) HULL[i] = HULL_V;
                else if WESTING(i)  HULL[i] = HULL_L;

                /* Set hull offset */
                ofs_H[i] = NON;

                for (j=0; j<16; j++) {

                        /* Set mast offset */
                        if WESTING(i)
                                ofs_M[i][j] = TWO;
                        else
                                ofs_M[i][j] = ONE;
                        
                        /* Heading(i) and Wind direction (j) */
                        if NORTHING(i) {
                                if STRICTNORTHING(j) {
                                        MAST[i][j] = MAST_I;
                                        SAIL[i][j] = SAIL_PINCH;
                                        ofs_S[i][j] = NON;
                                }
                                else if NEASTING(j) {
                                        MAST[i][j] = MAST_L;
                                        SAIL[i][j] = SAIL_LSWELL;
                                        ofs_S[i][j] = NON;
                                }
                                else if STRICTEASTING(j) {
                                        MAST[i][j] = MAST_L;
                                        SAIL[i][j] = SAIL_DL;
                                        ofs_S[i][j] = ONE;
                                }
                                else if SEASTING(j) {
                                        MAST[i][j] = MAST_L;
                                        SAIL[i][j] = SAIL_LSWELL;
                                        ofs_S[i][j] = NON;
                                }
                                else if STRICTSOUTHING(j) {
                                        MAST[i][j] = MAST_I;
                                        SAIL[i][j] = SAIL_SWELL;
                                        ofs_S[i][j] = NON;
                                }
                                else if SWESTING(j) {
                                        MAST[i][j] = MAST_J;
                                        SAIL[i][j] = SAIL_RSWELL;
                                        ofs_S[i][j] = NON;
                                }
                                else if STRICTWESTING(j) { 
                                        MAST[i][j] = MAST_J;
                                        SAIL[i][j] = SAIL_DR;
                                        ofs_S[i][j] = ONE;
                                }
                                else if NWESTING(j) {
                                        MAST[i][j] = MAST_J;
                                        SAIL[i][j] = SAIL_RSWELL;
                                        ofs_S[i][j] = NON;
                                }
                        }
                        else if EASTING(i) {
                                MAST[i][j] = MAST_J;
                                SAIL[i][j] = SAIL_DR;
                                ofs_S[i][j] = ONE;
                        }
                        else if SOUTHING(i) {
                                if STRICTNORTHING(j) {
                                        MAST[i][j] = MAST_I;
                                        SAIL[i][j] = SAIL_SWELL;
                                        ofs_S[i][j] = NON;
                                }
                                else if NEASTING(j) {
                                        MAST[i][j] = MAST_L;
                                        SAIL[i][j] = SAIL_LSWELL;
                                        ofs_S[i][j] = NON;
                                }
                                else if STRICTEASTING(j) {  
                                        MAST[i][j] = MAST_L;
                                        SAIL[i][j] = SAIL_DL;
                                        ofs_S[i][j] = ONE;
                                }
                                else if SEASTING(j) {
                                        MAST[i][j] = MAST_L;
                                        SAIL[i][j] = SAIL_LSWELL;
                                        ofs_S[i][j] = NON;
                                }
                                else if STRICTSOUTHING(j) { 
                                        MAST[i][j] = MAST_I;
                                        SAIL[i][j] = SAIL_PINCH;
                                        ofs_S[i][j] = NON;
                                }
                                else if SWESTING(j) {
                                        MAST[i][j] = MAST_J;
                                        SAIL[i][j] = SAIL_RSWELL;
                                        ofs_S[i][j] = NON;
                                }
                                else if STRICTWESTING(j) {
                                        MAST[i][j] = MAST_J;
                                        SAIL[i][j] = SAIL_DR;
                                        ofs_S[i][j] = ONE;
                                }
                                else if NWESTING(j) {
                                        MAST[i][j] = MAST_J;
                                        SAIL[i][j] = SAIL_RSWELL;
                                        ofs_S[i][j] = NON;
                                }
                        }
                        else if WESTING(i) {
                                MAST[i][j] = MAST_L;
                                SAIL[i][j] = SAIL_DL;
                                ofs_S[i][j] = ONE;
                        }
                }
        }
}
/* Set boat as active */
void nominate_boat(struct mob_t *mob)
{
        _BOAT = mob;
}
/* Initialize and create a new boat struct mob_t */
struct mob_t *new_boat(struct map_t *map)
{
        struct mob_t *mob = new_mob(map, 2, 4, 3, 3);
        mob->sw = new_sw(&boat_nibs, &boat_opts);
        mob->sw->tree = new_tree();
        rb_insert(mob->sw->tree, 13); // Key

        WINDOW *win = panel_window(mob->pan);
        wbkgrnd(win, &OCEAN[0]);

        vrt_refresh();

        return mob;
}
/* Acceptable arguments 

             win 
             pan             
             dim ------------- h
  hdg ------ now               w
  rig        req ------ hdg    y0
  anc                   rig    x0
                        anc    dy
                               dx
                               n
Return bits of boat state */
/*int get_boat(struct mob_t *mob, int a, int b)*/
/*{*/
        /*int buf = 0;*/
        /*struct shipstate *state;*/

        /*sem_wait(mob->sem);*/
                /*BOAT *boat = (BOAT *)mob->obj;*/
        /*sem_post(mob->sem);*/

        /*switch (a) {*/
        /*case __now__:*/
                /*state = &(boat->now);*/
                /*break;*/
        /*case __req__:*/
                /*state = &(boat->req);*/
                /*break;*/
        /*case __buf__:*/
                /*state = &(boat->buf);*/
                /*break;*/
        /*}*/
        /*switch (b) {*/
        /*case __hdg__:*/
                /*buf = state->hdg;*/
                /*break;*/
        /*case __rig__:*/
                /*buf = state->rig;*/
                /*break;*/
        /*case __anc__:*/
                /*buf = state->anc;*/
                /*break;*/
        /*}*/
        /*return buf;*/
/*}*/
/*[> Set the global wind state <]*/
/*void set_boat(struct mob_t *mob, int a, int b, int val)*/
/*{*/
        /*struct shipstate *state;*/

        /*sem_wait(mob->sem);*/
                /*BOAT *boat = (BOAT *)mob->obj;*/
        /*sem_post(mob->sem);*/

        /*switch (a) {*/
        /*case __now__:*/
                /*state = &(boat->now);*/
                /*break;*/
        /*case __req__:*/
                /*state = &(boat->req);*/
                /*break;*/
        /*case __buf__:*/
                /*state = &(boat->buf);*/
                /*break;*/
        /*}*/
        /*switch (b) {*/
        /*case __hdg__:*/
                /*state->hdg = val;*/
                /*break;*/
        /*case __rig__:*/
                /*state->rig = val;*/
                /*break;*/
        /*case __anc__:*/
                /*state->anc = val;*/
                /*break;*/
        /*}*/

/*}*/
/*void sync_orders(void)*/
/*{*/
        /*int newhdg, newrig, newanc;*/

        /*newhdg = get_boat(_BOAT, __buf__, __hdg__);*/
        /*newrig = get_boat(_BOAT, __buf__, __rig__);*/
        /*newanc = get_boat(_BOAT, __buf__, __anc__);*/

        /*set_boat(_BOAT, __req__, __hdg__, newhdg);*/
        /*set_boat(_BOAT, __req__, __rig__, newrig);*/
        /*set_boat(_BOAT, __req__, __anc__, newanc);*/
/*}*/
/* Refresh the graphics used to draw a boat, and update its .now to reflect 
 * the current values in &.ord */
void sync_boat(void)
{
        seek_heading();

        sem_wait(_BOAT->sem);
                WINDOW *win = panel_window(_BOAT->pan);
        sem_post(_BOAT->sem);

        int H = get_sw(_BOAT->sw, 13, HDG);
        int h;
        int R = get_sw(_BOAT->sw, 13, SAI);
        int G = get_sw(_BOAT->sw, 13, GUN);
        int W = get_wind(__dir__);

        /*H ^= 0x3; */
        /*H >>= 2;*/

        werase(win); /* Erase window */

        if ((R!=POL))  mvwadd_wchstr(win, 0, ofs_S[H][W], SAIL[H][W]);
        else           mvwadd_wchstr(win, 0, ofs_M[H][W], MAST[H][W]);

        if NORTHING(H)      h = HUL_N;
        else if EASTING(H)  h = HUL_E;
        else if SOUTHING(H) h = HUL_S;
        else if WESTING(H)  h = HUL_W;

        mvwadd_wchstr(win, 1, ofs_H[H], GGG->hull[h]->cch);

        switch (G) {
        case FIR:
                mvwadd_wch(win, 1, 1, GGG->gunf[0]->cch);
                if (roll_fair(2))
                        set_sw(_BOAT->sw, 13, GUN, LD0);
                break;
        case LD0:
                mvwadd_wch(win, 1, 1, GGG->guns[0]->cch);
                if (roll_fair(5))
                        set_sw(_BOAT->sw, 13, GUN, RDY);
                break;
        }

        scr_refresh();
}

/* Calculate the movement of the boat panel from the wind and hdg value */
void *sail_boat(void *ptr)
{
        sync_boat();
        /*int rig = get_boat(_BOAT, __now__, __rig__);*/
        /*int hdg = get_boat(_BOAT, __now__, __hdg__);*/
        int hdg = get_sw(_BOAT->sw, 13, HDG);
        /*wprintw(DIAGNOSTIC_WIN, "%u", hdg);*/
        int sai = get_sw(_BOAT->sw, 13, SAI);
        int wind = get_wind(__dir__);

        if ((sai != POL)/*&&(wind != hdg)*/) { /*not in irons*/
                if (hdg<=NE || hdg>=NW)
                        move_mob(_BOAT, 'u');
                else if (hdg>NE && hdg<SE)
                        move_mob(_BOAT, 'r');
                else if (hdg>=SE && hdg<=SW)
                        move_mob(_BOAT, 'd');
                else if (hdg<NW && hdg>SW)
                        move_mob(_BOAT, 'l');
        }
        scr_refresh();
        return NULL;
} 
/* Issue orders to the ORDERS global */
void order_boat(int order, int val)
{
        int hdg = get_sw(_BOAT->sw, 13, HDG);
        /*int sai = get_sw(_BOAT->sw, 13, SAI);*/

                switch(order) { 
                case 'p':
                        hdg = (hdg > 0) ? hdg-1 : NNW;
                        set_sw(_BOAT->sw, 13, HDG, hdg);
                        mark_hdg('L');
                        break;
                case 's':
                        hdg = (hdg < NNW) ? hdg+1 : NORTH;
                        set_sw(_BOAT->sw, 13, HDG, hdg);
                        mark_hdg('R');
                        break;
                case 'f':
                        set_sw(_BOAT->sw, 13, GUN, FIR);
                        break;
                case 'R':
                        set_sw(_BOAT->sw, 13, SAI, MAI); // Set mainsail
                        break;
                case 'r':
                        set_sw(_BOAT->sw, 13, SAI, POL); // Run with bare poles 
                        break;
                }
        sync_boat();
}


/* ========================================================================== */
/* BOAT DECK VIEW                                                        {{{1 */
/* -------------------------------------------------------------------------- */
   typedef struct deck_t {
           WINDOW *parent;
           WINDOW *flag;
   } BOATSTACK;

/* BOAT DECK GRAPHICS                                                    {{{2 */
/* FLAG GRAPHICS                                                         {{{3 */
wchar_t *_flag[56] = {
/* In irons, POS = 0 -------------------------------------------------------- */
L"                                   ◠◡◠∙                                        ",
L"                                   ◠◡◠│                                        ",
L"                                      │                                        ",
L"                                   ◡◠◡∙                                        ",
L"                                   ◡◠◡│                                        ",
L"                                      │                                        ",
/* Running, POS = 1 --------------------------------------------------------- */
L"                                      ∙◠◡◠                                     ",
L"                                      │◠◡◠                                     ",
L"                                      │                                        ",
L"                                      ∙◡◠◡                                     ",
L"                                      │◡◠◡                                     ",
L"                                      │                                        ",
/* Close hauled (port wind) POS = 2 ----------------------------------------- */
L"                                      ∙                                        ",
L"                                     ⎠│                                        ",
L"                                     ⎠│                                        ",
L"                                      ∙                                        ",
L"                                     ⎛│                                        ",
L"                                     ⎠│                                        ",
L"                                      ∙                                        ",
L"                                     ⎠│                                        ",
L"                                    ⎛ │                                        ",
/* Close hauled (stbd wind) POS = 3 ----------------------------------------- */
L"                                     ⎝∙                                        ",
L"                                     ⎝│                                        ",
L"                                      │                                        ",
L"                                     ⎞∙                                        ",
L"                                     ⎝│                                        ",
L"                                      │                                        ",
L"                                    ⎝ ∙                                        ",
L"                                     ⎞│                                        ",
L"                                      │                                        ",
/* Broad reach (port wind) POS = 4 ------------------------------------------ */
L"                                      ∙                                        ",
L"                                      │⎝                                       ",
L"                                      │⎝                                       ",
L"                                      ∙                                        ",
L"                                      │⎞                                       ",
L"                                      │⎝                                       ",
L"                                      ∙                                        ",
L"                                      │⎝                                       ",
L"                                      │ ⎞                                      ",
/* Broad reach (stbd wind) POS = 5 ------------------------------------------ */
L"                                      ∙⎠                                       ",
L"                                      │⎠                                       ",
L"                                      │                                        ",
L"                                      ∙⎛                                       ",
L"                                      │⎠                                       ",
L"                                      │                                        ",
L"                                      ∙ ⎠                                      ",
L"                                      │⎛                                       ",
L"                                      │                                        ",
/* Flag furled -------------------------------------------------------------- */
L"                                      ∙                                        ",
L"                                      │                                        ",
L"                                      │                                        "
};
                                                                       /* }}}3*/
/* DEFAULT WHOLE VESSEL                                                  {{{3 */
wchar_t *_whole[49] = {
L"                                      ∙◠◡                                      ",  /* 00 */
L"                                      │◠◡                                      ",  /* 01 */
L"                                      │                                        ",  /* 02 */
L"                                      ╽                                        ",  /* 03 */
L"                                      ┃                                        ",  /* 04 */
L"                                      ┃                                        ",  /* 05 */
L"                                    ┯┯┯┯┯                                      ",  /* 06 */
L"                                    ▒▓███                                      ",  /* 07 */
L"                                      ▓▏                                       ",  /* 08 */
L"                                      ▓▏                                       ",  /* 09 */
L"                                      ▓▏                                       ",  /* 10 */
L"                                      ▓▏                                       ",  /* 11 */
L"                                      ▓▏                                       ",  /* 12 */
L"                                      ▓▏                                       ",  /* 13 */
L"                                      ▓▏                                       ",  /* 14 */
L"                                      ▓▏                                       ",  /* 15 */
L"                                      ▓▏                                       ",  /* 16 */
L"                                      ▓▏                                       ",  /* 17 */
L"                                      ▓▏                                       ",  /* 18 */
L"                                      ▓▏                                       ",  /* 19 */
L"                                      ▓▏                                       ",  /* 20 */
L"                                      ▓▏                                       ",  /* 21 */
L"                                      ▓▏                                       ",  /* 22 */
L"                                      ▓▏                                       ",  /* 23 */
L"                                      ▓▏                                       ",  /* 24 */
L"                                      ▓▏                                       ",  /* 25 */
L"                                      ▓▏                                       ",  /* 26 */
L"                                      ▓▏                                       ",  /* 27 */
L"                                      ▓▏                                       ",  /* 28 */
L"                                      ▓▏                                       ",  /* 29 */
L"                                      ▓▏                                       ",  /* 30 */
L"    ┡━┓  ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",  /* 31 */
L"    ⧫ ┃                               ▓▏                                       ",  /* 32 */
L" ▁▁▁▁▁┃▁       ▁▁▁▁▁                  ▓▏                                       ",  /* 33 */
L" █▉▇▇▇▇▇▇▇▇▇▇┇┇▉▇▇▇┃           ▁▁▁▁  ▟█▙    ▁▁▁▁                               ",  /* 34 */
L"  ▉██████████┇┇▉▊ ▕┃           ▉▇▇▇▇▇▇▇▇▇▇▇▇▉▇▇┃               ┏┯┯┯┯┯┯┯┯┯┯┯┯┯┯┯",  /* 35 */
L"  █▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇██",  /* 36 */
L"  ▐█▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒██▒▒▒▒▒▒▒██▒▒▒▒▒▒▒▒▒▒▒▒██▒▒▒▒▒▒▒▒▒▒▒▒▒██▒▒▒▒▒▒▒▒▒▒██▒▒▒▒██▊",  /* 37 */
L"   ▛           ▟█   ▜▛       ▜▛            ▜▛             ▜▛          ▜▛    ██ ",  /* 38 */
L"   ▌          ▟██   ▕        ▕             ▕              ▕           ▕    ██  ",  /* 39 */
L"   ▜█▒▒▒▒▒▒▒▒▒▒▒▒██▒▒▒▒▛  ▟▒▒██▒▒▒▒▒▒▒▒▒▒▒▒██▒▒▒▒▒▒▒▒▒▒▒▒▒██▒▒▒▒▒▒▒▒▒▒██▒▒██   ",  /* 40 */
L"    █▛           ▜▛      ▟█  ▜▛                                   ▒▒▒▒████     ",  /* 41 */
L"     ▌           ▕      ▟██  ▕                                    ▒▒▒███       ",  /* 42 */
L"     ▜           ▟█▒▒▒▒▒███▒▒██▆▅▄▃▂▁                             ▒███         ",  /* 43 */
L"     ▐█▒▒▒▒▒▒▒▒▒▒██▒▒▒▒▒███▒▒██▒▒▒▒▒▒▒▒▒▒▒▒██▒▒▒▒▒▒▒▒▒▒▒▒▒██▒▒▒▒▒▒▒█           ",  /* 44 */
L"     ▐█████▒▒▒▒▒▒██▒▒▒▒▒███▒▒██▒▒▒▒▒▒▒▒▒▒▒▒██▒▒▒▒▒▒▒▒▒▒▒▒▒██▒▒▒▒▒███            ",  /* 45 */
L"     ▇▇▇▎▜████████████████████████████████████████████████████████▛            ",  /* 46 */
L"     ██▉▎  ▜████████████████████████████████████████████████████▛              ",  /* 47 */
L"     ██▉▎                                                                      "   /* 48 */
};
                                                                       /* }}}3*/
/* BOOM GRAPHICS                                                         {{{3 */
wchar_t *_boom[41] = {
L"    ┡━┓  ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓   ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓    ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓     ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓      ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓       ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓        ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓         ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓          ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓           ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓            ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓             ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓              ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓               ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓                ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓                 ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓                  ▃▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓                   ▃▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓                    ▃▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓                     ▃▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓                      ▃▃▃▃▃▃▃▃▃▓▏                                       ",
L"    ┡━┓                     ▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓                    ▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓                   ▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓                  ▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓                 ▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓                ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓               ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓              ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓             ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓            ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓           ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓          ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓         ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓        ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓       ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓      ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓     ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓    ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓   ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       ",
L"    ┡━┓  ▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃▃█▏                                       "
};
/*                                                                        }}}3*/
/*                                                                        }}}2*/

   /*PANEL *_DECK;*/
/* ========================================================================== */
/* -------------------------------------------------------------------------- */
 /*-------------------------------------------------------------------------- */
   /*void deck_init(void)*/
   /*{*/
     /*BOATSTACK *cake = MALLOC(BOATSTACK);*/
     /*if (cake == NULL) perror ("BOATSTACK was not allocated properly!");*/
 
     /*cake->parent = newwin(LINES, COLS, 0, 0);*/
     /*cake->flag   = derwin(cake->parent, 3, COLS, 0, 0);*/
 
     /*_DECK = new_panel(cake->parent);*/
     /*set_panel_userptr(_DECK, cake);*/
 
     /*hide_panel(_DECK);*/
 
     /*update_panels();*/
     /*doupdate();*/
   /*}*/
   /*int FlagFetch(void)*/
   /*{*/
     /*int pos = get_pointofsail();  */
     /*return (pos+(3*pos));*/
   /*}*/
/* -------------------------------------------------------------------------- */
   /*Draw the deck on the deck panel*/
 /*-------------------------------------------------------------------------- */
   /*void draw_deck(void)*/
   /*{*/
     /*BOATSTACK *cake = panel_userptr(_DECK);*/

     /*int ind = FlagFetch();*/
     /*int i;*/
     /*for (i=0; i<3; i++) {*/
         /*mvwaddwstr(cake->flag, i, 0, _flag[ind+i]);*/
     /*}*/
     /*for (i=3; i<50; i++) {*/
         /*mvwaddwstr(cake->parent, i, 0, _whole[i]);*/
     /*}*/
     /*update_panels();*/
     /*doupdate();*/
   /*}*/
/* -------------------------------------------------------------------------- */
   /*Toggles the visibility of the panel containing the deck view*/
   /*-------------------------------------------------------------------------- */
   /*void tog_deck(void)*/

   /*{*/
     /*if (panel_hidden(_DECK)) {*/
        /*draw_deck();*/
        /*show_panel(_DECK);*/
     /*}*/
     /*else*/
        /*hide_panel(_DECK);*/
   /*}*/
/*                                                                       }}}1 */
/* ========================================================================== */
