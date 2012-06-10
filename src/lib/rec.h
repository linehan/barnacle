#pragma once
#ifndef __RECTANGLE_H
#define __RECTANGLE_H


struct rec_t {
        int h;
        int w;
        int y;
        int x;
};


struct rec_t *new_rec(int h, int w, int y, int x);
void set_rec(struct rec_t *rec, int h, int w, int y, int x);
bool rec_intersect(struct rec_t *a, struct rec_t *b);
bool rec_contains_yx(struct rec_t *rec, int y, int x);


#endif
