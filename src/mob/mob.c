#include <stdlib.h>
#include "mob.h"
#include "../map/snake.h"
#include "../ai/a_star_test.h"
#include "../map/cell.h"
#include "../verb/verb.h"
#include "../noun/noun.h"
#include "../map/terrain.h"


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

        astar_init(mob->astar, map->tile, start);
        mob->path = new_path(0, 0, MOB_PATH_LEN);
        init_path(mob->path, 0, 0, MOB_PATH_LEN);
        mob->win = newwin(h, w, y, x);
        mob->pan = new_panel(mob->win);
        mob->mapid = map->id;
        mob->inv = new_inventory();

        set_mob(mob, false);
}





/**
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



inline void mob_mark_position(struct mob_t *mob)
{
        mx_set(ACTIVE->mobs, ufo_y(mob, ufo), ufo_x(mob, ufo), mob->name);
}
inline void mob_unmark_position(struct mob_t *mob)
{
        mx_set(ACTIVE->mobs, ufo_y(mob, ufo), ufo_x(mob, ufo), 0);
}

void mob_place(struct mob_t *mob, int y, int x)
{
        ufo_y(mob, ufo) = y;
        ufo_x(mob, ufo) = x;

        mob_move(mob, 'u');
}


WINDOW *torch_win;
PANEL  *torch_pan;

static bool torch_lit;

void torch_toggle(void)
{
        torch_lit ^= true;
        if (torch_lit)
                show_panel(torch_pan);
        else
                hide_panel(torch_pan);
}

#define TORCH_H 5
#define TORCH_W 7
#define TORCH_Hr 2
#define TORCH_Wr 3
int torch_h;
int torch_w;

void torch(struct mob_t *mob)
{
        int y;
        int x;
        int i;
        int j;
        int x0;
        int y0;

        if (!torch_lit)
                return;

        y = ufo_y(mob, ufo);
        x = ufo_x(mob, ufo);

        /* -------------------------------------- y-adjustments */
        if (y < TORCH_Hr+1) {
                torch_h = (TORCH_Hr + y);
                y0 = TORCH_Hr - y;
        }
        else if (y > LINES-(TORCH_Hr+1)) {
                torch_h = TORCH_Hr + (LINES-y);
                y0 = y - TORCH_Hr;
        }
        else {
                torch_h = TORCH_H;
                y0 = y - TORCH_Hr;
        }
        /* -------------------------------------- x-adjustments */
        if (x < TORCH_Wr) 
        {
                torch_w = (TORCH_Wr + x);
                x0 = TORCH_Wr - x;
        }
        else if (x > COLS-TORCH_Wr) {
                torch_w = TORCH_Wr + (COLS-x);
                x0 = x - TORCH_Wr;
        }
        else {
                torch_w = TORCH_W;
                x0 = x - TORCH_Wr;
        }
        /* -------------------------------------- initialization */
        if (!torch_win) {
                torch_win = newwin(torch_h, torch_w, y0, x0);
                torch_pan = new_panel(torch_win);
                blend(LIGHT1, 0.2, __DGREY, 1.0, LIGHT3);
                blend(LIGHT1, 0.5, __DGREY, 1.0, LIGHT2);
                blend(LIGHT1, 1.0, __DGREY, 1.0, LIGHT1);
                blend(__DGREY, 0.3, BLACK, 1.0, LIGHTB3);
                blend(__DGREY, 0.5, BLACK, 1.0, LIGHTB2);
                blend(__DGREY, 1.0, BLACK, 1.0, LIGHTB1);
        } else {
                if (wresize(torch_win, torch_h, torch_w))
                        replace_panel(torch_pan, torch_win);

                move_panel(torch_pan, y0, x0);
        }
        /* -------------------------------------- copy bg to torch buffer */
        copywin(PLATE(ACTIVE, BGR), torch_win, y0, x0, 0, 0, torch_h-1, torch_w-1, 0);
        overwrite(mob->win, torch_win);

        /* -------------------------------------- re-color the copied wchars */
        for (i=0; i<torch_h; i++) {
        for (j=0; j<torch_w; j++) {
                if ((i==0 && j==0)
                ||  (i==0 && j==1)
                ||  (i==0 && j==torch_w-1)
                ||  (i==0 && j==torch_w-2)
                ||  (i==torch_h-1 && j==0)
                ||  (i==torch_h-1 && j==1)
                ||  (i==torch_h-1 && j==torch_w-1)
                ||  (i==torch_h-1 && j==torch_w-2))
                {
                        mvwchgat(torch_win, i, j, 1, 0, LIGHTP3, NULL);
                }
                else if (i==0 || i==torch_h-1 || j==0 || j==1 || j==torch_w-1 || j==torch_w-2)
                        mvwchgat(torch_win, i, j, 1, 0, LIGHTP2, NULL);
                else 
                        mvwchgat(torch_win, i, j, 1, 0, LIGHTP1, NULL);
        }
        }
}


