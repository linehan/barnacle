/* 
 * astar.c -- the A* pathfinding algorithm 
 *
 *       "I saw — with shut eyes, but acute mental vision — 
 *        I saw the pale student of unhallowed arts kneeling 
 *        beside the thing he had put together."
 *
 *               Frankenstein; or, The Modern Prometheus
 *               Author's introduction (1818)
 * 
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <assert.h>
#include "../com/barnacle.h"
#include "astar.h"
#include "../map/cell.h"
#include "../eng/bytes.h"
#include "../map/map.h"



/* ALLOCATORS, INITIALIZERS, AND DESTRUCTORS
````````````````````````````````````````````````````````````````````````````` */
/**
 * new_astar -- allocate and return ptr to a new astar object
 */
struct astar_t *new_astar(void)
{
        struct astar_t *new;
        new = calloc(1, sizeof(struct astar_t));
        return (new);
}


/**
 * astar_init -- initialize the A* object with the conditions for the run 
 * @astar: pointer to an allocated struct astar_t
 * @map: pointer to an allocated and populated matrix
 * @start: pointer to a cell with the coordinates of the starting node 
 */
void astar_init(struct astar_t *astar, struct matrix_t *map, int y, int x)
{
        astar->map    = map;
        astar->start  = new_cell((uint32_t)y, (uint32_t)x); 
        astar->goal   = NULL;
        list_head_init(&astar->path);
        astar->OPEN   = new_bh(astar->map->itr.len);
        astar->CLOSED = new_bh(astar->map->itr.len);
}

/**
 * del_astar -- destroy an astar object
 * @astar: pointer to an astar object
 */
void del_astar(struct astar_t *astar)
{
        /*astar->map is owned by the world map */

        /*struct cell_t *tmp, *nxt;*/
        /*[> Takes care of start, goal, and current <]*/
        /*[>list_for_each_safe(&astar->path, tmp, nxt, node) {<]*/
                /*[>list_del_from(&astar->path, &tmp->node);<]*/
                /*[>free(tmp);<]*/
        /*[>}<]*/

        /*tmp = astar->current;*/
        /*while (tmp && tmp->parent) {*/
                /*nxt = tmp->parent;*/
                /*free(tmp);*/
                /*tmp = nxt;*/
        /*}*/
        del_bh(astar->OPEN);
        del_bh(astar->CLOSED);
};


/**
 * astar_set_start -- set the coordinates of the start cell 
 * @astar: pointer to an astar object
 * @y    : new y-coordinate
 * @x    : new x-coordinate
 */
void astar_set_start(struct astar_t *astar, int y, int x)
{
        set_cell(astar->start, y, x);
}

/**
 * astar_set_goal -- set the coordinates of the goal cell 
 * @astar: pointer to an astar object
 * @y    : new y-coordinate
 * @x    : new x-coordinate
 */
void astar_set_goal(struct astar_t *astar, int y, int x)
{
        set_cell(astar->goal, y, x);
}



/* HELPER FUNCTIONS 
````````````````````````````````````````````````````````````````````````````` */
/**
 * map_val -- return the value stored at (y,x) in the map matrix
 * @astar: pointer to the A* object
 * @y: y-index of matrix
 * @x: x-index of matrix
 */
uint32_t mapval(struct astar_t *astar, int y, int x)
{
        return get_byte(mx_val(astar->map, y, x), ALT);
}


/**
 * make_path -- when the algorithm has a solution, spool it onto the path ring
 * @astar: pointer to the astar object
 */
int make_path(struct astar_t *astar)
{
        struct cell_t *tmp = astar->current;

        while (tmp) {
                list_add(&astar->path, &tmp->node);
                tmp = tmp->parent;
        }
}





/* COST FUNCTIONS AND HEURISTICS 
````````````````````````````````````````````````````````````````````````````` */
/*
 * mov_cost -- compute the path cost between cell a to cell b 
 * @a: pointer to a cell
 * @b: pointer to a cell
 */
float mov_cost(struct cell_t *a, struct cell_t *b)
{
        return (float) abs((a->x - b->x)) + abs((a->y - b->y));
}


/*
 * gn -- the path cost from the start node to the current node 
 * @astar: pointer to the astar object (contains start cell)
 * @cell: pointer to the current cell
 */
float gn(struct astar_t *astar, struct cell_t *cell)
{
        return (float) mov_cost(astar->start, cell);
}


