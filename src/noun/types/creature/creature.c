#include "../../../com/arawak.h"

#include "creature.h"
#include "../../noun_model.h"
#include "../../../eng/fsm.h"




/*struct animation slashtest = { {L"Ꮂ", L"Ꮝ", L"ⰾ", 0, 0}, 4, 0 };*/
struct animation slashtest = { {L"Ꮂ", L"ᱽ", L"Ꮂ", L"ⰾ", 0}, 5 };
struct animation dodgetest = { {L"Ꮱ", L"Ꮱ", L"ȣ", L"Ꮱ", L"Ꮂ", L"ⰾ", 0}, 7, 3, 'd'};
/*struct animation u_runtest = { {L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 4, 1, 'u'};*/
/*struct animation d_runtest = { {L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 4, 1, 'd'};*/
/*struct animation l_runtest = { {L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 4, 1, 'l'};*/
/*struct animation r_runtest = { {L"Ꮂ", L"Ꮂ", L"ⰾ", 0}, 4, 1, 'r'};*/
/*ⰾᎲᱽᎲⰾ*/
/*ⰾᏡȣᎲⰾ*/

/*struct multiwin_t *new_slash(int h, int w, int y, int x)*/
/*{*/
        /*struct multiwin_t *new = new_multiwin(h, w, y, x, 3);*/

        /*wadd_wch(PEEK(new), mkcch(L"Ꮂ", 0, FLEX));*/
        /*NEXT(new);*/
        /*wadd_wch(PEEK(new), mkcch(L"Ꮝ", 0, FLEX));*/
        /*new->win[2] = NULL;*/
        /*NEXT(new);*/
        /*NEXT(new);*/

        /*return (new);*/
/*}*/


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
        noun->mob.slash = &slashtest;
        noun->mob.dodge = &dodgetest;
}





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
        case 't':
                mob_seek(noun_mob("Beefman"), &noun->mob);
                break;
        case 'g':
                top_panel(noun->mob.pan);
                break;
        case 'r':
                noun->mob.animate = noun->mob.slash;
                break;
        case ' ':
                noun->mob.animate = noun->mob.dodge;
                break;
        case KEY_ESC:
                return (MODE_RELEASE);
        }
        return (MODE_PERSIST);
}



