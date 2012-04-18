#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "morton.h"
#include "matrix.h"
#include "bh.h"
#include "a_star.h"
#include "a_star_test.h"



// 
//   ALLOCATORS, INITIALIZERS, DESTRUCTORS
//   ``````````  ````````````  ````````````  



/*
 * new_cell -- allocate and initialize a new struct cell_t
 * @y: y-coordinate of cell
 * @x: x-coordinate of cell
 *
 * Notes
 * The 'key' member is used to perform lookups and identification of cells
 * in the OPEN or CLOSED sets. Each cell is identified by a kind of perfect 
 * hash called a Morton code, or Z-order code, which can be quickly computed 
 * by interleaving a pair of y and x coordinates. See "morton.h"
 */
struct cell_t *new_cell(uint32_t y, uint32_t x)
{
        struct cell_t *new = malloc(sizeof(struct cell_t));

        new->y   = y;
        new->x   = x;
        new->key = mort(y,x); 

        return (new);
}


/*
 * astar_init -- initialize the A* object with the conditions for the run 
 * @astar: pointer to an allocated struct astar_t
 * @map: pointer to an allocated and populated matrix
 * @st_y: y-coordinate of the starting position
 * @st_x: x-coordinate of the starting position
 * @go_y: y-coordinate of the goal position
 * @go_x: x-coordinate of the goal position
 */
void astar_init(struct astar_t *astar, struct matrix_t *map, 
                int st_y, int st_x, int go_y, int go_x)
{
        astar->map    = map;
        astar->start  = new_cell(st_y, st_x);
        astar->goal   = new_cell(go_y, go_x);
        astar->OPEN   = new_bh(astar->map->rows * astar->map->cols);
        astar->CLOSED = new_bh(astar->map->rows * astar->map->cols);
}




//
//      INLINE HELPER FUNCTIONS
//      `````` `````` `````````




/*
 * same_cell -- if the keys of both cells match, they are the same 
 * @a: pointer to a cell
 * @b: pointer to a cell
 */
bool same_cell(struct cell_t *a, struct cell_t *b)
{
        return (a->key == b->key) ? true : false;
}


/*
 * map_val -- return the value stored at (y,x) in the map matrix
 * @astar: pointer to the A* object
 * @y: y-index of matrix
 * @x: x-index of matrix
 */
uint32_t mapval(struct astar_t *astar, int y, int x)
{
        return mx_val(astar->map, y, x);
}




//
//      COST FUNCTIONS AND HEURISTICS 
//      ```` ````````` ``` ``````````




/*
 * mov_cost -- compute the path cost between cell a to cell b 
 * @a: pointer to a cell
 * @b: pointer to a cell
 */
float mov_cost(struct cell_t *a, struct cell_t *b)
{
        return (float) MAXPRI - abs((a->x - b->x)) + abs((a->y - b->y));
}


/*
 * gn -- the path cost from the start node to the current node 
 * @astar: pointer to the astar object (contains start cell)
 * @cell: pointer to the current cell
 */
float gn(struct astar_t *astar, struct cell_t *cell)
{
        return MAXPRI - mov_cost(astar->start, cell);
}


/*
 * hn -- the estimated path cost from the current node to the goal node
 * @astar: pointer to the astar object (contains goal cell)
 * @cell: pointer to the current cell 
 */
float hn(struct astar_t *astar, struct cell_t *cell)
{
        return MAXPRI - mov_cost(cell, astar->goal); 
}




//
//      THE A* ALGORITHM
//      ``` `` `````````




/*
 * gen_neighbors -- generate an array of valid neighbors for the current cell
 * @astar: pointer to the astar object
 * @cell: pointer to the current cell
 */
struct cell_t **gen_neighbors(struct astar_t *astar, struct cell_t *cell)
{
        struct cell_t **neighbor;  /* array of neighbor cell pointers */
        int parent_y;              /* y-position of current cell's parent */
        int parent_x;              /* x-position of current cell's parent */


        /* To prevent backtracking, we test against the parent's coordinates */
        parent_y = (cell->parent) ? cell->parent->y : -1;
        parent_x = (cell->parent) ? cell->parent->x : -1;

        /* Allocate memory for 4 neighbor pointers */
        neighbor = malloc(4 * sizeof(struct cell_t *));

        /*
         * No graceful way to do this next bit...
         */

        /* Neighbor to the west of cell */
        if (((cell->x > 0)                                       // In-bounds  
        && (mapval(astar, cell->y, cell->x-1) < 2))              // Not wall   
        && !((parent_x == cell->x-1) && (parent_y == cell->y)))  // Not parent
        {
                neighbor[0] = new_cell(cell->y, cell->x-1);
        }
        /* Neighbor to the north of cell */
        if (((cell->y > 0)                                       // In-bounds
        && (mapval(astar, cell->y-1, cell->x) < 2))              // Not wall
        && !((parent_x == cell->x) && (parent_y == cell->y-1)))  // Not parent
        {
                neighbor[1] = new_cell(cell->y-1, cell->x);
        }
        /* Neighbor to the east of cell */
        if (((cell->x < astar->map->cols-1)                      // In-bounds
        && (mapval(astar, cell->y, cell->x+1) < 2))              // Not wall
        && !((parent_x == cell->x+1) && (parent_y == cell->y)))  // Not parent
        {
                neighbor[2] = new_cell(cell->y, cell->x+1);
        }
        /* Neighbor to the south of cell */
        if (((cell->y < astar->map->rows-1)                      // In-bounds
        && (mapval(astar, cell->y+1, cell->x) < 2))              // Not wall
        && !((parent_x == cell->x) && (parent_y == cell->y+1)))  // Not parent
        {
                neighbor[3] = new_cell(cell->y+1, cell->x);
        }

