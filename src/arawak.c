/******************************************************************************
  FILENAME:  dork.c
  This is the main program. 
*******************************************************************************/
#include "com/arawak.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gfx/gfx.h"
#include "map/map.h"
#include "test/test.h"

#include "noun/noun.h"
#include "noun/nounmenu.h"
#include "noun/nounmenu_control.h"

#include "eng/init.h"
#include "eng/loop.h"
#include "map/special/cave.h"
#include "map/special/arena.h"
#include "map/flow.h"
#include "gfx/ui/titlecard.h"
#include "gfx/ui/menu_inventory.h"
#include "item/item.h"
#include "eng/error.h"
#include "txt/gloss.h"
#include "gfx/ui/dock.h"


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

        dock_toggle();

        init_pair(FLEX, BLACK, WHITE);
        init_pair(FLEX2, BLACK, WHITE);

        new_noun("Guy", HUMAN, 0);
        new_noun("Dummy", HOPPER, 0);
        new_noun("Dorky", HOPPER, 0);

        nn("Guy")->mobile(true);
        nn("Guy")->player(true);

        nn("Dummy")->mobile(true);
        nn("Dorky")->mobile(true);

        nn("Guy")->step('d');
        nn("Guy")->step('d');
        nn("Guy")->step('d');

        nn("Dummy")->step('u');
        nn("Dorky")->step('r');
        nn("Dorky")->step('r');
        nn("Dorky")->step('r');

        inventory_mkmenu(&nn("Guy")->inv);

        enter_event_loop(); 

        endwin(); /* end curses mode */
        return 0;
}

