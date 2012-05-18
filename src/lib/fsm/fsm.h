#pragma once
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
#include "../list.h"
#include "../bheap.h"


/* MESSAGE TYPES 
````````````````````````````````````````````````````````````````````````````` */
/*
 * RESERVED MESSAGES
 * These are used by the implementation's state machine to identify entry,
 * exit, and update states, which may have special behavior.
 */
static const int SM_RESERVED_Enter  = 9999;
static const int SM_RESERVED_Exit   = 9998;
static const int SM_RESERVED_Update = 9997;

/*
 * MESSAGES (STATES)
 * These are defined in the implementation header states.h
 */
#include "states.h"

#define NUM_PENDING 20



/* CONTAINER TYPES 
````````````````````````````````````````````````````````````````````````````` */
/* 
 * SM_CB_ROUTE
 * A callback assigned to the state machine by the constructor. It will be 
 * passed to the message constructor and will eventually be called by the 
 * message router. 
 *
 * SM_METHOD_EMIT 
 * Used to create and send new messages.
 *
 * SM_METHOD_DEL 
 * State machine destructor method.
 */
typedef bool (*SM_CB_ROUTE)(void *self);
typedef void (*SM_METHOD_EMIT)(void *self, uint32_t to, enum sm_state tag, int mag, int delay, int pri);
typedef void (*SM_METHOD_DEL)(void *self);


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
        enum sm_state tag;     /* Message identifier */
        int mag;               /* A magnitude */
        int pri;
        int delay;             /* Delay between transmission and delivery */
        SM_CB_ROUTE route;
};


/* 
 * STATE MACHINE 
 * The state machine class is the "mailbox" in the postal analogy. An object
 * containing a state machine may check its current state and the magnitude 
 * of that state, much like you would check for mail in your mailbox. A state 
 * machine can also emit a state and magnitude to another state machine, just
 * as you may place an addressed letter in the mailbox and set the flag so 
 * the postman will know you have mail to send.
 */
struct sm_t {
        uint32_t id;           /* Who owns the sm (the "sender") */
        bool accept;           /* Whether the sm is accepting messages */
        bool pending;          /* Has the current state been consumed? */
        enum sm_state tag;     /* Current state in the sm */
        int mag;               /* Magnitude of the current state */
        struct bh_t *state;
        SM_CB_ROUTE route;     /* 'route' callback */
        SM_METHOD_EMIT emit;   /* 'emit' method */
        SM_METHOD_DEL del;     /* 'del' method */
};




/* PUBLIC FUNCTIONS
``````````````````````````````````````````````````````````````````````````````*/
struct sm_t *new_sm(uint32_t id, SM_CB_ROUTE);
void send_delayed_msgs(void);




/* ACCESSORS
``````````````````````````````````````````````````````````````````````````````*/
/* SET
 * sm_set -- set the state and state magnitude of a state machine
 * @sm: pointer to a state machine
 * @msg: the state msg
 * @mag: the state magnitude */
static inline bool sm_set(struct sm_t *sm, enum sm_state tag, int mag)
{
        if (sm->accept) {
                sm->tag = tag;
                sm->mag = mag;
                sm->pending = true; /* A new state is pending */
                return true;
        }
        return false;
}

/* ACCEPT
 * sm_accept -- receive a message and add it to the message heap
 * @sm: pointer to a state machine
 * @msg: the message structure
 */
static inline void sm_accept(struct sm_t *sm, struct msg_t *msg)
{
        bh_add(sm->state, msg->pri, msg->from, msg);
}

/* CONSUME
 * sm_consume -- set the state and magnitude to the pending state at hipri
 * @sm: pointer to a state machine
 */
static inline void sm_consume(struct sm_t *sm)
{
        struct msg_t *msg;

        msg = (struct msg_t *)bh_pop(sm->state);

        if (msg) {
                sm_set(sm, msg->tag, msg->mag);
                free(msg);
        }
}

/* SET SELF AFTER
 * sm_set_after -- set the state and state magnitude of a state machine at time
 * @sm: pointer to a state machine
 * @msg: the state msg
 * @mag: the state magnitude 
 * @delay: the number of ticks to wait */
static inline void sm_set_self_after(struct sm_t *sm, enum sm_state tag, int mag, int delay, int pri)
{
        sm->emit(sm, sm->id, tag, mag, delay, pri);
}


/* SET ACTIVE 
 * sm_accept -- set the 'accept' boolean value of a state machine
 * @sm: pointer to a state machine
 * @yn: true or false */
static inline void sm_active(struct sm_t *sm, bool yn)
{
        sm->accept = yn;
}

/* PEEK
 * sm_peek -- return the current state without consuming the pending flag
 * @sm: pointer to a state machine */
static inline int sm_peek(struct sm_t *sm)
{
        return (int)(sm->tag);
}

/* STATE 
 * sm_state -- return the current state msg of a state machine 
 * @sm: pointer to a state machine */
static inline int sm_state(struct sm_t *sm)
{
        sm->pending = false; /* The state has now been observed */
        return (int)(sm->tag);
}

/* PENDING
 * sm_pending -- check if there is a new state pending
 * @sm: pointer to a state machine */
static inline bool sm_pending(struct sm_t *sm)
{
        return (sm->pending);
}

/* MAGNITUDE
 * sm_mag -- return the current magnitude of a state machine 
 * @sm: pointer to a state machine */
static inline int sm_mag(struct sm_t *sm)
{
        return (sm->mag);
}

/* RESET
 * sm_reset -- set the state and state magnitude of a state machine to 0
 * @sm: pointer to a state machine */
static inline void sm_reset(struct sm_t *sm)
{
        sm_set(sm, 0, 0);
}

/* REFRESH 
 * sm_refresh -- if no new state is pending, reset the state machine
 * @sm: pointer to a state machine */
static inline void sm_refresh(struct sm_t *sm)
{
        if (!sm_pending(sm))
                sm_reset(sm);
}



#endif
