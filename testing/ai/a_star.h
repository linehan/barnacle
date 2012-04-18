#pragma once
#ifndef __A_STAR_H
#define __A_STAR_H
#include "matrix.h"


struct cell_t { 
        struct cell_t *parent;
        uint32_t key;
        uint32_t y; 
        uint32_t x; 
        float g; 
        float h;
        float f;
};

struct astar_t { 
        struct matrix_t *map;
        struct cell_t *start; 
        struct cell_t *goal; 
        struct bh_t *OPEN;
        struct bh_t *CLOSED;
};


bool a_star(struct matrix_t *map, int starty, int startx, int goaly, int goalx);


#endif