/**
 * mob_move -- move a mob in the specified direction
 * @mob: pointer to struct mob_t
 * @dir: direction in which to move the mob PANEL
 */
void mob_move(struct mob_t *mob, int dir)
{
        #define UR 231
        #define UL 225
        #define DR 214
        #define DL 208

        int y = ufo_y(mob, ufo);
        int x = ufo_x(mob, ufo);

        mob_unmark_position(mob);

        switch (dir) {
        case 'u':       
                ufo_up(mob, ufo);
                if (map_hit(ACTIVE, &mob->ufo.obj))
                        ufo_down(mob, ufo);
                break;
        case 'd':       
                ufo_down(mob, ufo);
                if (map_hit(ACTIVE, &mob->ufo.obj)) 
                        ufo_up(mob, ufo);
                break;
        case 'l':       
                ufo_left(mob, ufo);
                if (map_hit(ACTIVE, &mob->ufo.obj))
                        ufo_right(mob, ufo);
                break;
        case 'r':      
                ufo_right(mob, ufo);
                if (map_hit(ACTIVE, &mob->ufo.obj))
                        ufo_left(mob, ufo);
                break;
        case UR:
                ufo_up(mob, ufo);
                if (map_hit(ACTIVE, &mob->ufo.obj))
                        ufo_down(mob, ufo);
                ufo_right(mob, ufo);
                if (map_hit(ACTIVE, &mob->ufo.obj))
                        ufo_left(mob, ufo);
                break;
        case UL:
                ufo_up(mob, ufo);
                if (map_hit(ACTIVE, &mob->ufo.obj))
                        ufo_down(mob, ufo);
                ufo_left(mob, ufo);
                if (map_hit(ACTIVE, &mob->ufo.obj))
                        ufo_right(mob, ufo);
                break;
        case DR:
                ufo_down(mob, ufo);
                if (map_hit(ACTIVE, &mob->ufo.obj)) 
                        ufo_up(mob, ufo);

                ufo_right(mob, ufo);
                if (map_hit(ACTIVE, &mob->ufo.obj))
                        ufo_left(mob, ufo);
                break;
        case DL:
                ufo_down(mob, ufo);
                if (map_hit(ACTIVE, &mob->ufo.obj)) 
                        ufo_up(mob, ufo);

                ufo_left(mob, ufo);
                if (map_hit(ACTIVE, &mob->ufo.obj))
                        ufo_right(mob, ufo);
                break;
        }

        move_panel(mob->pan, ufo_y(mob, ufo), ufo_x(mob, ufo));
        update_panels();

        /* Actions performed whether the mob has moved or not */
        update:
                mob->astar->start->y = (uint32_t)ufo_y(mob, ufo);
                mob->astar->start->x = (uint32_t)ufo_x(mob, ufo);
                mob->astar->start->key = mort(mob->astar->start->y, mob->astar->start->x);

                mob_mark_position(mob);
                take_bkgrnd(panel_window(mob->pan), PEEK(ACTIVE->W));
                torch(mob);
                path_push(mob->path, y, x);
                doupdate();
                if (TILE(ACTIVE, ufo_y(mob,ufo), ufo_x(mob,ufo)) == DOR)
                        door_trigger(mob, mx_val(ACTIVE->door, ufo_y(mob,ufo), ufo_x(mob,ufo)));
}


