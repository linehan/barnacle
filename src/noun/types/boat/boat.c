#include "../../../com/arawak.h"
#include "boat.h"

#include "../../../lib/textutils.h"
#include "../../../lib/hash.h"
#include "../../../test/test.h"
#include "../../../lib/stoc/brnoise.h"
#include "../../../mob/mob.h"
#include "../../noun_model.h"


/*
 * draw_boat -- renders a boat on the screen, based on it's current state
 * @boat: the struct boat_t pointer
 */
void draw_boat(WINDOW * win, struct boat_t *boat)
{
        static int boom;  /* Counter for the guns to refresh */
        unsigned char xo; /* x-offset (from hull) */
        unsigned char H;  /* Current heading */ 
        unsigned char h;  /* Rounded heading */
        unsigned char R;  /* Rig (sail) state */
        unsigned char G;  /* Gun state */
        unsigned char W;  /* Current wind direction */
        int id; /* Type of boat */

        init_boatgfx();
        id = boat->type;

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
        //                                                         sail lookup
        //                                                      _____/____
        if ((R!=POL))  mvwadd_wchstr(win, 0, xo, _sail[id].cch[(stbl[H][W])]);
        else           mvwadd_wchstr(win, 0, xo, _mast[id].cch[(mtbl[H][W])]);
        //                                                      `````\````
        //                                                       mast lookup
        mvwadd_wchstr(win, 1, 0, _hull[id].cch[h]);

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

        struct noun_t *noun;
        struct boat_t *new = malloc(sizeof(struct boat_t)); 

        noun = new_noun(name, SAILBOAT, 0, new);
        noun_set_render(noun, sail_boat);
        noun_set_modify(noun, boat_control);

        mob_cfg(&noun->mob, map, BOAT_H, BOAT_W, 0, 0);
        init_path(&noun->mob.path, BOAT_PATH_Y, BOAT_PATH_X, BOAT_PATH_LEN);
        wbkgrnd(mob_win(&noun->mob), mkcch(br_dis(0), 0, SEA_MED));
}



/*
 * boat_control -- alter the state of a boat object
 * @obj: void * to a noun_t
 * @ord: the order to be made
 * @val: the value of the order 
 */
int boat_control(void *mynoun, int order)
{
        struct noun_t *noun = (struct noun_t *)mynoun;
        struct boat_t *boat = (struct boat_t *)noun->obj;

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
        case 'b':
                return (MODE_RELEASE);
                break;
        }
        draw_boat(mob_win(&noun->mob), boat);

        return MODE_PERSIST;
}



/*
 * sail_boat -- render the state of a boat object
 * @mynoun: void * to a noun_t
 */
void sail_boat(void *mynoun)
{
        struct noun_t *noun = (struct noun_t *)mynoun;
        struct boat_t *boat = (struct boat_t *)noun->obj;

        int H, S, W; // Heading, sail and wind enums.

        draw_boat(mob_win(&noun->mob), boat);

        H = get_nibble(boat->state, HDG);
        S = get_nibble(boat->state, SAI);
        W = 3;

        if ((S!=POL)&&(W!=H)) { /*not in irons*/
                if NORTHING(H)          mob_move(&noun->mob, 'u');
                else if EASTING(H)      mob_move(&noun->mob, 'r');
                else if SOUTHING(H)     mob_move(&noun->mob, 'd');
                else if WESTING(H)      mob_move(&noun->mob, 'l');
        }
        else path_pop(&noun->mob.path);

        mob_path(&noun->mob); // If it's down here, after draw_boat(), it makes
                         // a neat "tugging" effect as the boat moves
        scr_refresh();
} 

