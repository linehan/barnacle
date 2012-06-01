#pragma once
#ifndef __DICE_H
#define __DICE_H

#include "stoc.h"


#define HEADS 1
#define TAILS 0 

 //A stochastic value in [0,MT_MAX) 
//#define __mt32__ mt_random()

//[> A stochastic value in [0,1] <]
//#define __mt01__ (double)((double)(__mt32__) / (double)MT_MAX)

#define roll1d(n) (roll_fair(n))

int vose_alias(int n, double *prob_array);
int roll_fair(int n);
int flip_biased(double bias);
int flip_sign(double bias);

static inline int flip(double bias)
{
        return flip_biased(bias);
}
static inline int roll(int n)
{
        return roll_fair(n);
}


#endif
