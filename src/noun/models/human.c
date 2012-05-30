/* 
 * human.c -- Human noun class.
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
#include "../noun.h"
#include "../../map/light.h"
#include "../../item/item.h"
#include "../../gfx/ui/menu_inventory.h"



/******************************************************************************
 * MODEL: Human 
 * NOTES: This is the main player character
 ******************************************************************************/
struct ani_t walk_u = {L"ⲑⲑᎲⰾ", 0, 'u'};
struct ani_t walk_d = {L"ⲑⲑᎲⰾ", 0, 'd'};
struct ani_t walk_l = {L"ⲑⲑᎲⰾ", 0, 'l'};
struct ani_t walk_r = {L"ⲑⲑᎲⰾ", 0, 'r'};
/*----------------------------------------------------------------------------*/
struct ani_t run_u = {L"ⲑⰾᎲⰾ", 0, 'u'};
struct ani_t run_d = {L"ⲑⰾᎲⰾ", 0, 'd'};
struct ani_t run_l = {L"ⲑⰾᎲⰾ", 0, 'l'};
struct ani_t run_r = {L"ⲑⰾᎲⰾ", 0, 'r'};
/*----------------------------------------------------------------------------*/
struct ani_t poke_u = {L"ᎲᎲᎲᑾᒀᒀᒀᑾᎲᎲⰾ",  0, 0};
struct ani_t poke_l = {L"ᎲᎲⰾᕗᕗᕤᐢᑕᑡᎲᎲⰾ", 0, 0};
struct ani_t poke_r = {L"ᎲᎲⰾᕙᕙᕦᐢᑐᑞᎲᎲⰾ", 0, 0};
struct ani_t poke_d = {L"ᎲᎲᎲᑵᒅᒅᒅᑵᎲᎲⰾ",  0, 0};
/*----------------------------------------------------------------------------*/
struct ani_t dodge_d = {L"ᎲᎲᎲⰾⰾᏈᏈᏡᏡᏡȣȣȣᎲᎲⰾ", 8, 'd'};
/*----------------------------------------------------------------------------*/
struct ani_t jump_ul = {L"ᎲᎲᎲⰾⰾᎹᎹᎹᎹᎹᎹᎲᎲⰾ", 7, 'u'+'l'};
struct ani_t jump_ur = {L"ᎲᎲᎲⰾⰾᎹᎹᎹᎹᎹᎹᎲᎲⰾ", 7, 'u'+'r'};
/*----------------------------------------------------------------------------*/
struct ani_t dig_u = {L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", 7, 'u'};
struct ani_t dig_d = {L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", 7, 'd'};
struct ani_t dig_l = {L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", 7, 'l'};
struct ani_t dig_r = {L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", 7, 'r'};
/*----------------------------------------------------------------------------*/
struct ani_t pickup = {L"ᎲᎲᎲⰾ", 0, 0};



/**
 * RENDER
 * render_human -- the rendering method for human noun models
 * @self: pointer to a noun of model 'HUMAN'
 */
void render_human(void *self)
{
        struct noun_t *noun = (struct noun_t *)self;
        struct item_t *item;

        noun_animate(noun);
        noun->fall(noun);
        glow_light(doorlight, noun, false);

        list_for_each(&noun->inv, item, node) {
                if (item->equipped && item->burn)
                        item->burn(item, noun);
        }
}



/**
 * MODIFY
 * modify_human -- the state machine for human noun models
 * @self: pointer to a noun of model 'HUMAN'
 */
int modify_human(void *self)
{
        struct noun_t *noun = (struct noun_t *)self;
        struct item_t *item;
        int state;

        sm_consume(noun->sm);
        state = sm_state(noun->sm);

        switch (state) 
        { 
        /* ==================================== message response */
        case SM_DigUp:
                noun->animate(noun, &dig_u);
                break;
        case SM_DigDown:
                noun->animate(noun, &dig_d);
                break;
        case SM_DigLeft:
                noun->animate(noun, &dig_l);
                break;
        case SM_DigRight:
                noun->animate(noun, &dig_r);
                break;
        case SM_GoUp:
                noun->animate(noun, &walk_u);
                break;
        case SM_GoDown:
                noun->animate(noun, &walk_d);
                break;
        case SM_GoLeft:
                noun->animate(noun, &walk_l);
                break;
        case SM_GoRight:
                noun->animate(noun, &walk_r);
                break;
        case SM_RunUp:
                noun->animate(noun, &run_u);
                break;
        case SM_RunDown:
                noun->animate(noun, &run_d);
                break;
        case SM_RunLeft:
                noun->animate(noun, &run_l);
                break;
        case SM_RunRight:
                noun->animate(noun, &run_r);
                break;
        case SM_PickUp:
                noun->animate(noun, &pickup);
                noun->pickup(noun);
                break;
        case SM_DodgeDown:
                noun->animate(noun, &dodge_d);
                break;
        case SM_JumpUpLeft:
                noun->animate(noun, &jump_ul);
                break;
        case SM_JumpUpRight:
                noun->animate(noun, &jump_ur);
                break;
        case SM_GetHit:
                HP_SUB(&noun->vitals, sm_mag(noun->sm));
                say_stats(noun->vitals);
                break;
        case SM_PokeUp:
                noun->animate(noun, &poke_u);
                emit_to_noun(noun, 'u', SM_GetHit | SM_Wait(3));
                break;
        case SM_PokeDown:
                noun->animate(noun, &poke_d);
                emit_to_noun(noun, 'd', SM_GetHit | SM_Wait(3));
                break;
        case SM_PokeLeft:
                noun->animate(noun, &poke_l);
                emit_to_noun(noun, 'l', SM_GetHit | SM_Wait(3));
                break;
        case SM_PokeRight:
                noun->animate(noun, &poke_r);
                emit_to_noun(noun, 'r', SM_GetHit | SM_Wait(3));
                break;
        /* ==================================== keyboard input */
        /* ------------------------------------ walk (step) */
        case SM_Key('j'):
                sm_msg(noun->sm, SM_SELF, SM_GoDown | SM_Wait(wait_for(noun)));
                break;
        case SM_Key('k'):
                sm_msg(noun->sm, SM_SELF, SM_GoUp | SM_Wait(wait_for(noun)));
                break;
        case SM_Key('h'):
                sm_msg(noun->sm, SM_SELF, SM_GoLeft | SM_Wait(wait_for(noun)));
                break;
        case SM_Key('l'):
                sm_msg(noun->sm, SM_SELF, SM_GoRight | SM_Wait(wait_for(noun)));
                break;
        /* ------------------------------------ combat */
        case SM_Key('w'):
                sm_msg(noun->sm, SM_SELF, SM_PokeUp);
                break;
        case SM_Key('a'):
                sm_msg(noun->sm, SM_SELF, SM_PokeLeft);
                break;
        case SM_Key('s'):
                sm_msg(noun->sm, SM_SELF, SM_PokeDown);
                break;
        case SM_Key('d'):
                sm_msg(noun->sm, SM_SELF, SM_PokeRight);
                break;
        /* ------------------------------------ advanced movement */
        case SM_Key('u'):
                sm_msg(noun->sm, SM_SELF, SM_DodgeDown | SM_Wait(wait_for(noun)));
                break;
        case SM_Key('A'):
                sm_msg(noun->sm, SM_SELF, SM_JumpUpLeft | SM_Wait(wait_for(noun)));
                break;
        case SM_Key('D'):
                sm_msg(noun->sm, SM_SELF, SM_JumpUpRight | SM_Wait(wait_for(noun)));
                break;
        /* ------------------------------------------ run  */
        case SM_Key('J'):
                if (ACTIVE->tag == MAP2DF)
                        sm_msg(noun->sm, SM_SELF, SM_RunDown | SM_Opt(STICKY));
                break;
        case SM_Key('K'):
                if (ACTIVE->tag == MAP2DF)
                        sm_msg(noun->sm, SM_SELF, SM_RunUp | SM_Opt(STICKY));
                break;
        case SM_Key('H'):
                if (ACTIVE->tag == MAP2DF)
                        sm_msg(noun->sm, SM_SELF, SM_RunLeft | SM_Opt(STICKY));
                else
                        sm_msg(noun->sm, SM_SELF, SM_JumpUpLeft);
                break;
        case SM_Key('L'):
                if (ACTIVE->tag == MAP2DF)
                        sm_msg(noun->sm, SM_SELF, SM_RunRight | SM_Opt(STICKY));
                else
                        sm_msg(noun->sm, SM_SELF, SM_JumpUpRight);
                break;
        /* ------------------------------------------ misc */
        case SM_Key('t'):
                list_for_each(&noun->inv, item, node) {
                        if (item->tag == ITEM_TORCH)
                                item->equip(item, item->equipped^1);
                }
                break;
        case SM_Key(' '):
                if (equipped) 
                        equipped->use(equipped, noun);
                break;
        case SM_Key('e'):
                sm_msg(noun->sm, SM_SELF, SM_PickUp | SM_Wait(wait_for(noun)));
                break;
        case SM_Key('@'):
                if (equipped)
                        equipped->equip(equipped, true);
                break;
        case SM_Key(KEY_ESC):
                return (MODE_RELEASE);
        }
        /* ==================================== end switch */

        sm_refresh(noun->sm);

        return (MODE_PERSIST);
}


