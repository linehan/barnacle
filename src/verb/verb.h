#pragma once
#ifndef __VERB_MODEL_H
#define __VERB_MODEL_H

#include "../com/arawak.h"
#include "../lib/llist/list.h"


/* Abstract types 
````````````````````````````````````````````````````````````````````````````` */
struct verb_t {
        struct list_node node; /* Allows verbs to be added to linked list */
        uint32_t name;         /* Verb identifier */
        uint32_t from;         /* Sender (noun) identifier */
        uint32_t to;           /* Receiver (noun) identifier */
        uint32_t delay;        /* Delay between transmission and delivery */
        uint32_t value;        /* A value if the verb has a magnitude */
        bool (*send)(void *self);
};
        



/* Public functions 
````````````````````````````````````````````````````````````````````````````` */
void send_delayed_verbs(void);
void send_verb(int verb, uint32_t to, uint32_t from, int delay, bool (*send)(void *self));



/* State machine macros
````````````````````````````````````````````````````````````````````````````` */
/* Start the definition of a state machine */
#define BeginStateMachine                               \
        if (STATE_Global == state) {

/* An entry in the state machine for a particular state 'a' */
#define State(a)                                        \
        if (0)  {                                       \
                return (true);                          \
        } else if (a == state) {                        \
                if (0) {                

/* The transition into a state */
#define OnEnter                                         \
        return (true);                                  \
        } else if (MSG_RESERVED_Enter == msg->name) {

/* The transition out of a state */
#define OnExit                                          \
        return (true);                                  \
        } else if (MSG_RESERVED_Exit == msg->name) {

/* The transitions between enter and exit, one per tick */
#define OnUpdate                                        \
        return (true);                                  \
        } else if (MSG_RESERVED_Update == msg->name) {

/* A transition to occur on receipt of a message 'a' */
#define OnMsg(a)                                        \
        return (true);                                  \
        } else if (a == msg->name) {

/* Sets the current state (enacts a transition) to new state 'a' */
#define SetState(a)                                     \
        SetStateInGameObject(go, (int)a);

/* Finish the definition of a state machine */
#define EndStateMachine                                 \
        return (true);                                  \
        }} else {                                       \
                assert(!"Invalid State");               \
                return (false);                         \
        }                                               \
        return (false);



#endif

