/* 
 * fsm.c -- Finite-state machine library 
 * 
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <stdlib.h>
#include "../../com/barnacle.h"
#include "../../eng/tick.h"
#include "../list.h"
#include "../bheap.h"
#include "fsm.h"
#include "../../noun/noun.h"


/* MESSAGE STRUCTURE 
``````````````````````````````````````````````````````````````````````````````*/
/****************************************************************************** 
 * MESSAGE
 * The message is the "packet" or "letter" sent from one state machine to
 * another. It contains identifiers for the sender and recipient, the
 * message itself, a magnitude value associated with the message, and a
 * delay value that will determine how many game ticks must pass before
 * the message is routed to its intended recipient. 
 ******************************************************************************/
struct msg_t {
        struct list_node node; /* Allows msgs to be added to linked list */
        uint32_t from;         /* Sender (noun) identifier */
        uint32_t to;           /* Receiver (noun) identifier */
        uint32_t time;         /* Send at this time */
        int pri;               /* Message priority */
        enum sm_tag tag;       /* Message identifier */
        int mag;               /* Magnitude value (optional) */
        bool sticky;           /* Is message sticky? */
};


/****************************************************************************** 
 * STATE MACHINE 
 * The state machine class is the "mailbox" in the postal analogy. An object
 * containing a state machine may check its current state and the magnitude 
 * of that state, much like you would check for mail in your mailbox. A state 
 * machine can also emit a state and magnitude to another state machine, just
 * as you may place an addressed letter in the mailbox and set the flag so 
 * the postman will know you have mail to send.
 ******************************************************************************/
struct sm_t {
        uint32_t id;           /* Who owns the sm (the "sender") */
        bool pending;          /* Has the current state been consumed? */
        bool lock;
        int screen;            /* Messages with priority < screen ignored */
        enum sm_tag tag;       /* Current state in the sm */
        int mag;               /* Optional magnitude value */
        struct bheap_t *state;
        struct list_head delayed;
        bool (*route)(void *msg);     /* 'route' callback */
};



/* MESSAGE FUNCTIONS 
``````````````````````````````````````````````````````````````````````````````*/
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
struct msg_t *
new_msg(uint32_t from, uint32_t to, enum sm_tag tag, uint32_t time, int pri)
{
        struct msg_t *new = malloc(sizeof(struct msg_t));

        new->from   = from;
        new->to     = to;
        new->tag    = tag;
        new->time   = time;
        new->pri    = pri;
        new->mag    = 0;
        new->sticky = false;

        return (new);
}


/** 
 * MAKE MESSAGE 
 * sm_mkmsg -- create a new message (does not send)
 * @self:  pointer to the state machine object 
 * @to:    noun id of intended recipient
 * @state: msg identifier | delay | priority */
struct msg_t *sm_mkmsg(struct sm_t *sm, uint32_t to, uint32_t state)
{
        struct msg_t *new;
        uint32_t recipient; 
        uint32_t time;

        if (to == SM_SELF)
                recipient = sm->id;
        else
                recipient = to;

        time = get_tick() + SM_WAIT(state);
        new  = new_msg(sm->id, recipient, SM_TAG(state), time, SM_PRI(state));

        if (SM_OPT(state) == STICKY)
                new->sticky = true;

        return (new);
}


/**
 * SET MESSAGE MAGNITUDE
 * sm_mkmag -- set the magnitude of an unsent message
 * @msg: pointer to an unsent message
 * @mag: desired magnitude value */
void sm_mkmag(struct msg_t *msg, int magnitude)
{
        msg->mag = magnitude;
}




/* MESSAGE ACCESSORS 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * MESSAGE TO 
 * msg_to -- returns the identity of the recipient of a message
 * @msg: pointer to a message
 */
uint32_t msg_to(struct msg_t *msg)
{
        return (msg->to);
}




/* STATE MACHINE CONTROL FUNCTIONS 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * new_sm -- creates and returns a new state machine object
 * @id: identity to be given to the state machine 
 * @route: the desired callback implementation of message routing
 */
struct sm_t *new_sm(uint32_t id, bool (*route)(void *msg))
{
        struct sm_t *new = malloc(sizeof(struct sm_t));

        new->id      = id;
        new->pending = false;
        new->lock    = false;
        new->screen  = 0;
        new->tag     = SM_Ready;
        new->mag     = 0;
        new->state   = new_bh(NUM_PENDING);
        new->route   = route;

        list_head_init(&new->delayed); 

        return (new);
}


/**
 * del_sm -- delete a state machine object 
 * @sm: pointer to state machine object */
void del_sm(struct sm_t *sm)
{
        struct msg_t *tmp, *nxt;

        list_for_each_safe(&sm->delayed, tmp, nxt, node) {
                list_del_from(&sm->delayed, &tmp->node);
                free(tmp);
        }
        del_bh(sm->state);
}


