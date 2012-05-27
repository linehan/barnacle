#include "../com/arawak.h"
#include "../lib/textutils.h"
#include "../gfx/ui/notify.h"
#include "models.h"
#include "noun.h"
#include "../eng/fsm.h"
#include "../item/item.h"
#include "../item/tools.h"
#include "../gfx/ui/menu_inventory.h"
#include "../map/light.h"

#define MV(frame,dir) (frame), (dir)
#define NO_MV MV(0,0) 

struct ani_t *mk_ani(const wchar_t *wcs, int mv_frame, int mv_dir)
{
        struct ani_t *new = malloc(sizeof(struct ani_t));

        new->frame     = wdup(wcs);
        new->len       = wcslen(wcs);
        new->mv_frame  = mv_frame;
        new->mv_dir    = mv_dir;

        return (new);
}


int wait_for(struct noun_t *noun)
{
        int remaining;
        int total;

        if (noun->animation)
                total = noun->animation->len;
        else
                total = 0;

        assert(noun->frame <= total || !"Animation frame overflow"); 
        remaining = total - noun->frame;

        return (remaining);
}





/*
 * noun_animate -- draw successive character "frames", trigger movement, etc.
 * @noun: pointer to a noun
 */
void noun_animate(struct noun_t *noun)
{
        if (!noun->animation)
                return;

        struct ani_t *ani = noun->animation;

        /* Draw the current frame */
        if (noun->frame < noun->animation->len)
                wcch(noun->win, &noun->animation->frame[noun->frame], 0, FLEX);

        /* Move the panel if this is the mv_frame */
        if (noun->frame == ani->mv_frame)
                noun->_step(noun, ani->mv_dir);

        /* Increment the current frame and/or terminate the animation */
        if (++noun->frame == noun->animation->len) {
                noun->frame = 0;
                noun->animation = NULL;
                return;
        }

        return;
}


/******************************************************************************
 * MODEL: Person
 * NOTES: This is the main player character
 ******************************************************************************/
