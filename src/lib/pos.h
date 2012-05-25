#pragma once
#ifndef __POS_AND_REC_HEADER 
#define __POS_AND_REC_HEADER

#include "../com/arawak.h"
#include "rec.h"


/*
 * Object to describe some mobile entity and the bounding environment
 * containing it. Both are represetned as bounding rectangles of type
 * struct rec_t.
 */
struct pos_t {
        struct rec_t *ufo;
        struct rec_t *box;

        int saved_y;
        int saved_x;
        int ymin;    /* current ufo y coordinate */
        int xmin;    /* current ufo x coordinate */
        int ymax;
        int xmax;

        enum hdg_t saved_hdg;
        enum hdg_t hdg; /* current ufo heading (facing direction) */

        void (*move_l)(void *self);
        void (*move_r)(void *self);
        void (*move_u)(void *self);
        void (*move_d)(void *self);
        void (*restore)(void *self);
        void (*setyx)(void *self, int y, int x);
        void (*del)(void *self);
};

static inline int pos_y(struct pos_t *pos)
{
        return (pos->ufo->y);
}
static inline int pos_x(struct pos_t *pos)
{
        return (pos->ufo->x);
}
static inline int pos_h(struct pos_t *pos)
{
        return (pos->ufo->h);
}
static inline int pos_w(struct pos_t *pos)
{
        return (pos->ufo->w);
}
static inline void pos_u(struct pos_t *pos)
{
        pos->move_u(pos);
}
static inline void pos_d(struct pos_t *pos)
{
        pos->move_d(pos);
}
static inline void pos_l(struct pos_t *pos)
{
        pos->move_l(pos);
}
static inline void pos_r(struct pos_t *pos)
{
        pos->move_r(pos);
}
static inline int pos_ymax(struct pos_t *pos)
{
        return (pos->ymax);
}
static inline int pos_ymin(struct pos_t *pos)
{
        return (pos->ymin);
}
static inline int pos_xmax(struct pos_t *pos)
{
        return (pos->xmax);
}
static inline int pos_xmin(struct pos_t *pos)
{
        return (pos->xmin);
}
static inline int pos_boxw(struct pos_t *pos)
{
        return (pos->box->w);
}
static inline int pos_boxh(struct pos_t *pos)
{
        return (pos->box->h);
}
static inline int pos_hdg(struct pos_t *pos)
{
        return (pos->hdg);
}
static inline int pos_saved_y(struct pos_t *pos)
{
        return (pos->saved_y);
}
static inline int pos_saved_x(struct pos_t *pos)
{
        return (pos->saved_x);
}


/* 
 * Allocate a new struct ufo_t 
 */
struct pos_t *new_pos(int ufo_h, int ufo_w, int ufo_y, int ufo_x, 
                      int box_h, int box_w, int box_y, int box_x); 

/* 
 * Allocate a new struct ufo_t 
 */
void set_pos(struct pos_t *pos, 
             int ufo_h, int ufo_w, int ufo_y, int ufo_x, 
             int box_h, int box_w, int box_y, int box_x); 


#endif
