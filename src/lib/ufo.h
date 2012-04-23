#ifndef __REPORTS_OF_UFOS
#define __REPORTS_OF_UFOS
#include <stdint.h>

struct rec_t {
        uint32_t h; /* height */
        uint32_t w; /* width */
        uint32_t y; /* minimum (upper-left) y coordinate */
        uint32_t x; /* minimum (upper-left) x coordinate */
        uint32_t l; /* total "length" (h*w) */
};


/*
 * Object to describe some mobile entity and the bounding environment
 * containing it. Both are represetned as bounding rectangles of type
 * struct rec_t.
 */
struct ufo_t {
        uint32_t options;
        struct rec_t obj;
        struct rec_t box;
};




/* 
 * Allocate a new struct ufo_t 
 */
struct ufo_t *new_ufo(int h0, int w0, int y0, int x0, 
                      int h1, int w1, int y1, int x1);


/* 
 * Initialize a previously-allocated struct ufo_t
 */
void set_ufo(struct ufo_t *ufo, int h0, int w0, int y0, int x0, 
                                int h1, int w1, int y1, int x1);


bool mvufo_right(struct rec_t *obj, struct rec_t *box);
bool mvufo_left(struct rec_t *obj, struct rec_t *box);
bool mvufo_up(struct rec_t *obj, struct rec_t *box);
bool mvufo_down(struct rec_t *obj, struct rec_t *box);

#define ufo_right(ptr, member) \
        mvufo_right(&(ptr->member.obj), &(ptr->member.box))

#define ufo_left(ptr, member) \
        mvufo_left(&(ptr->member.obj), &(ptr->member.box))

#define ufo_up(ptr, member) \
        mvufo_up(&(ptr->member.obj), &(ptr->member.box))

#define ufo_down(ptr, member) \
        mvufo_down(&(ptr->member.obj), &(ptr->member.box))


#define rec_up(ptr, member)    ptr->member.y -= 1
#define rec_down(ptr, member)  ptr->member.y += 1
#define rec_left(ptr, member)  ptr->member.x -= 1
#define rec_right(ptr, member) ptr->member.x += 1

#define rec_y(rec) rec.y
#define rec_x(rec) rec.x

#define ufo_y(ptr, member) ptr->member.obj.y
#define ufo_x(ptr, member) ptr->member.obj.x

#define ufo_obj(ptr, member) ptr->member.obj
#define ufo_box(ptr, member) ptr->member.box




#endif
