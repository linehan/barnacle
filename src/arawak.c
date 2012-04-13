// vim:fdm=marker
/*******************************************************************************
  FILENAME:  dork.c
  This is the main program. 
*******************************************************************************/
#include "com/arawak.h"

#include <stdio.h>
#include <stdlib.h>

#include "lib/llist/list.h"

#include "gfx/gfx.h"
#include "map/map.h"
#include "map/arena.h"
#include "map/terrain.h"
#include "map/weather.h"
#include "test/test.h"
#include "lib/ufo.h"
#include "lib/morton.h"

#include "noun/noun_model.h"
#include "noun/noun_control.h"

#include "eng/init.h"
#include "eng/loop.h"

#include "mob/mob.h"
#include "noun/noun_view.h"
#include "noun/types/boat/boat.h"
#include "noun/types/person/person.h"
#include "gfx/ui/titlecard.h"

/* Day-toh-nah */
int main() 
{
        arawak_init();

        /*struct map_t *pad = new_map((LINES*3), (COLS*3), LINES, COLS, 0, 0);*/
        /*map_gen(pad, NULL, MAP_DOSMOOTH);*/

        struct map_t *pad = map_preset_arena();
        GLOBE = pad;

        print_status("\n ALL OK\n");
        print_hold("Press any key to continue");

        map_roll(pad, 0);
        map_roll(pad, 0);

        new_boat(pad, FUNBOAT, "Afarensis");
        set_mob(noun_mob("Afarensis"), false);

        toggle_instrument_panel();
        toggle_instrument_panel();

        load_noun_test();
        list_nouns(SUBJECT, ALL_NOUNS);
        list_nouns(OBJECT, ALL_NOUNS);
        view_dock();
        choose_noun('*');
        
        init_pair(FLEX, BLACK, WHITE);
        init_pair(FLEX2, BLACK, WHITE);

        new_creature("Guy", PERSON, L"д", FLEX);
        new_creature("Killer", MONSTER, L"⿍", FLEX);

        set_mob(noun_mob("Guy"), true);
        mob_move(noun_mob("Guy"), 'd');
        mob_move(noun_mob("Guy"), 'd');
        mob_move(noun_mob("Guy"), 'd');

        set_mob(noun_mob("Killer"), true);
        mob_move(noun_mob("Killer"), 'd');


        start_event_watchers(); /* Enter the event loop */

        endwin(); /* end curses mode */
        return 0;
}

