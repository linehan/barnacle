#include <stdlib.h>
#include "../com/arawak.h"
#include "verb.h"
#include "../noun/noun.h"


struct list_head delayed_verbs; /* Holds any delayed verbs */

inline void add_delayed(struct verb_t *verb);
void send_delayed_verbs(void);



/*
 * route_verb -- decides when, where, and whether to send a verb
 * @verb: pointer to the verb being routed
 */
void route_verb(struct verb_t *verb)
{
        if (verb->delay > 0) {
                add_delayed(verb);
        } else {
                verb->send(verb);
                free(verb);
        }
}


/*
 * send_verb -- creates a new verb_t and hands it to route_verb 
 * @name:  verb identifier
 * @to:    noun id of intended recipient
 * @from:  noun id of sender 
 * @delay: ticks of delay
 * @data:  any other data required
 */
void send_verb(int name, uint32_t to, uint32_t from, int delay, bool (*send)(void *self))
{
        struct verb_t *new = malloc(sizeof(struct verb_t));

        new->name       = name;
        new->to         = to;
        new->from       = from;
        new->delay      = delay;
        new->send       = send;

        route_verb(new);
}


/*
 * add_delayed -- add a verb to the data structure containing delayed verbs
 * @verb: pointer to the verb being routed
 */
inline void add_delayed(struct verb_t *verb)
{
        list_add(&delayed_verbs, &verb->node);
}


/*
 * send_delayed_verbs -- called every tick, decrements verb delays to 0
 */
void send_delayed_verbs(void)
{
        struct verb_t *tmp;
        struct verb_t *nxt;

        list_for_each_safe(&delayed_verbs, tmp, nxt, node) {
                if (tmp->delay != 0) {
                        tmp->delay--;
                } else {
                        route_verb(tmp);
                        list_del(&tmp->node); 
                }
        }
}


