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
#include <stdlib.h>
#include <math.h>

#include "../../lib/llist/list.h"
#include "stoc.h"


/* 
 * flip_biased - simulates a biased coin flip
 * Generates a random value between 0 and 1. If b is less than the random 
 * value, the coin is said to be "heads", otherwise it is "tails".  
 */
int flip_biased(double b)
{
        return (mt_normalize()<b) ? 1 : 0;
}



/* 
 * roll_fair - simulate a fair roll of an n sided die. Returns an int
 *             between 0 and (n-1)
 */
int roll_fair(int n)                                                          
{                                       
        n = (n > 0) ? (n-1) : n;

        return (floor(mt_normalize() * (double)n));
}                



/* 
 * The Vose alias method is an algorithm used to efficiently construct
 * a non-uniform discrete probability sampling. Such a probability set 
 * is desirable if one wishes to roll a weighted die, that is, a die with
 * sides/values that do not each have an equal probability of being rolled.
 * His solution is quite interesting and elegant, and worth reading about
 * in detail if you wish to examine this function more closely. Otherwise, 
 * you may assume that leprechauns are responsible for the return value. 
 */

struct alias_worklist { double prob; struct list_node node; };

static LIST_HEAD(Sm); /* The small alias worklist (linked list) */
static LIST_HEAD(Lg); /* The large alias worklist (linked list) */

/*
 * Given an array of probability values which sum to 1 (representing the
 * discrete parts of the non-uniform distribution, or the weight of each
 * of the n sides of a die), return the value of a roll with those weights.
 */
int vose_alias(int n, double *prob_array)
{
	struct alias_worklist *tmp_l;
        struct alias_worklist *tmp_s;
	double Pr[n]; /* Adjusted probability values */
	double Al[n]; /* References to certain adjusted probability values */
	int S;
        int i;

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
	return (n+1); /* impossible */
}
