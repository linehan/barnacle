#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdint.h>
#include "../gfx/gfx.h"
#include "../pan/test.h"

#include "../lib/bitwise.h"
#include "../lib/ufo.h"

#include "boat.h"

////////////////////////////////////////////////////////////////////////////////
void sail_boat(struct boat_t *boat)
{
        uint32_t key;
        int H, S, W; // Heading, sail and wind enums.

        draw_boat(boat);

        H = get_nibble(boat->state, HDG);
        S = get_nibble(boat->state, SAI);
        #include "../geo/weather.h"
        W = get_wind(__dir__);

        if ((S!=POL)&&(W!=H)) { /*not in irons*/
                if NORTHING(H)          move_boat(boat, 'u');
                else if EASTING(H)      move_boat(boat, 'r');
                else if SOUTHING(H)     move_boat(boat, 'd');
                else if WESTING(H)      move_boat(boat, 'l');
        }
        else path_pop(&boat->path);

        boat_path(boat);
        scr_refresh();
} 


uint32_t turn_helm(struct boat_t *boat, int opt) 
{
        #define POINTS 14
       
        if (opt==0) {
                if (boat->helm!=(POINTS-1))
                        boat->helm = boat->helm+1;
        }
        if (opt==1) {
                if (boat->helm > 1)
                        boat->helm = (boat->helm-1);
        }

        set_compass_helm(boat->helm);

        return (boat->helm);
}

uint32_t change_course(struct boat_t *boat, int opt)
{
        #define SAILPOINTS 67
       
        if (opt==0) 
                boat->hdg = (boat->hdg==0) ? (SAILPOINTS-1) : (boat->hdg-1);
        else 
                boat->hdg = (boat->hdg==(SAILPOINTS-1)) ? 0 : (boat->hdg+1);


        set_compass_heading(boat->hdg);

        return (boat->hdg);
}
        




////////////////////////////////////////////////////////////////////////////////
void boat_control(struct boat_t *boat, int order, int val)
{
        switch(order) 
        { 
        case 'j':
                turn_helm(boat, 1);
                /*change_course(boat, 1);*/
                break;
        case 'k':
                turn_helm(boat, 0);
                /*change_course(boat, 0);*/
                break;
        case 'f':
                set_nibble(&boat->state, GUN, FIR);
                break;
        case 'R':
                set_nibble(&boat->state, SAI, MAI); // Set mainsail
                break;
        case 'r':
                set_nibble(&boat->state, SAI, POL); // Run with bare poles 
                break;
        }

        draw_boat(boat);
}