        return (neighbor);
}


/*
 * groom_neighbors -- examine neighbor candidates; add some to the OPEN set 
 * @astar: pointer to the astar object
 * @cell: pointer to the current cell
 *
 * Notes
 * There is a lot of footsie going on in the second half of this function,
 * trying to decide whether or not the estimated g(n) is the real deal. 
 * This is necessary because A* requires an "admissible heuristic", that is, 
 * the heuristic must provide no more than the lowest-cost path to the goal.
 */
void groom_neighbors(struct astar_t *astar, struct cell_t *cell)                
{
        struct cell_t **neighbor;  /* array of neighbor cell pointers */
        float estimated_g;         /* estimated g(n) */
        bool estimate_is_better;   /* do we use the estimate or re-calculate */
        int i;

        /* Generate the neighbors of cell */
        neighbor = gen_neighbors(astar, cell);

        /* Loop over each neighbor */
        for (i=0; i<4; i++) {

                /* Illegal neighbors won't have been allocated */
                if (neighbor[i] == NULL) 
                        continue;

                /* We don't need anything from CLOSED */
                if (bh_has_member(astar->CLOSED, neighbor[i]->key))
                        continue;

                /* Compute our estimate g(n) */
                estimated_g = cell->g + mov_cost(cell, neighbor[i]);

                /* Add neighbor to OPEN unless it's already there */
                if (!bh_has_member(astar->OPEN, neighbor[i]->key)) {
                        bh_add(astar->OPEN, 0, neighbor[i]->key, neighbor[i]);
                        neighbor[i]->h = mov_cost(neighbor[i], astar->goal);
                        estimate_is_better = true;
                } else if (estimated_g < gn(astar, neighbor[i])) {
                        estimate_is_better = true; 
                } else {
                        estimate_is_better = false;
                }

                /* Give it the proper scores and change its priority */
                if (estimate_is_better) {
                        neighbor[i]->parent = cell;
                        neighbor[i]->g = estimated_g;
                        neighbor[i]->f = neighbor[i]->g + neighbor[i]->h;
                        bh_setpri(astar->OPEN, neighbor[i]->f, neighbor[i]->key);
                }
        }
}



/******************************************************************************
 * a_star -- generate an efficient path from a start node to a goal node
 * @map: pointer to a populated matrix
 * @st_y: y-coordinate of the starting position
 * @st_x: x-coordinate of the starting position
 * @go_y: y-coordinate of the goal position
 * @go_x: x-coordinate of the goal position
 *
 * This is the main entry point to the A* module. Here is a brief outline:
 ******************************************************************************
 * 1. Place start node in the OPEN set
 * 2. While the OPEN set is not empty
 *
 *      2a. Remove the top (least costly) element from the OPEN set
 *              ? If this element is the GOAL, we are done.
 *              ? If it's not the GOAL, place it in the CLOSED set.
 *
 *      2b. Request groom_neighbors() evaluate the 4 cells neighboring the 
 *          current one (the one we have just added to CLOSED). 
 *              ? If any neighbor is out-of-bounds in some way, we ignore it. 
 *              ? If any neighbor is in the CLOSED set, we ignore it. 
 *              ? If any neighbor is NOT in the OPEN set, we add it to the 
 *                OPEN set.
 *              
 *      2c. Continue the loop, selecting whichever neighbor has the lowest 
 *          cost (i.e., is the top element in the OPEN set).
 *
 * 3. Return
 ******************************************************************************/
bool a_star(struct matrix_t *map, int st_y, int st_x, int go_y, int go_x)
{
        struct cell_t *current;
        struct astar_t astar;
       
        astar_init(&astar, map, st_y, st_x, go_y, go_x);

        /* Set up the start tile and add to OPEN */
        astar.start->g = 0;                               // g(n)
        astar.start->h = hn(&astar, astar.start);         // h(n)
        astar.start->f = astar.start->g + astar.start->h; // f(n) 
        bh_add(astar.OPEN, astar.start->f, astar.start->key, astar.start);

        while (!bh_is_empty(astar.OPEN)) {

                current = bh_pop(astar.OPEN); 

                if (same_cell(astar.goal, current))
                        return true;

                bh_add(astar.CLOSED, current->f, current->key, current);
                groom_neighbors(&astar, current);

                test_rig(&astar, current); // displays test output
        }
        return false;
}