struct ani_t *guy_run_u;
struct ani_t *guy_run_d;
struct ani_t *guy_run_l;
struct ani_t *guy_run_r;
struct ani_t *guy_punch_r;
struct ani_t *guy_punch_l;
struct ani_t *guy_poke_u;
struct ani_t *guy_poke_d;
struct ani_t *guy_poke_l;
struct ani_t *guy_poke_r;
struct ani_t *guy_dodge_d;
struct ani_t *guy_falltest;
struct ani_t *guy_dodge_l;
struct ani_t *guy_dodge_r;
struct ani_t *guy_kick_r;
struct ani_t *guy_kick_l;
struct ani_t *guy_jump_ul;
struct ani_t *guy_jump_ur;
struct ani_t *guy_dig_u;
struct ani_t *guy_dig_d;
struct ani_t *guy_dig_l;
struct ani_t *guy_dig_r;
struct ani_t *guy_walk_u;
struct ani_t *guy_walk_d;
struct ani_t *guy_walk_l;
struct ani_t *guy_walk_r;
struct ani_t *guy_pickup;
/*----------------------------------------------------------------------------*/
void build_person_animations(void)
{
        static bool built = false;

        if (built)
                return;

        guy_walk_u   = mk_ani(L"ⲑⲑᎲⰾ", 0, 'u');
        guy_walk_d   = mk_ani(L"ⲑⲑᎲⰾ", 0, 'd');
        guy_walk_l   = mk_ani(L"ⲑⲑᎲⰾ", 0, 'l');
        guy_walk_r   = mk_ani(L"ⲑⲑᎲⰾ", 0, 'r');

        guy_run_u    = mk_ani(L"ⲑⰾᎲⰾ", 0, 'u');
        guy_run_d    = mk_ani(L"ⲑⰾᎲⰾ", 0, 'd');
        guy_run_l    = mk_ani(L"ⲑⰾᎲⰾ", 0, 'l');
        guy_run_r    = mk_ani(L"ⲑⰾᎲⰾ", 0, 'r');

        guy_punch_r  = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾ", 0, 0);
        guy_punch_l  = mk_ani(L"ᎲᎲᎲ᱙ᕦ᱙ᎲᎲⰾ", 0, 0);

        guy_poke_u   = mk_ani(L"ᎲᎲᎲᒀᒀᒀᒀᎲᎲⰾ", 0, 0);
        guy_poke_l   = mk_ani(L"ᎲᎲⰾᕗᕗᑕᑡᎲᎲⰾ", 4, 'l');
        guy_poke_r   = mk_ani(L"ᎲᎲⰾᕙᕙᑐᑞᎲᎲⰾ", 4, 'r');
        guy_poke_d   = mk_ani(L"ᎲᎲᎲᒀᒀᒀᒀᎲᎲⰾ", 0, 0);

        guy_dodge_d  = mk_ani(L"ᎲᎲᎲᏡᏡᏡᏡȣȣȣȣᏡᎲᎲⰾ", 8, 'd');
        guy_falltest = mk_ani(L"ᎲᎲᎲޗޗޗⲁⲁⲁᥑ", 4, 'r');
        guy_dodge_l  = mk_ani(L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ", 5, 'l');
        guy_dodge_r  = mk_ani(L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ", 5, 'r');
        guy_kick_r   = mk_ani(L"ᎲᎲᎲ࿂࿂ᱯ࿂࿂༱༱ꝬꝬỿᓀᓀᎲⰾ", 9, 'r');
        guy_kick_l   = mk_ani(L"ᎲᎲᎲ࿂࿂ᱯᱯ࿂࿂༱ꝬꝬꝬᓂᓂᎲⰾ", 9, 'l');
        guy_jump_ul  = mk_ani(L"ᎲᎲᎲⰾⰾᎹᎹᎹᎹᎹᎹᎲᎲⰾ", 7, 'u'+'l');
        guy_jump_ur  = mk_ani(L"ᎲᎲᎲⰾⰾᎹᎹᎹᎹᎹᎹᎲᎲⰾ", 7, 'u'+'r');

        guy_dig_u    = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", 7, 'u');
        guy_dig_d    = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", 7, 'd');
        guy_dig_l    = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", 7, 'l');
        guy_dig_r    = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", 7, 'r');

        guy_pickup   = mk_ani(L"ᎲᎲᎲⰾ", 0, 0);

        built = true;
}

/*----------------------------------------------------------------------------*/
/*
 * render_human -- the rendering method for human noun models
 * @self: pointer to a noun of model 'HUMAN'
 */
void render_human(void *self)
{
        struct noun_t *noun = (struct noun_t *)self;
        struct item_t *item;

        noun_animate(noun);
        noun->_fall(noun);
        glow_light(doorlight, noun, false);

        list_for_each(&noun->inv, item, node) {
                if (item->equipped && item->burn)
                        item->burn(item, noun);
        }
}

/*
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
        case SM_DigUp:
                noun->_animate(noun, guy_dig_u);
                break;
        case SM_DigDown:
                noun->_animate(noun, guy_dig_d);
                break;
        case SM_DigLeft:
                noun->_animate(noun, guy_dig_l);
                break;
        case SM_DigRight:
                noun->_animate(noun, guy_dig_r);
                break;
        case SM_GoUp:
                noun->_animate(noun, guy_walk_u);
                break;
        case SM_GoDown:
                noun->_animate(noun, guy_walk_d);
                break;
        case SM_GoLeft:
                noun->_animate(noun, guy_walk_l);
                break;
        case SM_GoRight:
                noun->_animate(noun, guy_walk_r);
                break;
        case SM_RunUp:
                noun->_animate(noun, guy_run_u);
                break;
        case SM_RunDown:
                noun->_animate(noun, guy_run_d);
                break;
        case SM_RunLeft:
                noun->_animate(noun, guy_run_l);
                break;
        case SM_RunRight:
                noun->_animate(noun, guy_run_r);
                break;
        case SM_PickUp:
                noun->_animate(noun, guy_pickup);
                noun->_take(noun, pos_y(noun->pos), pos_x(noun->pos));
                break;
        /* ------------------------------------ Combat */
        case SM_Hit:
                HP_SUB(&noun->vitals, sm_mag(noun->sm));
                say_stats(noun->vitals);
                break;
        case SM_PokeUp:
                noun->_animate(noun, guy_poke_u);
                emit_to_noun(noun, 'u', SM_Punch | SM_Wait(3));
                break;
        /* ------------------------------------ keyboard input */
        /* ------------------------------------ walk (step) */
        case SM_Key('j'):
        case SM_Key('s'):
                sm_msg(noun->sm, SM_SELF, SM_GoDown | SM_Wait(wait_for(noun)));
                break;
        case SM_Key('k'):
        case SM_Key('w'):
                sm_msg(noun->sm, SM_SELF, SM_GoUp | SM_Wait(wait_for(noun)));
                break;
        case SM_Key('h'):
        case SM_Key('a'):
                sm_msg(noun->sm, SM_SELF, SM_GoLeft | SM_Wait(wait_for(noun)));
                break;
        case SM_Key('l'):
        case SM_Key('d'):
                sm_msg(noun->sm, SM_SELF, SM_GoRight | SM_Wait(wait_for(noun)));
                break;
        /* ------------------------------------------ run  */
        case SM_Key('J'):
        case SM_Key('S'):
                sm_msg(noun->sm, SM_SELF, SM_RunDown | SM_Opt(STICKY));
                break;
        case SM_Key('K'):
        case SM_Key('W'):
                sm_msg(noun->sm, SM_SELF, SM_RunUp | SM_Opt(STICKY));
                break;
        case SM_Key('H'):
        case SM_Key('A'):
                sm_msg(noun->sm, SM_SELF, SM_RunLeft | SM_Opt(STICKY));
                break;
        case SM_Key('L'):
        case SM_Key('D'):
                sm_msg(noun->sm, SM_SELF, SM_RunRight | SM_Opt(STICKY));
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
                sm_msg(noun->sm, SM_SELF, SM_PokeUp);
                break;
        case SM_Key('r'):
                sm_msg(noun->sm, SM_SELF, SM_PickUp | SM_Wait(wait_for(noun)));
                break;
        case SM_Key('@'):
                if (equipped)
                        equipped->equip(equipped, true);
                break;
        case SM_Key('|'):
                break;
        case SM_Key(KEY_ESC):
                return (MODE_RELEASE);
        }

        sm_refresh(noun->sm);

        return (MODE_PERSIST);
}


