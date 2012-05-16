#include "../com/arawak.h"
#include "../lib/textutils.h"
#include "models.h"
#include "noun.h"
#include "../eng/fsm.h"
#include "../equip/equipment.h"
#include "../equip/equipment_items.h"
#include "inventory.h"

struct ani_t *mk_ani(const wchar_t *wcs, 
                     int mv_frame, int mv_dir, 
                     int msg_frame, int msg_tag, int msg_dir)
{
        struct ani_t *new = calloc(1, sizeof(struct ani_t));

        new->frame     = wcdup(wcs);
        new->len       = wcslen(new->frame);
        new->mv_frame  = mv_frame;
        new->mv_dir    = mv_dir;
        new->msg_frame = (msg_frame < 0) ? (new->len-1) : msg_frame;
        new->msg_tag   = msg_tag;
        new->msg_dir   = msg_dir;

        return (new);
}


#define MV(frame,dir) frame, dir
#define MSG(frame,verb,dir) frame, verb, dir
#define NO_MV  MV(0,0) 
#define NO_MSG MSG(0,0,0) 
#define MSG_RESET MSG(-1, SM_Reset, '*')


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

        /* Signal if this is the sig_frame */
        if (ani->i == ani->msg_frame)
                noun_set_signal(noun, ani->msg_tag, ani->msg_dir);

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
struct ani_t *run_u_test;
struct ani_t *run_d_test;
struct ani_t *run_l_test;
struct ani_t *run_r_test;
struct ani_t *punch_r_test;
struct ani_t *punch_l_test;
struct ani_t *slashtest;
struct ani_t *dodgetest;
struct ani_t *falltest;
struct ani_t *dodge_l_test;
struct ani_t *dodge_r_test;
struct ani_t *kick_r_test;
struct ani_t *kick_l_test;
struct ani_t *jump_ul;
struct ani_t *jump_ur;
struct ani_t *dig_u;
struct ani_t *dig_d;
struct ani_t *dig_l;
struct ani_t *dig_r;
/*----------------------------------------------------------------------------*/
wchar_t *base = L"ⰾ";//ⱚᏅᎤ
void build_person_animations(void)
{
        static bool built = false;

        if (built)
                return;

        run_u_test   = mk_ani(L"ⲑⲑᎲⰾ"              , MV(0,'u'), NO_MSG);
        run_d_test   = mk_ani(L"ⲑⲑᎲⰾ"              , MV(0,'d'), NO_MSG);
        run_l_test   = mk_ani(L"ⲑⲑᎲⰾ"              , MV(0,'l'), NO_MSG);
        run_r_test   = mk_ani(L"ⲑⲑᎲⰾ"              , MV(0,'r'), NO_MSG);
        punch_r_test = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾ"         , NO_MV,     NO_MSG);
        punch_l_test = mk_ani(L"ᎲᎲᎲ᱙ᕦ᱙ᎲᎲⰾ"         , NO_MV,     NO_MSG);
        slashtest    = mk_ani(L"ᎲᎲᎲᒀᒀᒀᒀᎲᎲⰾ"        , NO_MV,     MSG(3,SM_Punch,'u'));
        dodgetest    = mk_ani(L"ᎲᎲᎲᏡᏡᏡᏡȣȣȣȣᏡᎲᎲⰾ"   , MV(8,'d'), NO_MSG);
        falltest     = mk_ani(L"ᎲᎲᎲޗޗޗⲁⲁⲁᥑ"        , MV(4,'r'), NO_MSG);
        dodge_l_test = mk_ani(L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ"     , MV(5,'l'), NO_MSG);
        dodge_r_test = mk_ani(L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ"     , MV(5,'r'), NO_MSG);
        kick_r_test  = mk_ani(L"ᎲᎲᎲ࿂࿂ᱯ࿂࿂༱༱ꝬꝬỿᓀᓀᎲⰾ" , MV(9,'r'), NO_MSG);
        kick_l_test  = mk_ani(L"ᎲᎲᎲ࿂࿂ᱯᱯ࿂࿂༱ꝬꝬꝬᓂᓂᎲⰾ" , MV(9,'l'), NO_MSG);
        jump_ul      = mk_ani(L"ᎲᎲᎲⰾⰾᎹᎹᎹᎹᎹᎹᎲᎲⰾ"    , MV(7,'u'+'l'), NO_MSG);
        jump_ur      = mk_ani(L"ᎲᎲᎲⰾⰾᎹᎹᎹᎹᎹᎹᎲᎲⰾ"    , MV(7,'u'+'r'), NO_MSG);

        dig_u = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", MV(7,'u'), MSG_RESET);
        dig_d = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", MV(7,'d'), MSG_RESET);
        dig_l = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", MV(7,'l'), MSG_RESET);
        dig_r = mk_ani(L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", MV(7,'r'), MSG_RESET);

        built = true;
}

/*struct ani_t run_u_test   = {L"ⲑⲑᎲⰾ",              MV(0,'u'), NO_MV, NOVB};*/
/*struct ani_t run_d_test   = {L"ⲑⲑᎲⰾ",              MV(0,'d'), NO_MV, NOVB};*/
/*struct ani_t run_l_test   = {L"ⲑⲑᎲⰾ",              MV(0,'l'), NO_MV, NOVB};*/
/*struct ani_t run_r_test   = {L"ⲑⲑᎲⰾ",              MV(0,'r'), NO_MV, NOVB};*/
/*struct ani_t punch_r_test = {L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾ",         NO_MV, NO_MV, NOVB};*/
/*struct ani_t punch_l_test = {L"ᎲᎲᎲ᱙ᕦ᱙ᎲᎲⰾ",         NO_MV, NO_MV, NOVB};*/
/*struct ani_t slashtest    = {L"ᎲᎲᎲᒀᒀᒀᒀᎲᎲⰾ",        NO_MV, NO_MV, VB(3, SM_Punch,'u')};*/
/*struct ani_t dodgetest    = {L"ᎲᎲᎲᏡᏡᏡᏡȣȣȣȣᏡᎲᎲⰾ",   MV(8,'d'), NO_MV, NOVB};*/
/*struct ani_t falltest     = {L"ᎲᎲᎲޗޗޗⲁⲁⲁᥑ",        MV(4,'r'), NO_MV, NOVB};*/
/*struct ani_t dodge_l_test = {L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ",     MV(5,'l'), NO_MV, NOVB};*/
/*struct ani_t dodge_r_test = {L"ᎲᎲᎲᥑᥑⲁཚཚཚᎲᎲᎲⰾ",     MV(5,'r'), NO_MV, NOVB};*/
/*struct ani_t kick_r_test  = {L"ᎲᎲᎲ࿂࿂ᱯ࿂࿂༱༱ꝬꝬỿᓀᓀᎲⰾ", MV(9,'r'), NO_MV, NOVB};*/
/*struct ani_t kick_l_test  = {L"ᎲᎲᎲ࿂࿂ᱯᱯ࿂࿂༱ꝬꝬꝬᓂᓂᎲⰾ", MV(9,'l'), NO_MV, NOVB};*/
/*struct ani_t jump_ul      = {L"ᎲᎲᎲⰾⰾᎹᎹᎹᎹᎹᎹᎲᎲⰾ",    MV(7,'u'+'l'), NOVB};*/
/*struct ani_t jump_ur      = {L"ᎲᎲᎲⰾⰾᎹᎹᎹᎹᎹᎹᎲᎲⰾ",    MV(7,'u'+'r'), NOVB};*/
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
        noun->_step(noun, '*');
        top_panel(noun->inv->equipped_pan);
}

/*
 * modify_human -- the state machine for human noun models
 * @self: pointer to a noun of model 'HUMAN'
 */
int modify_human(void *self)
{
        struct noun_t *noun = (struct noun_t *)self;
        int enter = sm_state(noun->sm);

        switch (sm_state(noun->sm)) 
        { 
        case SM_Reset:
                sm_reset(noun->sm);
                break;

        case SM_DigUp:
                noun->animate(dig_u);
                break;
        case SM_DigDown:
                noun->animate(dig_d);
                break;
        case SM_DigLeft:
                noun->animate(dig_l);
                break;
        case SM_DigRight:
                noun->animate(dig_r);
                break;

        case SM_Keyboard:
                switch (sm_mag(noun->sm)) {
                case 'j':
                case 's':
                        noun->animate(run_d_test);
                        break;
                case 'k':
                case 'w':
                        noun->animate(run_u_test);
                        break;
                case 'H':
                case 'A':
                        noun->animate(jump_ul);
                        break;
                case 'h':
                case 'a':
                        noun->animate(run_l_test);
                        break;
                case 'L':
                case 'D':
                        noun->animate(jump_ur);
                        break;
                case 'l':
                case 'd':
                        noun->animate(run_r_test);
                        break;
                case 'g':
                        top_panel(noun->pan);
                        break;
                case 't':
                        noun->animate(punch_r_test);
                        break;
                case ' ':
                        if (CUR_KEY(noun->inv))
                                inv_use(noun->inv, CUR_KEY(noun->inv));
                        break;
                case 'e':
                        noun->animate(slashtest);
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
        if (sm_state(noun->sm) == enter)
                sm_reset(noun->sm);

        return (MODE_PERSIST);
}


/****************************************************************************** 
 * MODEL: Dummy
 * NOTES: Test unit for combat and collision
 ******************************************************************************/
/*----------------------------------------------------------------------------*/
wchar_t *dummy_base = L"Ⰹ";

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

        bonk_test  = mk_ani(L"ⰊⰊⰊⰊⰉ",         NO_MV,     NO_MSG);
        dummy_mv_u = mk_ani(L"ⰹⰹⰺⰺⰹⰹⰹⰉ",      MV(4,'u'), NO_MSG);
        dummy_mv_d = mk_ani(L"ⰹⰹⰺⰺⰹⰹⰹⰉ",      MV(4,'d'), NO_MSG);
        dummy_mv_l = mk_ani(L"ⰹⰹⰺⰺⰹⰹⰹⰉ",      MV(4,'l'), NO_MSG);
        dummy_mv_r = mk_ani(L"ⰹⰹⰺⰺⰹⰹⰹⰉ",      MV(4,'r'), NO_MSG);
        dummy_die  = mk_ani(L"ⰊⰊⰊⰊⰉ✶✶✺✺✺❁❁✴", NO_MV,     MSG(12, SM_Destroy, '*'));

        built = true;
}

/*struct ani_t bonk_test = {L"ⰊⰊⰊⰊⰉ", NO_MV, NOVB};*/
/*struct ani_t dummy_mv_u = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", MV(4,'u'), NOVB};*/
/*struct ani_t dummy_mv_d = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", MV(4,'d'), NOVB};*/
/*struct ani_t dummy_mv_l = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", MV(4,'l'), NOVB};*/
/*struct ani_t dummy_mv_r = {L"ⰹⰹⰺⰺⰹⰹⰹⰉ", MV(4,'r'), NOVB};*/
/*struct ani_t dummy_die  = {L"ⰊⰊⰊⰊⰉ✶✶✺✺✺❁❁✴", NO_MV, VB(12, SM_Destroy, '*')};*/

//Ⰺ✶✺❁✴
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
        noun->_step(noun, '*');

        if (noun->is_doomed)
                noun->_del(noun);
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
                        noun->_seek(noun, get_noun("Guy"));
                break;
        case SM_Punch:
                wprintw(CONSOLE_WIN, "Dummy hit!\n");
                noun->_animate(noun, dummy_die);
                dock_say(L"嶴", "FUCK!");
                break;
        case SM_GoUp:
                noun->_animate(noun, dummy_mv_u);
                break;
        case SM_GoDown:
                if (flip_biased(0.4))
                        dock_say(L"䥚", "I'm gonna hop all over you!");
                noun->_animate(noun, dummy_mv_d);
                break;
        case SM_GoLeft:
                noun->_animate(noun, dummy_mv_l);
                if (flip_biased(0.4))
                        dock_say(L"䥚", "Get fucked!");
                break;
        case SM_GoRight:
                noun->_animate(noun, dummy_mv_r);
                break;
        case SM_Destroy:
                doom(noun);
                oops = false;
                dock_say(L"\n","\n");
                break;
        }
        if (!noun->is_doomed && sm_state(noun->sm) == enter)
                sm_reset(noun->sm);

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
                oops = true;
                break;
        }

        astar_init(noun->astar, ACTIVE->tile, pos_y(noun->pos), pos_x(noun->pos));
        noun->map_id = ACTIVE->id;
}


