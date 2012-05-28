/* 
 * fsm.c -- Poor-quality global FSM for routing keyboard input
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
#include "../com/barnacle.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <ev.h>
#include "../lib/fsm/fsm.h"
#include "../noun/noun.h"
#include "../noun/noun.h"
#include "../gfx/gfx.h"
#include "../map/map.h"
#include "../test/test.h"
#include "../gfx/ui/menu_inventory.h"
#include "fsm.h"
#include "loop.h"
#include "tick.h"
#include "../test/looptest.h"

enum uimodes { 
        UI_COMPASS,
        UI_WORLDMAP,
        UI_NOUN,
        UI_VERB,
        UI_INSPECTOR,
        UI_PROMPT,
        UI_MOB,
        UI_BOAT,
        UI_BUILD,
        UI_INVENTORY
};

////////////////////////////////////////////////////////////////////////////////
static bool initialized;
static bool mode_locked;
static int mode;
static int oldmode;


static void setmode(int newmode)
{
        #define DEFAULT UI_WORLDMAP

        if (mode == newmode) return;
        if (mode == NOTOUCH) return;

        switch (newmode)
        {
        case MODE_PERSIST:
                mode_locked = true;
                break;
        case MODE_RELEASE:
                mode_locked = false;
                break;
        case MODE_RESTORE:
                mode = oldmode;
                mode_locked = true;
                break;
        case MODE_STARTED:
                operate_on(MODE_STARTED);
                break;
        case MODE_INITIAL:
                oldmode = mode = DEFAULT;
                mode_locked = false;
                initialized = true;
                break;
        case MODE_DEFAULT:
                newmode = DEFAULT;
        default:
                oldmode = mode;
                mode = newmode;
                mode_locked = true;
                break;
        }
}
        


////////////////////////////////////////////////////////////////////////////////
void operate_on(int input)
{
        setmode(MODE_RELEASE);

        switch (mode) 
        {
        case UI_NOUN:
                break;
        case UI_WORLDMAP:
                break;
        case UI_INSPECTOR:
                inspect_control(input);
                break;
        case UI_BOAT:
                /*noun_set_state(get_noun("Afarensis"), VERB_Keyboard, input);*/
                break;
        case UI_MOB:
                if (PLAYER)
                        sm_set(PLAYER->sm, SM_Key(input), 0); 
                break;
        case UI_INVENTORY:
                inventory_menu_control(input);
                break;
        }
}

////////////////////////////////////////////////////////////////////////////////
void director(int input)
{
        if (initialized == false) setmode(MODE_INITIAL);

        operate_on(input);

        if (mode_locked == true) return;
        else {
                switch (input) 
                { 
                case 'p':
                        game_is_paused = (game_is_paused) ? false : true;
                        break;
                case 'z':
                        dock_toggle();
                        break;
                case '^':
                        map_swap();
                        break;
                case 'm':
                        setmode(UI_MOB);
                        setmode(MODE_STARTED);
                        break;
                case 'b':
                        setmode(UI_BUILD);
                        setmode(MODE_STARTED);
                        break;
                case 'i':
                        setmode(UI_INVENTORY);
                        operate_on(MODE_STARTED);
                        break;
                case 'v':
                        setmode(UI_VERB);
                        setmode(MODE_STARTED);
                        break;
                case '?':
                        setmode(UI_INSPECTOR);
                        setmode(MODE_STARTED);
                        break;
                case '`':
                        TOGPAN(CONSOLE_PAN);
                        doupdate();
                        /*test_request();*/
                        break;
                case '~':
                        TOGPAN(DIAGNOSTIC_PAN);
                        doupdate();
                        break;
                case 'G':
                        gravity_enabled ^= true;
                        break;
                case '_':
                        MAPBOOK->render(ACTIVE);
                        break;
                case '@':
                        loop_test(true);
                        break;
                case 'T':
                        switch (getchar()) {
                        case 'd':
                                print_matrix(DIAGNOSTIC_WIN, ACTIVE->door);
                                break;
                        }
                        break;
                }
        }
}

