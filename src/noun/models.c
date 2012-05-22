#include "../com/arawak.h"
#include "../lib/textutils.h"
#include "../gfx/ui/notify.h"
#include "models.h"
#include "noun.h"
#include "../eng/fsm.h"
#include "../item/item.h"
#include "../item/tools.h"
#include "../gfx/ui/menu_inventory.h"


struct ani_t *mk_ani(const wchar_t *wcs, int mv_frame, int mv_dir)
{
        struct ani_t *new = calloc(1, sizeof(struct ani_t));

        new->frame     = wcsdup(wcs);
        new->len       = wcslen(wcs);
        new->mv_frame  = mv_frame;
        new->mv_dir    = mv_dir;

        return (new);
}


#define MV(frame,dir) frame, dir
#define MSG(frame,verb,dir) frame, verb, dir
#define NO_MV MV(0,0) 


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
        wadd_wch(noun->win, mkcch(&ani->frame[ani->i], 0, FLEX));

        /* Move the panel if this is the mv_frame */
        if (ani->i == ani->mv_frame)
                noun->_step(noun, ani->mv_dir);

        /* Increment the current frame and/or terminate the animation */
        if (++noun->animation->i == noun->animation->len) {
                noun->animation->i = 0;
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
/*----------------------------------------------------------------------------*/
void build_person_animations(void)
{
        static bool built = false;

        if (built)
                return;

        guy_walk_u   = mk_ani(L"ⲑⲑᎲⰾ"              , MV(0,'u'));
        guy_walk_d   = mk_ani(L"ⲑⲑᎲⰾ"              , MV(0,'d'));
        guy_walk_l   = mk_ani(L"ⲑⲑᎲⰾ"              , MV(0,'l'));
        guy_walk_r   = mk_ani(L"ⲑⲑᎲⰾ"              , MV(0,'r'));

        guy_run_u    = mk_ani(L"ⲑⰾᎲⰾ"              , MV(0,'u'));
        guy_run_d    = mk_ani(L"ⲑⰾᎲⰾ"              , MV(0,'d'));
        guy_run_l    = mk_ani(L"ⲑⰾᎲⰾ"              , MV(0,'l'));
        guy_run_r    = mk_ani(L"ⲑⰾᎲⰾ"              , MV(0,'r'));

        guy_punch_r  = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾ"         , NO_MV);
        guy_punch_l  = mk_ani(L"ᎲᎲᎲ᱙ᕦ᱙ᎲᎲⰾ"         , NO_MV);

        guy_poke_u   = mk_ani(L"ᎲᎲᎲᒀᒀᒀᒀᎲᎲⰾ"        , NO_MV);

        guy_dodge_d  = mk_ani(L"ᎲᎲᎲᏡᏡᏡᏡȣȣȣȣᏡᎲᎲⰾ"   , MV(8,'d'));
        guy_falltest = mk_ani(L"ᎲᎲᎲޗޗޗⲁⲁⲁᥑ"        , MV(4,'r'));
        guy_dodge_l  = mk_ani(L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ"     , MV(5,'l'));
        guy_dodge_r  = mk_ani(L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ"     , MV(5,'r'));
        guy_kick_r   = mk_ani(L"ᎲᎲᎲ࿂࿂ᱯ࿂࿂༱༱ꝬꝬỿᓀᓀᎲⰾ" , MV(9,'r'));
        guy_kick_l   = mk_ani(L"ᎲᎲᎲ࿂࿂ᱯᱯ࿂࿂༱ꝬꝬꝬᓂᓂᎲⰾ" , MV(9,'l'));
        guy_jump_ul  = mk_ani(L"ᎲᎲᎲⰾⰾᎹᎹᎹᎹᎹᎹᎲᎲⰾ"    , MV(7,'u'+'l'));
        guy_jump_ur  = mk_ani(L"ᎲᎲᎲⰾⰾᎹᎹᎹᎹᎹᎹᎲᎲⰾ"    , MV(7,'u'+'r'));

        guy_dig_u    = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ"     , MV(7,'u'));
        guy_dig_d    = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ"     , MV(7,'d'));
        guy_dig_l    = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ"     , MV(7,'l'));
        guy_dig_r    = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ"     , MV(7,'r'));

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
        static bool done;

        if (!done) {
                wbkgrnd(noun->win, mkcch(L"ⰾ", 0, FLEX));
                done = true;
        }

        struct item_t *item;

        noun_animate(noun);

        list_for_each(&noun->inv, item, node) {
                if (item->equipped && item->burn)
                        item->burn(item, noun);
        }
        sm_consume(noun->sm);
}

/*
 * modify_human -- the state machine for human noun models
 * @self: pointer to a noun of model 'HUMAN'
 */
int modify_human(void *self)
{
        struct noun_t *noun = (struct noun_t *)self;
        int state;
       
        state = sm_state(noun->sm);

        switch (state) 
        { 
        case SM_DigUp:
                noun->animate(guy_dig_u);
                break;
        case SM_DigDown:
                noun->animate(guy_dig_d);
                break;
        case SM_DigLeft:
                noun->animate(guy_dig_l);
                break;
        case SM_DigRight:
                noun->animate(guy_dig_r);
                break;
        case SM_GoUp:
                noun->animate(guy_walk_u);
                break;
        case SM_GoDown:
                noun->animate(guy_walk_d);
                break;
        case SM_GoLeft:
                noun->animate(guy_walk_l);
                break;
        case SM_GoRight:
                noun->animate(guy_walk_r);
                break;
        case SM_RunUp:
                noun->animate(guy_run_u);
                break;
        case SM_RunDown:
                noun->animate(guy_run_d);
                break;
        case SM_RunLeft:
                noun->animate(guy_run_l);
                break;
        case SM_RunRight:
                noun->animate(guy_run_r);
                break;
        /* ------------------------------------ keyboard input */
        /* ------------------------------------ walk (step) */
        case SM_Key('j'):
        case SM_Key('s'):
                sm_msg(noun->sm, SM_SELF, SM_GoDown);
                break;
        case SM_Key('k'):
        case SM_Key('w'):
                sm_msg(noun->sm, SM_SELF, SM_GoUp);
                break;
        case SM_Key('h'):
        case SM_Key('a'):
                sm_msg(noun->sm, SM_SELF, SM_GoLeft);
                break;
        case SM_Key('l'):
        case SM_Key('d'):
                sm_msg(noun->sm, SM_SELF, SM_GoRight);
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
        case SM_Key('g'):
                top_panel(noun->pan);
                break;
        case SM_Key('t'):
                noun->animate(guy_punch_r);
                break;
        case SM_Key(' '):
                if (equipped) 
                        equipped->use(equipped, noun);
                break;
        case SM_Key('e'):
                noun->animate(guy_poke_u);
                emit_to_noun(noun, 'u', SM_Punch | SM_Wait(3) | SM_Pri(1));
                break;
        case SM_Key('r'):
                noun->take(pos_y(noun->pos), pos_x(noun->pos));
                break;
        case SM_Key('}'):
                tab_cycle(2);
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

        bonk_test  = mk_ani(L"ⰊⰊⰊⰊⰉ",         NO_MV);
        dummy_mv_u = mk_ani(L"ⰹⰹⰺⰺⰹⰹⰹⰉ",      MV(4,'u'));
        dummy_mv_d = mk_ani(L"ⰹⰹⰺⰺⰹⰹⰹⰉ",      MV(4,'d'));
        dummy_mv_l = mk_ani(L"ⰹⰹⰺⰺⰹⰹⰹⰉ",      MV(4,'l'));
        dummy_mv_r = mk_ani(L"ⰹⰹⰺⰺⰹⰹⰹⰉ",      MV(4,'r'));
        dummy_die  = mk_ani(L"ⰊⰊⰊⰊⰉ✶✶✺✺✺❁❁✴", NO_MV);

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
        static bool done;

        if (!done) {
                wbkgrnd(noun->win, mkcch(L"Ⰹ", 0, FLEX));
                done = true;
        }
        noun_animate(noun);
        sm_consume(noun->sm);
}

/*
 * modify_dummy -- the state machine method for dummy noun models
 * @noun: pointer to a noun of model 'DUMMY'
 */
int modify_dummy(void *obj)
{
        struct noun_t *noun = (struct noun_t *)obj;
        static int wait = 0;
        int state;

        state = sm_state(noun->sm);

        switch (state)
        {
        case SM_Punch:
                noun->animate(dummy_die);
                say(L"嶴", "FUCK!");
                sm_msg(noun->sm, SM_SELF, SM_Destroy|SM_Wait(13)|SM_Pri(9));
                sm_screen(noun->sm, 9);
                break;
        case SM_GoUp:
                noun->animate(dummy_mv_u);
                break;
        case SM_GoDown:
                if (flip_biased(0.4))
                        say(L"䥚", "I'm gonna hop all over you!");
                noun->animate(dummy_mv_d);
                break;
        case SM_GoLeft:
                noun->animate(dummy_mv_l);
                if (flip_biased(0.4))
                        say(L"䥚", "Get fucked!");
                break;
        case SM_GoRight:
                noun->animate(dummy_mv_r);
                break;
        case SM_Destroy:
                alert(I_KILL, noun->name);
                noun->doom();
                oops = false;
                say(L"\n","\n");
                break;
        case SM_Seek:
                noun->_seek(noun, get_noun("Guy"));
                sm_msg(noun->sm, noun->sm->id, SM_Seek | SM_Wait(20));
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
        /* Class method table */
        modify[PERSON] = (modify[PERSON]) ? modify[PERSON] : &modify_human;
        render[PERSON] = (render[PERSON]) ? render[PERSON] : &render_human;
        modify[DUMMY]  = (modify[DUMMY])  ? modify[DUMMY]  : &modify_dummy;
        render[DUMMY]  = (render[DUMMY])  ? render[DUMMY]  : &render_dummy;

        noun->_modify = modify[noun->model];
        noun->_render = render[noun->model];

        build_dummy_animations();
        build_person_animations();

        switch (noun->model) 
        {
        case PERSON:
                noun->pos = new_pos(1, 1, CENTERED, LINES, COLS, 0, 0);
                noun->win = newwin(1, 1, CENTERED);
                noun->pan = new_panel(noun->win);
                noun->model = NOUN_CREATURE;
                break;
        case DUMMY:
                noun->pos = new_pos(1, 1, CENTERED, LINES, COLS, 0, 0);
                noun->win = newwin(1, 1, CENTERED);
                noun->pan = new_panel(noun->win);
                noun->model = NOUN_CREATURE;
                sm_set(noun->sm, SM_Seek, 0);
                oops = true;
                break;
        }

        astar_init(noun->astar, ACTIVE->tile, pos_y(noun->pos), pos_x(noun->pos));
        noun->map_id = ACTIVE->id;
}


