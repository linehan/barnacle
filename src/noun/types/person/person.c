#include "../../../com/arawak.h"

#include "person.h"
#include "../../noun_model.h"
#include "../../../eng/fsm.h"


void new_person(const char *name, int job)
{
        struct persno_t *new;
        struct noun_t *noun;

        new = malloc(sizeof(struct person_t)); 

        noun = new_noun(name, PERSON, 0, new);
        /*noun_set_render(get_noun(name), person_view);*/
        noun_set_modify(noun, modify_person);
        mob_cfg(&noun->mob, GLOBE, 1, 1, CENTERED);
        wbkgrnd(mob_win(&noun->mob), mkcch(L"д", 0, PUR_WHITE));
}


int modify_person(void *obj, int input)
{
        struct noun_t *noun = (struct noun_t *)obj;

        switch(input) 
        { 
        case 'j':
                mob_move(&noun->mob, 'd');
                break;
        case 'k':
                mob_move(&noun->mob, 'u');
                break;
        case 'h':
                mob_move(&noun->mob, 'l');
                break;
        case 'l':
                mob_move(&noun->mob, 'r');
                break;
        case 'm':
                return (MODE_RELEASE);
        }
        return (MODE_PERSIST);
}



