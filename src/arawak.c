/******************************************************************************
  FILENAME:  dork.c
  This is the main program. 
*******************************************************************************/
#include "com/arawak.h"

#include <stdio.h>
#include <stdlib.h>

#include "lib/list.h"
#include "gfx/gfx.h"
#include "map/map.h"
#include "map/special/arena.h"
#include "map/terrain.h"
#include "test/test.h"
#include "lib/pos.h"
#include "lib/morton.h"

#include "noun/noun.h"
#include "noun/nounmenu.h"
#include "noun/nounmenu_control.h"

#include "eng/init.h"
#include "eng/loop.h"
#include "map/special/cave.h"
#include "map/special/arena.h"
#include "map/flow.h"
#include "noun/types/boat/boat.h"
#include "noun/types/creature/creature.h"
#include "gfx/ui/titlecard.h"
#include "equip/equipment_items.h"


/* Rolling start */
int main(int argc, char *argv[]) 
{
        arawak_init();

        MAPBOOK = new_mapbook();

        if (!argv[1])
                map_set_extra(map_preset_arena());
        else if (strcmp(argv[1], "-arena") == 0)
                map_set_extra(map_preset_arena());
        else if (strcmp(argv[1], "-cave") == 0)
                map_set_extra(new_cave());

        print_status("\n ALL OK\n");
        print_hold("Press any key to continue");

        map_roll(ACTIVE, 0);
        map_roll(ACTIVE, 0);

        /*new_boat(FIELD, FUNBOAT, "Afarensis");*/
        /*set_mob(noun_mob("Afarensis"), false);*/

        dock_toggle();

        init_pair(FLEX, BLACK, WHITE);
        init_pair(FLEX2, BLACK, WHITE);

        new_noun("Guy", PERSON, 0);
        new_noun("Dummy", DUMMY, 0);

        nn("Guy")->mobile(true);
        nn("Dummy")->mobile(true);

        nn("Guy")->step('d');
        nn("Guy")->step('d');
        nn("Guy")->step('d');

        nn("Dummy")->step('u');

        fork_flow();

        start_event_watchers(); /* Enter the event loop */

        endwin(); /* end curses mode */
        return 0;
}

