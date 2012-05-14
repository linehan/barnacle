#include <stdlib.h>
#include "mob.h"
#include "../map/snake.h"
#include "../ai/a_star_test.h"
#include "../map/cell.h"
#include "../verb/verb.h"
#include "../noun/noun.h"
#include "../map/terrain.h"
#include "../lib/pos.h"
#include "inventory.h"

/*#define MOB(ptr) (struct mob_t *)(ptr)*/


/*void mob_step_method(void *self, int dir);*/
/*void mob_hit_method(void *self);*/
/*void mob_setyx_method(void *self, int y, int x);*/



/*struct mob_t *new_mob(struct noun_t *noun, struct map_t *map, int h, int w, int y, int x)*/
/*{*/
        /*struct mob_t *new;*/

        /*new = malloc(sizeof(struct mob_t));*/

        /*new->pos = new_pos(h, w, y, x, LINES, COLS, 0, 0);*/

        /*new->win = newwin(h, w, y, x);*/
        /*new->pan = new_panel(new->win);*/

        /*new->astar = new_astar();*/
        /*astar_init(new->astar, map->tile, y, x);*/

        /*new->noun_id = noun->id;*/

        /*new->active = false;*/

        /*new->step = &mob_step_method;*/
        /*new->hit  = &mob_hit_method;*/
        /*new->setyx = &mob_setyx_method;*/

        /*return (new);*/
/*}*/





/**
 * set_mob -- toggle a mob's visibility
 * Toggle whether or not a mob is visible on-screen, by setting the PANEL
 * member to hidden or showing it.
 * @mob: Pointer to struct mob_t
 * @onoff: Whether or not the mob should be visible
 */
/*void set_mob(struct mob_t *mob, bool onoff)*/
/*{*/
        /*mob->active = onoff;*/
        /*if (mob->active == true)*/
                /*show_panel(mob->pan);*/
        /*else*/
                /*hide_panel(mob->pan);*/

        /*doupdate();*/
/*}*/



/*inline void mob_mark_position(struct mob_t *mob)*/
/*{*/
        /*mx_set(ACTIVE->mobs, pos_y(mob->pos), pos_x(mob->pos), mob->noun_id);*/
/*}*/
/*inline void mob_unmark_position(struct mob_t *mob)*/
/*{*/
        /*mx_set(ACTIVE->mobs, pos_y(mob->pos), pos_x(mob->pos), 0);*/
/*}*/


/*inline bool mob_hit_test(struct mob_t *mob)*/
/*{*/
        /*return (map_hit(ACTIVE, mob->pos)) ? false : true;*/
/*}*/


/*void mob_hit_method(void *self)*/
/*{*/
        /*struct mob_t *mob = MOB(self);*/

        /*if (mob->hit_testing_enabled && mob_hit_test(mob))*/
                /*mob->pos->restore(&mob->pos);*/
/*}*/

/*inline void mob_on_move(struct mob_t *mob)*/
/*{*/
        /*mob->hit(mob);*/

        /*move_panel(mob->pan, pos_y(mob->pos), pos_x(mob->pos));*/
        /*update_panels();*/

        /*mob->astar->start->y = (uint32_t)pos_y(mob->pos);*/
        /*mob->astar->start->x = (uint32_t)pos_x(mob->pos);*/
        /*mob->astar->start->key = mort(mob->astar->start->y, mob->astar->start->x);*/

        /*mob_mark_position(mob);*/

        /*take_bkgrnd(panel_window(mob->pan), PEEK(ACTIVE->W));*/
        /*doupdate();*/

        /*if (DOOR(ACTIVE, pos_y(mob->pos), pos_x(mob->pos)))*/
                /*door_trigger(mob, DOOR(ACTIVE, pos_y(mob->pos), pos_x(mob->pos)));*/
/*}*/

/*void mob_setyx_method(void *self, int y, int x)*/
/*{*/
        /*struct mob_t *mob = MOB(self);*/

        /*mob->pos->setyx(mob->pos, y, x);*/
        /*mob_on_move(mob);*/
/*}*/

/**
 * mob_move -- move a mob in the specified direction
 * @mob: pointer to struct mob_t
 * @dir: direction in which to move the mob PANEL
 */
/*void mob_step_method(void *self, int dir)*/
/*{*/
        /*#define UR 231*/
        /*#define UL 225*/
        /*#define DR 214*/
        /*#define DL 208*/

        /*struct mob_t *mob = MOB(self);*/

        /*mob_unmark_position(mob);*/

        /*switch (dir) {*/
        /*case 'u':       */
                /*pos_u(mob->pos);*/
                /*break;*/
        /*case 'd':       */
                /*pos_d(mob->pos);*/
                /*break;*/
        /*case 'l':       */
                /*pos_l(mob->pos);*/
                /*break;*/
        /*case 'r':      */
                /*pos_r(mob->pos);*/
                /*break;*/
        /*case UR:*/
                /*pos_u(mob->pos);*/
                /*pos_r(mob->pos);*/
                /*break;*/
        /*case UL:*/
                /*pos_u(mob->pos);*/
                /*pos_l(mob->pos);*/
                /*break;*/
        /*case DR:*/
                /*pos_d(mob->pos);*/
                /*pos_r(mob->pos);*/
                /*break;*/
        /*case DL:*/
                /*pos_d(mob->pos);*/
                /*pos_l(mob->pos);*/
                /*break;*/
        /*}*/

        /*mob_on_move(mob);*/
