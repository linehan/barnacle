#ifndef __NOUN_ANIMATE_H
#define __NOUN_ANIMATE_H
#include "../com/arawak.h"

struct noun_t;


struct ani_t {
        const wchar_t *wcs; /* Frames in the animation reel */
        int msg_at;         /* Frame on which to issue a move signal */
        int msg;            /* Direction in which to move */
        size_t len;         /* Length of the animation (late binding) */
};


void noun_animate(struct noun_t *noun);
int  wait_for(struct noun_t *noun);


#endif

