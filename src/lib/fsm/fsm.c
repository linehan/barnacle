#include <stdlib.h>
#include "../../com/arawak.h"
#include "../../eng/tick.h"
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


/* 
 * MESSAGE
 * The message is the "packet" or "letter" sent from one state machine to
 * another. It contains identifiers for the sender and recipient, the
 * message itself, a magnitude value associated with the message, and a
 * delay value that will determine how many game ticks must pass before
 * the message is routed to its intended recipient. 
 *
 * The message type also contains a 'route' method, which is assigned to
 * the message by the state machine when the message is created. It is
 * implementation defined, and must contain logic to route the message
 * from the state machine belonging to the 'from' identity to the state
 * machine belonging to the 'to' identity.
 */
struct msg_t {
        struct list_node node; /* Allows msgs to be added to linked list */
        uint32_t from;         /* Sender (noun) identifier */
        uint32_t to;           /* Receiver (noun) identifier */
        uint32_t time;         /* Send at this time */
        int pri;               /* Message priority */
        enum sm_tag tag;       /* Message identifier */
        int mag;               /* Magnitude value (optional) */
        bool sticky;           /* Is message sticky? */
        SM_CB_ROUTE route;
};



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
new_msg(uint32_t from, uint32_t to, enum sm_tag tag, uint32_t time, int pri, SM_CB_ROUTE route)
{
        struct msg_t *new = malloc(sizeof(struct msg_t));

        new->tag    = tag;
        new->to     = to;
        new->from   = from;
        new->time   = time;
        new->pri    = pri;
        new->route  = route;
        new->sticky = false;
        new->mag    = 0;

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

/**
 * TO 
 * msg_to -- returns the identity of the recipient of a message
 * @msg: pointer to a message
 *
 * Note: The reason this function exists is that the route() callback is
 * implementation-defined and thus needs to access the 'to' member of a
 * message struct, in order to route the message. However, we want to keep
 * the message type hidden in this source file. This function provides
 * the minimum amount of data sharing necessary to implement the callback.
 */
uint32_t msg_to(struct msg_t *msg)
{
        return (msg->to);
}

/**
 * MSG SET STICKY
 * msg_set_sticky -- sets as "sticky" i.e. state will persist until new state
 * @msg: pointer to a message */
void msg_set_sticky(struct msg_t *msg)
{
        msg->sticky = true;
}

/** 
 * MAKE MESSAGE 
 * sm_mkmsg -- create a new message (does not send)
 * @self:  pointer to the state machine object 
 * @to:    noun id of intended recipient
 * @state: msg identifier | delay | priority */
void sm_mkmsg(struct msg_t **msg, struct sm_t *sm, uint32_t to, uint32_t state)
{
        uint32_t recipient; 
        uint32_t time;

        if (to == SM_SELF)
                recipient = sm->id;
        else    recipient = to;

        time = get_tick() + SM_WAIT(state);

        *msg = new_msg(sm->id, recipient, SM_TAG(state), time, SM_PRI(state), sm->route);

        if (SM_OPT(state) == STICKY)
                msg_set_sticky(*msg);
}

/**
 * MESSAGE MAGNITUDE
 * sm_mkmag -- set the magnitude of an unsent message
 * @msg: pointer to an unsent message
 * @mag: desired magnitude value */
void sm_mkmag(struct msg_t *msg, int magnitude)
{
        msg->mag = magnitude;
}


/* FSM INSTANTIATION 
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
        new->mag    = 0;
        new->route  = route;

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



/* FSM CONTROL 
``````````````````````````````````````````````````````````````````````````````*/
/** 
 * SET
 * sm_set -- set the state and state magnitude of a state machine
 * @sm: pointer to a state machine
 * @msg: the state msg
 * @mag: the state magnitude */
bool sm_set(struct sm_t *sm, enum sm_tag tag, int magnitude)
{
        sm->tag     = tag;
        sm->mag     = magnitude;
        sm->pending = true;  /* A new state is pending */
        sm->lock    = false; /* A new state breaks any "sticky" lock */

        return true;
}

/**
 * STATE 
 * sm_state -- return the current state msg of a state machine 
 * @sm: pointer to a state machine */
int sm_state(struct sm_t *sm)
{
        sm->pending = false; /* The state has now been observed */

        return (int)(sm->tag);
}

/**
 * MAGNITUDE
 * sm_mag -- return the magnitude of the current state msg of a state machine
 * @sm: pointer to a state machine */
int sm_mag(struct sm_t *sm)
{
        return sm->mag;
}

/** 
 * SEND MESSAGE 
 * sm_msg -- send a new message (draft + pass to the emitter)
 * @self:  pointer to the state machine object 
 * @to:    noun id of intended recipient
 * @msg:   msg identifier | delay | priority */
void sm_msg(struct sm_t *sm, uint32_t to, uint32_t state)
{
        struct msg_t *msg;
        sm_mkmsg(&msg, sm, to, state);
        do_route(msg);
}

/** 
 * MSG WITH MAGNITUDE
 * sm_msg -- send a new message (draft + pass to the emitter)
 * @self:  pointer to the state machine object 
 * @to:    noun id of intended recipient
 * @msg:   msg identifier | delay | priority 
 * @mag:   magnitude to be attached to the message */
void sm_msgmag(struct sm_t *sm, uint32_t to, uint32_t state, int magnitude)
{
        struct msg_t *msg;
        sm_mkmsg(&msg, sm, to, state);
        sm_mkmag(msg, magnitude);
        do_route(msg);
}



/* FSM TRANSITIONING 
``````````````````````````````````````````````````````````````````````````````*/
/* ACCEPT
 * sm_accept -- receive a message and add it to the message heap
 * @sm: pointer to a state machine
 * @msg: the message structure
 */
void sm_accept(struct sm_t *sm, struct msg_t *msg)
{
        if (msg->pri < sm->screen)
                free(msg);
        else
                bh_add(sm->state, msg->pri, msg->from, msg);
}

/* CONSUME
 * sm_consume -- set the state and magnitude to the pending state at hipri
 * @sm: pointer to a state machine
 */
void sm_consume(struct sm_t *sm)
{
        struct msg_t *msg;

        msg = (struct msg_t *)bh_pop(sm->state);

        if (msg) {
                sm_set(sm, msg->tag, msg->mag);
                if (msg->sticky)
                        sm_lock(sm, true);
                free(msg);
        }
}

/* RESET
 * sm_reset -- set the state and state magnitude of a state machine to 0
 * @sm: pointer to a state machine */
void sm_reset(struct sm_t *sm)
{
        sm_set(sm, 0, 0);
}

/* REFRESH 
 * sm_refresh -- if no new state is pending, reset the state machine
 * @sm: pointer to a state machine */
void sm_refresh(struct sm_t *sm)
{
        if (!sm_is_locked(sm) && !sm_is_pending(sm))
                sm_reset(sm);
}

