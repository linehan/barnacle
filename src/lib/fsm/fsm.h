#pragma once
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
#include "../list.h"


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
typedef void (*SM_METHOD_EMIT)(void *self, uint32_t to, enum sm_state tag, int mag, int delay);
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
        enum sm_state tag;     /* Current state in the sm */
        int mag;               /* Magnitude of the current state */
        SM_CB_ROUTE route;     /* 'route' callback */
        SM_METHOD_EMIT emit;   /* 'emit' method */
        SM_METHOD_DEL del;     /* 'del' method */
};




/* PUBLIC FUNCTIONS
``````````````````````````````````````````````````````````````````````````````*/
struct sm_t *new_sm(uint32_t id, SM_CB_ROUTE);




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
                return true;
        }
        return false;
}

/* ACCEPT 
 * sm_accept -- set the 'accept' boolean value of a state machine
 * @sm: pointer to a state machine
 * @yn: true or false */
static inline void sm_active(struct sm_t *sm, bool yn)
{
        sm->accept = yn;
}

/* STATE 
 * sm_state -- return the current state msg of a state machine 
 * @sm: pointer to a state machine */
static inline int sm_state(struct sm_t *sm)
{
        return (int)(sm->tag);
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


#endif