/****************************************************************************** 
 * MODEL: Dummy
 * NOTES: Test unit for combat and collision
 ******************************************************************************/
/*----------------------------------------------------------------------------*/
struct ani_t *bonk_test;
struct ani_t *dummy_mv_u;
struct ani_t *dummy_mv_d;
struct ani_t *dummy_mv_l;
struct ani_t *dummy_mv_r;
struct ani_t *dummy_die; 

static void build_dummy_animations(void)
{
        static bool built = false;

        if (built)
                return;

        bonk_test  = mk_ani(L"ⰊⰊⰊⰊⰉ", 0, 0);
        dummy_mv_u = mk_ani(L"ⰹⰹⰺⰺⰹⰹⰹⰉ", 4, 'u');
        dummy_mv_d = mk_ani(L"ⰹⰹⰺⰺⰹⰹⰹⰉ", 4, 'd');
        dummy_mv_l = mk_ani(L"ⰹⰹⰺⰺⰹⰹⰹⰉ", 4, 'l');
        dummy_mv_r = mk_ani(L"ⰹⰹⰺⰺⰹⰹⰹⰉ", 4, 'r');
        dummy_die  = mk_ani(L"ⰊⰊⰊⰊⰉ✶✶✺✺✺❁❁✴", 0, 0);

        built = true;
}

