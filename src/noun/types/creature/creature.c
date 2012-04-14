#include "../../../com/arawak.h"

#include "creature.h"
#include "../../noun_model.h"
#include "../../../eng/fsm.h"


void new_creature(const char *name, int job, const wchar_t *wch, short pair)
{
        struct person_t *new;
        struct noun_t *noun;

        new = malloc(sizeof(struct person_t)); 

        noun = new_noun(name, job, 0, new);
        noun_set_modify(noun, modify_creature);
        mob_cfg(&noun->mob, GLOBE, 1, 1, CENTERED);

        noun->options |= NOUN_CREATURE;

        /*init_pair(pair, BLACK, WHITE);*/
        setcchar(&new->cch, wch, 0, pair, NULL);
        wbkgrnd(mob_win(&noun->mob), &new->cch);

        /*wbkgrnd(mob_win(&noun->mob), mkcch(wch, 0, pair));*/

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
        case KEY_ESC:
                return (MODE_RELEASE);
        }
        return (MODE_PERSIST);
}