/**
 * do_route -- decides whether to send or delay a message
 * @sm : pointer to state machine object
 * @msg: pointer to the msg being routed
 */
bool do_route(struct sm_t *sm, struct msg_t *msg)
{
        assert((sm && msg) || !"Exception in do_route()");

        if (msg->time > get_tick()) {
                list_add(&sm->delayed, &msg->node);
                return true;
        } else {
                return sm->route(msg);
        }
}


/**
 * send_delayed_msgs -- called every tick, decrements msg delays to 0
 * @sm: pointer to state machine object
 */
void send_delayed_msgs(struct sm_t *sm)
{
        struct msg_t *tmp, *nxt;
        uint32_t current_time;

        if (list_empty(&sm->delayed))
                return;

        current_time = get_tick();

        list_for_each_safe(&sm->delayed, tmp, nxt, node) {
                if (tmp->time == current_time) {
                        /* We must delete from the ring BEFORE routing,
                         * because the message may be free'd en route,
                         * and then we'd have a big hole in the list 
                         * we couldn't do anything about. */
                        list_del_from(&sm->delayed, &tmp->node); 
                        do_route(sm, tmp);
                }
        }
}


/* MESSAGE CREATION 
``````````````````````````````````````````````````````````````````````````````*/
/** 
 * SEND MESSAGE 
 * sm_msg -- send a new message (draft + pass to the emitter)
 * @self:  pointer to the state machine object 
 * @to:    noun id of intended recipient
 * @msg:   msg identifier | delay | priority */
void sm_msg(struct sm_t *sm, uint32_t to, uint32_t state)
{
        struct msg_t *msg;
        msg = sm_mkmsg(sm, to, state);
        if (do_route(sm, msg) == SM_NO_ROUTE)
                free(msg);
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
        msg      = sm_mkmsg(sm, to, state);
        msg->mag = magnitude;
        if (do_route(sm, msg) == SM_NO_ROUTE)
                free(msg);
}



/* STATE TRANSITION 
``````````````````````````````````````````````````````````````````````````````*/
/** 
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
 * sm_accept -- receive a message and add it to the message heap
 * @sm: pointer to a state machine
 * @msg: the message structure
 * @ret: returns true if accepted, false if rejected
 */
bool sm_accept(struct sm_t *sm, struct msg_t *msg)
{
        if (msg->pri < sm->screen) {
                return false;
        } else {
                bh_add(sm->state, (float)msg->pri, msg->from, (void *)msg);
                return true;
        }
}


/** 
 * sm_consume -- set the state and magnitude to the pending state at hipri
 * @sm: pointer to a state machine
 */
void sm_consume(struct sm_t *sm)
{
        send_delayed_msgs(sm);

        struct msg_t *msg;
        msg = (struct msg_t *)bh_pop(sm->state);

        if (msg) {
                sm_set(sm, msg->tag, msg->mag);
                if (msg->sticky) {
                        sm_lock(sm, true);
                }
                /*free(msg);*/
        }
}


/** 
 * sm_reset -- set the state and state magnitude of a state machine to 0
 * @sm: pointer to a state machine
 */
void sm_reset(struct sm_t *sm)
{
        sm_set(sm, 0, 0);
}


/**
 * sm_refresh -- if no new state is pending, reset the state machine
 * @sm: pointer to a state machine
 */
void sm_refresh(struct sm_t *sm)
{
        if (!sm_is_locked(sm) && !sm_is_pending(sm))
                sm_reset(sm);
}



/* STATE MACHINE QUERIES AND ACCESSORS
``````````````````````````````````````````````````````````````````````````````*/
/**
 * sm_state -- return the current state msg of a state machine 
 * @sm: pointer to a state machine */
int sm_state(struct sm_t *sm)
{
        sm->pending = false; /* The state has now been observed */

        return (int)(sm->tag);
}

/**
 * sm_mag -- return the magnitude of the current state msg of a state machine
 * @sm: pointer to a state machine */
int sm_mag(struct sm_t *sm)
{
        return sm->mag;
}

/** 
 * sm_is_pending -- check if there is a new state pending
 * @sm: pointer to a state machine */
bool sm_is_pending(struct sm_t *sm)
{
        return (sm->pending);
}

/** 
 * sm_screen -- set the minimum priority to 'screen'
 * @sm: pointer to a state machine
 * @screen: minimum priority of state transition */
void sm_screen(struct sm_t *sm, int screen)
{
        sm->screen = screen;
}

/** 
 * sm_is_locked -- return the lock status of the state machine
 * @sm: pointer to a state machine */
bool sm_is_locked(struct sm_t *sm)
{
        return sm->lock;
}

/** 
 * sm_lock -- set the lock status of the state machine
 * @sm: pointer to a state machine
 * @yn: desired status of lock */
void sm_lock(struct sm_t *sm, bool yn)
{
        sm->lock = yn;
}

