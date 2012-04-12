#pragma once
#ifndef __PARTICLE_H
#define __PARTICLE_H

#include "map.h"

/* The main function that can be called by the event loop */
void surface_flow(struct map_t *map);

/* Control bindings */
void rot_surface_rule(void);    /* Cycle between rules */
void init_surface_flow(int n);  /* Initialize the particle generator */
void set_surface_flow(int n);   /* Set the number of particles */
void inc_surface_flow(void);    /* Increment the number of particles */
void dec_surface_flow(void);    /* Decrement the number of particles */

#endif
