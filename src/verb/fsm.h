#pragma once
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "verb.h"

/* Messages and reserved states
````````````````````````````````````````````````````````````````````````````` */
static const int SM_RESERVED_Enter  = 9999;
static const int SM_RESERVED_Exit   = 9998;
static const int SM_RESERVED_Update = 9997;

enum sm_state {
        SM_Default,
        SM_Keyboard,
        SM_GoUp,
        SM_GoDown,
        SM_GoLeft,
        SM_GoRight,
        SM_Die,
        SM_Punch,
        SM_Run,
        SM_Walk,
        SM_Jump,
};


struct sm_t {
        uint32_t id;
        int state;
        int value;
        bool accepting;
        bool (*send)(void *self);
};



static inline
struct sm_t *new_sm(uint32_t id, bool (*send)(void *self))
{
        struct sm_t *new = calloc(1, sizeof(struct sm_t));

        new->id   = id;
        new->send = send;

        return (new);
}



static inline
void sm_emit(struct sm_t *sm, uint32_t to, uint32_t delay, enum sm_state state)
{
        send_verb((int)state, sm->id, to, delay, sm->send);
}


static inline void sm_active(struct sm_t *sm, bool yesno)
{
        sm->accepting = yesno;
}


static inline bool sm_set(struct sm_t *sm, int state, int value)
{
        if (sm->accepting) {
                sm->state = state;
                sm->value = value;
                return true;
        }
        return false;
}

static inline int sm_state(struct sm_t *sm)
{
        return (int)(sm->state);
}

static inline int sm_value(struct sm_t *sm)
{
        return (sm->value);
}

#define SM_RESET sm_reset

static inline void sm_reset(struct sm_t *sm)
{
        sm_set(sm, 0, 0);
}

#endif
