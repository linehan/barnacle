#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "ufo.h"


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
struct rec_t *new_rec(int h, int w, int y0, int x0)
{
        struct rec_t *new = malloc(sizeof(struct rec_t));

        new->h = h;
        new->w = w;
        new->y = y0;
        new->x = x0;

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
void set_rec(struct rec_t *rec, int h, int w, int y0, int x0)
{
        assert(rec != NULL);

        rec->h = h;
        rec->w = w;
        rec->y = y0;
        rec->x = x0;
}


/*
 * Test to determine whether the rectangles described by obj and box 
 * are intersecting. 
 * @obj: Pointer to struct rec_t.
 * @box: Pointer to struct rec_t.
 *
 * RETURNS: 0 if no intersection detected, 1 if intersection detected.
 */
bool rec_hit(struct rec_t *obj, struct rec_t *box)
{
        assert(obj != NULL && box != NULL);

        if ((obj->x +  obj->w >= box->x) &&
            (obj->x <= box->x +  box->w) && 
            (obj->y +  box->h >= box->y) && 
            (obj->y <= box->y +  box->h)) 
                return false; 
        else 
                return true; 
}



// Move ufo right by 1 (positive x direction)
bool mvufo_right(struct rec_t *obj, struct rec_t *box)
{
        assert(obj != NULL && box != NULL);

        obj->x++;

        if (rec_hit(obj, box)) {
                obj->x--;
                return false;
        }
        else    return true;
}

// Move ufo left by 1 (negative x direction)
bool mvufo_left(struct rec_t *obj, struct rec_t *box)
{
        assert(obj != NULL && box != NULL);

        obj->x--;

        if (rec_hit(obj, box)) {
                obj->x++;
                return false;
        }
        else    return true;
}

// Move ufo up by 1 (negative y direction)
bool mvufo_up(struct rec_t *obj, struct rec_t *box)
{
        assert(obj != NULL && box != NULL);

        obj->y--;

        if (rec_hit(obj, box)) {
                obj->y++;
                return false;
        }
        else    return true;
}


// Move ufo down by 1 (positive y direction)
bool mvufo_down(struct rec_t *obj, struct rec_t *box)
{
        assert(obj != NULL && box != NULL);

        obj->y++;

        if (rec_hit(obj, box)) {
                obj->y--;
                return false;
        }
        else    return true;
}




/*
 * Creates a new struct ufo_t, described as the minimal bounding rectangle
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
struct ufo_t *new_ufo(int h0, int w0, int y0, int x0, int h1, int w1, int y1, int x1)
{
        struct ufo_t *new = malloc(sizeof(struct ufo_t));

        new->options = 0;

        set_rec(&new->obj, h0, w0, y0, x0);
        set_rec(&new->box, h1, w1, y1, x1);

        return (new);
}



/*
 * Identical to new_ufo(), except that it initializes a previously-allocated
 * struct ufo_t in-place.
 *
 * @ufo: Pointer to a previously-allocated struct ufo_t.
 * @h0: Height of the object's bounding rectangle.
 * @w0: Width of the object's bounding rectangle.
 * @y0: Minimum (initial) y value of the object's bounding rectangle.
 * @x0: Minimum (initial) x value of theo bject's bounding rectangle.
 * @h1: Height of the environment's bounding rectangle.
 * @w1: Width of the environment's bounding rectangle.
 * @y1: Minimum (initial) y value of the environment's bounding rectangle.
 * @x1: Minimum (initial) x value of the environment's bounding rectangle.
 */
void set_ufo(struct ufo_t *ufo, int h0, int w0, int y0, int x0, int h1, int w1, int y1, int x1)
{
        assert(ufo != NULL);

        ufo->options = 0;

        set_rec(&ufo->obj, h0, w0, y0, x0);
        set_rec(&ufo->box, h1, w1, y1, x1);
}




