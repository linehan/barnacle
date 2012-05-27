#pragma once
#ifndef __A_STAR_H
#define __A_STAR_H
#include "../lib/matrix.h"
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
void            del_astar(struct astar_t *astar);
void           astar_init(struct astar_t *astar, struct matrix_t *map, int y, int x);
void      astar_set_start(struct astar_t *astar, int y, int x);
void       astar_set_goal(struct astar_t *astar, int y, int x);


/* Stand back, folks */
bool a_star(struct astar_t *astar, struct cell_t *goal);


#endif
