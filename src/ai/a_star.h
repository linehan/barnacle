#pragma once
#ifndef __A_STAR_H
#define __A_STAR_H
#include "../lib/matrix.h"
#include "../lib/morton.h"
#include "../lib/bheap.h"
#include "../map/cell.h"



struct astar_t { 
        struct matrix_t *map;
        struct cell_t *start; 
        struct cell_t *goal; 
        struct cell_t *current;
        struct list_head path;
        struct bh_t *OPEN;
        struct bh_t *CLOSED;
};



struct astar_t *new_astar(void);
bool a_star(struct astar_t *astar, struct cell_t *goal);
void astar_init(struct astar_t *astar, struct matrix_t *map, int y, int x);



#endif
