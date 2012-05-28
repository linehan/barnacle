/* 
 * pos.c -- Positional object that can be added as a struct component.
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
#include "pos.h"

#define POS(ptr) ((struct pos_t *)(ptr))



inline void pos_save(struct pos_t *pos)
{
        pos->saved_y = pos->ufo->y;
        pos->saved_x = pos->ufo->x;
        pos->saved_hdg = pos->hdg;
}



void _setyx_method(void *self, int y, int x)
{
        assert(self != NULL);

        struct pos_t *pos = POS(self);

        pos_save(pos);

        /* UFO coordinates must be in the box boundary */
        pos->ufo->y = clamp(y, pos->ymin, pos->ymax);
        pos->ufo->x = clamp(x, pos->xmin, pos->xmax);
}

void _restore_method(void *self)
{
        assert(self != NULL);
        struct pos_t *pos = POS(self);
        pos->setyx(pos, pos->saved_y, pos->saved_x);
        pos->hdg = pos->saved_hdg;
}


// Move pos right by 1 (positive x direction)
void _r_method(void *self)
{
        assert(self != NULL);

        struct pos_t *pos = POS(self);

        pos_save(pos);
        INC(pos->ufo->x, pos->xmax);
        pos->hdg = EAST;
}

// Move pos left by 1 (negative x direction)
void _l_method(void *self)
{
        assert(self != NULL);

        struct pos_t *pos = POS(self);

        pos_save(pos);
        DEC(pos->ufo->x, pos->xmin);
        pos->hdg = WEST;
}

// Move pos up by 1 (negative y direction)
void _u_method(void *self)
{
        assert(self != NULL);

        struct pos_t *pos = POS(self);

        pos_save(pos);
        DEC(pos->ufo->y, pos->ymin);
        pos->hdg = NORTH;
}


// Move pos down by 1 (positive y direction)
void _d_method(void *self)
{
        assert(self != NULL);

        struct pos_t *pos = POS(self);

        pos_save(pos);
        INC(pos->ufo->y, pos->ymax);
        pos->hdg = SOUTH;
}


void method_delete(void *self)
{
        struct pos_t *pos = POS(self);

        free(pos->ufo);
        free(pos->box);
        free(pos);
}



/*
 * Creates a new struct pos_t, described as the minimal bounding rectangle
 * around some object, represented as the height, width, and initial y,x 
 * coordinates of the object (to be consistent with ncurses function style).
 *
 * @h0: Height of the object's bounding rectangle.
 * @w0: Width of the object's bounding rectangle.
 * @y0: Minimum (initial) y value of the object's bounding rectangle.
 * @x0: Minimum (initial) x value of theo bject's bounding rectangle.
 * @h1: Height of the environment's bounding rectangle.
 * @w1: Width of the environment's bounding rectangle.
 * @y1: Minimum (initial) y value of the environment's bounding rectangle.
 * @x1: Minimum (initial) x value of the environment's bounding rectangle.
 */
struct pos_t *new_pos(int ufo_h, int ufo_w, int ufo_y, int ufo_x, 
                      int box_h, int box_w, int box_y, int box_x)
{
        struct pos_t *new = malloc(sizeof(struct pos_t));

        new->ufo = new_rec(ufo_h, ufo_w, ufo_y, ufo_x);
        new->box = new_rec(box_h, box_w, box_y, box_x);

        new->ymin = box_y;
        new->xmin = box_x;
        new->ymax = ((box_h + box_y) - ufo_h);
        new->xmax = ((box_w + box_x) - ufo_w);

        new->move_l = &_l_method;
        new->move_r = &_r_method;
        new->move_u = &_u_method;
        new->move_d = &_d_method;
        new->setyx  = &_setyx_method;
        new->restore = &_restore_method;
        new->del = &method_delete;

        return (new);
}

void set_pos(struct pos_t *pos, 
             int ufo_h, int ufo_w, int ufo_y, int ufo_x, 
             int box_h, int box_w, int box_y, int box_x)
{
        set_rec(pos->ufo, ufo_h, ufo_w, ufo_y, ufo_x);
        set_rec(pos->box, box_h, box_w, box_y, box_x);

        pos->ymin = box_y;
        pos->xmin = box_x;
        pos->ymax = ((box_h + box_y) - ufo_h);
        pos->xmax = ((box_w + box_x) - ufo_w);
}

