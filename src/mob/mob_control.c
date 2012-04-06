#include <stdlib.h>
#include "../noun/noun_model.h"
#include "../eng/fsm.h"
#include "mob_model.h"


/*
 * Request a mob by name. Uses get_noun() to look up the relevant noun
 * and then returns the mob member of the struct noun_t.
 */
struct mob_t *get_mob(const char *name)
{
        struct noun_t *noun = get_noun(name);
        return (&(noun->mob));
}



int mob_control(struct mob_t *mob, int order, int val)
{
        switch(order) 
        { 
        case 'j':
                move_mob(mob, 'd');
                break;
        case 'k':
                move_mob(mob, 'u');
                break;
        case 'h':
                move_mob(mob, 'l');
                break;
        case 'l':
                move_mob(mob, 'r');
                break;
        case 'm':
                return (MODE_RELEASE);
        }
        return (MODE_PERSIST);
}