/*
 * hn -- the estimated path cost from the current node to the goal node
 * @astar: pointer to the astar object (contains goal cell)
 * @cell: pointer to the current cell 
 */
float hn(struct astar_t *astar, struct cell_t *cell)
{
        return (float) mov_cost(astar->goal, cell); 
}



/* THE A* ALGORITHM 
````````````````````````````````````````````````````````````````````````````` */
/**
 * gen_neighbors -- generate an array of valid neighbors for the current cell
 * @astar: pointer to the astar object
 * @cell: pointer to the current cell
 */
struct cell_t **gen_neighbors(struct astar_t *astar, struct cell_t *cell)
{
        #define WALL_VALUE 4
        struct cell_t **neighbor;
        int parent_y;              /* y-position of current cell's parent */
        int parent_x;              /* x-position of current cell's parent */

        /* To prevent backtracking, we test against the parent's coordinates */
        parent_y = (cell->parent) ? cell->parent->y : -1;
        parent_x = (cell->parent) ? cell->parent->x : -1;

        /* Allocate memory for 4 neighbor pointers */
        neighbor = calloc(4, sizeof(struct cell_t *));

        /*
         * No graceful way to do this next bit...
         */

        /* Neighbor to the west of cell */
        if (((cell->x > 0)                                       // In-bounds  
        && (mapval(astar, cell->y, cell->x-1) < WALL_VALUE))     // Not wall   
        && !((parent_x == cell->x-1) && (parent_y == cell->y)))  // Not parent
        {
                neighbor[0] = new_cell(cell->y, cell->x-1);
        }
        /* Neighbor to the north of cell */
        if (((cell->y > 0)                                       // In-bounds
        && (mapval(astar, cell->y-1, cell->x) < WALL_VALUE))     // Not wall
        && !((parent_x == cell->x) && (parent_y == cell->y-1)))  // Not parent
        {
                neighbor[1] = new_cell(cell->y-1, cell->x);
        }
        /* Neighbor to the east of cell */
        if (((cell->x < astar->map->itr.cols-1)                  // In-bounds
        && (mapval(astar, cell->y, cell->x+1) < WALL_VALUE))     // Not wall
        && !((parent_x == cell->x+1) && (parent_y == cell->y)))  // Not parent
        {
                neighbor[2] = new_cell(cell->y, cell->x+1);
        }
        /* Neighbor to the south of cell */
        if (((cell->y < astar->map->itr.rows-1)                  // In-bounds
        && (mapval(astar, cell->y+1, cell->x) < WALL_VALUE))     // Not wall
        && !((parent_x == cell->x) && (parent_y == cell->y+1)))  // Not parent
        {
                neighbor[3] = new_cell(cell->y+1, cell->x);
        }

        return (neighbor);
}


/**
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
                        neighbor[i]->f = MAXPRI - (neighbor[i]->g + neighbor[i]->h);
                        bh_setpri(astar->OPEN, neighbor[i]->f, neighbor[i]->key);
                }
        }
        free(neighbor); /* gen_neighbors allocated these pointers */
}



/******************************************************************************
 * a_star -- generate an efficient path between a start node and a goal node
 * @map: pointer to a populated matrix
 * @start: pointer to a cell with the coordinates of the starting node 
 * @goal: pointer to a cell with the coordinates of the goal node
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
bool a_star(struct astar_t *astar, struct cell_t *goal)
{
        assert(astar != NULL && goal != NULL);

        struct cell_t *current;

        astar->goal = goal;

        /* Set up the start tile and add to OPEN */
        astar->start->g = 0;                                 // g(n)
        astar->start->h = hn(astar, astar->start);           // h(n)
        astar->start->f = astar->start->g + astar->start->h; // f(n) 
        bh_add(astar->OPEN, astar->start->f, astar->start->key, astar->start);

        while (!bh_is_empty(astar->OPEN)) {

                current = bh_pop(astar->OPEN); 

                if (same_cell(astar->goal, current)) {
                        astar->current = current;
                        make_path(astar);
                        bh_empty(astar->CLOSED);
                        bh_empty(astar->OPEN);
                        return true;
                }

                bh_add(astar->CLOSED, current->f, current->key, current);
                groom_neighbors(astar, current);

                /*test_rig(astar, current); // displays test output*/
        }
        astar->current = NULL;
        return false;
}


