#pragma once
#ifndef __NOUN_MODELS_H
#define __NOUN_MODELS_H

struct ani_t  *__animation_fref; /* For mob.h */
struct noun_t *__noun_fref;

enum noun_models {
        SAILBOAT, 
        PERSON, 
        MONSTER, 
        DUMMY
};


typedef int  (*MODIFY_METHOD)(void *self);
typedef void (*RENDER_METHOD)(void *self);


/* Function tables for different noun models */
MODIFY_METHOD modify[100];
RENDER_METHOD render[100];


void apply_noun_model(struct noun_t *noun);



/* Animation of nouns during rendering 
``````````````````````````````````````````````````````````````````````````````*/
struct ani_t {
        wchar_t *frame;  /* Frames in the animation reel */
        int mv_frame;    /* Frame on which to issue a move signal */
        int mv_dir;      /* Direction in which to move */
        int len;         /* Length of the animation (late binding) */
        int i;           /* Incrementer for internal use */ 
};

typedef struct ani_t ANI;

#endif
