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
#include "noun/types/boat/boat.h"
#include "gfx/ui/titlecard.h"

/* Day-toh-nah */
int main() 
{
        arawak_init();

        /*[>struct map_t *pad = new_map((LINES*3), (COLS*3), LINES, COLS, 0, 0);<]*/
        /*[>gen_map(pad, NULL);<]*/
        struct map_t *pad = make_arena(LINES-30, COLS-20, 0, 0);
        GLOBE = pad;

        print_status("\n ALL OK\n");
        print_hold("Press any key to continue");

        roll_map(pad, 0);
        roll_map(pad, 0);

        new_boat(pad, FUNBOAT, "Afarensis");
        noun_render(get_noun("Afarensis"));

        toggle_instrument_panel();
        toggle_instrument_panel();

        draw_compass();
        load_noun_test();
        view_dock();
        choose_noun('*');

        struct noun_t *noun = get_noun("Robert Aruga");
        set_mob(&noun->mob, GLOBE, 1, 1, LINES/2, COLS/2);

        start_event_watchers(); /* Enter the event loop */

        endwin(); /* end curses mode */
        return 0;
}

