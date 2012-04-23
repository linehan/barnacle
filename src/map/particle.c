/*
 * particle.c
 *
 * A humble test that ended being decent enough to get its own source file.
 * Particles are randomly or deterministically placed on the screen, and
 * a function, or "rule", is applied to every particle, transforming them in
 * some way.
 */
#include <stdlib.h>
#include "../com/arawak.h"
#include "../lib/matrix.h"
#include "../lib/stoc/stoc.h"
#include "../eng/bytes.h"
#include "../test/test.h"
#include "tile.h"
#include "particle.h"


/* 
 * A coordinate pair and limits for the coordinate's translation.
 * To the algorithm, this struct is the "particle".
 */
struct yx_t {
        int y;
        int x;
        int h;
        int w;
        bool done; /* Indicates availibility to the particle allocator */ 
};

/* 
 * Set the members of a struct yx_t all at once 
 * @yx: pointer to an existing struct yx_t
 * @h: height limit
 * @w: width limit
 * @y: starting y-coordinate
 * @x: starting x-coordinate
 */
inline void yx_set(struct yx_t *yx, int h, int w, int y, int x)
{
        yx->h = h;
        yx->w = w;
        yx->y = y;
        yx->x = x;
        yx->done = false;
}

/*
 * Set the coordinates of a struct yx_t to a random pair
 * @yx: pointer to an existing struct yx_t
 */
inline void yx_any(struct yx_t *yx)
{
        yx->y = roll1d(yx->h);
        yx->x = roll1d(yx->w);
}


/* Transformation rules */
inline void RULE_R(struct yx_t *yx);
inline void RULE_L(struct yx_t *yx);
inline void RULE_U(struct yx_t *yx);
inline void RULE_D(struct yx_t *yx);
inline void RULE_DR(struct yx_t *yx);
inline void RULE_DL(struct yx_t *yx);
inline void RULE_UR(struct yx_t *yx);
inline void RULE_UL(struct yx_t *yx);


/* Shared variables */
static int req_particles;       /* The number of particles requested */
static int act_particles;       /* The number of particles active */
static int max_particles;       /* The maximum possible number of particles */
static int rule;                /* The currently active rule */



/*
 * surface_flow -- draw and transform particles according to a ruleset
 * @map: pointer to an allocated struct map_t
 */
void surface_flow(struct map_t *map)
{
        static struct yx_t particle[9500] = {0};
        static bool testing = true;

        static int h;
        static int w;
        int i=0;

        while (act_particles < req_particles) {
                while (!particle[i++].h == 0);
                --i;
                yx_set(&particle[i], LINES, COLS, 0, 0);
                yx_any(&particle[i]);
                act_particles++;
        }

        for (i=0; i<act_particles; i++) {
                place_ocean_tile(map, particle[i].y, particle[i].x);

                switch (rule) {
                case 0:
                        RULE_R(&particle[i]);
                        break;
                case 1:
                        RULE_DR(&particle[i]);
                        break;
                case 2:
                        RULE_D(&particle[i]);
                        break;
                case 3:
                        RULE_DL(&particle[i]);
                        break;
                case 4:
                        RULE_L(&particle[i]);
                        break;
                case 5:
                        RULE_UL(&particle[i]);
                        break;
                case 6:
                        RULE_U(&particle[i]);
                        break;
                case 7:
                        RULE_UR(&particle[i]);
                        break;
                }

                if (particle[i].done) {
                        particle[i].h = 0;
                        act_particles--;
                } else {
                        place_swell_tile(map, particle[i].y, particle[i].x);
                }
        }


        if (testing) {
                werase(DIAGNOSTIC_WIN);
                wprintw(DIAGNOSTIC_WIN, 
                        "req_particles: %05d\n"
                        "act_particles: %05d\n", 
                        req_particles, 
                        act_particles);

                for (i=0; i<(LINES/5)-3; i++) {                      
                        wprintw(DIAGNOSTIC_WIN, 
                                "particle y: %05d"
                                "particle x: %05d\n", 
                                particle[i].y,
                                particle[i].x);
                }
                doupdate();
        }
}


