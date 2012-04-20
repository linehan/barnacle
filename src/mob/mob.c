#include <stdlib.h>
#include "mob.h"
#include "../ai/a_star_test.h"
#include "../map/cell.h"



/*
 * mob_cfg -- initialize a previously-allocated struct mob_t.
 *
 * @mob: The previously-allocated struct mob_t.
 * @map: The map in which the mob will be set.
 * @h: Height of the mob.
 * @w: Width of the mob.
 * @y: Initial y-coordinate of the mob.
 * @x: Initial x-coordinate of the mob.
 */
void mob_cfg(struct mob_t *mob, struct map_t *map, int h, int w, int y, int x)
{
        #define MOB_PATH_LEN 13

        struct cell_t *start = new_cell((uint32_t)y, (uint32_t)x);

        mob->astar = malloc(sizeof(struct astar_t));

        set_ufo(&mob->ufo, h, w, y, x, 
                           map->ufo.obj.h, map->ufo.obj.w, 0, 0);

        astar_init(mob->astar, map->mx, start);
        init_path(&mob->path, 0, 0, MOB_PATH_LEN);
        WINDOW *win = newwin(h, w, y, x);
        mob->pan    = new_panel(win);

        set_mob(mob, false);
}


/*
 * set_mob -- toggle a mob's visibility
 * Toggle whether or not a mob is visible on-screen, by setting the PANEL
 * member to hidden or showing it.
 * @mob: Pointer to struct mob_t
 * @onoff: Whether or not the mob should be visible
 */
void set_mob(struct mob_t *mob, bool onoff)
{
        mob->active = onoff;
        if (mob->active == true)
                show_panel(mob->pan);
        else
                hide_panel(mob->pan);

        doupdate();
}


/*
 * mob_move -- move a mob in the specified direction
 * @mob: pointer to struct mob_t
 * @dir: direction in which to move the mob PANEL
 */
void mob_move(struct mob_t *mob, int dir)
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
                map_roll(GLOBE, 'd');
                ufo_up(mob, ufo);
        }
        // Scroll right at right boundary
        if (ufo_x(mob, ufo) == COLS-10) {
                map_roll(GLOBE, 'r');
                ufo_left(mob, ufo);
        }

        mob->astar->start->y = (uint32_t)ufo_y(mob, ufo);
        mob->astar->start->x = (uint32_t)ufo_x(mob, ufo);
        mob->astar->start->key = mort(mob->astar->start->y, mob->astar->start->x);

        move_panel(mob->pan, ufo_y(mob, ufo), ufo_x(mob, ufo));
        update_panels();

        take_bkgrnd(panel_window(mob->pan), PEEK(GLOBE->W));

        path_push(&mob->path, y, x);
        doupdate();
}


/*
 * Draw the snake path behind a boat 
 */
void mob_path(struct mob_t *mob)
{
        werase(PEEK(GLOBE->L[HIG]));
        draw_path((PEEK(GLOBE->L[HIG])), &mob->path);
        wrefresh((PEEK(GLOBE->L[HIG])));
        doupdate();
}


void mob_seek(struct mob_t *s, struct mob_t *g)
{
        struct cell_t *tmp;

        if ((s->astar->current == NULL) 
        || !(same_cell(s->astar->current, g->astar->start))) 
        {
                if (a_star(s->astar, g->astar->start)) {
                        wprintw(CONSOLE_WIN, "Yep\n");
                        print_path(s->astar->current);
                } else {
                        wprintw(CONSOLE_WIN, "Nope\n");
                        return;
                }
        }
        /*return;*/


        werase(CONSOLE_WIN);
        wprintw(CONSOLE_WIN, 
                " Your position: y:%u, x:%u key:%u\n"
                " His position:  y:%u, x:%u key:%u\n"
                " Thinks goal:   y:%u, x:%u key:%u\n"
                " Actual goal:   y:%u, x:%u key:%u\n"
                " Next move:     y:%u, x:%u key:%u\n"
                " Next move:     y:%u, x:%u key:%u\n"
                " OPEN set:      n:%d\n"
                " CLOSED set:    n:%d\n",
                g->astar->start->y,
                g->astar->start->x,
                g->astar->start->key,
                s->astar->start->y,
                s->astar->start->x,
                s->astar->start->key,
                s->astar->goal->y,
                s->astar->goal->x,
                s->astar->goal->key,
                s->astar->current->y,
                s->astar->current->x,
                s->astar->current->key,
                s->astar->current->parent->y,
                s->astar->current->parent->x,
                s->astar->current->parent->key,
                s->astar->OPEN->n,
                s->astar->CLOSED->n);
}
         



