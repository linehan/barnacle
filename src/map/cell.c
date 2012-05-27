#include <stdlib.h>
#include "../com/arawak.h"
#include "../lib/morton.h"
#include "cell.h"


/* CORE FUNCTIONS
``````````````````````````````````````````````````````````````````````````````*/
/**
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
struct cell_t *new_cell(int y, int x)
{
        struct cell_t *new = calloc(1, sizeof(struct cell_t));

        new->y   = (uint32_t)y;
        new->x   = (uint32_t)x;
        new->key = mort(new->y,new->x); 

        return (new);
}


/**
 * del_cell -- destroy a cell object
 * @cell: pointer to a cell object (will be free'd)
 */
void del_cell(struct cell_t *cell)
{
        free(cell);
}


/**
 * set_cell -- set the (y,x) coordinates of an existing cell
 * @cell: pointer to a cell object
 * @y   : new y-coordinate
 * @x   : new x-coordinate
 */
void set_cell(struct cell_t *cell, int y, int x)
{
        cell->y = (uint32_t)y;
        cell->x = (uint32_t)x;
        cell->key = MORT(y,x);
}


/**
 * same_cell -- if the keys of both cells match, they are the same 
 * @a: pointer to a cell object
 * @b: pointer to a cell object
 */
bool same_cell(struct cell_t *a, struct cell_t *b)
{
        return (a->key == b->key) ? true : false;
}


/* CELL PATH FUNCTIONS 
``````````````````````````````````````````````````````````````````````````````*/
/**
 * cellpath_start -- return the start node of a cell path
 * @path: pointer to a cell path
 */
struct cell_t *cellpath_start(struct list_head *path)
{
        if (list_empty(path))
                return NULL;
        else
                return list_top(path, struct cell_t, node);
}


/**
 * cellpath_goal -- if a path list is not empty, return the goal node
 * @path: pointer to a path
 */
struct cell_t *cellpath_goal(struct list_head *path)
{
        if (list_empty(path))
                return NULL;
        else
                return list_tail(path, struct cell_t, node);
}


/**
 * cellpath_next -- return the next node of a cell path
 * @path: pointer to a cell path
 */
struct cell_t *cellpath_next(struct list_head *path)
{
        struct cell_t *tmp;

        if (list_empty(path))
                return NULL;

        tmp = list_top(path, struct cell_t, node);
              list_del(&tmp->node);
        tmp = list_top(path, struct cell_t, node);

        return tmp;
}





/* GROSS 
``````````````````````````````````````````````````````````````````````````````*/
/*
 * mk_neighbors -- generate an array of valid neighbors for the current cell
 * @cell: pointer to the current cell
 * @h: height of cell plane
 * @w: width of cell plane
 * @opt: options
 */
struct cell_t **mk_neighbors(struct cell_t *cell, int h, int w, int opt)
{
        #define CAN_W(y,x)  (((x)>0) && !(((px)==((x)-1)) && ((py)==(y))))
        #define CAN_E(y,x)  (((x)<w) && !(((px)==((x)+1)) && ((py)==(y))))
        #define CAN_N(y,x)  (((y)>0) && !(((px)==(x)) && ((py)==((y)-1))))
        #define CAN_S(y,x)  (((y)<h) && !(((px)==(x)) && ((py)==((y)+1))))
        #define CAN_SW(y,x) (CAN_S(y,x) && CAN_W(y,x) && OPT(opt, WLK_MO))
        #define CAN_SE(y,x) (CAN_S(y,x) && CAN_E(y,x) && OPT(opt, WLK_MO))
        #define CAN_NW(y,x) (CAN_N(y,x) && CAN_W(y,x) && OPT(opt, WLK_MO)) 
        #define CAN_NE(y,x) (CAN_N(y,x) && CAN_E(y,x) && OPT(opt, WLK_MO))

        struct cell_t **neighbor;  /* array of neighbor cell pointers */
        int py;                    /* y-position of current cell's parent */
        int px;                    /* x-position of current cell's parent */
        int y;
        int x;

        /* To prevent backtracking, we test against the parent's coordinates */
        py = (cell->parent && (!OPT(opt, WLK_FW))) ? cell->parent->y : -1;
        px = (cell->parent && (!OPT(opt, WLK_FW))) ? cell->parent->x : -1;

        y = cell->y;
        x = cell->x;

        /* Allocate memory for 4 neighbor pointers */
        neighbor = calloc(NUM_MOORE, sizeof(struct cell_t *));

        /* Fill the von Neumann neighborhood */
        neighbor[0] = (CAN_N(y,x)) ? new_cell(y-1, x) : NULL;
        neighbor[1] = (CAN_E(y,x)) ? new_cell(y, x+1) : NULL;
        neighbor[2] = (CAN_S(y,x)) ? new_cell(y+1, x) : NULL;
        neighbor[3] = (CAN_W(y,x)) ? new_cell(y, x-1) : NULL;

        /* Fill the Moore neighborhood */
        neighbor[4] = (CAN_NE(y,x)) ? new_cell(y-1, x+1) : NULL;
        neighbor[5] = (CAN_SE(y,x)) ? new_cell(y+1, x+1) : NULL;
        neighbor[6] = (CAN_NW(y,x)) ? new_cell(y-1, x-1) : NULL;
        neighbor[7] = (CAN_SW(y,x)) ? new_cell(y+1, x-1) : NULL;

        return (neighbor);
}

/*
 * cell_walk -- select and assign a new active node to the 'cell' pointer
 * @cell: pointer to the current cell
 * @h: height of cell plane
 * @w: width of cell plane
 * @opt: options
 */
void cell_walk(struct cell_t **cell, int h, int w, int opt) 
{
        assert(cell || !"Cell does not exist!\n");

        struct cell_t **nbr;
        int select;

        /* Generate neighbors for the caller's cell */
        nbr = mk_neighbors(*cell, h, w, opt); 
        assert(nbr || !"nbr[] does not exist!\n");

        /* 
         * Choose a random edge if we are walking drunk, else choose
         * the specified edge.
         */
        if (OPT(opt, WLK_DRUNK)) {
                for (;;) {
                        select = roll_fair(NUM_MOORE);
                        if (!(select < NUM_MOORE))
                                continue;
                        if (nbr[select] != NULL)
                                break;
                }
        } else {
                select = NBR(opt);
        }
        assert(select < NUM_MOORE || !"select is too big!\n");

        /* 
         * The selected neighbor is made the active node; the prior
         * active node (the 'cell' of the caller) becomes the selected
         * neighbor's parent.
         */
        assert(*cell || !"Cell points to nothing!\n");
        assert(nbr[select] || !"nbr[select] is NULL!\n");

        nbr[select]->parent = *cell;
        *cell = nbr[select];
}