/*
 * Initialize the surface flow with a request of n particles
 */
void init_surface_flow(int n)
{
        max_particles = LINES * COLS;
        req_particles = n;
}
/*
 * Increment the requested surface flow by one 
 */
void inc_surface_flow(void)
{
        INC(req_particles, max_particles);
}
/*
 * Decrement the requested surface flow by one 
 */
void dec_surface_flow(void)
{
        DEC(req_particles, 0);
}
/*
 * Request a surface flow of n
 */
void set_surface_flow(int n)
{
        req_particles = (n < max_particles) ? n : max_particles;
}




/*
 * Cycle through the rules
 */
void rot_surface_rule(void)
{
        #define NRULES 8
        rule = (rule + 1) % NRULES;
}

/*
 * These 8 rules are all the linear translations in 8-connected space,
 * that is, up, down, left, right, up-left, up-right, down-right, down-left.
 */
inline void RULE_R(struct yx_t *yx)
{
        if (likely(yx->x < yx->w))
                yx->x++;
        else
                yx->done = true;
}
inline void RULE_L(struct yx_t *yx)
{
        if (likely(yx->x > 0))
                yx->x--;
        else
                yx->done = true;
}
inline void RULE_U(struct yx_t *yx)
{
        if (likely(yx->y > 0))
                yx->y--;
        else
                yx->done = true;
}
inline void RULE_D(struct yx_t *yx)
{
        if (likely(yx->y < yx->h))
                yx->y++;
        else
                yx->done = true;
}
inline void RULE_DR(struct yx_t *yx)
{
        RULE_D(yx);
        RULE_R(yx);
}
inline void RULE_DL(struct yx_t *yx)
{
        RULE_D(yx);
        RULE_L(yx);
}
inline void RULE_UR(struct yx_t *yx)
{
        RULE_U(yx);
        RULE_R(yx);
}
inline void RULE_UL(struct yx_t *yx)
{
        RULE_U(yx);
        RULE_L(yx);
}


double **z;
double **z1;

/*
 * sweet_flow -- hacky in-place shallow water DFT with matrix swap 
 * @map: pointer to an allocated struct map_t
 */
void sweet_flow(struct map_t *map)
{
        static const float c  = 1.0;  /* Wave velocity */
        static const float h  = 2.0;  /* Field height; distance b/t two vertices */
        static const float dt = 0.8;  /* The time/sampling interval */
        static const float d = 1.18;  /* A "dampening" factor, for collisions */

        /* We precompute the two wave equation integral coefficients */
        float A = (c * dt/h)*(c * dt/h);
        float B = (2 - 4*A);

        /* Map size */
        int rows = map->ufo.box.h-1;
        int cols = map->ufo.box.w-1;

        double **tmp; /* For when we swap matrices at the end */
        int i;
        int j;

        /* Allocate the matrices z and z1, to hold the water surface data. */
        if (z == NULL) {
                z    = simplex_matrix(map->ufo.box.h, cols+1);
                z1   = malloc(map->ufo.box.h * sizeof(double *));
                for (i=0; i<map->ufo.box.h; i++)
                        z1[i] = malloc(map->ufo.box.w * sizeof(double));
        }

        for (i=1; i<rows; i++) {
                for (j=1; j<cols; j++) {

                        /* Re-set the position to plain ocean */
                        /*place_ocean_tile(map, i, j);*/
                        mvwchgat(PLATE(ACTIVE, BGR), i, j, 1, 0, SEA_MED, NULL);

                        /* Compute the integral */
                        z1[i][j] = A*(z[i-1][j] + z[i+1][j] + z[i][j+1])
                                 + B*z[i][j] - z1[i][j];

                        /* 
                         * If the value corresponds to a map tile above
                         * sea level, multiply it by the dampening
                         * factor
                         */
                        if ((get_nibble(map->mx->mx[i][j], ALT)) != 0)
                                z1[i][j] *= d;

                        /* These are the "waves" on the map */
                        if (z1[i][j] > 0.00) 
                                place_swell_tile(map, i, j);
                }
        }
        /* Swap the matrices */ 
        tmp = z1;
        z1  = z;
        z   = tmp;
}


