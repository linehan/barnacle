#ifndef __REPORTS_OF_UFOS
#define __REPORTS_OF_UFOS
#include <stdint.h>

// A ufo!
struct ufo_t {
        uint32_t x_min; // lower x bound
        uint32_t y_min; // lower y bound
        uint32_t x_max; // upper x bound
        uint32_t y_max; // upper y bound
        uint32_t x_now; // current x value
        uint32_t y_now; // current y value
        uint32_t w; // width
        uint32_t h; // height
};

/*
  Creates a new ufo type. The *_min and *_max values create a bounding box,
  and do not represent the height and width of the ufo itself. Rather, the
  bounding box takes into account the height and width when calculating the
  effective boundaries (by subtracting the height and width of the ufo).
*/
static 
struct ufo_t *new_ufo(int h, int w, int y_min, int x_min, int y_max, int x_max, int y_now, int x_now)
{
        struct ufo_t *new = malloc(sizeof *new);

        new->h = h;
        new->w = w;
        new->y_min = y_min;
        new->x_min = x_min;
        new->y_max = (y_max-h);
        new->x_max = (x_max-w);
        new->y_now = y_now;
        new->x_now = x_now;

        return (new);
}



// Move ufo right by 1 (positive x direction)
static inline void ufomvr(struct ufo_t *myufo)
{
        if (myufo->x_now == myufo->x_max)
                myufo->x_now = myufo->x_min;
        else
                myufo->x_now++;
}
// Move ufo left by 1 (negative x direction)
static inline void ufomvl(struct ufo_t *myufo)
{
        if (myufo->x_now == myufo->x_min)
                myufo->x_now = myufo->x_max;
        else
                myufo->x_now--;
}
// Move ufo up by 1 (negative y direction)
static inline void ufomvu(struct ufo_t *myufo)
{
        if (myufo->y_now == myufo->y_min)
                myufo->y_now = myufo->y_max;
        else
                myufo->y_now--;
}
// Move ufo down by 1 (positive y direction)
static inline void ufomvd(struct ufo_t *myufo)
{
        if (myufo->y_now == myufo->y_max)
                myufo->y_now = myufo->y_min;
        else
                myufo->y_now++;
}


// Expands to the current y value of struct ufo myufo
#define ufo_y(myufo) (myufo->y_now)
// Expands to the current x value of struct ufo myufo
#define ufo_x(myufo) (myufo->x_now)

// Move ufo right
#define ufo_r(myufo) (ufomvr(myufo))
// Left
#define ufo_l(myufo) (ufomvl(myufo))
// Up
#define ufo_u(myufo) (ufomvu(myufo))
// Down
#define ufo_d(myufo) (ufomvd(myufo))


#endif
