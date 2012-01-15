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
/******************************************************************************/
/* BOAT MAP VIEW                                                         {{{1 */
/******************************************************************************/
enum { _hdg_ = 1751410432,_rig_ = 1919510272,_anc_ = 1634624256,
       _now_ = 1852798720,_req_ = 1919250688,_pan_ = 1885433344,
       _win_ = 2003398144,_dim_ = 1684630784,  _h_ = 1744860928,  
         _w_ = 1996519728, _y0_ = 2033188984, _x0_ = 2016411748, 
        _dy_ = 1685651556, _dx_ = 1685586030,  _n_ = 1845520484,
       _buf_ = 1651860992 
};

struct shipstate {
        int hdg;
        int rig;
        int anc;
};
typedef struct boat_t {
        cchar_t *aft;
        cchar_t *mid;
        cchar_t *bow;
        cchar_t *boom[2];
        cchar_t *sail[6];
        cchar_t *pole[2];
        cchar_t *mast[4][4][2]; /* mast state */
        struct shipstate now; /* initialize to zero */
        struct shipstate req;
        struct shipstate buf;
} BOAT;

static struct shipstate ORDERS;
MOB *_BOAT;
sem_t *BOAT_OBJ_LOCK;
sem_t *BOAT_MOB_LOCK;
/******************************************************************************/
/* Initialize the standing orders global variable and the semaphores used for 
 * locking boat objects. */
