/* 
 * animate.c -- Noun animations.
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
#include "../com/barnacle.h"
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
                noun->step(noun, noun->animation->msg);

        /* Increment the current frame and/or terminate the animation */
        if (++noun->frame == noun->animation->len) {
                noun->frame = 0;
                noun->animation = NULL;
                return;
        }

        return;
}

