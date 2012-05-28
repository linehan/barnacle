#include "../com/arawak.h"
#include "../lib/textutils.h"
#include "noun.h"
#include "models.h"
#include "models/human.h"
#include "models/hopper.h"



/* The all-important late-binding dynamic linker
``````````````````````````````````````````````````````````````````````````````*/
/*
 * apply_noun_model -- implement the polymorphic noun model scheme
 * @noun: pointer to a noun with model field specified
 */
void apply_noun_model(struct noun_t *noun)
{
        switch (noun->model) 
        {
        case HUMAN:
                HP_SET(&noun->vitals, 25);
                SP_SET(&noun->vitals, 13);
                AP_SET(&noun->vitals, 2);
                noun->_modify = &modify_human;
                noun->_render = &render_human;
                noun->sprite = wdup(L"ⰾ");
                noun->color  = FLEX;
                break;
        case HOPPER:
                noun->_modify = &modify_hopper;
                noun->_render = &render_hopper;
                sm_msg(noun->sm, SM_SELF, SM_Seek | SM_Wait(20));
                noun->sprite = wdup(L"Ⰹ");
                noun->color  = FLEX;
                break;
        }

        noun->pos = new_pos(1, 1, CENTERED, LINES, COLS, 0, 0);
        noun->win = newwin(1, 1, CENTERED);
        noun->pan = new_panel(noun->win);
        wbkgrnd(noun->win, mkcch(noun->sprite, 0, noun->color));

        astar_init(noun->astar, ACTIVE->tile, pos_y(noun->pos), pos_x(noun->pos));
        noun->map_id = ACTIVE->id;
}


