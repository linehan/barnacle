#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */

#include "boat_model.h"
#include "../../../lib/stoc/stoc.h"

#define HULLCOUNT 3 
#define MASTCOUNT 3 
#define SAILCOUNT 3 
#define GUNCOUNT  2 

struct gpkg _hull[HULLCOUNT]={
{ 4 , {1 , 1 , 1 , 1} , BOAT_WOOD , {L"◥◤"  , L"◥■◤"   , L"◥◤"  , L"◥■◤"}}   ,
{ 4 , {1 , 1 , 1 , 2} , BOAT_WOOD , {L"▗▅▖" , L"▃▂▃▖"  , L"▗▅▖" , L"▗▃▂▃"}}  ,
{ 4 , {1 , 2 , 1 , 2} , BOAT_WOOD , {L"▜▛"  , L"▜▄▂▃▖" , L"▗▃▖" , L"▗▃▂▄▛"}}
};
struct gpkg _mast[MASTCOUNT] = {
{ 3, {0}, BOAT_WOOD, {L"⎢",L"⎦",L"⎣"}},
{ 3, {0}, BOAT_WOOD, {L"⎢",L"⎦",L"⎣"}},
{ 3, {0}, BOAT_WOOD, {L"⎢",L"⎦",L"⎣"}}
};
struct gpkg _sail[SAILCOUNT] = {
{ 8, {0}, BOAT_WHITE, {L"⎠",L"⎝",L"⎞",L"⎛",L"⎠⎝",L"⎛⎞",L"⎛⎝",L"⎠⎞"}},
{ 8, {0}, BOAT_WHITE, {L"⎠",L"⎝",L"⎞",L"⎛",L"⎠⎝",L"⎛⎞",L"⎛⎝",L"⎠⎞"}},
{ 8, {0}, BOAT_WHITE, {L"⎠",L"⎝",L"⎞",L"⎛",L"⎠⎝",L"⎛⎞",L"⎛⎝",L"⎠⎞"}}
};
struct gpkg _cann[GUNCOUNT] = {
{ 8, {0}, GUN_FLASH, {L"✶",L"✶",L"✶",L"✶",L"✶",L"✶",L"✶",L"✶"}},
{ 8, {0}, GUN_SMOKE, {L"✺",L"✺",L"✺",L"✺",L"✺",L"✺",L"✺",L"✺"}}
};

inline void init_boatgfx(void)
{
        if (_hull[0].cch[0] != NULL) return;

        unsigned char i;
        i = HULLCOUNT;  while (i-->0) { build_gpkg(&_hull[i]); };
        i = MASTCOUNT;  while (i-->0) { build_gpkg(&_mast[i]); };
        i = SAILCOUNT;  while (i-->0) { build_gpkg(&_sail[i]); };
        i = GUNCOUNT;   while (i-->0) { build_gpkg(&_cann[i]); };
}

////////////////////////////////////////////////////////////////////////////////
void move_boat(void *obj, int dir)
{
        struct boat_t *boat = (struct boat_t *)noun->obj;

        if (dir != 'u' && dir != 'd' && dir != 'l' && dir != 'r') return;

        int y = ufo_y(boat, mob.ufo);
        int x = ufo_x(boat, mob.ufo);

        switch (dir) {
        case 'u':       
                ufo_up(boat, mob.ufo);
                if (map_hit(GLOBE, &boat->mob.ufo.obj))
                        ufo_down(boat, mob.ufo);
                break;
        case 'd':       
                ufo_down(boat, mob.ufo);
                if (map_hit(GLOBE, &boat->mob.ufo.obj)) 
                        ufo_up(boat, mob.ufo);
                break;
        case 'l':       
                ufo_left(boat, mob.ufo);
                if (map_hit(GLOBE, &boat->mob.ufo.obj))
                        ufo_right(boat, mob.ufo);
                break;
        case 'r':      
                ufo_right(boat, mob.ufo);
                if (map_hit(GLOBE, &boat->mob.ufo.obj))
                        ufo_left(boat, mob.ufo);
                break;
        }

        // Scroll down at bottom boundary
        if (ufo_y(boat, mob.ufo) == LINES-10) {
                roll_map(GLOBE, 'd');
                ufo_up(boat, mob.ufo);
        }
        // Scroll right at right boundary
        if (ufo_x(boat, mob.ufo) == COLS-10) {
                roll_map(GLOBE, 'r');
                ufo_left(boat, mob.ufo);
        }

        move_panel(boat->mob.pan, ufo_y(boat, mob.ufo), ufo_x(boat, mob.ufo));
        update_panels();

        path_push(&boat->mob.path, y, x);
        doupdate();
}



////////////////////////////////////////////////////////////////////////////////
void boat_path(struct boat_t *boat)
{
        werase(PEEK(GLOBE->L[HIG]));
        draw_path((PEEK(GLOBE->L[HIG])), &boat->mob.path);
        wrefresh((PEEK(GLOBE->L[HIG])));
        doupdate();
}



////////////////////////////////////////////////////////////////////////////////
void draw_boat(void *obj, int opt)
{
        struct boat_t *boat = (struct boat_t *)obj;

        WINDOW *win;
        unsigned char h;                                // Rounded heading
        unsigned char xo;                               // x-offset (from hull)
        unsigned char H, R, G, W;                       // States
        int id;

        init_boatgfx();

        id = boat->type;
        win = mob_win(&boat->mob);

        // Retreive states
        H = get_nibble(boat->state, HDG);
        R = get_nibble(boat->state, SAI);
        G = get_nibble(boat->state, GUN);
        W = 4;

        // Maps 16 directions --> 4 cardinals
        if NORTHING(H)      h = 0;
        else if EASTING(H)  h = 1;
        else if SOUTHING(H) h = 2;
        else if WESTING(H)  h = 3;

        xo = _hull[id].ofs[h]; // Offsets for hull orientation 'h'

        werase(win);
//                                                               sail lookup
//                                                              _____/____
        if ((R!=POL))  mvwadd_wchstr(win, 0, xo, _sail[id].cch[(stbl[H][W])]);
        else           mvwadd_wchstr(win, 0, xo, _mast[id].cch[(mtbl[H][W])]);
//                                                              `````\````
//                                                               mast lookup
        mvwadd_wchstr(win, 1, 0, _hull[id].cch[h]);

        static int boom;
        switch (G) {
        case FIR:
                mvwadd_wch(win, 1, xo, _cann[0].cch[0]);
                if (++boom > 1)
                        set_nibble(&boat->state, GUN, LD0);
                break;
        case LD0:
                mvwadd_wch(win, 1, xo, _cann[1].cch[0]);
                if (boom-- == 1)
                        set_nibble(&boat->state, GUN, RDY);
                break;
        }
        scr_refresh();
}





