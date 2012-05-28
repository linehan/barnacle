/* 
 * barnacle.c -- main
 * Performs checks, preps the engine, and enters the main loop.
 * For a very long time this file was called "dork.c".
 * 
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "com/barnacle.h"

#include "gfx/gfx.h"
#include "map/map.h"
#include "test/test.h"

#include "noun/noun.h"

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
        barnacle_init();

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
