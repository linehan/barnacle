#include "../com/arawak.h"
#include "noun.h"
#include "../eng/fsm.h"
#include "models.h"
#include "noun.h"
#include "../mob/mob.h"
#include "../equip/rope.h"
#include "../equip/equipment.h"
#include "../mob/inventory.h"


/*
 * noun_animate -- draw successive character "frames", trigger movement, etc.
 * @noun: pointer to a noun
 */
void noun_animate(struct noun_t *noun)
{
        if (!noun->animation) 
                return;

        struct ani_t *ani = noun->animation;

        /* Assign length if it hasn't been */
        if (ani->len == 0)
                ani->len = wcslen(ani->frame);

        /* Draw the current frame */
        wadd_wch(noun->win, mkcch(&ani->frame[ani->i], 0, FLEX));

        /* Move the panel if this is the mv_frame */
        if (ani->i == ani->mv_frame)
                noun_move(noun, ani->mv_dir);
        if (ani->i == ani->mv_frame_alt)
                noun_move(noun, ani->mv_dir_alt);

        /* Signal if this is the sig_frame */
        if (ani->i == ani->verb_frame)
                noun_set_signal(noun, ani->verb_id, ani->verb_dir);

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
/*----------------------------------------------------------------------------*/
wchar_t *base = L"ⰾ";//ⱚᏅᎤ
struct ani_t run_u_test   = {L"ⲑⲑᎲⰾ",              MV(0,'u'), NOMV, NOVB};
struct ani_t run_d_test   = {L"ⲑⲑᎲⰾ",              MV(0,'d'), NOMV, NOVB};
struct ani_t run_l_test   = {L"ⲑⲑᎲⰾ",              MV(0,'l'), NOMV, NOVB};
struct ani_t run_r_test   = {L"ⲑⲑᎲⰾ",              MV(0,'r'), NOMV, NOVB};
struct ani_t punch_r_test = {L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾ",         NOMV, NOMV, NOVB};
struct ani_t punch_l_test = {L"ᎲᎲᎲ᱙ᕦ᱙ᎲᎲⰾ",         NOMV, NOMV, NOVB};
struct ani_t slashtest    = {L"ᎲᎲᎲᒀᒀᒀᒀᎲᎲⰾ",        NOMV, NOMV, VB(3, SM_Punch,'u')};
struct ani_t dodgetest    = {L"ᎲᎲᎲᏡᏡᏡᏡȣȣȣȣᏡᎲᎲⰾ",   MV(8,'d'), NOMV, NOVB};
struct ani_t falltest     = {L"ᎲᎲᎲޗޗޗⲁⲁⲁᥑ",        MV(4,'r'), NOMV, NOVB};
struct ani_t dodge_l_test = {L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ",     MV(5,'l'), NOMV, NOVB};
struct ani_t dodge_r_test = {L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ",     MV(5,'r'), NOMV, NOVB};
struct ani_t kick_r_test  = {L"ᎲᎲᎲ࿂࿂ᱯ࿂࿂༱༱ꝬꝬỿᓀᓀᎲⰾ", MV(9,'r'), NOMV, NOVB};
struct ani_t kick_l_test  = {L"ᎲᎲᎲ࿂࿂ᱯᱯ࿂࿂༱ꝬꝬꝬᓂᓂᎲⰾ", MV(9,'l'), NOMV, NOVB};
struct ani_t jump_ul      = {L"ᎲᎲᎲⰾⰾᎹᎹᎹᎹᎹᎹᎲᎲⰾ",    MV(7,'u'+'l'), NOVB};
struct ani_t jump_ur      = {L"ᎲᎲᎲⰾⰾᎹᎹᎹᎹᎹᎹᎲᎲⰾ",    MV(7,'u'+'r'), NOVB};
/*----------------------------------------------------------------------------*/
/*
 * render_human -- the rendering method for human noun models
 * @self: pointer to a noun of model 'HUMAN'
 */
void render_human(void *self)
{
        struct noun_t *noun = (struct noun_t *)self;
        static bool done;

        if (!done)
                wbkgrnd(noun->win, mkcch(L"ⰾ", 0, FLEX));

        inv_burn(noun->inv, TORCHKEY(noun->inv));
        noun_animate(noun);
        noun->step(noun, '*');
        top_panel(noun->inv->equipped_pan);
}

/*
 * modify_human -- the state machine for human noun models
 * @self: pointer to a noun of model 'HUMAN'
 */
int modify_human(void *self)
{
        struct noun_t *noun = (struct noun_t *)self;
        int state = sm_state(noun->sm);

        switch (sm_state(noun->sm)) 
        { 
        case SM_Keyboard:
                switch (sm_value(noun->sm)) {
                case 'j':
                case 's':
                        set_animation(noun, &run_d_test);
                        break;
                case 'k':
                case 'w':
                        set_animation(noun, &run_u_test);
                        break;
                case 'H':
                case 'A':
                        set_animation(noun, &jump_ul);
                        break;
                case 'h':
                case 'a':
                        set_animation(noun, &run_l_test);
                        break;
                case 'L':
                case 'D':
                        set_animation(noun, &jump_ur);
                        break;
                case 'l':
                case 'd':
                        set_animation(noun, &run_r_test);
                        break;
                case 'g':
                        top_panel(noun->pan);
                        break;
                case 't':
                        set_animation(noun, &punch_r_test);
                        break;
                case ' ':
                        if (CUR_KEY(noun->inv))
                                inv_use(noun->inv, CUR_KEY(noun->inv));
                        break;
                case 'e':
                        set_animation(noun, &slashtest);
                        break;
                case '}':
                        tab_cycle(2);
                        break;
                case '@':
                        if (TORCHKEY(noun->inv)) {
                                tab_tog(0);
                                inv_use(noun->inv, TORCHKEY(noun->inv));
                        }
                        break;
                case '|':
                        if (ROPEKEY(noun->inv)) {
                                tab_tog(1);
                                inv_use(noun->inv, ROPEKEY(noun->inv));
                                tab_tog(1);
                        }
                        break;
                case KEY_ESC:
                        return (MODE_RELEASE);
                }
                break;
        }
        SM_RESET(noun->sm);

        return (MODE_PERSIST);
}


/****************************************************************************** 
 * MODEL: Dummy
 * NOTES: Test unit for combat and collision
 ******************************************************************************/
/*----------------------------------------------------------------------------*/
wchar_t *dummy_base = L"Ⰹ";
struct ani_t bonk_test = {L"ⰊⰊⰊⰊⰉ", NOMV, NOVB};
struct ani_t dummy_mv_u = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", MV(4,'u'), NOVB};
struct ani_t dummy_mv_d = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", MV(4,'d'), NOVB};
struct ani_t dummy_mv_l = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", MV(4,'l'), NOVB};
struct ani_t dummy_mv_r = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", MV(4,'r'), NOVB};
/*/ⰊⰺⰹꝾⰹⰑ⚲ꝿ*ⰋⰔⱄⰫ*/
/*----------------------------------------------------------------------------*/
/*
 * render_dummy -- the rendering method for dummy noun models
 * @noun: pointer to a noun of model 'DUMMY'
 */
void render_dummy(void *self)
{
        struct noun_t *noun = (struct noun_t *)self;
        static bool done;

        if (!done)
                wbkgrnd(noun->win, mkcch(L"Ⰹ", 0, FLEX));

        noun_animate(noun);
        noun->step(noun, '*');
}

/*
 * modify_dummy -- the state machine method for dummy noun models
 * @noun: pointer to a noun of model 'DUMMY'
 */
int modify_dummy(void *obj)
{
        struct noun_t *noun = (struct noun_t *)obj;
        static int wait = 0;
        int enter = sm_state(noun->sm);

        wait = (wait+1)%20;
        
        switch (sm_state(noun->sm))
        {
        case SM_Default:
                if (wait == 13)
                        noun->seek(noun, get_noun("Guy"));
                break;
        case SM_Punch:
                wprintw(CONSOLE_WIN, "Dummy hit!\n");
                set_animation(noun, &bonk_test);
                dock_say(L"嶴", "FUCK!");
                break;
        case SM_GoUp:
                set_animation(noun, &dummy_mv_u);
                break;
        case SM_GoDown:
                if (flip_biased(0.4))
                        dock_say(L"䥚", "I'm gonna hop all over you!");
                set_animation(noun, &dummy_mv_d);
                break;
        case SM_GoLeft:
                set_animation(noun, &dummy_mv_l);
                if (flip_biased(0.4))
                        dock_say(L"䥚", "Get fucked!");
                break;
        case SM_GoRight:
                set_animation(noun, &dummy_mv_r);
                break;
        }
        if (sm_state(noun->sm) == enter)
                SM_RESET(noun->sm);

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

        noun->modify = modify[noun->model];
        noun->render = render[noun->model];

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
                break;
        }

        astar_init(noun->astar, ACTIVE->tile, pos_y(noun->pos), pos_x(noun->pos));
        noun->map_id = ACTIVE->id;
}


