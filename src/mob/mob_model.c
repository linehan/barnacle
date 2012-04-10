#include <stdlib.h>
#include "mob_model.h"


/*
 * Initialize a previously-allocated struct mob_t.
 *
 * @mob: The previously-allocated struct mob_t.
 * @map: The map in which the mob will be set.
 * @h: Height of the mob.
 * @w: Width of the mob.
 * @y: Initial y-coordinate of the mob.
 * @x: Initial x-coordinate of the mob.
 */
void set_mob(struct mob_t *mob, struct map_t *map, int h, int w, int y, int x)
{


        #define MOB_PATH_LEN 13

        set_ufo(&mob->ufo, h, w, y, x, 
                           map->ufo.obj.h, map->ufo.obj.w, 0, 0);

        init_path(&mob->path, 0, 0, MOB_PATH_LEN);

        WINDOW *win = newwin(h, w, y, x);
        mob->pan    = new_panel(win);
        wrefresh(win);
}


PANEL *mob_pan(struct mob_t *mob)
{
        return (mob->pan);
}
WINDOW *mob_win(struct mob_t *mob)
{
        return panel_window(mob->pan);
}

////////////////////////////////////////////////////////////////////////////////
void move_mob(struct mob_t *mob, int dir)
{
        if (dir != 'u' && dir != 'd' && dir != 'l' && dir != 'r') return;

        int y = ufo_y(mob, ufo);
        int x = ufo_x(mob, ufo);

        switch (dir) {
        case 'u':       
                ufo_up(mob, ufo);
                if (map_hit(GLOBE, &mob->ufo.obj))
                        ufo_down(mob, ufo);
                break;
        case 'd':       
                ufo_down(mob, ufo);
                if (map_hit(GLOBE, &mob->ufo.obj)) 
                        ufo_up(mob, ufo);
                break;
        case 'l':       
                ufo_left(mob, ufo);
                if (map_hit(GLOBE, &mob->ufo.obj))
                        ufo_right(mob, ufo);
                break;
        case 'r':      
                ufo_right(mob, ufo);
                if (map_hit(GLOBE, &mob->ufo.obj))
                        ufo_left(mob, ufo);
                break;
        }

        // Scroll down at bottom boundary
        if (ufo_y(mob, ufo) == LINES-10) {
                roll_map(GLOBE, 'd');
                ufo_up(mob, ufo);
        }
        // Scroll right at right boundary
        if (ufo_x(mob, ufo) == COLS-10) {
                roll_map(GLOBE, 'r');
                ufo_left(mob, ufo);
        }

        move_panel(mob->pan, ufo_y(mob, ufo), ufo_x(mob, ufo));
        update_panels();

        path_push(&mob->path, y, x);
        doupdate();
}

