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

/* Model (class) definitions 
``````````````````````````````````````````````````````````````````````````````*/
#define MV(frame,dir) frame, dir
#define VB(frame,verb,dir) frame, verb, dir
#define NOMV MV(0,0) 
#define NOVB VB(0,0,0) 

/* Animation of nouns during rendering 
``````````````````````````````````````````````````````````````````````````````*/
struct ani_t {
        wchar_t frame[20];      /* Frames in the animation reel */
        int mv_frame;           /* Frame on which to issue a move signal */
        int mv_dir;             /* Direction in which to move */
        int mv_frame_alt;       /* Frame on which to issue a move signal */
        int mv_dir_alt;         /* Direction in which to move */
        int verb_frame;         /* Frame on which to issue a verb signal */
        int verb_id;            /* Verb to issue */
        int verb_dir;           /* Neighbor to address the verb to */
        int len;                /* Length of the animation (late binding) */
        int i;                  /* Incrementer for internal use */ 
};
static struct ani_t dig_r_test = {L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", MV(7,'r'), NOMV, NOVB};
static struct ani_t dig_d_test = {L"ᎲᎲᎲᱽᕤᱽᎲᎲⰾⲑⲑᎲⰾ", MV(7,'d'), NOMV, NOVB};
static struct ani_t dig_l_test = {L"ᎲᎲᎲ᱙ᕦ᱙ᎲᎲⰾⲑⲑᎲⰾ", MV(7,'l'), NOMV, NOVB};

#endif
