#ifndef __REPORTS_OF_UFOS
#define __REPORTS_OF_UFOS
#include <stdint.h>

struct rec_t {
        uint32_t h;
        uint32_t w;
        uint32_t y;
        uint32_t x;
};

// A ufo!
struct ufo_t {
        uint32_t options;
        struct rec_t obj;
        struct rec_t box;
        int (*up)(void *self);
        int (*down)(void *self);
        int (*left)(void *self);
        int (*right)(void *self);
};


struct ufo_t *new_ufo(int h0, int w0, int y0, int x0, 
                      int h1, int w1, int y1, int x1);

void set_ufo(struct ufo_t *ufo, 
             int h0, int w0, int y0, int x0, 
             int h1, int w1, int y1, int x1);

int rec_hit(struct rec_t *rec0, struct rec_t *rec1);


#define ufo_up(ptr, member)    ptr->member.up(&(ptr->member))
#define ufo_down(ptr, member)  ptr->member.down(&(ptr->member))
#define ufo_left(ptr, member)  ptr->member.left(&(ptr->member))
#define ufo_right(ptr, member) ptr->member.right(&(ptr->member))

#define ufo_y(ptr, member) ptr->member.obj.y
#define ufo_x(ptr, member) ptr->member.obj.x

#endif
