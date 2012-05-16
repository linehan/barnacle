#include <stdlib.h>
#include "../../com/arawak.h"
#include "../list.h"
#include "fsm.h"


/* MESSAGE STORAGE, ROUTING, AND RETREIVAL
``````````````````````````````````````````````````````````````````````````````*/
/* DELAYED LIST
 * Delayed msgs are stored in a linked list until their delay has expired, 
 * at which time they are routed to their destination.
 */
LIST_HEAD(delayed_msgs);


/**
 * ADD TO DELAYED LIST 
 * add_delayed -- add a msg to the data structure containing delayed msgs
 * @msg: pointer to the msg being routed
 */
inline void add_delayed(struct msg_t *msg)
{
        list_add(&delayed_msgs, &msg->node);
}

/**
 * ROUTE
 * route_msg -- decides when, where, and whether to send a msg
 * @msg: pointer to the msg being routed
 */
void route_msg(struct msg_t *msg)
{
        if (msg->delay > 0)
                add_delayed(msg);
        else {
                msg->route(msg);
                free(msg);
        }
}

/**
 * UPDATE
 * send_delayed_msgs -- called every tick, decrements msg delays to 0
 */
void send_delayed_msgs(void)
{
        if (list_empty(&delayed_msgs))
                return;

        struct msg_t *tmp;
        struct msg_t *nxt;

        list_for_each_safe(&delayed_msgs, tmp, nxt, node) {
                if (tmp->delay > 0) {
                        tmp->delay--;
                } else {
                        list_del_from(&delayed_msgs, &tmp->node); 
                        route_msg(tmp);
                }
        }
}

/**
 * EMIT 
 * emit_msg -- creates a new msg_t and hands it to route_msg 
 * @from:  noun id of sender 
 * @to:    noun id of intended recipient
 * @msg:   msg identifier
 * @mag:   magnitude of the msg (optional)
 * @delay: ticks of delay
 * @route: custom function pointer for routing msg to target 
 */
void emit_msg(uint32_t from, uint32_t to, enum sm_state tag, int mag, int delay, SM_CB_ROUTE route)
{
        struct msg_t *new = malloc(sizeof(struct msg_t));

        new->tag   = tag;
        new->mag   = mag;
        new->to    = to;
        new->from  = from;
        new->delay = delay;
        new->route = route;

        route_msg(new);
}



/* STATE MACHINE METHODS 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * DESTROY METHOD
 * method_sm_destroy -- simple destructor
 * @self: pointer to state machine object */
void method_sm_destroy(void *self)
{
        free((struct sm_t *)self);
}

/**
 * EMIT METHOD
 * method_sm_emit -- draft a new message and pass it to the emitter
 * @self:  pointer to the state machine object 
 * @to:    noun id of intended recipient
 * @msg:   msg identifier
 * @mag:   magnitude of the msg (optional)
 * @delay: ticks of delay */ 
void method_sm_emit(void *self, uint32_t to, enum sm_state tag, int mag, int delay)
{
        struct sm_t *sm = (struct sm_t *)self;
        emit_msg(sm->id, to, tag, mag, delay, sm->route);
}

/**
 * CONSTRUCTOR
 * new_sm -- creates and returns a new state machine object
 * @id: identity to be given to the state machine 
 * @route: the desired callback implementation of message routing
 */
struct sm_t *new_sm(uint32_t id, bool (*route)(void *self))
{
        struct sm_t *new = calloc(1, sizeof(struct sm_t));

        new->id     = id;
        new->route  = route;
        new->emit   = &method_sm_emit;
        new->del    = &method_sm_destroy;
        new->accept = true;

        return (new);
}

