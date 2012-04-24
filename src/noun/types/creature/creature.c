#include "../../../com/arawak.h"

#include "creature.h"
#include "../../noun_model.h"
#include "../../../eng/fsm.h"



// med left ᱙
// med left Ꮆ
// strong left ᕦ

// strong right ᕤ
// med right ᱽ

struct animation punch_r_test = { {L"Ꮂ", L"ᱽ", L"Ꮂ", L"ⰾ", 0}, 5 };
struct animation punch_l_test = { {L"Ꮂ", L"Ꮆ", L"Ꮂ", L"ⰾ", 0}, 5 };

struct animation dodgetest = { {L"Ꮱ", L"Ꮱ", L"ȣ", L"Ꮱ", L"Ꮂ", L"ⰾ", 0}, 7, 3, 'd'};
struct animation falltest = { {L"ᥑ", L"ᥑ", L"ޗ", L"ⲁ", L"ᥑ", 0}, 7, 3, 'r'};
/*struct animation falltest = { {L"ᥑ", L"ᥑ", L"ⲁ", L"ⴋ", L"ᥑ", 0}, 7, 3, 'l'};*/


/*struct animation run_u1_test = { {L"ⲑ",L"ⲑ", L"ⰾ", 0}, 5, 2, 'u'};*/
/*struct animation run_u2_test = { {L"ϯ", L"ϯ", L"ⰾ", 0}, 5, 2, 'u'};*/
/*ϯⲑⰪⱀⱚ*/
/*L"ϯ", L"ϯ"*/
/*ⰾⰄⰴⰾ*/
/*ⰾᎲⰴⰾ*/
/*struct animation u_runtest = { {L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 4, 1, 'u'};*/
/*struct animation d_runtest = { {L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 4, 1, 'd'};*/
/*struct animation l_runtest = { {L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 4, 1, 'l'};*/
/*struct animation r_runtest = { {L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 4, 1, 'r'};*/
/*ⰾᎲᱽᎲⰾ*/
/*ⰾᏡȣᎲⰾ*/



void new_creature(const char *name, int job, const wchar_t *wch, short pair)
{
        struct person_t *new;
        struct noun_t *noun;

        new = malloc(sizeof(struct person_t)); 

        noun = new_noun(name, job, 0, new);
        noun_set_modify(noun, modify_creature);
        mob_cfg(&noun->mob, FIELD, 1, 1, CENTERED);

        noun->options |= NOUN_CREATURE;

        /*init_pair(pair, BLACK, WHITE);*/
        setcchar(&new->cch, wch, 0, pair, NULL);
        wbkgrnd(mob_win(&noun->mob), &new->cch);

        noun->mob.animate = NULL;
        noun->mob.punch_l = &punch_l_test;
        noun->mob.punch_r = &punch_r_test;
        noun->mob.dodge   = &dodgetest;
        noun->mob.fall = &falltest;
}



static bool whichrun;

int modify_creature(void *obj, int input)
{
        struct noun_t *noun = (struct noun_t *)obj;

        switch(input) 
        { 
        case 'j':
        case 's':
                mob_move(&noun->mob, 'd');
                break;
        case 'k':
        case 'w':
                mob_move(&noun->mob, 'u');
                break;
        case 'h':
        case 'a':
                mob_move(&noun->mob, 'l');
                break;
        case 'l':
        case 'd':
                mob_move(&noun->mob, 'r');
                break;
        /*case 't':*/
                /*mob_seek(noun_mob("Beefman"), &noun->mob);*/
                /*break;*/
        case 'g':
                top_panel(noun->mob.pan);
                break;
        case 'r':
                noun->mob.animate = noun->mob.punch_l;
                break;
        case 't':
                noun->mob.animate = noun->mob.punch_r;
                break;
        case ' ':
                noun->mob.animate = noun->mob.dodge;
                break;
        case '|':
                noun->mob.animate = noun->mob.fall;
                break;
        case KEY_ESC:
                return (MODE_RELEASE);
        }
        return (MODE_PERSIST);
}



