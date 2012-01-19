// vim:fdm=marker
/*******************************************************************************
    FILENAME:  boat.c
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

#include "lib/list/list.h"
#include "lib/gfx.h"
#include "lib/memmacro.h"

#include "test.h"

#include "sprite.h"
#include "terrain.h"
#include "palette.h"
#include "weather.h"
#include "control.h"
#include "instruments.h"
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
enum boat_options {
        __now__ = 0, 
        __req__ = 1, 
        __buf__ = 2, 

        __hdg__ = 3, 
        __rig__ = 4, 
        __anc__ = 5
};

struct shipstate {
        int hdg;
        int rig;
        int anc;
};

typedef struct boat_t {
        struct shipstate now; /* initialize to zero */
        struct shipstate req;
        struct shipstate buf;
} BOAT;

MOB *_BOAT;

const wchar_t gMAST_I[] = L"⎢";
const wchar_t gMAST_J[] = L"⎦";
const wchar_t gMAST_L[] = L"⎣";
const wchar_t gHULL_V[] = L"◥◤";
const wchar_t gHULL_W[] = L"◥■◤";
const wchar_t gSAIL_DL[] = L"⎝";
const wchar_t gSAIL_DR[] = L"⎠";
const wchar_t gSAIL_UL[] = L"⎛";
const wchar_t gSAIL_UR[] = L"⎞";
const wchar_t gSAIL_PINCH[] = L"⎠⎝";
const wchar_t gSAIL_SWELL[] = L"⎛⎞";
const wchar_t gSAIL_LSWELL[] = L"⎛⎝";
const wchar_t gSAIL_RSWELL[] = L"⎠⎞";

cchar_t MAST_I[2], MAST_J[2], MAST_L[2];
cchar_t HULL_V[3], HULL_W[4];
cchar_t SAIL_DL[2], SAIL_DR[2], SAIL_UL[2], SAIL_UR[2];
cchar_t SAIL_PINCH[3], SAIL_SWELL[3], SAIL_LSWELL[3], SAIL_RSWELL[3];

cchar_t *HULL[16];
cchar_t *SAIL[16][16];
cchar_t *MAST[16][16];

int ofs_H[16];
int ofs_S[16][16];
int ofs_M[16][16];

static int NON = 0;
static int ONE = 1;
static int TWO = 2;
/******************************************************************************/
/* Initialize the standing orders global variable and the semaphores used for 
 * locking boat objects. */
void boat_init(void)
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
                setcchar(&HULL_W[i], &gHULL_W[i], 0, BOAT_WOOD, NULL);
        }
        for (i=0; i<16; i++) {

                /* Hull is dependent on heading only */
                if NORTHING(i)      HULL[i] = HULL_V;
                else if EASTING(i)  HULL[i] = HULL_W;
                else if SOUTHING(i) HULL[i] = HULL_V;
                else if WESTING(i)  HULL[i] = HULL_W;

                /* Set hull offset */
                ofs_H[i] = NON;

                for (j=0; j<16; j++) {

                        /* Set mast offset */
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
void nominate_boat(MOB *mob)
{
        _BOAT = mob;
}
/* Initialize and create a new boat MOB */
MOB *new_boat(ENV *env)
{
        BOAT *boat = MALLOC(BOAT);
        if (boat == NULL) 
                perror ("BOAT was not allocated properly!");

        MOB *mob = new_mob(boat, env, 2, 4, 0, 0);

        WINDOW *win = panel_window(mob->pan);
        wbkgrnd(win, &OCEAN[0]);

        master_refresh();

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
int get_boat(MOB *mob, int a, int b)
{
        int buf = 0;
        struct shipstate *state;

        sem_wait(mob->sem);
                BOAT *boat = (BOAT *)mob->obj;
        sem_post(mob->sem);

        switch (a) {
        case __now__:
                state = &(boat->now);
                break;
        case __req__:
                state = &(boat->req);
                break;
        case __buf__:
                state = &(boat->buf);
                break;
        }
        switch (b) {
        case __hdg__:
                buf = state->hdg;
                break;
        case __rig__:
                buf = state->rig;
                break;
        case __anc__:
                buf = state->anc;
                break;
        }
        return buf;
}
/* Set the global wind state */
void set_boat(MOB *mob, int a, int b, int val)
{
        struct shipstate *state;

        sem_wait(mob->sem);
                BOAT *boat = (BOAT *)mob->obj;
        sem_post(mob->sem);

        switch (a) {
        case __now__:
                state = &(boat->now);
                break;
        case __req__:
                state = &(boat->req);
                break;
        case __buf__:
                state = &(boat->buf);
                break;
        }
        switch (b) {
        case __hdg__:
                state->hdg = val;
                break;
        case __rig__:
                state->rig = val;
                break;
        case __anc__:
                state->anc = val;
                break;
        }

}
void sync_orders(void)
{
        int newhdg, newrig, newanc;

        newhdg = get_boat(_BOAT, __buf__, __hdg__);
        newrig = get_boat(_BOAT, __buf__, __rig__);
        newanc = get_boat(_BOAT, __buf__, __anc__);

        set_boat(_BOAT, __req__, __hdg__, newhdg);
        set_boat(_BOAT, __req__, __rig__, newrig);
        set_boat(_BOAT, __req__, __anc__, newanc);
}
/* Refresh the graphics used to draw a boat, and update its .now to reflect 
 * the current values in &.ord */
void sync_boat(void)
{
        seek_heading();

        sem_wait(_BOAT->sem);
                WINDOW *win = panel_window(_BOAT->pan);
        sem_post(_BOAT->sem);

        int H = get_boat(_BOAT, __now__, __hdg__);
        int R = get_boat(_BOAT, __now__, __rig__);
        int W = get_wind(__dir__);

        werase(win); /* Erase window */

        if ((R==1))  mvwadd_wchstr(win, 0, ofs_S[H][W], SAIL[H][W]);
        else         mvwadd_wchstr(win, 0, ofs_M[H][W], MAST[H][W]);
        mvwadd_wchstr(win, 1, ofs_H[H], HULL[H]);

        master_refresh();
}
/* Calculate the movement of the boat panel from the wind and hdg value */
void *sail_boat(void *ptr)
{
        sync_boat();
        int rig = get_boat(_BOAT, __now__, __rig__);
        int hdg = get_boat(_BOAT, __now__, __hdg__);
        int wind = get_wind(__dir__);

        if ((rig == 1)&&(wind != hdg)) { /* not in irons */
                if NORTHING(hdg)
                        move_mob(_BOAT, 'u');
                else if EASTING(hdg)
                        move_mob(_BOAT, 'r');
                else if SOUTHING(hdg)
                        move_mob(_BOAT, 'd');
                else if WESTING(hdg)
                        move_mob(_BOAT, 'l');
        }
        master_refresh();
        return NULL;
} 
/* Issue orders to the ORDERS global */
void order_boat(int order, int val)
{
        int hh = get_boat(_BOAT, __buf__, __hdg__);
        int rr = get_boat(_BOAT, __now__, __rig__);

                switch(order) { 
                case 'p':
                        set_boat(_BOAT, __buf__, __hdg__, ((16+(hh-1))%16));
                        mark_hdg('L');
                        break;
                case 's':
                        set_boat(_BOAT, __buf__, __hdg__, ((hh+1)%16));
                        mark_hdg('R');
                        break;
                case 'r':
                        set_boat(_BOAT, __now__, __rig__, (rr+1)%2); /* toggle 0/1 */
                        break;
                case 'x':
                        sync_orders();
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
