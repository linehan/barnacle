// vim:fdm=marker
/*******************************************************************************
  FILENAME:  dork.c
  This is the main program. 
*******************************************************************************/
#define _XOPEN_SOURCE_EXTENDED = 1  /* extended character sets */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <math.h>

#include "lib/llist/list.h"

#include "gfx/gfx.h"
#include "map/map.h"
#include "map/terrain.h"
#include "map/weather.h"
#include "test/test.h"
#include "lib/ufo.h"
#include "lib/morton.h"

#include "noun/noun_model.h"
#include "noun/noun_control.h"

#include "eng/init.h"
#include "eng/loop.h"

#include "mob/mob_model.h"
#include "mob/boat/boat_model.h"
#include "gfx/ui/titlecard.h"

/* Day-toh-nah */
int main() 
{
        arawak_init();

        struct map_t *pad = new_map((LINES*3), (COLS*3), LINES, COLS, 0, 0);
        gen_map(pad);
        GLOBE = pad;

        print_hold("ALL OK -- Press any key to continue");

        roll_map(pad, 0);
        roll_map(pad, 0);

        uint32_t key;
        key = new_boat(pad, FUNBOAT, "Afarensis");

        draw_boat(get_boat("Afarensis"));

        toggle_instrument_panel();
        toggle_instrument_panel();

        draw_compass();
        load_noun_test();
        view_dock();
        choose_noun('*');

        /*struct noun_t *noun = get_noun("Robert Aruga");*/
        /*set_mob(&noun->mob, GLOBE, 1, 1, 4, 4);*/

        mvwp(DIAGNOSTIC_WIN, 0, 10, br_atl(8), PUR_RED);
        mvwp(DIAGNOSTIC_WIN, 1, 10, br_atm(0), PUR_RED);


        start_event_watchers();

        endwin(); /* end curses mode */
        return 0;
}

