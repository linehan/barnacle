/******************************************************************************
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
#include "noun/types/creature/creature.h"
#include "gfx/ui/titlecard.h"
#include "ai/a_star_test.h"

/* Rolling start */
int main(int argc, char *argv[]) 
{
        arawak_init();

        struct map_t *sand = map_preset_sand();

        MAPBOOK = new_mapbook();
        map_set_extra(map_preset_sand());

        print_status("\n ALL OK\n");
        print_hold("Press any key to continue");

        map_roll(ACTIVE, 0);
        map_roll(ACTIVE, 0);

        new_boat(FIELD, FUNBOAT, "Afarensis");
        set_mob(noun_mob("Afarensis"), false);

        /*toggle_instrument_panel();*/
        /*toggle_instrument_panel();*/

        view_dock();
        load_noun_test();
        list_nouns(SUBJECT, ALL_NOUNS);
        list_nouns(OBJECT, ALL_NOUNS);
        choose_noun('*');
        
        init_pair(FLEX, BLACK, WHITE);
        init_pair(FLEX2, BLACK, WHITE);

        new_creature("Guy", PERSON, L"ⰾ", FLEX);
        /*new_creature("Beefman", MONSTER, L"Ⰾ", FLEX);*/

        noun_set_mob(get_noun("Guy"), true);
        mob_move(noun_mob("Guy"), 'd');
        mob_move(noun_mob("Guy"), 'd');
        mob_move(noun_mob("Guy"), 'd');

        /*noun_set_mob(get_noun("Beefman"), true);*/
        /*mob_move(noun_mob("Beefman"), 'd');*/

        fork_sweet_flow();

        start_event_watchers(); /* Enter the event loop */

        endwin(); /* end curses mode */
        return 0;
}

