#include "../com/arawak.h"
#include "noun.h"


/**
 * GET DELAY
 * wait_for -- return the number of frames (ticks) left in an animation
 * @noun: pointer to a noun object
 */
int wait_for(struct noun_t *noun)
{
        int remaining;
        int total;

        if (noun->animation)
                total = noun->animation->len;
        else
                total = 0;

        assert(noun->frame <= total || !"Animation frame overflow"); 
        remaining = total - noun->frame;

        return (remaining);
}


/**
 * DO ANIMATE
 * noun_animate -- draw successive character "frames", trigger messages, etc.
 * @noun: pointer to a noun
 */
void noun_animate(struct noun_t *noun)
{
        if (!noun->animation)
                return;

        /* Draw the current frame */
        if (noun->frame < noun->animation->len)
                wcch(noun->win, &noun->animation->wcs[noun->frame], 0, FLEX);

        /* Move the panel if this is the move frame */
        if (noun->animation->msg_at == noun->frame)
                noun->_step(noun, noun->animation->msg);

        /* Increment the current frame and/or terminate the animation */
        if (++noun->frame == noun->animation->len) {
                noun->frame = 0;
                noun->animation = NULL;
                return;
        }

        return;
}

