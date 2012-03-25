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

#include "gen/dice.h"
#include "gfx/gfx.h"
#include "geo/map.h"
#include "geo/terrain.h"
#include "geo/weather.h"
#include "mob/mob.h"
#include "pan/instruments.h"
#include "pan/test.h"
#include "lib/ufo.h"
#include "lib/morton.h"

#include "gen/name/name.h"
#include "pan/crewgen.h"
#include "pan/new_character.h"
#include "noun/noun_model.h"
#include "noun/noun_control.h"


#include "init/init.h"
#include "loop/loop.h"

#include "boat/boat.h"


/* Day-toh-nah */
int main() 
{
        arawak_init();

        struct map_t *pad = new_map((LINES*3), (COLS*3));
        gen_map(pad);
        GLOBE = pad;

        roll_map(pad, 0);
        roll_map(pad, 0);

        uint32_t key;
        key = new_boat(pad, FUNBOAT, "Afarensis");
        wprintw(DIAGNOSTIC_WIN, "%u\n", key);

        if (get_boat("Afarensis") == NULL)
                wprintw(DIAGNOSTIC_WIN, "NULL");
        else
                wprintw(DIAGNOSTIC_WIN, "NOT NULL");

        draw_boat(get_boat("Afarensis"));

        toggle_instrument_panel();
        toggle_instrument_panel();
        draw_compass();
        load_noun_test();
        view_dock();
        choose_noun('*');

        set_wind(__pre__, 4);

        start_event_watchers();

        endwin(); /* end curses mode */
        return 0;
}