void boat_init(void)
{
        ORDERS.hdg = 0;
        ORDERS.rig = 0;
        ORDERS.anc = 0;

        BOAT_OBJ_LOCK = MALLOC(sem_t);
        BOAT_MOB_LOCK = MALLOC(sem_t);
        sem_init(BOAT_OBJ_LOCK, 0, 1);
        sem_init(BOAT_MOB_LOCK, 0, 1);
}
/* Set boat as active */
void nominate_boat(MOB *mob)
{
        _BOAT = mob;
}
/* Initialize and create a new boat MOB */
MOB *new_boat(void)
{
        BOAT *boat = MALLOC(BOAT);
        if (boat == NULL) 
                perror ("BOAT was not allocated properly!");

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

        /* MAST STATE                                                {{{2
        The mast object is used to retreive the proper character for the
        representation of the mast, based on the current wind direction, 
        heading, and sail status.                                             
        */
        /* wind from north ^ */
        boat->mast[0][0][0] = boat->pole[0]; /* north */
        boat->mast[0][0][1] = boat->sail[0];  
        boat->mast[0][1][0] = boat->boom[0]; /* east  */
        boat->mast[0][1][1] = boat->sail[3]; 
        boat->mast[0][2][0] = boat->boom[0]; /* south */
        boat->mast[0][2][1] = boat->sail[4]; 
        boat->mast[0][3][0] = boat->boom[1]; /* west  */
        boat->mast[0][3][1] = boat->sail[2]; 
        /* wind from south v */
        boat->mast[2][0][0] = boat->boom[1]; /* north */
        boat->mast[2][0][1] = boat->sail[5]; 
        boat->mast[2][1][0] = boat->boom[0]; /* east  */
        boat->mast[2][1][1] = boat->sail[3]; 
        boat->mast[2][2][0] = boat->pole[1]; /* south */
        boat->mast[2][2][1] = boat->sail[0]; 
        boat->mast[2][3][0] = boat->boom[1]; /* west  */
        boat->mast[2][3][1] = boat->sail[2]; 
        /* wind from east <-- */
        boat->mast[1][0][0] = boat->boom[0]; /* north */
        boat->mast[1][0][1] = boat->sail[2]; 
        boat->mast[1][1][0] = boat->pole[1]; /* east  */
        boat->mast[1][1][1] = boat->sail[1]; 
        boat->mast[1][2][0] = boat->boom[0]; /* south */
        boat->mast[1][2][1] = boat->sail[2]; 
        boat->mast[1][3][0] = boat->boom[1]; /* west  */
        boat->mast[1][3][1] = boat->sail[2];
        /* wind from west --> */ 
        boat->mast[3][0][0] = boat->boom[1]; /* north */
        boat->mast[3][0][1] = boat->sail[3]; 
        boat->mast[3][1][0] = boat->boom[0]; /* east  */
        boat->mast[3][1][1] = boat->sail[3]; 
        boat->mast[3][2][0] = boat->boom[1]; /* south */
        boat->mast[3][2][1] = boat->sail[3]; 
        boat->mast[3][3][0] = boat->pole[0]; /* west  */
        boat->mast[3][3][1] = boat->sail[0];                      /* }}}2 */

        MOB *mob = new_mob(boat, 2, 3, 0, 0);

        WINDOW *win = panel_window(mob->pan);
        wbkgrnd(win, &OCEAN);

        /* draw it for the first time */
        mvwadd_wch(win, 0, 1, boat->mast[0][2][0]);
        mvwadd_wch(win, 1, 0, boat->aft);
        wadd_wch(win,       boat->mid);
        wadd_wch(win,       boat->bow);

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
int get_boat(const char *str1, const char *str2)
{
        int buf = 0;
        struct shipstate *state;

        int32_t strnum1 =
            str1[0] << 24 | 
            str1[1] << 16 |
            str1[2] << 8  |
            str1[3];
        int32_t strnum2 =
            str2[0] << 24 | 
            str2[1] << 16 |
            str2[2] << 8  |
            str2[3];

        sem_wait(BOAT_MOB_LOCK);
                BOAT *boat = (BOAT *)_BOAT->obj;
        sem_post(BOAT_MOB_LOCK);

        sem_wait(BOAT_OBJ_LOCK);
                switch (strnum1) {
                case _now_:
                        state = &(boat->now);
                        break;
                case _req_:
                        state = &(boat->req);
                        break;
                case _buf_:
                        state = &(boat->buf);
                        break;
                }
                switch (strnum2) {
                case _hdg_:
                        buf = state->hdg;
                        break;
                case _rig_:
                        buf = state->rig;
                        break;
                case _anc_:
                        buf = state->anc;
                        break;
                }
        sem_post(BOAT_OBJ_LOCK);
        return buf;
}
/* Set the global wind state */
void set_boat(const char *str1, const char *str2, int val)
{
        int buf = 0;
        struct shipstate *state;

        int32_t strnum1 =
            str1[0] << 24 | 
            str1[1] << 16 |
            str1[2] << 8  |
            str1[3];
        int32_t strnum2 =
            str2[0] << 24 | 
            str2[1] << 16 |
            str2[2] << 8  |
            str2[3];

        sem_wait(BOAT_MOB_LOCK);
                BOAT *boat = (BOAT *)_BOAT->obj;
        sem_post(BOAT_MOB_LOCK);

        sem_wait(BOAT_OBJ_LOCK);
                switch (strnum1) {
                case _now_:
                        state = &(boat->now);
                        break;
                case _req_:
                        state = &(boat->req);
                        break;
                case _buf_:
                        state = &(boat->buf);
                        break;
                }
                switch (strnum2) {
                case _hdg_:
                        state->hdg = val;
                        break;
                case _rig_:
                        state->rig = val;
                        break;
                case _anc_:
                        state->anc = val;
                        break;
                }
        sem_post(BOAT_OBJ_LOCK);

}
void sync_orders(void)
{
        int newhdg, newrig, newanc;

        newhdg = get_boat("buf", "hdg");
        newrig = get_boat("buf", "rig");
        newanc = get_boat("buf", "anc");

        set_boat("req", "hdg", newhdg);
        set_boat("req", "rig", newrig);
        set_boat("req", "anc", newanc);
}
/* Refresh the graphics used to draw a boat, and update its .now to reflect 
 * the current values in &.ord */
void sync_boat(void)
{
        seek_heading();

        sem_wait(BOAT_MOB_LOCK);
                BOAT *boat = (BOAT *)_BOAT->obj;
                WINDOW *win = panel_window(_BOAT->pan);
        sem_post(BOAT_MOB_LOCK);

        int h = get_boat("now", "hdg");
        int r = get_boat("now", "rig");
        int w = get_wind("dir");

        int w4 = floor(w/4);
        int h4 = floor(h/4);

                switch (h) {
                case NNW:
                case NORTH:
                case NNE:
                        werase(win);
                        if (w == SOUTH) 
                                mvwadd_wch(win, 0, 1, boat->mast[w4][0][r]);
                        else
                                mvwadd_wch(win, 0, 2, boat->mast[w4][0][r]);
                        mvwadd_wch(win, 1, 1, boat->aft);
                          wadd_wch(win,       boat->bow);
                        master_refresh();
                        break;
                case WSW:
                case WEST:
                case WNW:
                        werase(win);
                        mvwadd_wch(win, 0, 1, boat->mast[w4][3][r]);
                        mvwadd_wch(win, 1, 0, boat->aft);
                          wadd_wch(win,       boat->mid);
                          wadd_wch(win,       boat->bow);
                        master_refresh();
                        break;
                case ENE:
                case EAST:
                case ESE:
                        werase(win);
                        mvwadd_wch(win, 0, 1, boat->mast[w4][1][r]);
                        mvwadd_wch(win, 1, 0, boat->aft);
                          wadd_wch(win,       boat->mid);
                          wadd_wch(win,       boat->bow);
                        master_refresh();
                        break;
                case SSE:
                case SOUTH:
                case SSW:
                        werase(win);
                        if (w == WEST) 
                                mvwadd_wch(win, 0, 2, boat->mast[w4][2][r]);
                        else           
                                mvwadd_wch(win, 0, 1, boat->mast[w4][2][r]);
                        mvwadd_wch(win, 1, 1, boat->aft);
                          wadd_wch(win,       boat->bow);
                        master_refresh();
                        break;
                }
}
/* Calculate the movement of the boat panel from the wind and hdg value */
void *sail_boat(void *ptr)
{
        sem_wait(BOAT_MOB_LOCK);
                int x = _BOAT->dim.dx;
                int y = _BOAT->dim.dy;
        sem_post(BOAT_MOB_LOCK);

        int hdg = get_boat("now", "hdg");
        int bhdg = get_boat("buf", "hdg");
        int rhdg = get_boat("req", "hdg");
        int rig = get_boat("now", "rig");
        int anc = get_boat("now", "anc");


        int wind = get_wind("dir");
        int pre = get_wind("pre");

        werase(DIAGNOSTIC_WIN);
        wprintw(DIAGNOSTIC_WIN, "NOW_HDG: %d\nBUF_HDG:%d\nREQ_HDG:%d\nRIG: %d\nANC: %d\nWIN: %d\nPRE: %d", hdg, bhdg, rhdg, rig, anc, wind, pre);

        if (rig == 1) {
                if (wind != hdg) { /* not in irons */
                        switch(hdg) {
                        case NORTH:
                                if (y > 0) y--;
                                break;
                        case NNE:
                                if (y > 0) y--;
                                break;
                        case NE:
                                if (y > 0) y--;
                                if (x < COLS) x++;
                                break;
                        case ENE:
                                if (x < COLS) x++;
                                break;
                        case EAST:
                                if (x < COLS) x++;
                                break;
                        case ESE:
                                if (x < COLS) x++;
                                break;
                        case SE:
                                if (x < COLS) x++;
                                if (y < LINES) y++;
                                break;
                        case SSE:
                                if (y < LINES) y++;
                                break;
                        case SOUTH:
                                if (y < LINES) y++;
                                break;
                        case SSW:
                                if (y < LINES) y++;
                                break;
                        case SW:
                                if (y < LINES) y++;
                                if (x > 0) x--;
                                break;
                        case WSW:
                                if (x > 0) x--;
                                break;
                        case WEST:
                                if (x > 0) x--;
                                break;
                        case WNW:
                                if (x > 0) x--;
                                break;
                        case NW:
                                if (x > 0) x--;
                                if (y > 0) y--;
                                break;
                        case NNW:
                                if (y > 0) y--;
                                break;
                        }
                        sem_wait(BOAT_MOB_LOCK);
                                move_panel(_BOAT->pan, y, x);
                                _BOAT->dim.dx = x;
                                _BOAT->dim.dy = y;
                        sem_post(BOAT_MOB_LOCK);
                }
        }
        master_refresh();
        
        return NULL;
} 
/* Issue orders to the ORDERS global */
void order_boat(int order, int val)
{
        int hh = get_boat("buf", "hdg");
        int rr = get_boat("now", "rig");

                switch(order) { 
                case 'p':
                        set_boat("buf", "hdg", ((16+(hh-1))%16));
                        mark_hdg('L');
                        break;
                case 's':
                        set_boat("buf", "hdg", ((hh+1)%16));
                        mark_hdg('R');
                        break;
                case 'h':
                        ORDERS.hdg = val;
                        break;
                case 'r':
                        set_boat("now", "rig", (rr+1)%2); /* toggle 0/1 */
                        break;
                case 'a':
                        ORDERS.anc = val;
                        break;
                case 'x':
                        sync_orders();
                        break;
                }
        sync_boat();
}
/* Return the current point of sail */
int get_pointofsail(void)
{
        int wind = get_wind("dir");
        wind = wind%4;
        sem_wait(BOAT_OBJ_LOCK); 
                int hdg  = ORDERS.hdg;
        sem_post(BOAT_OBJ_LOCK); 

        return ((abs((wind - hdg)))%4);
}
/*                                                                       }}}1 */
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
