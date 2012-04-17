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
#include "noun/types/creature/creature.h"
#include "gfx/ui/titlecard.h"
#include "lib/pqueue.h"
#include "lib/binheap.h"

/* Rolling start */
int main(int argc, char *argv[]) 
{
        struct map_t *pad;

        arawak_init();

        if (argc == 2) {
                if (strcmp(argv[1], "-water")) 
                        pad = map_preset_arena();
                else if (strcmp(argv[1], "-arena"))
                        pad = map_preset_water();
        }
        if (pad == NULL) {
                pad = new_map((LINES*3), (COLS*3), LINES, COLS, 0, 0);
                      map_gen(pad, NULL, MAP_DOSMOOTH);
        }

        GLOBE = pad;

        print_status("\n ALL OK\n");
        print_hold("Press any key to continue");

        map_roll(pad, 0);
        map_roll(pad, 0);

        new_boat(pad, FUNBOAT, "Afarensis");
        set_mob(noun_mob("Afarensis"), false);

        toggle_instrument_panel();
        toggle_instrument_panel();

        view_dock();
        load_noun_test();
        list_nouns(SUBJECT, ALL_NOUNS);
        list_nouns(OBJECT, ALL_NOUNS);
        choose_noun('*');
        
        init_pair(FLEX, BLACK, WHITE);
        init_pair(FLEX2, BLACK, WHITE);

        new_creature("Guy", PERSON, L"д", FLEX);
        /*new_creature("Killbot", MONSTER, L"⿍", FLEX);*/

        noun_set_mob(get_noun("Guy"), true);
        mob_move(noun_mob("Guy"), 'd');
        mob_move(noun_mob("Guy"), 'd');
        mob_move(noun_mob("Guy"), 'd');

        /*noun_set_mob(get_noun("Killbot"), true);*/
        /*mob_move(noun_mob("Killbot"), 'd');*/


        /*struct pq_t *pq = new_pq(20);*/
        /*struct pq_node_t *node = malloc(8 * sizeof(struct pq_node_t));*/

        struct heap_t heap;
        struct heap_node_t node[10];

        heap_init(&heap);

        int i;
        for (i=0; i<5; i++) {
                heap_node_init(&node[i], i, NULL);
        }

        for (i=0; i<5; i++) {
                heap_insert(&heap, &node[i]);
        }

        /*int i;*/
        wprintw(DIAGNOSTIC_WIN, "Priority queue\n");
        for (i=1; i<5; i++) {
                struct heap_node_t *tmp;
                tmp = heap_take(&heap);
                if (tmp != NULL) 
                        wprintw(DIAGNOSTIC_WIN, "%d\n", tmp->key);
                else
                        wprintw(DIAGNOSTIC_WIN, "NULL!\n");
        }

        fork_sweet_flow();

        start_event_watchers(); /* Enter the event loop */

        endwin(); /* end curses mode */
        return 0;
}

