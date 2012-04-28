#include "../com/arawak.h"
#include "noun.h"
#include "../eng/fsm.h"
#include "models.h"
#include "../mob/mob.h"

#define STATE_RESET(noun) noun_set_state(noun, 0, 0)


/* Animation of nouns during rendering 
``````````````````````````````````````````````````````````````````````````````*/
struct ani_t {
        wchar_t frame[20];      /* Frames in the animation reel */
        int mv_frame;           /* Frame on which to issue a move signal */
        int mv_dir;             /* Direction in which to move */
        int verb_frame;         /* Frame on which to issue a verb signal */
        int verb_id;            /* Verb to issue */
        int verb_dir;           /* Neighbor to address the verb to */
        int len;                /* Length of the animation (late binding) */
        int i;                  /* Incrementer for internal use */ 
};

/*
 * noun_animate -- draw successive character "frames", trigger movement, etc.
 * @noun: pointer to a noun
 */
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



/* Model (class) definitions 
``````````````````````````````````````````````````````````````````````````````*/
#define MV(frame,dir) frame, dir
#define VB(frame,verb,dir) frame, verb, dir
#define NOMV MV(0,0) 
#define NOVB VB(0,0,0) 

/******************************************************************************
 * MODEL: Person
 * NOTES: This is the main player character
 ******************************************************************************/
/*----------------------------------------------------------------------------*/
wchar_t *base = L"ⰾ";
struct ani_t run_u_test   = {L"Ꮂⲑⲑⱚⰾ",             MV(1,'u'), NOVB};
struct ani_t run_d_test   = {L"Ꮂⲑⲑⱚⰾ",             MV(1,'d'), NOVB};
struct ani_t run_l_test   = {L"Ꮂⲑⲑⱚⰾ",             MV(1,'l'), NOVB};
struct ani_t run_r_test   = {L"Ꮂⲑⲑⱚⰾ",             MV(1,'r'), NOVB};
struct ani_t punch_r_test = {L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾ",         NOMV, NOVB};
struct ani_t punch_l_test = {L"ᎲᎲᎲ᱙ᕦ᱙ᎲᎲⰾ",         NOMV, NOVB};
struct ani_t slashtest    = {L"ᎲᎲᎲᒀᒀᒀᒀᎲᎲⰾ",        NOMV, VB(4,VERB_Punch,'u')};
struct ani_t dodgetest    = {L"ᎲᎲᎲᏡᏡᏡᏡȣȣȣȣᏡᎲᎲⰾ",   MV(9,'d'), NOVB};
struct ani_t falltest     = {L"ᎲᎲᎲޗޗޗⲁⲁⲁᥑ",        MV(5,'r'), NOVB};
struct ani_t dodge_l_test = {L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ",     MV(6,'l'), NOVB};
struct ani_t dodge_r_test = {L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ",     MV(6,'r'), NOVB};
struct ani_t kick_r_test  = {L"ᎲᎲᎲ࿂࿂ᱯ࿂࿂༱༱ꝬꝬỿᓀᓀᎲⰾ", MV(10,'r'), NOVB};
struct ani_t kick_l_test  = {L"ᎲᎲᎲ࿂࿂ᱯᱯ࿂࿂༱ꝬꝬꝬᓂᓂᎲⰾ", MV(10,'l'), NOVB};
/*----------------------------------------------------------------------------*/
/*
 * render_human -- the rendering method for human noun models
 * @noun: pointer to a noun of model 'HUMAN'
 */
void render_human(void *self)
{
        struct noun_t *noun = (struct noun_t *)self;
        static bool done;

        if (!done)
                wbkgrnd(noun->mob.win, mkcch(L"ⰾ", 0, FLEX));

        noun_animate(noun);
        mob_move(&noun->mob, '*');
}

/*
 * modify_human -- the state machine for human noun models
 * @noun: pointer to a noun of model 'HUMAN'
 */
int modify_human(void *self)
{
        struct noun_t *noun = (struct noun_t *)self;

        switch (noun->state) 
        { 
        case VERB_Keyboard:
                switch (noun->value) {
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
        STATE_RESET(noun);

        return (MODE_PERSIST);
}


/****************************************************************************** 
 * MODEL: Dummy
 * NOTES: Test unit for combat and collision
 ******************************************************************************/
/*----------------------------------------------------------------------------*/
wchar_t *dummy_base = L"Ⰹ";
struct ani_t bonk_test = {L"ⰊⰊⰊⰊⰉ", NOMV, NOVB};
struct ani_t dummy_mv_u = {L"ⰹⰹꝿꝿⰺⰹⰹⰹⰉ", MV(4,'u'), NOVB};
struct ani_t dummy_mv_d = {L"ⰹⰹꝿꝿⰺⰹⰹⰹⰉ", MV(4,'d'), NOVB};
struct ani_t dummy_mv_l = {L"ⰹⰹꝿꝿⰺⰹⰹⰹⰉ", MV(4,'l'), NOVB};
struct ani_t dummy_mv_r = {L"ⰹⰹꝿꝿⰺⰹⰹⰹⰉ", MV(4,'r'), NOVB};
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
                wbkgrnd(noun->mob.win, mkcch(L"Ⰹ", 0, FLEX));

        noun_animate(noun);
        mob_move(&noun->mob, '*');
}

/*
 * modify_dummy -- the state machine method for dummy noun models
 * @noun: pointer to a noun of model 'DUMMY'
 */
int modify_dummy(void *obj)
{
        struct noun_t *noun = (struct noun_t *)obj;
        static int wait = 0;
        int enter = noun->state;

        wait = (wait+1)%20;
        
        switch (noun->state)
        {
        case VERB_Default:
                if (wait == 13)
                        mob_seek(noun, get_noun("Guy"));
                break;
        case VERB_Punch:
                wprintw(CONSOLE_WIN, "Dummy hit!\n");
                noun->mob.animate = &bonk_test;
                break;
        case VERB_GoUp:
                noun->mob.animate = &dummy_mv_u;
                break;
        case VERB_GoDown:
                noun->mob.animate = &dummy_mv_d;
                break;
        case VERB_GoLeft:
                noun->mob.animate = &dummy_mv_l;
                break;
        case VERB_GoRight:
                noun->mob.animate = &dummy_mv_r;
                break;
        }
        if (noun->state == enter)
                STATE_RESET(noun);

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
        noun->mob.name = noun->key;

        switch (noun->model) 
        {
        case PERSON:
                mob_cfg(&noun->mob, ACTIVE, 1, 1, CENTERED);
                noun->options |= NOUN_CREATURE;
                break;
        case DUMMY:
                mob_cfg(&noun->mob, ACTIVE, 1, 1, CENTERED);
                noun->options |= NOUN_CREATURE;
                break;
        }
}


