#include "../com/arawak.h"
#include "noun_model.h"
#include "../eng/fsm.h"


void noun_animate(struct noun_t *noun)
{
        if (!noun->mob.animate) return;

        struct mob_t *mob = &noun->mob;

        /* Assign length if it hasn't been */
        if (mob->animate->len == 0)
                mob->animate->len = wcslen(mob->animate->frame);

        /* Increment the current frame and/or terminate the animation */
        if (++mob->animate->i == mob->animate->len) {
                mob->animate->i = 0;
                mob->animate = NULL;
                return;
        }

        /* Draw the current frame */
        wadd_wch(mob->win, mkcch(&mob->animate->frame[mob->animate->i], 0, FLEX));

        /* Move the panel if this is the mv_frame */
        if (mob->animate->i == mob->animate->mv_frame)
                mob_move(mob, mob->animate->mv_dir);

        /* Signal if this is the sig_frame */
        if (mob->animate->i == mob->animate->verb_frame)
                mob_set_signal(mob, mob->animate->verb_id, mob->animate->verb_dir);

        return;
}





/******************************************************************************
 * MODEL: Person
 * NOTES: This is the main player character
 ******************************************************************************/
/*----------------------------------------------------------------------------*/
wchar_t *base = L"ⰾ";
struct animation run_u_test   = {L"Ꮂⲑⲑⱚⰾ", 1, 'u'};
struct animation run_d_test   = {L"Ꮂⲑⲑⱚⰾ", 1, 'd'};
struct animation run_l_test   = {L"Ꮂⲑⲑⱚⰾ", 1, 'l'};
struct animation run_r_test   = {L"Ꮂⲑⲑⱚⰾ", 1, 'r'};
struct animation punch_r_test = {L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾ"};
struct animation punch_l_test = {L"ᎲᎲᎲ᱙ᕦ᱙ᎲᎲⰾ"};
struct animation slashtest    = {L"ᎲᎲᎲᒀᒀᒀᒀᎲᎲⰾ", 0, 4, VERB_Punch, 'u'};
struct animation dodgetest    = {L"ᎲᎲᎲᏡᏡᏡᏡȣȣȣȣᏡᎲᎲⰾ", 9, 'd'};
struct animation falltest     = {L"ᎲᎲᎲޗޗޗⲁⲁⲁᥑ", 5, 'r'};
struct animation dodge_l_test = {L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ", 6, 'l'};
struct animation dodge_r_test = {L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ", 6, 'r'};
struct animation kick_r_test  = {L"ᎲᎲᎲ࿂࿂ᱯ࿂࿂༱༱ꝬꝬỿᓀᓀᎲⰾ", 10, 'r'};
struct animation kick_l_test  = {L"ᎲᎲᎲ࿂࿂ᱯᱯ࿂࿂༱ꝬꝬꝬᓂᓂᎲⰾ", 10, 'l'};
/*----------------------------------------------------------------------------*/
/*
 * render_human -- the rendering method for human noun models
 * @noun: pointer to a noun of model 'HUMAN'
 */
void render_human(struct noun_t *noun)
{
        static bool done;
        if (!done)
                wbkgrnd(noun->mob.win, mkcch(L"ⰾ", 0, FLEX));

        noun_animate(noun);
}

/*
 * modify_human -- the state machine for human noun models
 * @noun: pointer to a noun of model 'HUMAN'
 */
int modify_human(void *obj, int verb, int value)
{
        struct noun_t *noun = (struct noun_t *)obj;

        switch (verb) 
        { 
        case VERB_Keyboard:
                switch (value) {
                case 'j':
                case 's':
                        noun->mob.animate = &run_d_test;
                        break;
                case 'k':
                case 'w':
                        noun->mob.animate = &run_u_test;
                        break;
                case 'H':
                case 'A':
                        noun->mob.animate = &kick_l_test;
                        break;
                case 'h':
                case 'a':
                        noun->mob.animate = &run_l_test;
                        break;
                case 'L':
                case 'D':
                        noun->mob.animate = &kick_r_test;
                        break;
                case 'l':
                case 'd':
                        noun->mob.animate = &run_r_test;
                        break;
                case 'g':
                        top_panel(noun->mob.pan);
                        break;
                case 'r':
                        noun->mob.animate = &punch_l_test;
                        break;
                case 't':
                        noun->mob.animate = &punch_r_test;
                        break;
                case ' ':
                        noun->mob.animate = &dodgetest;
                        break;
                case 'e':
                        noun->mob.animate = &slashtest;
                        break;
                case '|':
                        noun->mob.animate = &falltest;
                        break;
                case KEY_ESC:
                        return (MODE_RELEASE);
                }
                break;
        }
        return (MODE_PERSIST);
}


/****************************************************************************** 
 * MODEL: Dummy
 * NOTES: Test unit for combat and collision
 ******************************************************************************/
/*----------------------------------------------------------------------------*/
wchar_t *dummy_base = L"Ⰹ";
struct animation bonk_test = { L"ⰊⰊⰊⰊⰉ"};
/*----------------------------------------------------------------------------*/
/*
 * render_dummy -- the rendering method for dummy noun models
 * @noun: pointer to a noun of model 'DUMMY'
 */
void render_dummy(struct noun_t *noun)
{
        static bool done;

        if (!done)
                wbkgrnd(noun->mob.win, mkcch(L"Ⰹ", 0, FLEX));

        noun_animate(noun);
}

/*
 * modify_dummy -- the state machine method for dummy noun models
 * @noun: pointer to a noun of model 'DUMMY'
 */
int modify_dummy(void *obj, int verb, int value)
{
        struct noun_t *noun = (struct noun_t *)obj;
        
        switch (verb)
        {
        case VERB_Punch:
                wprintw(CONSOLE_WIN, "Dummy hit!\n");
                noun->mob.animate = &bonk_test;
                break;
        }

        return 0;
}




/*
 * apply_noun_model -- implement the polymorphic noun model scheme
 * @noun: pointer to a noun with model field specified
 */
void apply_noun_model(struct noun_t *noun)
{
        switch (noun->model) 
        {
        case PERSON:
                mob_cfg(&noun->mob, FIELD, 1, 1, CENTERED);
                noun->mob.name = noun->key;
                noun->options |= NOUN_CREATURE;
                noun_set_modify(noun, &modify_human);
                noun_set_render(noun, &render_human);
                break;
        case DUMMY:
                mob_cfg(&noun->mob, FIELD, 1, 1, CENTERED);
                noun->mob.name = noun->key;
                noun->options |= NOUN_CREATURE;
                noun_set_modify(noun, &modify_dummy);
                noun_set_render(noun, &render_dummy);
                break;
        }
}


