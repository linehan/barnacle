/* 
 * hopper.c -- Enemy noun class.
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
#include "../models.h"
#include "../../gfx/ui/notify.h"
#include "hopper.h"

/****************************************************************************** 
 * MODEL: Hopper 
 * NOTES: Test unit for combat and collision
 ******************************************************************************/
/*----------------------------------------------------------------------------*/
struct ani_t roll  = {L"ⰹⰹⰺⰺⰊⰊⰋⰋⰝⰝⰝⰋⰋⰹⰹⰺⰺⰉ"};
struct ani_t hit1  = {L"ⰹⰹⰶⰶⰶⰆⰆⰆⰹⰹⰉ"};
struct ani_t hit2  = {L"ⰹⰹⰆⰆⰆⰶⰶⰶⰆⰆⰆⰹⰹⰉ"};
struct ani_t hop_u = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", 4, 'u'};
struct ani_t hop_d = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", 4, 'd'};
struct ani_t hop_l = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", 4, 'l'};
struct ani_t hop_r = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", 4, 'r'};
struct ani_t die   = {L"ⰊⰊⰊⰊⰉ✶✶✺✺✺❁❁✴៹៹ᔊᔊⸯⸯ"};
/*----------------------------------------------------------------------------*/


static wchar_t *icon[]={L"蕳",L"蔄",L"簡",L"囧"};
enum {                   CALM, YELL, ORLY, HURT};


static struct phrase_t phrase = {3,{
        "Get fucked!",
         "I'm hoppin' right for ya!",
         "This hop's for you!"
}};



/**
 * RENDER
 * render_hopper -- the rendering method for hopper noun models
 * @noun: pointer to a noun of model 'HOPPER'
 */
void render_hopper(void *self)
{
        struct noun_t *noun = (struct noun_t *)self;
        noun_animate(noun);
}

/**
 * MODIFY
 * modify_hopper -- the state machine method for hopper noun models
 * @noun: pointer to a noun of model 'HOPPER'
 */
int modify_hopper(void *obj)
{
        struct noun_t *noun = (struct noun_t *)obj;
        uint32_t nbr;
        int state;

        nbr = noun_nbr_model(noun, pos_hdg(noun->pos), HUMAN);

        if (!noun->is_mobile)
                return 0;

        sm_consume(noun->sm);
        state = sm_state(noun->sm);

        switch (state)
        {
        case SM_GetHit:
                noun->_animate(noun, &die);
                sm_msg(noun->sm, SM_SELF, SM_Destroy|SM_Wait(wait_for(noun))|SM_Pri(9));
                sm_screen(noun->sm, 9);
                say_speak(icon[HURT], "Nooo!");
                break;
        case SM_Attack:
                if (flip_biased(0.8))
                        noun->_animate(noun, &hit1);
                else    noun->_animate(noun, &hit2);
                sm_msgmag(noun->sm, nbr, SM_GetHit | SM_Wait(3), 3);
                break;
        case SM_GoUp:
                noun->_animate(noun, &hop_u);
                break;
        case SM_GoDown:
                if (flip_biased(0.4))
                        picksay(icon[CALM], &phrase);
                noun->_animate(noun, &hop_d);
                break;
        case SM_GoLeft:
                noun->_animate(noun, &hop_l);
                if (flip_biased(0.4))
                        picksay(icon[YELL], &phrase);
                break;
        case SM_GoRight:
                noun->_animate(noun, &hop_r);
                break;
        case SM_Destroy:
                alert(I_KILL, noun->name);
                noun_item_drop(noun);
                noun->_doom(noun);
                say_speak(L"", "");
                break;
        case SM_Seek:
                if (nbr != NONOUN)
                        sm_msg(noun->sm, SM_SELF, SM_Attack);
                else
                        noun->_seek(noun, get_noun("Guy"));

                sm_msg(noun->sm, SM_SELF, SM_Seek | SM_Wait(20) | SM_Pri(1));
                break;
        case SM_Default:
                break;
        }

        sm_refresh(noun->sm);

        return 0;
}

