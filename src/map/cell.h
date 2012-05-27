#pragma once
#ifndef __CELL_H
#define __CELL_H

#include "../lib/list.h"


struct cell_t { 
        uint32_t key;          /* Morton code (perfect hash) */
        uint32_t y;            /* Cartesian y-coordinate */
        uint32_t x;            /* Cartesian x-coordinate */
        float g;               /* g(n) movement cost (used by A*) */
        float h;               /* h(n) heuristic (used by A*) */
        float f;               /* f(n) total movement cost (used by A*) */
        struct list_node node; /* Cells are nodes in a circular linked list */
        struct cell_t *parent; /* Traversal of a path the old-fashioned way */
};


/* Core functions */
struct cell_t *new_cell(int y, int x);
void           set_cell(struct cell_t *cell, int y, int x);
void           del_cell(struct cell_t *cell);
bool          same_cell(struct cell_t *a, struct cell_t *b);

/* Cellpath functions */
struct cell_t *cellpath_start(struct list_head *path);
struct cell_t *cellpath_goal(struct list_head *path);
struct cell_t *cellpath_next(struct list_head *path);

/* Maze generation functions */
struct cell_t **mk_neighbors(struct cell_t *cell, int h, int w, int opt);
void               cell_walk(struct cell_t **cell, int h, int w, int opt);



/* The caller may request a specific neighbor */
#define NBR_MASK  0x0000000F
#define NBR(opt)  ((opt) & NBR_MASK) /* The value of the neighbor byte */
#define WLK_FW    0x00000010  /* Only proceed forward; do not backtrack */
#define WLK_MO    0x00000020  /* Only consider the Moore neighborhood */
#define WLK_VN    0x00000040  /* Only consider the von Neumann neighborhood */
#define WLK_DRUNK 0x00000080  /* Select a random neighbor (drunken walk) */

/* Return the value of the specific option 'req' in the option word 'opt' */
#define OPT(opt,req) (((opt) & (req)) == (req)) ? true : false

#define NUM_VON_NEUMANN 4
#define NUM_MOORE       8


#endif
