#include <stdlib.h>
#include "../../com/arawak.h"
#include "../list.h"
#include "../bheap.h"
#include "fsm.h"


/* MESSAGE STORAGE, ROUTING, AND RETREIVAL
``````````````````````````````````````````````````````````````````````````````*/
/* DELAYED LIST
 * Delayed msgs are stored in a linked list until their delay has expired, 
 * at which time they are routed to their destination.
 */
LIST_HEAD(delayed_msgs);


/**
 * add_delayed -- add a msg to the data structure containing delayed msgs
 * @msg: pointer to the msg being routed
 */
inline void add_delayed(struct msg_t *msg)
{
        list_add(&delayed_msgs, &msg->node);
}



/**
 * NEW MESSAGE 
 * new_msg -- creates a new msg_t and hands it to route_msg 
 * @from:  noun id of sender 
 * @to:    noun id of intended recipient
 * @msg:   msg identifier
 * @mag:   magnitude of the msg (optional)
 * @delay: ticks of delay
 * @route: custom function pointer for routing msg to target 
 */
inline struct msg_t *
new_msg(uint32_t from, uint32_t to, enum sm_state tag, int mag, uint32_t time, int pri, SM_CB_ROUTE route)
{
        struct msg_t *new = malloc(sizeof(struct msg_t));

        new->tag   = tag;
        new->mag   = mag;
        new->to    = to;
        new->from  = from;
        new->time  = time;
        new->pri   = pri;
        new->route = route;


        return (new);
}


/**
 * ROUTE
 * do_route -- decides when, where, and whether to send a msg
 * @msg: pointer to the msg being routed
 */
void do_route(struct msg_t *msg)
{
        if (msg->time != get_tick()) 
                add_delayed(msg);
        else 
                msg->route(msg);
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
        uint32_t current_time;

        current_time = get_tick();

        list_for_each_safe(&delayed_msgs, tmp, nxt, node) {
                if (tmp->time == current_time) {
                        list_del(&tmp->node); 
                        do_route(tmp);
                }
        }
}





/* STATE MACHINE METHODS 
``````````````````````````````````````````````````````````````````````````````*/
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
        new->state  = new_bh(NUM_PENDING);

        new->route  = route;
        new->accept = true;

        return (new);
}

/**
 * DESTRUCTOR 
 * del_sm -- simple destructor
 * @self: pointer to state machine object */
void del_sm(struct sm_t *sm)
{
        del_bh(sm->state);
        free(sm);
}


/**
 * EMIT 
 * sm_emit -- draft a new message and pass it to the emitter
 * @self:  pointer to the state machine object 
 * @to:    noun id of intended recipient
 * @msg:   msg identifier
 * @mag:   magnitude of the msg (optional)
 * @delay: ticks of delay */ 
void sm_emit(struct sm_t *sm, uint32_t to, enum sm_state tag, int mag, int delay, int pri)
{
        uint32_t recipient; 
        uint32_t time;
        struct msg_t *msg;

        if (to == SM_SELF)
                recipient = sm->id;
        else    recipient = to;

        time = get_tick() + delay;

        msg = new_msg(sm->id, recipient, tag, mag, time, pri, sm->route);

        do_route(msg);
}


