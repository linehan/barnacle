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



/* STATE OPTION MACROS
``````````````````````````````````````````````````````````````````````````````*/
/******************************************************************************
 * In order to provide a sense of "optional" arguments, namely parameters
 * specifying message priority or delivery delay, the following macros
 * may be combined using the bitwise OR operator '|' when passing a state.
 * e.g.
 *      sm_msg(my->sm, recipient_id, SM_SomeState | SM_Wait(12) | SM_Pri(5));
 *
 * Because the bytes not specified will default to 0, you can omit any of
 * these options (except the state itself of course) with no consequences.
 * 
 * SM_FOO is used internally to separate the option parameters
 * SM_Foo is used by the caller to include the option parameters
 ******************************************************************************/

/* Tag is a required param and is right-aligned, so no wrapper is required */
#define SM_TAG_OFFSET      0
#define SM_TAG_MASK        0x0000FFFF
#define SM_TAG(a)          ((SM_TAG_MASK & (a)) >> SM_TAG_OFFSET)

/*
 * SM_Key(k) is a special tag that accepts a parameter 'k' equal
 * to a 16-bit input value such as those received from a keyboard.
 */
#define SM_KEYBOARD        0x0000FFFF
#define SM_KEYPRESS(a)     (SM_KEYBOARD | (a))
#define SM_Key(a)          (SM_KEYBOARD & (a))

/* Specifies a delay in game ticks before a message is routed */
#define SM_WAIT_OFFSET     16 
#define SM_WAIT_MASK       0x00FF0000
#define SM_WAIT(a)         ((SM_WAIT_MASK & (a)) >> SM_WAIT_OFFSET)
#define SM_Wait(a)         (((a) << SM_WAIT_OFFSET))

/* Specifies a priority value used by the state machine's priority queue */
#define SM_PRIORITY_OFFSET 24 
#define SM_PRIORITY_MASK   0x0F000000
#define SM_PRI(a)          ((SM_PRIORITY_MASK & (a)) >> SM_PRIORITY_OFFSET)
#define SM_Pri(a)          (((a) << SM_PRIORITY_OFFSET))

/* Miscellaneous option flags are set in the leftmost byte */
#define SM_OPTIONS_OFFSET  28 
#define SM_OPTIONS_MASK    0xF0000000
#define STICKY             0x10000000 
#define SM_OPT(a)          ((SM_OPTIONS_MASK & (a)))
#define SM_Opt(a)          (a)



/* MISC MACROS
``````````````````````````````````````````````````````````````````````````````*/
/*
 * Pass SM_SELF as the value of the 'to' parameter if you want the message
 * to be routed to the state machine that sent it.
 */
#define SM_SELF UINT32_MAX 

/* Size of the priority queue used by each state machine to hold messages */
#define NUM_PENDING 20UL

/* Error values for the routing functions */
#define SM_ROUTE_OK true
#define SM_NO_ROUTE false




/* CONTAINER TYPES 
````````````````````````````````````````````````````````````````````````````` */
/* 
 * SM_CB_ROUTE
 * A callback assigned to the state machine by the constructor. It will be 
 * passed to the message constructor and will eventually be called by the 
 * message router. 
 */

struct msg_t; /* Forward reference for the callback to use */
struct sm_t;  /* Forward reference for the state machine */



/* CONSTRUCTORS / DESTRUCTORS
``````````````````````````````````````````````````````````````````````````````*/
struct sm_t *new_sm(uint32_t id, bool (*route)(void *msg));
void         del_sm(struct sm_t *sm);


/* SEND MESSAGES 
``````````````````````````````````````````````````````````````````````````````*/
void sm_msg(struct sm_t *sm, uint32_t to, uint32_t state);
void sm_msgmag(struct sm_t *sm, uint32_t to, uint32_t state, int magnitude);


/* TRANSITION STATE 
``````````````````````````````````````````````````````````````````````````````*/
bool sm_accept(struct sm_t *sm, struct msg_t *msg);
void sm_consume(struct sm_t *sm);
void sm_refresh(struct sm_t *sm);


/* ACCESSORS 
``````````````````````````````````````````````````````````````````````````````*/
uint32_t msg_to(struct msg_t *msg);

int  sm_state(struct sm_t *sm);
int  sm_mag(struct sm_t *sm);;
bool sm_is_pending(struct sm_t *sm);
void sm_screen(struct sm_t *sm, int screen);
bool sm_is_locked(struct sm_t *sm);
void sm_lock(struct sm_t *sm, bool yn);



#endif
