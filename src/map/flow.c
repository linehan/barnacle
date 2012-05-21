#include <stdlib.h>
#include <pthread.h>
#include "../com/arawak.h"
#include "../lib/matrix.h"
#include "../lib/stoc/stoc.h"
#include "../eng/bytes.h"
#include "../test/test.h"
#include "tile.h"
#include "../eng/bytes.h"


/* -------------------------------------------------------------------------- */
/* 
 * Constants that can be adjusted to yield different characteristics 
 */ 
const double c  = 9.0;  /* Wave velocity */
const double h  = 2.0;  /* Field height; distance b/t two vertices */
const double dt = 0.08;  /* The time/sampling interval */
/*const double d  = 1.18; [> A "dampening" factor, for collisions <]*/
const double d = 1.25;

/* 
 * Precomputed integral coefficients for the wave equation in 2D
 */
static double A; 
static double B; 

static int cols;
static int rows;

/*
 * These matrices hold the height values of each particle
 */
double **z;
double **z1;

/*thr_pool_t *flow_thread_pool; */
struct map_t *flowmap;

pthread_t       flowth;         /* The flow thread */
pthread_mutex_t flow_lock;      /* The rendering mutex */

/* -------------------------------------------------------------------------- */


/*
 * init_flow -- allocate the two matrices and get ready to rock
 * @map: pointer to the map structure which will receive the water effects
 */
void init_flow(struct map_t *map)
{
        /* We have to init them here since they aren't constant expressions */
        A = ((c * dt/h)*(c * dt/h));
        B = (2 - 4*A);

        flowmap = map;
        rows = pos_h(map->pos) - 1;
        cols = pos_w(map->pos) - 1;

        /* Allocate the two matrices */
        z  = simplex_matrix(rows+1, cols+1);
        z1 = empty_simplex_matrix(rows+1, cols+1);
}



/* -------------------------------------------------------------------------- */



/*
 * flow -- hacky in-place shallow water DFT with matrix swap 
 * @map: pointer to an allocated struct map_t
 */
void *flow(struct map_t *map)
{
        /*assert (z != NULL && z1 != NULL);*/
        double **tmp; /* For when we swap matrices at the end */
        int i;
        int j;

        for (i=1; i<rows; i++) {
                for (j=1; j<cols; j++) {

                        /* Compute the integral */
                        z1[i][j] = A*(z[i-1][j] + z[i+1][j] + z[i][j+1])
                                 + B*z[i][j] - z1[i][j];
                        /* 
                         * If the value corresponds to a map tile above
                         * sea level, multiply it by the dampening
                         * factor
                         */
                        if (ELEV(flowmap, i, j) != 0)
                                z1[i][j] *= d;
                }
        }
        tmp = z1;
        z1  = z;
        z   = tmp;
}



/* -------------------------------------------------------------------------- */



/*
 * render_flow -- request that the flow state be rendered on screen
 */
void *render_flow(struct map_t *map)
{
        /*assert(z != NULL && z1 != NULL);*/
        int i;
        int j;

        for (i=1; i<rows; i++) {
                for (j=1; j<cols; j++) {

                        /* Don't draw if it isn't above sea level */
                        if (TILE(map, i, j) != OCN) 
                                continue;

                        if (z[i][j] > 0.00) {
                                mvwadd_wch(PLATE(flowmap, BGR), i, j, &hibase);
                        }
                        /*
                         * Don't bother to draw the non-highlighted background,
                         * it will get OR-ed anyway during the copywin() call
                         * as part of map_refresh()
                         */
                        else
                                mvwchgat(PLATE(flowmap, BGR), i, j, 1, 0, SEA_MED, NULL);
                }
        }
        return NULL;
}



/* -------------------------------------------------------------------------- */

void do_flow(struct map_t *map)
{
        if (z == NULL)
                init_flow(map);

        if (map->id != flowmap->id)
                flowmap = map;

        flow(map);
        render_flow(map);
}


/*void fork_flow(void)*/
/*{*/
        /*pthread_create(&flowth, NULL, &flow, ACTIVE);*/
/*}*/


