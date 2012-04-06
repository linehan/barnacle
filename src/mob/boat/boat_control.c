#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */

#include "boat_model.h"
#include "../../test/test.h"

////////////////////////////////////////////////////////////////////////////////
void sail_boat(struct boat_t *boat)
{
        int H, S, W; // Heading, sail and wind enums.

        draw_boat(boat);

        H = get_nibble(boat->state, HDG);
        S = get_nibble(boat->state, SAI);
        /*#include "../geo/weather.h"*/
        /*W = get_wind(__dir__);*/
        W = 1;

        if ((S!=POL)&&(W!=H)) { /*not in irons*/
                if NORTHING(H)          move_boat(boat, 'u');
                else if EASTING(H)      move_boat(boat, 'r');
                else if SOUTHING(H)     move_boat(boat, 'd');
                else if WESTING(H)      move_boat(boat, 'l');
        }
        else path_pop(&boat->path);

        boat_path(boat); // If it's down here, after draw_boat(), it makes
                         // a neat "tugging" effect as the boat moves
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
        




////////////////////////////////////////////////////////////////////////////////
void boat_control(struct boat_t *boat, int order, int val)
{
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
        case '@':
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
