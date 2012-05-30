#pragma once
#ifndef __NOUN_MODELS_H
#define __NOUN_MODELS_H

struct ani_t  *__animation_fref; /* For mob.h */
struct noun_t *__noun_fref;

enum noun_model {
        SYSTEM,
        SAILBOAT, 
        HUMAN, 
        MONSTER, 
        HOPPER,
        OMEGA
};

typedef int  (*MODIFY_METHOD)(void *self);
typedef void (*RENDER_METHOD)(void *self);

void apply_noun_model(struct noun_t *noun);


#endif