/**
 * Draw the snake path behind a mob
 */
void mob_path(struct mob_t *mob)
{
        werase(PEEK(ACTIVE->L[HIG]));
        draw_path((PEEK(ACTIVE->L[HIG])), mob->path);
        wrefresh((PEEK(ACTIVE->L[HIG])));
        doupdate();
}


void mob_fall(struct mob_t *mob)
{
        int i;
        int j;

        if (!gravity_enabled)
                return;

        i = mob->ufo.obj.y;
        j = mob->ufo.obj.x;

        DEC(i,0);

        if (TILE(ACTIVE, i, j) == CAVEFLOOR)
                mob_move(mob, 'd');
}




void mob_seek_test(struct mob_t *s, struct mob_t *g)
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
        tmp = cellpath_next(&s->astar->path);

        werase(CONSOLE_WIN);
        wprintw(CONSOLE_WIN, 
                " Your position: y:%u, x:%u key:%u\n"
                " His position:  y:%u, x:%u key:%u\n"
                " Thinks goal:   y:%u, x:%u key:%u\n"
                " Actual goal:   y:%u, x:%u key:%u\n"
                " Next move:     y:%u, x:%u key:%u\n"
                " Screen dims:   c:%u, r:%u, l:%u\n"
                " OPEN set:      n:%u, max:%u\n"
                " CLOSED set:    n:%u, max:%u\n",
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
                tmp->y,
                tmp->x,
                tmp->key,
                s->astar->map->cols,
                s->astar->map->rows,
                s->astar->map->len,
                s->astar->OPEN->n,
                s->astar->OPEN->max,
                s->astar->CLOSED->n,
                s->astar->CLOSED->max);
}


void mob_seek(struct noun_t *snoun, struct noun_t *gnoun)
{
        struct mob_t *s = &snoun->mob;
        struct mob_t *g = &gnoun->mob;

        if ((s->astar->current == NULL) 
        || !(same_cell(s->astar->current, g->astar->start))) 
        {
                if (a_star(s->astar, g->astar->start)) {
                        /*wprintw(CONSOLE_WIN, "Yep\n");*/
                        /*print_path(s->astar->current);*/
                } else {
                        /*wprintw(CONSOLE_WIN, "Nope\n");*/
                        return;
                }
        }
        struct cell_t *tmp;
        tmp = cellpath_next(&s->astar->path);

        if (tmp->x > s->astar->start->x)
                noun_set_state(snoun, VERB_GoRight, 0);
        if (tmp->x < s->astar->start->x)
                noun_set_state(snoun, VERB_GoLeft, 0);
        if (tmp->y > s->astar->start->y)
                noun_set_state(snoun, VERB_GoDown, 0);
        if (tmp->y < s->astar->start->y)
                noun_set_state(snoun, VERB_GoUp, 0);
}


void mob_set_signal(struct mob_t *mob, int verb, int dir)
{
        int y = ufo_y(mob, ufo);
        int x = ufo_x(mob, ufo);
        
        switch (dir) {
        case 'u':
                DEC(y, 0);
                break;
        case 'd':
                INC(y, mob->ufo.box.h);
                break;
        case 'l':
                DEC(x, 0);
                break;
        case 'r':
                INC(x, mob->ufo.box.w);
                break;
        }

        send_verb(verb, mx_val(ACTIVE->mobs, y, x), mob->name, 0, NULL);
}


