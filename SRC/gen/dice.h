#pragma once
#ifndef __DICE_H
#define __DICE_H

extern int MT_PRIMED;

typedef struct dice_t {
	int n; /* number of rolls */
	int d; /* number of sides */
	double *w; /* weight of each side */
	void *(*func)(int); /* function pointers */
} DICE;

int init_random();
int flip_biased(double bias);
int roll_fair(int sides);
DICE *new_dice(const char *str, ...);
int vose_alias(int n, double *prob_array);


#endif
