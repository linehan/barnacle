/* 
 * rec.c -- Rectangle storage structures.
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
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "rec.h"

/*
 * Create a new struct rec_t to describe a rectangle, and initialize
 * it with a set of values.
 *
 * @h: Height of the rectangle.
 * @w: Width of the rectangle.
 * @y0: Minimum (initial) y value of the rectangle's position.
 * @x0: Minimum (initial) x value of the rectangle's position.
 *
 * RETURNS: Pointer to allocated struct rec_t.
 */
struct rec_t *new_rec(int h, int w, int y, int x)
{
        struct rec_t *new = malloc(sizeof(struct rec_t));

        new->h = h;
        new->w = w;
        new->y = y;
        new->x = x;

        return (new);
}


/*
 * Define values for a previously-allocated struct rec_t. 
 *
 * @rec: Pointer to an allocated struct rec_t
 * @h: Height of the rectangle
 * @w: Width of the rectangle
 * @y0: Minimum (initial) y value of the rectangle's position
 * @x0: Minimum (initial) x value of the rectangle's position
 */
void set_rec(struct rec_t *rec, int h, int w, int y, int x)
{
        assert(rec != NULL);

        rec->h = h;
        rec->w = w;
        rec->y = y;
        rec->x = x;
}


bool rec_intersect(struct rec_t *a, struct rec_t *b)
{
        assert(a != NULL && b != NULL);

        if ((a->x + a->w >= b->x)
        &&  (a->x        <= b->x + b->w)
        &&  (a->y + a->h >= b->y)
        &&  (a->y        <= b->y + b->h))
                return false;
        else
                return true;
}