/*}*/



/*void mob_fall(struct mob_t *mob)*/
/*{*/
        /*int i;*/
        /*int j;*/

        /*if (!gravity_enabled)*/
                /*return;*/

        /*i = pos_y(mob->pos);*/
        /*j = pos_x(mob->pos);*/

        /*INC(i,0);*/

        /*if (TILE(ACTIVE, i, j) == CAVEFLOOR)*/
                /*mob->step(mob, 'd');*/
/*}*/




/*void mob_seek_test(struct mob_t *s, struct mob_t *g)*/
/*{*/
        /*struct cell_t *tmp;*/

        /*if ((s->astar->current == NULL) */
        /*|| !(same_cell(s->astar->current, g->astar->start))) */
        /*{*/
                /*if (a_star(s->astar, g->astar->start)) {*/
                        /*wprintw(CONSOLE_WIN, "Yep\n");*/
                        /*print_path(s->astar->current);*/
                /*} else {*/
                        /*wprintw(CONSOLE_WIN, "Nope\n");*/
                        /*return;*/
                /*}*/
        /*}*/
        /*tmp = cellpath_next(&s->astar->path);*/

        /*werase(CONSOLE_WIN);*/
        /*wprintw(CONSOLE_WIN, */
                /*" Your position: y:%u, x:%u key:%u\n"*/
                /*" His position:  y:%u, x:%u key:%u\n"*/
                /*" Thinks goal:   y:%u, x:%u key:%u\n"*/
                /*" Actual goal:   y:%u, x:%u key:%u\n"*/
                /*" Next move:     y:%u, x:%u key:%u\n"*/
                /*" Screen dims:   c:%u, r:%u, l:%u\n"*/
                /*" OPEN set:      n:%u, max:%u\n"*/
                /*" CLOSED set:    n:%u, max:%u\n",*/
                /*g->astar->start->y,             */
                /*g->astar->start->x,*/
                /*g->astar->start->key,*/
                /*s->astar->start->y,            */
                /*s->astar->start->x,*/
                /*s->astar->start->key,*/
                /*s->astar->goal->y,            */
                /*s->astar->goal->x,*/
                /*s->astar->goal->key,*/
                /*s->astar->current->y,        */
                /*s->astar->current->x,*/
                /*s->astar->current->key,*/
                /*tmp->y,*/
                /*tmp->x,*/
                /*tmp->key,*/
                /*s->astar->map->cols,*/
                /*s->astar->map->rows,*/
                /*s->astar->map->len,*/
                /*s->astar->OPEN->n,*/
                /*s->astar->OPEN->max,*/
                /*s->astar->CLOSED->n,*/
                /*s->astar->CLOSED->max);*/
/*}*/


/*void mob_seek(struct noun_t *snoun, struct noun_t *gnoun)*/
/*{*/
        /*struct mob_t *s = snoun->mob;*/
        /*struct mob_t *g = gnoun->mob;*/

        /*if ((s->astar->current == NULL) */
        /*|| !(same_cell(s->astar->current, g->astar->start))) */
        /*{*/
                /*if (a_star(s->astar, g->astar->start)) {*/
                        /*[>wprintw(CONSOLE_WIN, "Yep\n");<]*/
                        /*[>print_path(s->astar->current);<]*/
                /*} else {*/
                        /*[>wprintw(CONSOLE_WIN, "Nope\n");<]*/
                        /*return;*/
                /*}*/
        /*}*/
        /*struct cell_t *tmp;*/
        /*tmp = cellpath_next(&s->astar->path);*/

        /*if (tmp->x > s->astar->start->x)*/
                /*noun_set_state(snoun, VERB_GoRight, 0);*/
        /*if (tmp->x < s->astar->start->x)*/
                /*noun_set_state(snoun, VERB_GoLeft, 0);*/
        /*if (tmp->y > s->astar->start->y)*/
                /*noun_set_state(snoun, VERB_GoDown, 0);*/
        /*if (tmp->y < s->astar->start->y)*/
                /*noun_set_state(snoun, VERB_GoUp, 0);*/
/*}*/


/*void mob_set_signal(struct mob_t *mob, int verb, int dir)*/
/*{*/
        /*int y = pos_y(mob->pos);*/
        /*int x = pos_x(mob->pos);*/
        
        /*switch (dir) {*/
        /*case 'u':*/
                /*DEC(y, pos_xmin(mob->pos));*/
                /*break;*/
        /*case 'd':*/
                /*INC(y, pos_ymax(mob->pos));*/
                /*break;*/
        /*case 'l':*/
                /*DEC(x, pos_ymin(mob->pos));*/
                /*break;*/
        /*case 'r':*/
                /*INC(x, pos_xmax(mob->pos));*/
                /*break;*/
        /*}*/

        /*send_verb(verb, mx_val(ACTIVE->mobs, y, x), mob->noun_id, 0, NULL);*/
/*}*/


