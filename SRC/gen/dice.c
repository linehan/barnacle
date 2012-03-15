// vim:fdm=marker
/*******************************************************************************
FILENAME:  dice.c                                                   
Subroutines for the generation of pseudorandom numbers, dice rolls, and
other probabilistic results. 
--------------------------------------------------------------------------------
   DETAILS                                                               {{{1

    This document and certain functions here use the "dice notation" 
    first alluded to in the 1974 "Blue Box" edition of Dungeons & Dragons:

       "...In some places the reader will note an abbreviated notation 
        for the type of die has [sic] been used. The first number is 
        the number of dice used, the letter "d" appears, and the last 
        number is the type of dice used. Thus, "2d4" would mean that 
        two 4-sided dice would be thrown (or one 4-sided die would be 
        thrown twice); "3d12" would indicate that three 12-sided dice 
        are used, and so on." 

    The system is very simple and goes a long way toward clarifying any
    discussion that involves the rolling of dice. The convention is:

          AdX, where A: the number of dice
                     X: the number of sides ("faces") on each die

    This notation also accomodates various mathematical operators that can
    be used to shape the outcome of a roll or series of rolls:

          AdX+B, where B: a number added to the sum of AdX

    Other operators that can be used include -, *, %, or anything else you
    put your mind to. Along with these are the special operands L (lowest)
    and H (highest), which take the lowest or highest result of the rolls 
    as their value, e.g.

          AdX-L (Roll AdX and discard the lowest result)

    It can be seen that for this example, the probability curve of the rolls 
    will be skewed toward the higher numbers. When rolling dice, we want to 
    ensure that the results are normally distributed. To do this, consider 
    the central limit theorem:

    Let ⦃X₁,...,Xₙ⦄ be a random sample of size n -- that is, a sequence of 
    independent and identically distributed random variables with expected 
    values μ and variances σ². 
       
    Suppose we are interested in the behavior of the sample average of 
    these random variables: Sₙ := (X₁+⋯+Xₙ)/n. 
       
    The central limit theorem asserts that as n gets larger and larger, 
    the distribution of Sₙ approximates normal, with mean μ and variance 
    (1/n)σ².
       
    In fact, Sₙ approaches a normal distribution regardless of the shape 
    of the distributions of individual X's!

    So, if we roll a fair die, the probability distribution will be linear,
    i.e., each face will have an equal probability of being rolled. In order
    to make the results of our dice rolls normally distributed, we simply
    roll at least three dice, and work on the sum, the aggregate, of these 
    throws.                                                              }}}1
--------------------------------------------------------------------------------
*******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "../lib/llist/list.h"
#include "../pan/test.h"

#include "mtwist.h"

#define HEADS 1  
#define TAILS 0 
/******************************************************************************/
typedef struct dice_t {
	int n; /* number of rolls */
	int d; /* number of sides */
	double *w; /* weight of each side */
	void *(*func)(int); /* function pointers */
} DICE;

struct pautomaton {
        int n;  /* number of states */
	int *Q; /* states */
	int *F; /* accepting states */
	int v;  /* row index */
	double **P; /* transition matrix */
};

struct alias_worklist {
	double prob;
	struct list_node node;
};

/* Vose alias worklists */
static LIST_HEAD(Sm);
static LIST_HEAD(Lg);

/* The Mersenne twister generator creates fresh random numbers in
 * batches of 624. When init_random() is called, mt_init() will
 * generate a new batch of randoms, and MT_PRIMED will be set to
 * 624. Every subsequent call to mt_normalize() (which is the sole
 * caller of the Mersenne twister itself) includes a call to mt_check(),
 * which decrements MT_PRIMED by 1, and calls init_random() again if 
 * MT_PRIMED gets to 0. */
static int MT_PRIMED = 0;
/******************************************************************************/
/* Initialize rand() and mt_random() */
int init_random()
{
	/* Seed rand() with the system time BEFORE calling mt_init, otherwise
	* all your numbers will start looking awfully familiar... */
	srand(time(NULL));
	mt_init();
	MT_PRIMED = 624;
	return 0;
}
/* The Mersenne twister generator will produce 624 high quality random digits, 
 * and after they have been used up, will need to be re-initialized. */