/*----------------------------------------------------------------------------*/
/*
 * render_dummy -- the rendering method for dummy noun models
 * @noun: pointer to a noun of model 'DUMMY'
 */
void render_dummy(void *self)
{
        struct noun_t *noun = (struct noun_t *)self;

        noun_animate(noun);
}

/*
 * modify_dummy -- the state machine method for dummy noun models
 * @noun: pointer to a noun of model 'DUMMY'
 */
int modify_dummy(void *obj)
{
        struct noun_t *noun = (struct noun_t *)obj;
        uint32_t nbr;
        int state;

        /*nbr = noun_nbr(noun, pos_hdg(noun->pos));*/

        if (!noun->is_mobile)
                return 0;

        sm_consume(noun->sm);
        state = sm_state(noun->sm);

        switch (state)
        {
        case SM_Punch:
                noun->_animate(noun, dummy_die);
                sm_msg(noun->sm, SM_SELF, SM_Destroy|SM_Wait(wait_for(noun))|SM_Pri(9));
                sm_screen(noun->sm, 9);
                break;
        case SM_GoUp:
                noun->_animate(noun, dummy_mv_u);
                break;
        case SM_GoDown:
                if (flip_biased(0.4))
                        say_speak(L"䥚", "I'm gonna hop all over you!");
                noun->_animate(noun, dummy_mv_d);
                break;
        case SM_GoLeft:
                noun->_animate(noun, dummy_mv_l);
                if (flip_biased(0.4))
                        say_speak(L"䥚", "Get fucked!");
                break;
        case SM_GoRight:
                noun->_animate(noun, dummy_mv_r);
                break;
        case SM_Destroy:
                alert(I_KILL, noun->name);
                noun_item_drop(noun);
                noun->_doom(noun);
                say_speak(L"", "");
                break;
        case SM_Seek:
                /*if (nbr) {*/
                        /*noun->_animate(noun, bonk_test);*/
                        /*sm_msgmag(noun->sm, nbr, SM_Hit, 3);*/
                /*} else {*/
                        noun->_seek(noun, get_noun("Guy"));
                /*}*/
                sm_msg(noun->sm, SM_SELF, SM_Seek | SM_Wait(20) | SM_Pri(1));
                break;
        case SM_Default:
                break;
        }

        sm_refresh(noun->sm);

        return 0;
}


/* The all-important late-binding dynamic linker
``````````````````````````````````````````````````````````````````````````````*/
/*
 * apply_noun_model -- implement the polymorphic noun model scheme
 * @noun: pointer to a noun with model field specified
 */
void apply_noun_model(struct noun_t *noun)
{
        build_dummy_animations();
        build_person_animations();

        switch (noun->model) 
        {
        case PERSON:
                HP_SET(&noun->vitals, 25);
                SP_SET(&noun->vitals, 13);
                AP_SET(&noun->vitals, 2);
                noun->pos     = new_pos(1, 1, CENTERED, LINES, COLS, 0, 0);
                noun->win     = newwin(1, 1, CENTERED);
                noun->pan     = new_panel(noun->win);
                noun->_modify = &modify_human;
                noun->_render = &render_human;
                noun->sprite = wdup(L"ⰾ");
                noun->color  = FLEX;
                break;
        case DUMMY:
                noun->pos     = new_pos(1, 1, CENTERED, LINES, COLS, 0, 0);
                noun->win     = newwin(1, 1, CENTERED);
                noun->pan     = new_panel(noun->win);
                noun->_modify = &modify_dummy;
                noun->_render = &render_dummy;
                sm_msg(noun->sm, SM_SELF, SM_Seek | SM_Wait(20));
                noun->sprite = wdup(L"Ⰹ");
                noun->color  = FLEX;
                break;
        }

        wbkgrnd(noun->win, mkcch(noun->sprite, 0, noun->color));
        astar_init(noun->astar, ACTIVE->tile, pos_y(noun->pos), pos_x(noun->pos));
        noun->map_id = ACTIVE->id;
}


