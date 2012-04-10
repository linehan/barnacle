#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */

#include "boat.h"

#include "../../../lib/textutils.h"
#include "../../../lib/hash.h"
#include "../../../test/test.h"
#include "../../../lib/stoc/brnoise.h"
#include "../../../mob/mob_model.h"
#include "../../noun_model.h"

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



struct boat_t *get_boat(const char *name)
{
        struct noun_t *noun = get_noun(name);
        struct boat_t *boat;

        if (noun->type == SAILBOAT)
                boat = (struct boat_t *)noun->obj;

        return (boat);
}

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

/*
 * Draw the snake path behind a boat 
 */
void boat_path(struct boat_t *boat)
{
        werase(PEEK(GLOBE->L[HIG]));
        draw_path((PEEK(GLOBE->L[HIG])), &boat->mob.path);
        wrefresh((PEEK(GLOBE->L[HIG])));
        doupdate();
}


/*
 * draw_boat -- renders a boat on the screen, based on it's current state
 * @boat: the struct boat_t pointer
 */
void draw_boat(struct boat_t *boat)
{
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



uint32_t turn_helm(struct boat_t *boat, int opt) 
{
        #define POINTS 15
        #define HARD 4
       
        switch (opt)
        {
        case TURN_L:
                if (boat->helm > 0)
                        boat->helm = (boat->helm-1);
                break;
        case TURN_R:
                if (boat->helm < POINTS)
                        boat->helm = boat->helm+1;
                break;
        case TURN_HL:
                if (boat->helm > HARD)
                        boat->helm = (boat->helm-HARD);
                break;
        case TURN_HR:
                if (boat->helm < (POINTS-HARD))
                        boat->helm = (boat->helm+HARD);
                break;
        }

        set_compass_helm(boat->helm);

        return (boat->helm);
}

uint32_t change_course(struct boat_t *boat, int opt)
{
        #define SAILPOINTS 15
       
        switch (opt)
        {
        case TURN_L:
                boat->hdg = (boat->hdg > 0) ? (boat->hdg-1) : SAILPOINTS;
                break;
        case TURN_R:
                boat->hdg = (boat->hdg < SAILPOINTS) ? (boat->hdg+1) : 0;
                break;
        }

        set_nibble(&boat->state, HDG, boat->hdg);
        set_compass_heading(boat->hdg);

        return (boat->hdg);
}
        




/*
 * Allocate and initialize a new boat, which can be referenced by the key
 * generated from the fasthash() of its name string.
 *
 * @map: The map in which the boat will be set.
 * @type: An enumerated option specifying the type of boat.
 * @name: A string constant specifying the name of the boat.
 *
 * RETURNS: The hashed key value (based on the name string)
 */
void new_boat(struct map_t *map, int type, const char *name)
{
        #define BOAT_PATH_LEN 13
        #define BOAT_PATH_Y 1 
        #define BOAT_PATH_X 0
        #define BOAT_H 2
        #define BOAT_W 3

        struct boat_t *new = malloc(sizeof(struct boat_t)); 

        set_mob(&new->mob, map, BOAT_H, BOAT_W, 0, 0);
        wbkgrnd(mob_win(&new->mob), mkcch(br_dis(0), 0, SEA_MED));

        new_noun(name, SAILBOAT, 0, new);
        noun_set_render(get_noun(name), sail_boat);
        noun_set_modify(get_noun(name), boat_control);
}



/*
 * boat_control -- alter the state of a boat object
 * @obj: void * to a noun_t
 * @ord: the order to be made
 * @val: the value of the order 
 */
void boat_control(void *obj, int order)
{
        struct boat_t *boat = (struct boat_t *)obj;

        switch(order) 
        { 
        case 'j':
                turn_helm(boat, TURN_L);
                break;
        case 'k':
                turn_helm(boat, TURN_R);
                break;
        case 'h':
                turn_helm(boat, TURN_HL);
                break;
        case 'l':
                turn_helm(boat, TURN_HR);
                break;
        case '!':
                set_nibble(&boat->state, GUN, FIR);
                break;
        case 'f':
                set_nibble(&boat->state, SAI, MAI); // Set mainsail
                break;
        case 'g':
                set_nibble(&boat->state, SAI, POL); // Run with bare poles 
                break;
        }

        draw_boat(boat);
}



/*
 * sail_boat -- render the state of a boat object
 * @mynoun: void * to a noun_t
 */
void sail_boat(void *obj)
{
        struct boat_t *boat = (struct boat_t *)obj;

        int H, S, W; // Heading, sail and wind enums.

        draw_boat(boat);

        H = get_nibble(boat->state, HDG);
        S = get_nibble(boat->state, SAI);
        W = 3;

        if ((S!=POL)&&(W!=H)) { /*not in irons*/
                if NORTHING(H)          move_mob(&boat->mob, 'u');
                else if EASTING(H)      move_mob(&boat->mob, 'r');
                else if SOUTHING(H)     move_mob(&boat->mob, 'd');
                else if WESTING(H)      move_mob(&boat->mob, 'l');
        }
        else path_pop(&boat->mob.path);

        boat_path(boat); // If it's down here, after draw_boat(), it makes
                         // a neat "tugging" effect as the boat moves
        scr_refresh();
} 