int mt_check(void)
{
	if (--MT_PRIMED == 0)
		init_random();
	else
		return 0;
}
/* Transform the return value of mt_random() to the range [0, 1]. All calls
 * to the Mersenne twister go through this function, so this function alone
 * modifies the count of MT_PRIMED. */
double mt_normalize()
{
	double max = (double)MT_MAX;
	double a   = (double)mt_random();
	mt_check();
	return (a/max);
}
/* Flip a biased coin with probability of heads = b(ias) 
 * Generates a random value between 0 and 1. If b (<1) is less than the random 
 * value generated, the coin is said to be "heads", otherwise it is "tails".  */
int flip_biased(double b)
{
	return (mt_normalize()<b) ? 1 : 0;
}
/* Return a value for a fair dice roll */
int roll_fair(int n)                                                          
{                                                                             
	double r = mt_normalize();
	int ans = floor(r*(double)n);
	/*wprintw(DIAGNOSTIC_WIN, "ROLL:%d ", ans);*/
	return ans;
}                                   
/* Return a pointer to a die */
DICE *new_dice(char *str, ...)
{
	int i;
	DICE *new = malloc(sizeof *new);
	if (new == NULL) 
		perror ("DICE was not allocated properly!");

	sscanf(str, "%dd%d", &(new->n), &(new->d));
	new->w = malloc(new->d * sizeof(double));
	new->func = malloc(new->d * sizeof(void *));

	for (i=0; i<(new->d); i++) {
		if (&(new->w[i]) == NULL) 
			perror("DICE weight was not allocated properly!");
	}

	va_list(argp); 
	va_start(argp, str);
	for (i=0; i<(new->d); i++) {
		new->w[i] = va_arg(argp, double);
		wprintw(DIAGNOSTIC_WIN, "%f ", new->w[i]);
	}
	va_end(argp);
	return new;
}
/* Implement Vose's alias method */
int vose_alias(int n, double *prob_array)
{
	int S, i;
	double Pr[n]; /* Adjusted probability values */
	double Al[n]; /* References to certain adjusted probability values */
	struct alias_worklist *tmp_l, *tmp_s;

	for (i=0; i<n; i++) {
		struct alias_worklist *new = malloc(sizeof *new);
		new->prob = (prob_array[i]*(double)n);
		if ((new->prob)<1.0)
			list_add(&Sm, &(new->node));
		else
			list_add(&Lg, &(new->node));
	}
	for (i=0; ((!(list_empty(&Sm))) && (!(list_empty(&Lg)))); i++) {
		tmp_s = list_top(&Sm, struct alias_worklist, node);
			list_del_from(&Sm, &(tmp_s->node));
		tmp_l = list_top(&Lg, struct alias_worklist, node);
			list_del_from(&Lg, &(tmp_l->node));

		Pr[i] = tmp_s->prob;
		Al[i] = tmp_l->prob;

		Al[i] = ((Al[i]+Pr[i])-1.0);

		if (Al[i]<1.0) 
			list_add(&Sm, &(tmp_l->node));
		else           
			list_add(&Lg, &(tmp_l->node));
	}
	for (; (!(list_empty(&Lg))); i++) {
		Pr[i] = 1.0; 
		tmp_l = list_top(&Lg, struct alias_worklist, node);
			list_del_from(&Lg, &(tmp_l->node));
			free(tmp_l);
	}
	for (; (!(list_empty(&Sm))); i++) {
		Pr[i] = 1.0;
		tmp_s = list_top(&Sm, struct alias_worklist, node);
			list_del_from(&Sm, &(tmp_s->node));
			free(tmp_s);
	}
	for (i=0; i<n; i++) {
		S = roll_fair(n);
		if ((flip_biased(Pr[S])) == HEADS) 
			return S;
		else
			return (int)Al[S];
	}
        wprintw(DIAGNOSTIC_WIN, "BAD DOG");
	return (n+1); /* impossible */
}

