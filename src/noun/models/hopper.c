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
#include "../../lib/textutils.h"
#include "../../lib/stoc/stoc.h"

/****************************************************************************** 
 * MODEL: Hopper 
 * NOTES: Test unit for combat and collision
 ******************************************************************************/
/*----------------------------------------------------------------------------*/
static struct ani_t roll  = {L"ⰹⰹⰺⰺⰊⰊⰋⰋⰝⰝⰝⰋⰋⰹⰹⰺⰺⰉ"};
static struct ani_t hit1  = {L"ⰹⰹⰶⰶⰶⰆⰆⰆⰹⰹⰉ"};
static struct ani_t hit2  = {L"ⰹⰹⰆⰆⰆⰶⰶⰶⰆⰆⰆⰹⰹⰉ"};
static struct ani_t hop_u = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", 4, 'u'};
static struct ani_t hop_d = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", 4, 'd'};
static struct ani_t hop_l = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", 4, 'l'};
static struct ani_t hop_r = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", 4, 'r'};
static struct ani_t die   = {L"ⰊⰊⰊⰊⰉ✶✶✺✺✺❁❁✴៹៹ᔊᔊⸯⸯ"};
/*----------------------------------------------------------------------------*/


static wchar_t *icon[]={L"蕳",L"蔄",L"簡",L"囧"};
enum {                   CALM, YELL, ORLY, HURT};


static const char *phrase[]={
        "I'm gonna hop all over you!",
        "It's hoppin' time!",
        "You're shoppin' for a hoppin'!",
        "Get fucked!",
        "I'm hoppin' right for ya!",
        "This hop's for you!"
};
static int num_phrases = ARRAY_SIZE(phrase);


static const char *names[]={
        "Hopcroft" , "Chopin"     , "Chopra"    , "Poppo"    , "Popetet"  ,
        "Focco"    , "Fokko"      , "Popo"      , "Poppiko"  , "Puppe"    ,
        "Vocco"    , "Rabbod"     , "Ratbod"    , "Rathbod"  , "Redbod"   ,
        "Ruotbodo" , "Folcmar"    , "Jakop"     , "Joopi"    , "Joper"    ,
        "Jopi"     , "Joppa"      , "Joppe"     , "Juoppo"   , "Koppi"    ,
        "Loptr"    , "Roope"      , "Roopert"   , "Roopi"    , "Ruoppo"   ,
        "Skopti"   , "Topi"       , "Topia"     , "Topias"   , "Topija"   ,
        "Toppe"    , "Toppi"      , "Toppu"     , "Hope"     , "Hopkins"  ,
        "Cropper"  , "Copernicus" , "Hopkinson" , "Hopkin"   , "Hopson"   ,
        "Hobbes"   , "Hobson"     , "Hopovich"  , "Trollope" , "Winthrop" ,
        "Thobias"  , "Orlof"      , "Orlop"
};
static int num_names = ARRAY_SIZE(names);


/**
 * pick_name -- assign a name to a spawned hopper
 */
void pick_name(struct noun_t *noun)
{
        int k;

        k = roll_fair(num_names-1);
        noun->name = cdup(names[k]);
}
        
void spawn_hopper(void *obj)
{
        struct noun_t *noun = (struct noun_t *)obj;
        pick_name(noun);
}



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
                noun->animate(noun, &die);
                sm_msg(noun->sm, SM_SELF, SM_Destroy|SM_Wait(wait_for(noun))|SM_Pri(9));
                sm_screen(noun->sm, 9);
                say_speak(icon[HURT], "Nooo!");
                break;
        case SM_Attack:
                if (flip_biased(0.8))
                        noun->animate(noun, &hit1);
                else    noun->animate(noun, &hit2);
                sm_msgmag(noun->sm, nbr, SM_GetHit | SM_Wait(3), 3);
                break;
        case SM_GoUp:
                noun->animate(noun, &hop_u);
                break;
        case SM_GoDown:
                if (flip_biased(0.3))
                        say_speak(icon[CALM], phrase[(roll_fair(num_phrases-2))]);
                noun->animate(noun, &hop_d);
                break;
        case SM_GoLeft:
                noun->animate(noun, &hop_l);
                if (flip_biased(0.3))
                        say_speak(icon[YELL], phrase[(roll_fair(num_phrases-2))]);
                break;
        case SM_GoRight:
                noun->animate(noun, &hop_r);
                break;
        case SM_Destroy:
                alert(I_KILL, noun->name);
                noun_item_drop(noun);
                noun->doom(noun);
                say_speak(L"", "");
                break;
        case SM_Seek:
                if (nbr != NONOUN)
                        sm_msg(noun->sm, SM_SELF, SM_Attack);
                else
                        noun->seek(noun, get_noun("Guy"));

                sm_msg(noun->sm, SM_SELF, SM_Seek | SM_Wait(20) | SM_Pri(1));
                break;
        case SM_Default:
                break;
        }

        sm_refresh(noun->sm);

        return 0;
}

