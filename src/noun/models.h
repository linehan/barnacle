#pragma once
#ifndef __NOUN_MODELS_H
#define __NOUN_MODELS_H

struct animation *__animation_fref; /* For mob.h */
struct noun_t    *__noun_fref;

enum noun_models {
        SAILBOAT, 
        PERSON, 
        MONSTER, 
        DUMMY
};

typedef int  (*MODIFY_METHOD)(void *self, int verb, int value);
typedef void (*RENDER_METHOD)(void *self);
/* Function tables for different noun models */
MODIFY_METHOD modify[100];
RENDER_METHOD render[100];

void apply_noun_model(struct noun_t *noun);

#endif
