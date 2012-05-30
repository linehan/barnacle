/* 
 * models.c -- Noun models.
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
#include "../lib/textutils.h"
#include "noun.h"
#include "models.h"
#include "models/human.h"
#include "models/hopper.h"
#include "models/omega.h"



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
                noun->modify = &modify_human;
                noun->render = &render_human;
                noun->sprite = wdup(L"ⰾ");
                noun->color  = FLEX;
                break;
        case HOPPER:
                noun->modify = &modify_hopper;
                noun->render = &render_hopper;
                sm_msg(noun->sm, SM_SELF, SM_Seek | SM_Wait(20));
                noun->sprite = wdup(L"Ⰹ");
                noun->color  = FLEX;
                break;
        case OMEGA:
                /*noun->modify = &modify_omega;*/
                /*noun->render = &render_omega;*/
                sm_msg(noun->sm, SM_SELF, SM_Seek | SM_Wait(20));
                noun->sprite = wdup(L"ᘯ");
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


