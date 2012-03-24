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
#include <stdint.h>

#include "../gfx/gfx.h"
#include "../gfx/sprite.h"
#include "../gfx/palette.h"
#include "../geo/terrain.h"
#include "../geo/weather.h"
#include "../pan/instruments.h"
#include "../pan/test.h"

#include "../lib/llist/list.h"
#include "../lib/bitwise.h"
#include "../lib/ufo.h"

#include "mob.h"
#include "boat.h"
#include "../gfx/gfx_boat.h"

/*******************************************************************************
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
*******************************************************************************/
#define BOAT_KIND 'b'

/*struct dom_t *DOMAIN;*/

struct rb_tree *BOAT_TREE;
struct rb_node *BOAT_NODE;
uint32_t TEMPKEY = 0;

struct mob_t *_BOAT;

struct boat_gfx *tester;

uint32_t keys[10];
/******************************************************************************/
/*
  Spool up the genera for each of the graphical elements that comprise
  a boat. All of the graphics can be found in "../gfx/gfx_boat.h"
*/
void init_boats(void)
{
        /*DOMAIN = register_dom(BOAT_KIND, boat_nibs, boat_opts);*/
        BOAT_TREE = new_tree(1);

        unsigned char i;
        i = HULLCOUNT;  while (i-->0) { build_gpkg(&_hull[i]); };
        i = MASTCOUNT;  while (i-->0) { build_gpkg(&_mast[i]); };
        i = SAILCOUNT;  while (i-->0) { build_gpkg(&_sail[i]); };
        i = GUNCOUNT;   while (i-->0) { build_gpkg(&_cann[i]); };

        MY_BOAT(test, FUNBOAT);
        tester = test;
}

/*
  Create a new boat mob. This is essentially just creating a mob of genera
  "boat". That genera is associated with a set of nibbles and options that
  correspond to it's states and settings. This could be generalized.
*/
uint32_t new_boat(struct map_t *map)
{
        // Generate ufo and semaphore.
        struct mob_t *new = new_mob(map, 2, 4, 3, 3);

        // Make the panel.
        WINDOW *win = newwin(2, 4, 3, 3);
        wbkgrnd(win, &OCEAN[0]);
        new->pan = new_panel(win);
        /*mob_setpath(new, 1, 0, 13);*/

        // Register the mob_t as an item and return the key.
        /*keys[0] = register_itm(BOAT_KIND, new);*/

        rb_store(BOAT_TREE, TEMPKEY, new);
        return (TEMPKEY);
}

void nominate_boat(uint32_t key)
{
        BOAT_NODE = rb_retreive(BOAT_TREE, key);
        _BOAT = (struct mob_t *)BOAT_NODE->extra;
}
/*
  The boat's state is checked and any changes of state result in some action
  being taken. For example, if the state indicates the guns are in a state
  of firing, a muzzle flash will be drawn. If the heading has changed, then
  a new hull orientation will be selected and drawn, etc.

  The "sync_" function should be a staple of each type of object we use.
  this function maps the boat's state to its actions.
*/
void sync_boat(void)
{
        unsigned char h;                                // Rounded heading
        unsigned char xo;                               // x-offset (from hull)
        unsigned char H, R, G, W;                       // States
        cchar_t **sail, **mast, **hull, **gunf, **guns; // Shorter nicknames

        /*seek_heading(); // Tumble the compass (see "../pan/instruments.h")*/

        // Point nicknames at appropriate graphics arrays.
        sail = tester->sail->cch; 
        mast = tester->mast->cch; 
        hull = tester->hull->cch; 
        gunf = tester->gunf->cch; 
        guns = tester->guns->cch; 

        sem_wait(&_BOAT->sem);
                WINDOW *win = panel_window(_BOAT->pan);
        sem_post(&_BOAT->sem);

        // Retreive states
        H = get_state(BOAT_TREE, TEMPKEY, 0, HDG);
        R = get_state(BOAT_TREE, TEMPKEY, 0, SAI);
        G = get_state(BOAT_TREE, TEMPKEY, 0, GUN);
        W = get_wind(__dir__);

        // Maps 16 directions --> 4 cardinals
        if NORTHING(H)      h = 0;
        else if EASTING(H)  h = 1;
        else if SOUTHING(H) h = 2;
        else if WESTING(H)  h = 3;

        xo = tester->hull->ofs[h]; // Offsets for hull orientation 'h'
        werase(win);
//                                                      sail lookup
//                                                     _____/____
        if ((R!=POL))  mvwadd_wchstr(win, 0, xo, sail[(stbl[H][W])]);
        else           mvwadd_wchstr(win, 0, xo, mast[(mtbl[H][W])]);
//                                                     `````\````
//                                                       mast lookup
        mvwadd_wchstr(win, 1, 0, hull[h]);

        switch (G) {
        case FIR:
                mvwadd_wch(win, 1, xo, gunf[ROU_F]);
                if (roll_fair(2))
                        set_state(BOAT_TREE, TEMPKEY, 0, GUN, LD0);
                break;
        case LD0:
                mvwadd_wch(win, 1, xo, guns[ROU_F]);
                if (roll_fair(5))
                        set_state(BOAT_TREE, TEMPKEY, 0, GUN, RDY);
                break;
        }
        scr_refresh();
}

/*
  Calculate the movement of the boat and proceed in that direction.
  Essentially, this is a glorified macro at this point, since the ufo
  macros practically automate this process.
*/
void *sail_boat(void *ptr)
{
        int H, S, W; // Heading, sail and wind enums.

        sync_boat();
        H = get_state(BOAT_TREE, TEMPKEY, 0, HDG);
        S = get_state(BOAT_TREE, TEMPKEY, 0, SAI);
        W = get_wind(__dir__);

        if ((S!=POL)&&(W!=H)) { /*not in irons*/
                if NORTHING(H)          move_mob(_BOAT, 'u');
                else if EASTING(H)      move_mob(_BOAT, 'r');
                else if SOUTHING(H)     move_mob(_BOAT, 'd');
                else if WESTING(H)      move_mob(_BOAT, 'l');
        }
        else path_pop(&_BOAT->path);

        mob_drawpath(_BOAT);
        scr_refresh();
        return NULL;
} 

/*
  This is the interface for the control thread. order_boat() is called
  with some arguments that fall out of the switch statement, triggering
  associated functions in this source file, usually to update the state
  of the boat.

  This, too, can be made more generalized, with a more friendly state
  interface in dom.h, since obviously more than just boats will need to
  have their state modified by the control thread.
*/
void order_boat(int order, int val)
{
        int H; // Heading enum

        H = get_state(BOAT_TREE, TEMPKEY, 0, HDG);

                switch(order) { 
                case 'h':
                        take_hdg();
                        break;
                case 'p':
                        H = (H>0) ? H-1 : NNW;
                        /*set_state(BOAT_TREE, TEMPKEY, 0, HDG, H);*/
                        mark_hdg('L');
                        break;
                case 's':
                        H = (H < NNW) ? H+1 : NORTH;
                        /*set_state(BOAT_TREE, TEMPKEY, 0, HDG, H);*/
                        mark_hdg('R');
                        break;
                case 'f':
                        set_state(BOAT_TREE, TEMPKEY, 0, GUN, FIR);
                        break;
                case 'R':
                        set_state(BOAT_TREE, TEMPKEY, 0, SAI, MAI); // Set mainsail
                        break;
                case 'r':
                        set_state(BOAT_TREE, TEMPKEY, 0, SAI, POL); // Run with bare poles 
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
